/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1986 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
/*
 * Miscellaneous C functions
 */

#include "mailblok.h"
#include "cfuncs.h"

int cmisc;                    /* just to get dragged in by loader */

/* xcmp - case independent bcmp */
xcmp(s1,s2,l)
register char *s1, *s2;
register int l;
{
    register int d;

    for (; l > 0; l--, s1++, s2++)
        if ((d = mailblok.upcase[*s1] - mailblok.upcase[*s2]))
            return d;
    return 0;
}

/* rdcard - do a cms rdcard macro */
static struct {
    union {
        double foo;                  /* force dword alignment */
        char name[8];
    } u;
    unsigned bufptr;
    u_short len1, len2;
} rdcard_plist;

rdcard(bp, l)
char *bp;
int l;
{
    int rc;
    register int i;

    bcopy("CARDRD  ",rdcard_plist.u.name, 8);
    rdcard_plist.bufptr = 0x80000000 | (int)bp;
    rdcard_plist.len1 = rdcard_plist.len2 = l;
    if ((rc = svc202(&rdcard_plist)) != 0)
        return -1;
    for (i = rdcard_plist.len2-1; i >= 0 && isspace(bp[i]); i--);
    return i+1;
}

/* punchc - do a cms punchc macro */
static struct {
    union {
        double foo;                  /* force dword alignment */
        char name[8];
    } u;
    unsigned bufptr;
} punchc_plist;

punchc(bp, l)
char *bp;
int l;
{
    int rc;
    char buf[80];
    register i;

    bcopy(bp, buf, (l > 80)? (l = 80): l);
    for (i = l; i < 80; i++)         /* pad w/ blanks */
        buf[i] = ' ';
    bcopy("CARDPH  ",punchc_plist.u.name, 8);
    punchc_plist.bufptr = (unsigned)buf;
    return svc202(&punchc_plist);
}

/* wrterm - do a cms wrterm macro */
static struct {
    union {
        double foo;                  /* force dword alignment */
        char name[8];
    } u;
    unsigned bufptr;
    char flags[2];
    u_short len;
} wrterm_plist;

wrterm(sp)
char *sp;
{
    int rc;

    bcopy("TYPLIN  ",wrterm_plist.u.name, 8);
    wrterm_plist.bufptr = 0x01000000 | (int)sp;
    wrterm_plist.flags[0] = 'B';
    wrterm_plist.flags[1] = 0;
    wrterm_plist.len = strlen(sp);
    if ((rc = svc202(&wrterm_plist)) != 0)
        return -1;
}

/* wtr - call the message writer */
/* sample invocation: wtr("SMT", 4, 2, "x", 47, "ca", "hello");   */
/*                               ^msg # */
extern monwtr(), acall();

wtr(mod, id, n, f1, p1, f2, p2, f3, p3, f4, p4, f5, p5, f6, p6, f7, p7)
char *mod;
{
    int wtrplist[10], i, *p;
    char data[4];

    if (strlen(mod) != 3) return;
    wtrplist[0] = id & 0x00ffffff;
    data[0] = 0x04;
    bcopy(mod, &data[1], 3);           /* module identifier */
    bcopy(data, &wtrplist[1], 4);
    for (i = 2, p = &f1; i < 10 && n > 0; i++, n--, p++) {
        int flag = 0;
        char *fmt = (char *)*p++;
        while (*fmt != '\0') switch(*fmt++) {
        case 'a':
        case 'A':
            flag |= 0x10000000;
            break;
        case 'b':
        case 'B':
            flag |= 0x08000000;
            break;
        case 'c':
        case 'C':
            flag |= 0x04000000;
            break;
        case 'd':
        case 'D':
            flag |= 0x02000000;
            break;
        case 'x':
        case 'X':
            flag |= 0x01000000;
            break;
        default:
            return;
        }
        wtrplist[i] = (int)*p&0x00ffffff | flag;
    }
    wtrplist[i-1] |= 0x80000000;
    acall(monwtr, 0, wtrplist);
}

/*
 * salloc - alloc some memory from the mailer stack.
 * adjusts the MAILBLOK stack pointer, length pair.
 */
int
salloc(amount)
int amount;
{
    int ptr;

    amount += 7;                    /* round up to the next hightest */
    amount &= 0xFFFFFFF8;           /* douleword boundary */
    if (mailblok.stackp[1] < amount)
        return 0;                   /* stack overflow */
    mailblok.stackp[1] -= amount;
    ptr = mailblok.stackp[0];
    mailblok.stackp[0] += amount;
    bzero(ptr, amount);
    return ptr;
}

/*
 * CMS disk file I/O
 * file name format used is: "fn.ft.fm"
 *  fsinit() - just fills the name into the fscb
 *  fserase  - erases the named file
 *  fsread   - read
 *  fswrite  - write
 *  fsclose  - close
 *  fspoint  - set read/write record pointer
 *  (file format options, etc are set by stuffing the fscb)
 */
fsinit(f, n)
register struct fscb *f;
register char *n;
{
    static char nameini[] = "                A1";
    register i;

    bzero(f, sizeof(*f));
    bcopy(nameini, f->fscbfn, 18);     /* initialize fn ft fm */
    for (i = 0; i < 8 && *n != '.' && *n != '\0'; i++, n++)
        f->fscbfn[i] = mailblok.upcase[*n];
    if (*n++ != '.') return 24;
    for (i = 0; i < 8 && *n != '.' && *n != '\0'; i++, n++)
        f->fscbft[i] = mailblok.upcase[*n];
    if (*n == '\0') return 0;
    if (*n++ != '.') return 24;
    for (i = 0; i < 2 && *n != '\0'; i++, n++)
        f->fscbfm[i] = mailblok.upcase[*n];
    if (*n != '\0') return 24;
    return 0;
}

fserase(name)
char *name;
{
    struct fscb f;
    int rc;

    if (rc = fsinit(&f, name))
        return rc;
    bcopy("ERASE   ", f.fscbcomm, 8);
    return svc202(&f);
}

fsread(f, buf, buflen)
struct fscb *f;
char *buf;
int *buflen;
{
    int rc;

    f->fscbbuf = buf;
    f->fscbsize = *buflen;
    bcopy("RDBUF   ", f->fscbcomm, 8);
    f->fscbflg |= FSCBEPL;
    f->fscbnoit = f->fscbanit = 1;
    rc = svc202(f);
    *buflen = f->fscbnord;
    return rc;
}

fswrite(f, buf, buflen)
struct fscb *f;
char *buf;
int *buflen;
{
    f->fscbbuf = buf;
    f->fscbsize = *buflen;
    bcopy("WRBUF   ", f->fscbcomm, 8);
    f->fscbflg |= FSCBEPL;
    f->fscbnoit = f->fscbanit = 1;
    return svc202(f);
}

fsclose(f)
struct fscb *f;
{
    bcopy("FINIS   ", f->fscbcomm, 8);
    return svc202(f);
}

fspoint(f,wr,rd)
struct fscb *f;
int wr,rd;
{
    bcopy("EPOINT  ", f->fscbcomm, 8);
    f->fscbwptr = wr;
    f->fscbrptr = rd;
    return svc202(f);
}

/*
 * _rdhdr:   Return next record from mail header.  Called by exits
 *           that want to read the mail file rather than simply
 *           push it along with PUNHDR.  This duplicates a lot of
 *           code that was originally in PUNHDR.
 *
 *  Chugs through the list of FIELDDs hanging from mailfld and returns
 *  them in the order seen.  Those marked invisible are skipped and
 *  those marked edited are generated from the (edited) TOKEND list
 *  rather than the usual method of simply using everything between
 *  fistr and fiend.
 *
 *  returns: 0 or 12 for EOH or no header (HDRSCAN failed)
 *
 *  How it works:
 *
 *  The curfld static variable points to the current fieldd being worked
 *  on.  It is initially zero and is reset to zero when EOH is returned
 *  as a means of determining whether we are in the midst of getting
 *  header lines or have just been invoked for the first time for a new
 *  header.
 *
 *  The curstr static variable points to the current position in the
 *  fieldd being worked on (i.e. between fitok->tokptr and fiend)
 *  It is equal to fiend when the fieldd has been completely exhausted.
 *  This indicates that curfld should be set to point to the next fieldd
 *  to start the cycle over again.  This is how we keep track of how
 *  far we have progressed through the current fieldd between calls.
 *  Curtok is used similarly for edited fields.
 */
int
_rdhdr(buf,lenp)
char *buf;  /* buffer to return record in */
int *lenp;  /* input: max length of buf, output: actual length */
{
    static struct fieldd *curfld = 0;  /* initial & final value */
    static char *curstr = 0;           /* current pos within fieldd */
    static struct tokend *curtok = 0;  /*  ditto for edited fieldds */
    static struct tokend *prevtok = 0;
    static char beginning = 0;

    if (curfld == 0) {                 /* initial call */
        if (!(mailblok.mailflg1&MFLG1HDS)) /* no header was scanned */
          return 12;                   /* so report EOH */
        if ((curfld = mailblok.mailfld) == 0) /* head of fieldd list */
          return 12;
        beginning = 1;                 /* at beginning of field */
    }

top:
    if (beginning) {                   /* extra work at beginning */
        beginning = 0;
        while (curfld->fiflg1&FIINVFLG)  /* "invisible" field */
          if ((curfld = FINEXT(curfld)) == 0) /* skip it */
            return 12;
        if (curfld->fiflg1&FIEDIFLG) { /* it's been edited (argh) */
            curtok = FITOK(curfld);    /* so use tokend list */
            prevtok = 0;
            curstr = 0;                /* indicate by zeroing other */
        } else {
            curstr = TOKPTR(FITOK(curfld));
            curtok = 0;                /* indicate by zeroing other */
        }
    }

    /* now actually figure out how much to return and copy it to buf */
    if (!(curfld->fiflg1&FIEDIFLG)) { /* unedited */
        int slen = curfld->fiend - curstr;
        register int n;

        if (slen <= 0) {                /* all used up */
            if ((curfld = FINEXT(curfld)) == 0) /* get next field */
              return 12;
            beginning = 1;
            goto top;                   /* and start over */
        }

        /* First look for x15(newline) line break and then see if
           what's left is still too long for the given record length.
           If so, back up until we find a blank and split the line. */
        for (n = 0; curstr[n] != 0x15 && n < slen; n++) ;
        slen = n;
        if (slen > *lenp)               /* find a line break */
          for (n = *lenp-1; curstr[n] != ' ' && n > 0; n--) ;
        else
          n = slen;
        bcopy(curstr,buf,n);            /* move into buffer */
        *lenp = n;                      /* return length moved */
        curstr += n;                    /* update ptr for next call */
        if ((curstr < curfld->fiend) && (*curstr == 0x15))
          ++curstr;                     /* skip the newline */
    } else {                            /* edited - this is ugly! */
        int n = *lenp;
        static int fold = 0;

        if (curtok == 0) {              /* used up this record */
            if ((curfld = FINEXT(curfld)) == 0) /* get next field */
              return 12;
            beginning = 1;
            fold = 0;
            goto top;                   /* and start over */
        }
        if (fold) {
            fold = 0;
            *buf++ = ' ';               /* insert leading blank */
            --n;
        }
        while (n > 0) {
            if (curtok->toklen < n) {   /* room for next token */
                int extra;
                if (prevtok
                    && (prevtok->toktype == TOKATM
                        || prevtok->toktype == TOKQSTR)
                    && (curtok->toktype == TOKATM
                        || curtok->toktype == TOKQSTR)) {
                    *buf++ = ' ';
                    extra = 1;
                } else
                  extra = 0;
                bcopy(TOKPTR(curtok),buf,curtok->toklen);
                buf += curtok->toklen;
                n -= curtok->toklen + extra;
                if (curtok == FITOK(curfld)) { /* first token */
                    *buf++ = ':';             /* gets a colon! */
                    --n;
                }
                prevtok = curtok;
                if ((curtok = TOKNEXT(curtok)) == 0)
                  break;
            } else {                    /* fold to next record */
                fold = 1;
                break;
            }
        }
        *lenp -= n;                     /* subtract residual */
    }
    return 0;
}

/*
 * rdtxt: returns the text portion of the message.  This is the
 *  companion of rdhdr as puntxt is the companion of punhdr.
 *  Records too long to fit in the supplied buffer get wrapped.
 *
 * returns 0 or 12 for EOF.
 *
 * How it works:
 *  Either reads cards from the reader or the SMTP DATA file
 *  depending on the MFLG2SMT flag.
 */
int
_rdtxt(buf,lenp)
char *buf;   /* buffer to return record in */
int *lenp;   /* input: max length of buf, output: actual length */
{
    static struct fscb fs;
    static int beginning = 1;
    static char inbuf[132],*inp; /* xxx not a very general solution */
    static int resid = 0;      /* residual length */

    /* Do one-time source-specific(rdr/smtp) initialization */
    if (beginning) {
        if (mailblok.mailflg2&MFLG2SMT) {       /* BSMTP */
            fsclose("smtp.data");
            fsinit(&fs,"smtp.data");
            fspoint(&fs, 0, mailblok.mailhrec+2); /* skip over header */
        } else {                       /* reader */
            int i;
            cpcmd("CLOSE 00C",9);
            if (cpcmd("READY 00C",9)) return 12;
            /* skip header records */
            for (i = 0; i < mailblok.mailhrec; i++)
              if (rdcard(inbuf,mailblok.mailsfb.sfbrecsz) < 0)
                return 12;
        }
        resid = 0;
        beginning = 0;
    }

    /* read a new record if nothing left in inbuf from previous call */
    if (resid == 0) {
        resid = sizeof(inbuf);
        /* read a new record.  resid will contain length read */
        if (mailblok.mailflg2&MFLG2SMT) {  /* source is smtp data */
            if (fsread(&fs,inbuf,&resid)) {
                beginning = 1;         /* EOF or other FSREAD error */
                return 12;
            }
        } else {                       /* source is reader */
            if ((resid = rdcard(inbuf,mailblok.mailsfb.sfbrecsz)) < 0) {
                beginning = 1;         /* EOF or other RDCARD error */
                return 12;
            }
        }
        inp = inbuf;                   /* set up pointer */
    }

    /* Return as much of inbuf as possible in caller's buffer and
       leave any residual around for a subsequent call. */
    if (resid > *lenp) {           /* record has to be wrapped */
        bcopy(inp,buf,*lenp);      /* copy as much as will fit */
        resid -= *lenp;            /* residual length */
        inp += *lenp;              /* set up pointer for residual */
    } else {                       /* record fits just fine */
        bcopy(inp,buf,resid);      /* copy it all */
        *lenp = resid;             /* update returned length */
        resid = 0;                 /* set to zero for next time */
    }
    return 0;
}
