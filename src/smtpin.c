/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1987 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
/* Changes in 1.24:
 *  - include mailvers in service ready message.
 *  - send bsmtp reply class m
 *  - BSMTP replies are now only generated depending on the appearance
 *    of VERB and RPLY commands.
 *  - added "RPLY MAIL|SMTP" command.
 *  - failure reply from DATA command now includes the reasons from
 *    the userlog file.
 *  - Bug fix (some valid addresses wouldn't parse) from Andy Hooper,
 *    Queens University (QUCDN).
 *  - rep_wrt() will now wrap lines longer than 80 characters.
 *  - added ":REA" command to ignore :READ header from CMS PUNCH cmd.
 *  - sprinkle in many messages for the console
 *  - parse DATA RFC822 header into MAILKTAB for benefit of user exits.
 *    (the authoritave FROM: is the SMTP one pointed to by MAISFLD).
 *  - added smtprep() which actually parses SMTP replies and sends
 *    appropriate error mail.
 */
/*
 * Batch SMTP support for files received.  Smtpin() handles a smtp
 * command stream in the reader and smtprep() handles a smtp reply
 * stream.
 *
 * Alan Crosswell 12/85
 */
#include "types.h"
#include "cfuncs.h"
#include "cmsbloks.h"
#include "mailblok.h"

/* Debugging flags (define at compile time) */

/* DEBUG_CHECK_LOCAL        Trace check_local routine                */
/* DEBUG_FIND_ROUTD         Trace find_routd routine                 */
/* DEBUG_S_GETDATA          Trace s_getdata routine                  */
/* DEBUG_S_INPUT            Trace s_input routine                    */
/* DEBUG_S_RCPT             Trace s_rcpt routine                     */
/* TRACE                    Procedure call trace                     */

struct routd *find_routd();

typedef enum {                    /* states of the server */
    initial = 0,                  /* 0: connection established */
    seenhelo,                     /* 1: HELO command seen */
    seenmail,                     /* 2: MAIL command seen */
    error,                        /* 3: error */
    final                         /* 4: final */
} s_state;
#define NSTATE 4

/* global SMTP server information */
static struct {
    int (*repf)();               /* function to pass reply text to */
    s_state state;               /* server state */
    char *domain;                /* HELO domain */
    int dl;                      /* length of it */
    int rcpts;                   /* number of valid RCPT's */
    struct fieldd *first_rcpt;   /* pointer to rcpt fieldd */
    struct tbentry from_tbe;     /* tbentry for FROM: */
    struct tbentry rcpt_tbe;     /* tbentry for TO: */
    struct fscb data_file;       /* fscb for data */
    enum {disknone=0,diskfull,diskother} disk_error;
    enum {
        noverb=0,                /* no VERB seen */
        on,                      /* VERB ON seen */
        off                      /* VERB OFF seen */
    } verb;
    enum {                       /* whichever is first is default */
        norply=0,                /* no RPLY seen */
        mail,                    /* send reply as mail (mailret) */
        smtp                     /* send reply as smtp reply */
    } rply;
    int hdrbytes;                /* length of header part of DATA */
    char trusted;                /* file is from a trusted mailer */
    char charlatan;              /* file is from a charlatan */
    char fakefrom;               /* MAIL FROM: isn't right */
    char in_data;                /* in data reading mode */
    char error;                  /* true if an error occurred */
    char pasthdr;                /* are past the header in the DATA */
} S;

static char Verbonmsg[] = "250 Verbose mode ON.";
static char Verboffmsg[] = "250 Verbose mode OFF.";

/*
 * SMTPIN:  Handle a BSMTP request:
 *   0) Look sender up in incoming table and see if a trusted mailer.
 *   1) Initialize the reply file.
 *   2) Initialize the SMTP finite state machine.
 *   3) Loop,  reading each line from the request file and pass
 *      it along to the smtp interpreter.
 *   4) Return the reply file to the requestor.
 */
smtpin()
{
    char rbuf[160];
    int rlen;
    int rep_wrt();

#ifdef TRACE
    wrterm("==> Entered smtpin");
#endif

    cpcmd("CLOSE 00C",9);
    if (cpcmd("READY 00C",9)) return 12;
    rep_init();                      /* init the reply file */
    s_init(rep_wrt);
    while ((rlen = rdcard(rbuf,(int)mailblok.mailsfb.sfbrecsz)) >= 0)
        if (s_input(rbuf, rlen) != 0) break;
    cpcmd("CLOSE 00C",9);
    rep_send();                      /* send the reply file back */
    s_finish();                      /* finish the smtp session */
    return 4;
}

/*
 * check_incoming: See if the file is from a Mailer listed in the
 * incoming table.  First,  see if mail is from self.
 */
static
check_incoming()
{
    struct tbentry *te;
    char key[16];
    struct {
        int *mb;
    } val_plist;

#ifdef TRACE
    wrterm("==> Entered check_incoming");
#endif

    if (mailblok.mailflg1&MFLG1LCL &&
        bcmp(mailblok.mailuser,mailblok.mailvmid,8) == 0)
      return 1;
    bcopy(mailblok.mailnode,key,8);
    bcopy(mailblok.mailuser,&key[8],8);
    if (ctbluk(&te, mailblok.mailvtab, key, 16) == TBMATCH) {
        struct vald *v = (struct vald *)te->tbe_dsp;
        if (v->vaaddr == 0)         /* no user-supplied exit */
            return 1;               /* so trust him now */
        val_plist.mb = &mailblok;
        if (acall(v->vaaddr, 0, &val_plist) == 0)
            return 1;               /* user exit trusts him */
    }

    /* 'mailuser@mailnode' not found. Check for generic id specified */
    /* 'mailuser@*default*'.                                         */

    bcopy("*DEFAULT*",key,8);
    bcopy(mailblok.mailuser,&key[8],8);
    if (ctbluk(&te, mailblok.mailvtab, key, 16) == TBMATCH) {
        struct vald *v = (struct vald *)te->tbe_dsp;
        if (v->vaaddr == 0)         /* no user-supplied exit */
            return 1;               /* so trust him now */
        val_plist.mb = &mailblok;
        if (acall(v->vaaddr, 0, &val_plist) == 0)
            return 1;               /* user exit trusts him */
    }
    return 0;                       /* not in table - don't trust */
}

static struct fscb repfile;
rep_init()
{
    int rc;

#ifdef TRACE
    wrterm("==> Entered rep_init");
#endif

    fserase("smtp.reply.a");
    rc = fsinit(&repfile, "smtp.reply.a");
    repfile.fscbfv = 'V';
    return rc;
}

/*
 * rep_wrt:  Write SMTP reply message into reply log file.  Wrap lines
 *  longer than 80 characters (76 actually: 4 for code and continuation)
 *   nnncmessage text
 *   254-blather blather
 *   254 blather
 *  NOTE: This will do the right thing even if called with a continued
 *  message since it copies the caller-supplied continuation which is
 *  what the final message will contain.
 */
rep_wrt(s,l)
char *s;
int l;
{
    int rc,len,olen, olen2;
    char *cp,obuf[81];

#ifdef TRACE
    wrterm("==> Entered rep_wrt");
#endif

    for (len = l-4, cp = &s[4]; len > 0; len -= olen, cp += olen) {
        olen = (len > 76)?76:len;   /* max(len,76) */
        bcopy(s,obuf,4);            /* copy "nnnc" */
        if (len > 76)
          obuf[3] = '-';            /* continuation */
        bcopy(cp,&obuf[4],olen);    /* copy message */
        olen2 = olen + 4;
        obuf[olen2] = '\0';
        if (mailblok.mailopts & MOPTVL)      /* verbose log */
          wrterm(obuf);
        if (rc = fswrite(&repfile, obuf, &olen2)) {
            wtr("SMT",50,2,"ca","fswrite ","d",rc);
            return;
        }
    }
}

/*
 * rep_send:  Send reply back as MAIL, SMTP or not at all depending
 *  on if an error occurred and VERB and RPLY commands.  Decision is
 *  made like this:
 */
#define DONT 0
#define MAIL 1
#define SMTP 2
#define BOTH (MAIL|SMTP)
static char how[3][4] = {
/* how to      |   verb==none   |   verb!=none  */
/* reply:      | error   !error | error   !error*/
/* ------------+------------------+-------------*/
/* rply==none */ MAIL,    DONT,   SMTP,   SMTP,
/* rply==mail */ MAIL,    DONT,   BOTH,   SMTP,
/* rply==smtp */ SMTP,    DONT,   SMTP,   SMTP
};
/* (for verb!=none, the VERB ON/OFF commands are used to determine
 *  if 050 echoes are sent in the reply.  SMTP replies when verb==
 *  none are sent w/050 msgs as if VERB ON)
 */
static
rep_send()
{
    register i = (int) S.rply;
    register j = ((S.verb==noverb)?0:2) + (S.error?0:1);

#ifdef TRACE
    wrterm("==> Entered rep_send");
#endif

    fsclose(&repfile);
    if (mailblok.mailflg2&(MFLG2NLF|MFLG2NOH)) {
        /* Was an SMTP error (FROM: <>) or no host (FROM: <user>      */
        wtr("SMT",68,0);            /* Note that we would have sent   */
                                    /* error mail had it not been for */
                                    /* the MAIL FROM */
    } else {
        if (how[i][j] & SMTP)
          rep_smtp();               /* send SMTP reply */
        if (how[i][j] & MAIL)
          rep_mail();               /* send MAIL reply */
    }
    fserase("smtp.reply.a");
}

static
rep_smtp()
{
    char buf[100];
    int buflen;
    int rc, rc2;
    int verbmode = 0;

#ifdef TRACE
    wrterm("==> Entered rep_smtp");
#endif

    /* point punch at sender */
    bcopy("SPOOL 00D COPY 1 NOCONT CLASS M TO ",buf, buflen = 35);
    if (mailblok.mailflg1&MFLG1LCL) {
        bcopy(mailblok.mailuser, &buf[buflen], 8);
        buflen += 8;
        cpcmd(buf, buflen);
        cpcmd("TAG DEV 00D",11);
    } else {
        bcopy(mailblok.rscsname, &buf[buflen], 8);
        buflen += 8;
        cpcmd(buf, buflen);
        bcopy("TAG DEV 00D ", buf, buflen=12);
        bcopy(mailblok.mailnode, &buf[buflen], 8);
        buflen += 8;
        buf[buflen++] = ' ';
        bcopy(mailblok.mailuser, &buf[buflen], 8);
        buflen += 8;
        bcopy(" 50 W=", &buf[buflen], 6);
        buflen += 6;
        bcopy(mailblok.mailuser, &buf[buflen], 8);
        buflen += 8;
        cpcmd(buf, buflen);
    }

    /* copy repfile to punch, sending 050's depending on VERB */
    fsinit(&repfile,"smtp.reply.a");
    repfile.fscbfv = 'V';
    do {
        buflen = sizeof(buf);
        switch (rc = fsread(&repfile, buf, &buflen)) {
        case 0:
            if (S.verb != noverb) {
                if (!bcmp(buf,Verbonmsg,sizeof(Verbonmsg)-1))
                  verbmode = 1;
                else if (!bcmp(buf,Verboffmsg,sizeof(Verboffmsg)-1))
                  verbmode = 0;
            } else /* verb == none; must be rply == smtp */
              verbmode = 1;
            if (!verbmode && !bcmp(buf,"050",3))
              break;          /* VERB OFF and an 050 msg */
            punch(buf,buflen);
            break;
        case 12:           /* EOF */
            fsclose(&repfile);
            break;
        default:
            wtr("SMT",50,2,"ca","fsread#1","d",rc);
            break;
        }
    } while (rc == 0);
    cpcmd("CLOSE 00D NAME BSMTP REPLY",26);
    wtr("SMT",56,2,"ca",mailblok.mailnode,"ca",mailblok.mailuser);
}

/*
 * return the error mail to the MAIL FROM address if a valid one
 * was parsed.  Make sure that valid MAIL FROM address is not a
 * mailer (self or other) to prevent loops.  If no valid MAIL FROM,
 * only send the error mail to the spool file origin if it was NOT
 * a trusted mailer.
 */
static
rep_mail()
{
    struct fscb *fscbptr = (S.disk_error != diskother) ? &S.data_file
                                                       : 0;

#ifdef TRACE
    wrterm("==> Entered rep_mail");
#endif

    if (mailblok.mailsfld) {  /* there was a valid MAIL FROM */
        if (!is_mailer(mailblok.mailsfld))
         err_mail(mailblok.mailsfld->fistr+1,
         (mailblok.mailsfld->fiend-mailblok.mailsfld->fistr-2),fscbptr);
        else
          err_loop(fscbptr);
    } else if (!S.trusted) {    /* NOT a trusted mailer so safe to */
        char buf[20];         /* to send reply mail to rscs origin */
        int bl;

        bcopy(mailblok.mailuser,buf,bl = mailblok.mailulen);
        buf[bl++] = '@';
        bcopy(mailblok.mailnode,&buf[bl],mailblok.mailnlen);
        bl += mailblok.mailnlen;
        err_mail(buf,bl,fscbptr);
    } else                    /* From a trusted mailer (shouldn't */
      err_loop(fscbptr);  /* happen -- haha) */
}


/*
 * smtp server
 */

/* forward declarations for functions in the table */
s_state s_helo(), s_mail(), s_rcpt(), s_data(), s_rset(),
        s_noop(), s_quit(), s_verb(), s_tick(), s_read(), s_rply(),
        s_oseq(), s_badc(), s_errr(), s_nosp(), s_getdata(), s_syntax(),
        s_syntax553(), s_maxr(), parse_data_hdr();
/*
 * command table:  Contains command name and ptr to function to
 * call for each command (given current server state). When called,
 * the function has 3 args:  func(state, s, l);  State is the server
 * state at the time of call, s and l are the ptr to and length of
 * the command string.  The function returns the new state.
 */
static struct {                 /* command table */
    char cmd[5];                /* command name */
    s_state (*func[NSTATE])();  /* ptr to function for each state */
} cmdtable[] = {
/* command   initial  seenhelo seenmail error   */
    "HELO",  s_helo , s_oseq , s_oseq , s_errr ,
    "MAIL",  s_oseq , s_mail , s_oseq , s_errr ,
    "RCPT",  s_oseq , s_oseq , s_rcpt , s_errr ,
    "DATA",  s_data , s_data , s_data , s_data ,
    "RSET",  s_rset , s_rset , s_rset , s_rset ,
    "NOOP",  s_noop , s_noop , s_noop , s_noop ,
    "QUIT",  s_quit , s_oseq , s_oseq , s_quit ,
    "VERB",  s_verb , s_verb , s_verb , s_verb ,
    "TICK",  s_tick , s_tick , s_tick , s_tick ,
    "RPLY",  s_rply , s_rply , s_rply , s_rply ,
    ":REA",  s_read , s_read , s_read , s_read ,
};
#define CMDTSIZE (sizeof(cmdtable)/sizeof(*cmdtable))

/*
 * s_init - initialize the SMTP server
 *   has 1 arg: ptr to function which replies are written to.
 *   that function is called as func(s,l) where s is the pointer
 *   to the reply text and l is the length.
 */
s_init(func)
int (*func)();                    /* function to write replies to */
{
    char buf[200];
    int len;

#ifdef TRACE
    wrterm("==> Entered s_init");
#endif

    mailblok.mailflg2 |= MFLG2SMT;
    mailblok.mailflg2 |= MFLG2NOR;   /* we build our own received */
    wtr("SMT",54,0);
    if (s_init0(func)) return 1;
    S.verb = 0;                      /* 'cuz s_init0 preserves it */
    S.trusted = check_incoming();

    bcopy("220 ",buf,len = 4);
    len += mydomain(&buf[len], sizeof(buf)-len);
    bcopy(" Columbia MAILER ", &buf[len], 17); len += 17;
    bcopy(mailblok.mailvers,&buf[len],8); len += 8;
    while (buf[len-1] == ' ') --len; /* trim blanks from version */
    bcopy(" BSMTP service ready.", &buf[len], 21); len += 21;
    (*func)(buf, len);
    return 0;
}

s_init0(func)                       /* common init code */
int (*func)();
{
    int vrb = S.verb;

#ifdef TRACE
    wrterm("==> Entered s_init0");
#endif

    bzero(&S, sizeof(S));          /* zero the whole mess */
    S.repf = func;
    S.state = initial;
    S.verb = vrb;
    mailblok.mailchn = 0;          /* zero the mailchn ptr */
    fserase("smtp.data");
    if (fsinit(&S.data_file, "smtp.data") != 0) {
        S.state = s_nosp(error, 0, 0);
        return 1;
    }
    S.data_file.fscbfv = 'V';
    return 0;
}

/*
 * mydomain - build local domain name string.
 */
static
mydomain(bp, bl)
char *bp;                       /* where to write the result */
int bl;                         /* limit on result length */
{
    u_char *lcldp;
    int  lcldl;

#ifdef TRACE
    wrterm("==> Entered mydomain");
#endif

    if (mailblok.lclhost[0] == (char *)0) { /*user didn't provide one*/
        bcopy(mailblok.cpustr,bp,lcldl = mailblok.cpuslen);
        if (mailblok.rscsdom[0] != 0) {
            bcopy(&mailblok.rscsdom[0][1],&bp[lcldl],
                  (int)mailblok.rscsdom[0][0]);
            lcldl += mailblok.rscsdom[0][0];
        }
    } else {
        lcldp = &mailblok.lclhost[0][1];  /* pointer to lcl host name */
        lcldl = mailblok.lclhost[0][0];  /* 1st byte is string length */
        bcopy(lcldp, bp, lcldl);
    }
    return lcldl;
}

/*
 * s_input:  Called once per line of data obtained from the SMTP
 *  connection.  Two args:  s is pointer to the line and l is length.
 *  Parses the line and invokes the appropriate command processing
 *  function.
 * NOTE: should it immediately quit when error state is entered?
 */
s_input(s,l)
char *s;
int l;
{
    char buf[200];
    register int i;
    int rlen;
#ifdef DEBUG_S_INPUT
    char dbgmsg[133];

#endif
#ifdef TRACE
    wrterm("==> Entered s_input");
#endif

    if (S.state == error)     /* remember errors across state */
      S.error = 1;            /* transitions. */

    /* if S.in_data flag is on,  then we are reading mail data instead
       of commands */
    if (S.in_data) {
        S.state = s_getdata(S.state, s,l);  /* get the data line */
        return 0;
    }

    /* Check if BSMTP line is continued on second record.
     * If continuation exists, read in next record and update total
     * length
     */
    if (s[79] == 0x15) {
#ifdef DEBUG_S_INPUT
       wrterm("==> BSMTP: Continue line being processed");
#endif
       if ((rlen = rdcard(&s[l],(int)mailblok.mailsfb.sfbrecsz)) >= 0)
          l = l+rlen;
#ifdef DEBUG_S_INPUT
       sprintf(dbgmsg, "==> BSMTP: Continued line - new length is %d" ,
               l);
       wrterm(dbgmsg);
#endif
    }

#ifdef DEBUG_S_INPUT
    bcopy("==> BSMTP:>", dbgmsg , 11);
    bcopy(s, &dbgmsg[11], l);
    bcopy("<", &dbgmsg[11+l], 2);
    wrterm(dbgmsg);
#endif

    /* always echo the command */
    bcopy("050 ",buf,4);      /* stuff in the echo code */
    if (l > sizeof(buf)-4) l = sizeof(buf) - 4;
    bcopy(s, &buf[4], l);
    (*S.repf)(buf,l+4);       /* write the echo */

    /* if in the final state,  no more commands are allowed */
    if (S.state == final)
        return 4;             /* just ignore any extra junk */

    /* now look the command name up in the command table */
    for (i = 0; i < CMDTSIZE; i++)
        if (xcmp(s, cmdtable[i].cmd, 4) == 0) {  /* match */
            S.state = (*cmdtable[i].func[S.state])(S.state, s, l);
            return 0;
        }
    S.state = s_badc(S.state,s,l);  /* unrecognized cmd */
    return 0;
}

/*
 * s_finish:  Finish up the SMTP work.  Called when transmission ends.
 *  The transmission may possibly be incomplete.
 */
s_finish()
{

#ifdef TRACE
    wrterm("==> Entered s_finish");
#endif

    mailblok.mailflg2 &= ~MFLG2SMT;
    mailblok.mailflg2 &= ~MFLG2NOR;           /* restore the flag */
    mailblok.mailflg2 &= ~MFLG2NLF;
    mailblok.mailflg2 &= ~MFLG2NOH;
    wtr("SMT",55,0);
}

static s_state
s_helo(st, s, l)
s_state st;
char *s;
int l;
{
    char *p, buf[200];
    int len, buflen;

#ifdef TRACE
    wrterm("==> Entered s_helo");
#endif

    for (p = &s[5], l -= 5; *p != ' ' && l > 0; p++, l--);
    if (!(S.domain = (char *)salloc(len = p - &s[5])))
        return s_nosp(st, s, l);
    bcopy(&s[5], S.domain, S.dl = len);  /* save the domain for later */

    /* if not from a trusted mailer, domain must be "nice" */
    if (!S.trusted && !check_domain(S.domain, len))
      if (mailblok.mailopts & MOPTAC) { /* allow charlatans */
          bcopy("250 ", buf, 4);
          buflen = 4 + mydomain(&buf[4], sizeof(buf)-4);
          bcopy(" You are a charlatan; ",&buf[buflen],22); buflen+=22;
          bcopy(S.domain, &buf[buflen], len); buflen += len;
          bcopy(" is bogus for a file with RSCS origin ",
                &buf[buflen],38); buflen += 38;
          bcopy(mailblok.mailnode,&buf[buflen],8); buflen += 8;
          buf[buflen++] = ' ';
          bcopy(mailblok.mailuser,&buf[buflen],8); buflen += 8;
          (*S.repf)(buf, buflen);
          S.charlatan = 1;
          return seenhelo;
      } else {                 /* don't allow charlatans */
          bcopy("501 ", buf, 4);
          bcopy(S.domain, &buf[4], len);
          buflen = 4+len;
          bcopy(" is bogus for a file with RSCS origin ",
                &buf[buflen],38);
          buflen += 38;
          bcopy(mailblok.mailnode,&buf[buflen],8);
          buflen += 8;
          buf[buflen++] = ' ';
          bcopy(mailblok.mailuser,&buf[buflen],8);
          buflen += 8;
          (*S.repf)(buf, buflen);
          return error;
      }
    bcopy("250 ", buf, 4);
    buflen = 4 + mydomain(&buf[4], sizeof(buf)-4);
    bcopy(" Hello ", &buf[buflen], 7);
    buflen += 7;
    bcopy(S.domain, &buf[buflen], len);
    buflen += len;
    (*S.repf)(buf, buflen);
    return seenhelo;
}

/*
 * check if a domain name matches the local domain
 *  Do the following tests until one succeeds:
 *  1) Compare the domain to MAILNODE.
 *  2) Compare the domain to MAILNODE concatenated w/ RSCS-Domains.
 *  3) See if domain matches a Local-Host (LCLHOST*).
 */
static
check_local(s,l)
char *s;
int l;
{
    int ri;
    struct tbentry *te;
#ifdef DEBUG_CHECK_LOCAL
    char dbgmsg[133];
#endif

#ifdef TRACE
    wrterm("==> Entered check_local");
#endif

    /* (1) Compare domain to mailnode */
    if (mailblok.mailnlen == l && xcmp(mailblok.mailnode,s,l) == 0)
        return 1;
#ifdef DEBUG_CHECK_LOCAL
    else {
        bcopy("==> No match: <", dbgmsg , 15);
        bcopy(mailblok.mailnode, &dbgmsg[15], mailblok.mailnlen);
        bcopy(">", &dbgmsg[15+mailblok.mailnlen], 2);
        wrterm(dbgmsg);
    }
#endif

    /* (2) Compare domain concatenated w/RSCS-domains to mailnode */
    if (mailblok.rscsdom[0] != 0) {
        for (ri= 0; mailblok.rscsdom[ri] != 0 &&
              ri < MAXRSCSDOM; ri++) {
           int rl = mailblok.rscsdom[ri][0];
           char *rp = &mailblok.rscsdom[ri][1];
           int i;

           if (rl < l && xcmp(rp, &s[i = l-rl], rl) == 0)
               if (mailblok.mailnlen == i &&
                   xcmp(s, mailblok.mailnode, i) == 0)
                   return 1;
#ifdef DEBUG_CHECK_LOCAL
           BCOPY("==> No match: <", dbgmsg , 15);
           bcopy(rp ,&dbgmsg[15], rl);
           bcopy(">", &dbgmsg[15+rl], 2);
           wrterm(dbgmsg);
#endif
        }
    }

    /* (3) See if Local-Domain matches */
    if (mailblok.lclhost[0] != 0)
        for (ri = 0; mailblok.lclhost[ri] != 0 && ri < MAXLCLHOST; ri++)
        if (*mailblok.lclhost[ri] == l &&
             xcmp(&mailblok.lclhost[ri][1], s, l) == 0)
            return 1;
#ifdef DEBUG_CHECK_LOCAL
        else {
            bcopy("==> No match: <", dbgmsg , 15);
            bcopy(rp ,&dbgmsg[15], rl);
            bcopy(">", &dbgmsg[15+rl], 2);
            wrterm(dbgmsg);
        }
#endif

    /* time to give up */
    return 0;
}

/*
 * check that domain name matches the RSCS origin
 *  Do the following tests until one succeeds:
 *  1) Compare the domain to MAILNODE.
 *  2) Compare the domain to MAILNODE concatenated w/ RSCS-Domains.
 *  3) If file is local, see if domain matches a Local-Host (LCLHOST*).
 *  4) Look for the domain in MAILRTAB and compare the RTROUTE
 *     to MAILNODE.
 *  5) Do the same for MAILDTAB.
 */
static
check_domain(s,l)
char *s;
int l;
{
    int ri;
    struct tbentry *te;

#ifdef TRACE
    wrterm("==> Entered check_domain");
#endif

    /* (1) Compare domain to mailnode */
    if (mailblok.mailnlen == l && xcmp(mailblok.mailnode,s,l) == 0)
        return 1;

    /* (2) Compare domain concatenated w/RSCS-domains to mailnode */
    if (mailblok.rscsdom[0] != 0) {
        for (ri= 0; mailblok.rscsdom[ri] != 0 &&
              ri < MAXRSCSDOM; ri++) {
           int rl = mailblok.rscsdom[ri][0];
           char *rp = &mailblok.rscsdom[ri][1];
           int i;

           if (rl < l && xcmp(rp, &s[i = l-rl], rl) == 0)
               if (mailblok.mailnlen == i &&
                   xcmp(s, mailblok.mailnode, i) == 0)
                   return 1;
        }
    }

    /* (3) If file is from local user,  see if Local-Domain matches */
    if (mailblok.mailflg1&MFLG1LCL && mailblok.lclhost[0] != 0)
        for (ri = 0; mailblok.lclhost[ri] != 0 && ri < MAXLCLHOST; ri++)
        if (*mailblok.lclhost[ri] == l &&
             xcmp(&mailblok.lclhost[ri][1], s, l) == 0)
            return 1;

    /* (4) Look in MAILRTAB and compare RTROUTE to MAILNODE */
    if (ctbluk(&te, mailblok.mailrtab, s, l) == TBMATCH &&
        bcmp(mailblok.mailnode,
             ((struct routd *)te->tbe_dsp)->rtroute, 8) == 0)
        return 1;

    /* (5) Look in MAILDTAB and compare RTROUTE to MAILNODE */
    if (ctbluk(&te, mailblok.maildtab, s, l) == TBMATCH &&
        bcmp(mailblok.mailnode,
             ((struct routd *)te->tbe_dsp)->rtroute, 8) == 0)
        return 1;

    /* time to give up */
    return 0;
}

/*
 * s_mail: Parse the from: address with APARSE, building a FIELDD
 * on the stack.  Stuff it all into MAILSFLD.  Also add it to MAILKTAB
 * -- this allows later code to find the FROM: field via TBLUK.
 * Also,  call check_sender() to make sure it's not a forgery.
 */
static s_state
s_mail(st, s, l)
s_state st;
char *s;
int l;
{
    struct fieldd *f;
    struct tokend *t;
    char buf[200]; int len;
    struct tbentry *te;
    struct tbs *ts;
    struct mbox *a;

#ifdef TRACE
    wrterm("==> Entered s_state");
#endif

    mailblok.mailsfld = 0;
    if (xcmp(" FROM:", &s[4], 6) != 0)     /* check syntax */
        return s_syntax(st, s, l);

    /* allocate space from stack */
    f = (struct fieldd *)salloc(sizeof(*f));
    t = (struct tokend *)salloc(sizeof(*t));
    ts = (struct tbs *)salloc(l-5+sizeof(*ts)-1);
    if (!f || !t || !ts)
        return s_nosp(st, s, l);

    /*
     * copy "FROM:<......>" onto stack preceded by tbs header.
     * then set up the tokend pointing to it too.
     */
    bcopy(&s[5], &ts->tbs_str, l-5);
    ts->tbs_siz = 4;                    /* length of "FROM" */
    t->tokptr = &ts->tbs_str;           /* point tokend at it */
    t->toktype = TOKATM;                /* FROM: is an atom */
    t->toklen = 5;                      /* length of FROM: */
    te = &S.from_tbe;                   /* where we put it for now */
    te->tbe_str = ts;                   /* point at tbs */
    te->tbe_dsp = f;                    /* disp. value is &fieldd */

    /* now set up the fieldd */
    f->fitok = t;
    f->fistr = t->tokptr+5;             /* point past the FROM: */
    f->fiend = f->fistr + l - 10;       /* end of the string */
    while (*(f->fistr) == ' ' && f->fistr <= f->fiend)
      ++(f->fistr);                     /* skip intervening blanks */

    if (f->fistr[0] == '<') {           /* look for MAIL FROM:<> */
        register int i = 1;
        while ((f->fistr[i] == ' ') && (&f->fistr[i] < f->fiend))
          i++;                          /* trim white space */
        if (f->fistr[i] == '>')         /* MAIL FROM:<> */
          if (!do_null_from(f))         /* dummy up the MAIL FROM */
            return s_nosp(st, s, l);    /* no space left */
    }

    if (_aparse(f) != 0)                /* parse the string */
        return s_syntax(st, s, l);
    mailblok.mailsfld = f;              /* set mailblok sender ptr */

    if (((a = (struct mbox *)f->fipars) !=0) &&
         (a->mbxdcnt == 0))
       mailblok.mailflg2 |= MFLG2NOH;    /* Remember no host id */

    /* check to see if this is a forged from: userid */
    if (!S.trusted) {   /* not a trusted mailer */
        if (!check_sender(f->fipars)) {
            bcopy("501 ", buf, 4);
            bcopy(s+10,&buf[4], l-10);
            len = 4 + l-10;
            bcopy(" does not correspond to RSCS origin of ",
            &buf[len],39);
            len += 39;
            bcopy(mailblok.mailnode, &buf[len], 8);  len += 8;
            buf[len++] = ' ';
            bcopy(mailblok.mailuser, &buf[len], 8);  len += 8;
            if (mailblok.mailopts & MOPTAC) {    /* allow charlatans */
                bcopy("250 ", buf, 4);      /* rewrite reply code */
                while (buf[len-1] == ' ') --len;
                bcopy(". You are a charlatan.",&buf[len],22); len += 22;
                S.fakefrom = 1;
                (*S.repf)(buf, len);
                return seenmail;        /* xxx Add a delivery-notice? */
            } else {                        /* dissallow charlatans */
                (*S.repf)(buf, len);
                mailblok.mailsfld = 0;
                return error;
            }
            /* NOTREACHED */
        } else {  /* not trusted, but sender checks out ok */
            bcopy("250 ", buf, 4);
            bcopy(s+10, &buf[4], l-10);     /* copy in from address */
            len = 4 + l-10;
            bcopy("... sender OK.", &buf[len], 14);
            (*S.repf)(buf, len+14);
            return seenmail;
        }
    } else {                         /* a trusted mailer */
        bcopy("250 ", buf, 4);
        bcopy(s+10, &buf[4], l-10);     /* copy in from address */
        len = 4 + l-10;
        bcopy("... sender OK.", &buf[len], 14);
        (*S.repf)(buf, len+14);
        return seenmail;
    }
    /* NOTREACHED */
}

/*
 * do_null_from: Handle MAIL FROM:<>.
 *
 * Dummies up a sender fieldd so later code that needs it has a valid
 * sender (e.g. exits that want to send a message saying "you have new
 * mail from ...") but also sets a flag indicating that a error loops
 * are to be avoided.
 */
static int
do_null_from(f)
struct fieldd *f;
{
    char *dummied;
    int dl = 0;

#ifdef TRACE
    wrterm("==> Entered do_null_from");
#endif

    wtr("SMT",67,2,"ca",mailblok.mailnode,"ca",mailblok.mailuser);
    mailblok.mailflg2 |= MFLG2NLF;
    dummied = (char *)salloc(20);   /* <mailuser@mailnode>\0 */
    if (!dummied)
      return 0;
    dummied[dl++] = '<';
    bcopy(mailblok.mailuser,&dummied[dl],mailblok.mailulen);
    dl += mailblok.mailulen;
    dummied[dl++] = '@';
    bcopy(mailblok.mailnode,&dummied[dl],mailblok.mailnlen);
    dl += mailblok.mailnlen;
    dummied[dl++] = '>';
    dummied[dl] = '\0';
    f->fistr = dummied;
    f->fiend = &dummied[dl];
    return 1;
}

extern genlist();
/*
 * Check_sender - check to see if sender address matches RSCS origin.
 *  (1) Compare sender userid to MAILUSER
 *  (2) Check sender domain name with check_domain()
 */
static
check_sender(a)
struct mbox *a;
{
    int len;
    char *ptr;
    struct node *n;
    char *hs; int hl;
    char hoststring[256]; int hostlen;
    char *hoststringptr;
    struct {
      int *node;
      int *hs;
      int *hl;
      int *hoststringptr;
      int *hostlen;
    } plist;
    int i;

#ifdef TRACE
    wrterm("==> Entered check_sender");
#endif

    if (mailblok.mailflg2&MFLG2NLF) /* always fail */
      return 0;

    /* (1) See if sender userid matches MAILUSER */

    if (a->mbxrtl != 0) {
        n = MBOXRTL(a)->noddown;
        plist.node = &n;
        plist.hs = &hs;
        plist.hl = &hl;
        for (i=0; i < sizeof(hoststring); hoststring[i] = ' ',i++);
        hoststringptr = &hoststring[0];
        plist.hoststringptr = &hoststringptr;
        hostlen = sizeof(hoststring);
        plist.hostlen = &hostlen;
        acall(genlist, 0, &plist);
        if (hl > 8)
            hl = 8;
        if (hl > mailblok.mailulen ||
            xcmp(hs, mailblok.mailuser, hl) != 0)
            return 0;
    }

    /* (2) See if domain checks out */
    return check_domain(TOKPTR(MBOXDML(a)->noddown),
                        MBOXDML(a)->noddown->toklen);
}

/*
 * s_rcpt: Parse the recipient address,  look it up in MAILRTAB,
 *  MAILDTAB, etc.  Add to ACTADR chain depending on exit type
 *  from ROUTD.  This replicates a lot of the work done in
 *  BUILDLST,  but it is easier to do again here than try to integrate
 *  the two.  This also allows immediate feedback on bad rcpt's that
 *  can be sent as the SMTP reply.
 */
extern delroute();
static s_state
s_rcpt(st, s, l)
s_state st;
char *s;
int l;
{
    char buf[200];
    int buflen;
    struct fieldd *f;
    struct tokend *t;
    struct tokend  *faketoke;
    struct tbs *ts;
    struct mbox *a;
    struct routd *r;
    struct tbentry *te;
    char *hs; int hl;
    int rlen;
    char hoststring[256]; int hostlen;
    char *hoststringptr;
    struct node *n;
    struct node *fakenode;
    struct {
      int *node;
      int *hs;
      int *hl;
      int *hoststringptr;
      int *hostlen;
    } plist;
    struct {
      int *delmbox;
    } plist2;
    int i;
#ifdef DEBUG_S_RCPT
    char dbgmsg[133];
#endif

#ifdef TRACE
    wrterm("==> Entered s_rcpt");
#endif

    if (xcmp(" TO:", &s[4], 4) != 0)    /* check syntax */
        return s_syntax(st, s, l);

    /* allocate space from stack */
    f = (struct fieldd *)salloc(sizeof(*f));
    if (!f)
        return s_nosp(st, s, l);

    /*
     * on the first pass,  do a little extra work:
     *  1) Set up the initial fieldd (all subsequent ones are extents)
     *  2) Set up initial tbs and tokend and MAILKTAB entry.
     */
    if (S.rcpts == 0) {
        t = (struct tokend *)salloc(sizeof(*t));
        ts = (struct tbs *)salloc(sizeof(struct tokend)+l-5-1);
        if (!ts || !t)
            return s_nosp(st,s,l);

        /* copy TO:<...> onto stack preceded by tbs header */
        bcopy(&s[5], &ts->tbs_str, l-5);
        ts->tbs_siz = 2;
        t->tokptr = &ts->tbs_str;
        t->toktype = TOKATM;
        t->toklen = 3;
        te = &S.rcpt_tbe;            /* put it here for now */
        te->tbe_str = ts;
        te->tbe_dsp = f;

        /* set up initial fieldd */
        S.first_rcpt = f;
        f->fitok = t;
        f->fistr = t->tokptr+3;
        f->fiend = f->fistr + l - 8;
    } else /* rcpts > 0 */ {
        /* allocate stack space */
        if (!(f->fistr = (char *)salloc(l - 8)))
            return s_nosp(st,s,l);
        /* copy <.....> onto stack (don't need to TO: or tbs header) */
        bcopy(&s[8], f->fistr, l - 8);

        /* set up extn fieldd.  Insert into chain.  Use same tokend */
        f->fiend = f->fistr + l - 8;
        f->fitok = S.first_rcpt->fitok;
        f->fiextn = S.first_rcpt->fiextn; S.first_rcpt->fiextn = f;
    }

    /* Now parse it, look up in various tables and insert in MAILCHN */

    if (_aparse(f) != 0)
        return s_syntax553(st, s, l);

    a = (struct mbox *)f->fipars;

    /* Check for routeaddr and if it is us, drop it before any other
     * processing occurs.                                            */

    while (a->mbxrtl != 0) {
#ifdef DEBUG_S_RCPT
       wrterm("==> s_rcpt: Processing routeaddr");
#endif
       n = MBOXRTL(a)->noddown;
       plist.node = &n;
       plist.hs = &hs;
       plist.hl = &hl;
       for (i=0; i < sizeof(hoststring); hoststring[i] = ' ',i++);
       hoststringptr = &hoststring[0];
       plist.hoststringptr = &hoststringptr;
       hostlen = sizeof(hoststring);
       plist.hostlen = &hostlen;
       acall(genlist, 0, &plist);
#ifdef DEBUG_S_RCPT
       bcopy("==> s_rcpt: <", dbgmsg, 13);
       bcopy(hs,&dbgmsg[13],hl);
       bcopy(">",&dbgmsg[hl+13],2);
       wrterm(dbgmsg);
#endif
       if (check_local(hs,hl)) {
#ifdef DEBUG_S_RCPT
          wrterm("==> s_rcpt: Removing route address");
#endif
          plist2.delmbox = &a;
          acall(delroute, 0, &plist2);        /* Remove route addr */
          f->fiflg1 |= FIEDIFLG;              /* Remember changed  */
       }
       else
          break;
    }

    /* If address was just 'userid' fix it to be 'userid@local-host' */

    if (a->mbxdml == 0) {
#ifdef DEBUG_S_RCPT
       wrterm("==> s_rcpt: Adding @local-host to userid");
#endif
       fakenode = (struct node *)salloc(sizeof(*fakenode));
       if (!fakenode) return s_nosp(st,s,l);
       faketoke = (struct tokend*)salloc(sizeof(*faketoke));
       if (!faketoke) return s_nosp(st,s,l);
       a->mbxdml = fakenode;
       fakenode->noddown = faketoke;
       faketoke->tokptr = &mailblok.cpustr[0];
       faketoke->toklen = mailblok.cpuslen;
       faketoke->toktype = TOKATM;
    }

    /* (2) Look up the host name in MAILRTAB, MAILDTAB */

    if (a->mbxrtl == 0) {
#ifdef DEBUG_S_RCPT
       wrterm("==> s_rcpt: Using domain for lookup");
#endif
       n = a->mbxdml;
    }
    else {
#ifdef DEBUG_S_RCPT
       wrterm("==> s_rcpt: Using route-addr for lookup");
#endif
       n = MBOXRTL(a)->noddown;
    }
    plist.node = &n;
    plist.hs = &hs;
    plist.hl = &hl;
    for (i=0; i < sizeof(hoststring); hoststring[i] = ' ',i++);
    hoststringptr = &hoststring[0];
    plist.hoststringptr = &hoststringptr;
    hostlen = sizeof(hoststring);
    plist.hostlen = &hostlen;
    acall(genlist, 0, &plist);
#ifdef DEBUG_S_RCPT
    bcopy("==> s_rcpt routing with: <", dbgmsg, 26);
    bcopy(hs,&dbgmsg[26],hl);
    bcopy(">",&dbgmsg[hl+26],2);
    wrterm(dbgmsg);
#endif
    if ((r = find_routd(hs, hl)) == 0) {
        (*S.repf)("550 Mailbox not found.", 22);
        return st;
    }

    /* add to mailchn */
    if (add_mailchn(f,r)) {
        /* now that everything has worked,  add to fieldd extension */
        ++S.rcpts;
        bcopy("250 ", buf, buflen=4);
        bcopy(f->fistr, &buf[buflen], f->fiend-f->fistr);
        buflen += f->fiend - f->fistr;
        bcopy("... recipient OK.", &buf[buflen], 17);
        buflen += 17;
        (*S.repf)(buf, buflen);
        return st;
    } else
        return s_nosp(st, s, l);
}

/*
 * add_mailchn:  Add routd and fieldd to mailchn (with an actadr block)
 *  Depending on the exit type,  the addition is done differently:
 *  RTPROG type 1: Always create and add an ACTADR
 *  2: Add a new one only if one for the same node doesn't exist
 *  3: Add one for each using the ACTCHN pointer.
 */
static
add_mailchn(f, r)
struct fieldd *f;
struct routd *r;
{
    struct actadr *a, *b;

#ifdef TRACE
    wrterm("==> Entered add_mailchn");
#endif

    switch (r->rtflags&(RTP1|RTP2|RTP3)) {
    case RTP1:
        if (!(a = (struct actadr *)salloc(sizeof(*a))))
            return 0;
        a->actnext = mailblok.mailchn; /* insert in front of chain */
        mailblok.mailchn = a;
        a->actmbox = f->fipars;
        a->actrt = r;
        return 1;
    case RTP2:
        /* search for a matching rtroute/rtuser/rtprog/rtparm */
        for (a = mailblok.mailchn; a != 0; a = a->actnext)
            if (bcmp(a->actrt->rtroute, r->rtroute, 8) == 0
                && bcmp(a->actrt->rtuser, r->rtuser, 8) == 0
                && bcmp(a->actrt->rtprog, r->rtprog, 8) == 0
                && bcmp(a->actrt->rtparm, r->rtparm, 8) == 0)
              return 1;         /* already one there */
        /* none there yet;  insert this one */
        if (!(a = (struct actadr *)salloc(sizeof(*a))))
            return 0;
        a->actnext = mailblok.mailchn;
        mailblok.mailchn = a;
        a->actmbox = f->fipars;
        a->actrt = r;
        return 1;
    case RTP3:
        /* search for matching rtroute/rtuser/rtprog */
        for (a = mailblok.mailchn; a != 0; a = a->actnext)
            if (bcmp(a->actrt->rtroute, r->rtroute, 8) == 0
                && bcmp(a->actrt->rtuser, r->rtuser, 8) == 0
                && bcmp(a->actrt->rtprog, r->rtprog, 8) == 0) {
                if (!(b = (struct actadr *)salloc(sizeof(*b))))
                    return 0;
                b->actchn = a->actchn;
                a->actchn = b;
                b->actmbox = f->fipars;
                b->actrt = r;
                return 1;
            }
        /* didn't find it so create a new one */
        if (!(a = (struct actadr *)salloc(sizeof(*a))))
            return 0;
        a->actnext = mailblok.mailchn;
        mailblok.mailchn = a;
        a->actmbox = f->fipars;
        a->actrt = r;
        return 1;

    default:
        wrterm("screw up in add_mailchn.");
        return 0;
    } /* end switch */
    /* NOTREACHED */
}

static s_state
s_data(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_data");
#endif

    mailblok.mailhrec = mailblok.mailhlin = 0;
    add_received();                /* add received: stamp to data */
    (*S.repf)("354 Start mail input.  End with <crlf>.<crlf>", 45);
    S.in_data = 1;
    return st;
}

/*
 * s_getdata:  Called repeatedly as long as S.in_data is true.
 *  Sets S.in_data false when the end of data signal is seen (".")
 *  When end of data is seen,  actually cause the mail to be
 *  delivered by calling sendlst().
 *  This routine is called even when a DATA command is out of sequence
 *  so that the data may be eaten up instead of trying to interpret
 *  it as commands.
 */
extern sendlst(), mailret();
static s_state
s_getdata(st,s,l)
char *s;
int l;
{
    struct actadr *a, *b;
    char buf[200];
    struct {
        int *mb;
    } plist;
#ifdef DEBUG_S_GETDATA
    char dbgmsg[133];
#endif

#ifdef TRACE
    wrterm("==> Entered s_getdata");
#endif

    if (l == 0) S.pasthdr = 1;     /* end of header portion of data */
    if (l == 1 && *s == '.') {     /* end of data */
        S.in_data = 0;
        fsclose(&S.data_file);
        switch (S.disk_error) {
        case disknone:
            break;
        case diskfull:
            (*S.repf)("552 DATA aborted:  out of disk space.",37);
            return error;
        case diskother:
            (*S.repf)("451 DATA aborted: CMS filesystem error.", 39);
            return error;
        }
        if (st == seenmail) {
            /* now send the stuff in MAILCHN off */
            if (st = parse_data_hdr(st,s,l) == error)
              return st;
            if (mailblok.maxrcvd && count_received() > mailblok.maxrcvd)
              return s_maxr(st, s, l);
            wtr("SMT",43,1,"d",S.rcpts);
            plist.mb = &mailblok;
            mailblok.mailflg1 |= MFLG1HDS;  /* header scanned ok */
#ifdef DEBUG_S_GETDATA
            wrterm("==> s_getdata: Calling sendlst");
#endif
            switch (acall(sendlst,0,&plist)) {
            case 0:
                (*S.repf)("250 Mail accepted for delivery.", 31);
                break;
            case 12:
#ifdef DEBUG_S_GETDATA
                wrterm("==> s_getdata: RC=12 from sendlst");
#endif
                return s_nosp(st, s, l);    /* croak */
            default:
#ifdef DEBUG_S_GETDATA
                wrterm("==> s_getdata: Errors from sendlst");
#endif
                log_errors();               /* copy into bsmtp.reply */
                return error;
            }
        } else {
           (*S.repf)("451 Request aborted: command out of sequence",44);
           return error;
        }
        /* now get back to initial state in case there's more work */
        if (s_init0(S.repf))
            return s_nosp(st, s, l);
        else
            return initial;
    } else {                       /* a regular data line */
        int rc;

#ifdef DEBUG_S_GETDATA
        bcopy("==> s_getdata: >", dbgmsg, 16);
        bcopy(s, &dbgmsg[16], l);
        bcopy("<", &dbgmsg[16+l], 2);
        wrterm(dbgmsg);
#endif
        if (S.disk_error != disknone) return st;
        if (*s == '.') ++s, --l;   /* de-double the dot */
        else if (l == 0) s = " ", l = 1;
        if (!S.pasthdr) {          /* still in header part of DATA */
            S.hdrbytes += l + 1;   /* number to copy into MAILHBUF */
            ++mailblok.mailhrec;    /* increment # of header lines */
            ++mailblok.mailhlin;
        }
        switch (rc = fswrite(&S.data_file, s, &l)) {
        case 0:                    /* no error */
            return st;
        case 13:                   /* disk full */
            S.disk_error = diskfull;
            return error;
        default:                   /* some other error */
            wtr("SMT",50,2,"ca","fswrite ","d",rc);
            S.disk_error = diskother;
            return error;
        }
    }
    /* NOTREACHED */
}

static s_state
s_rset(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_rset");
#endif

    if (s_init0(S.repf))
        return s_nosp(st, s, l);
    (*S.repf)("250 Service reset OK.", 21);
    return initial;
}
static s_state
s_noop(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_noop");
#endif

    (*S.repf)("250 Ok", 6);
    return st;
}
static s_state
s_quit(st, s, l)
s_state st;
char *s;
int l;
{
    char buf[200];
    int len;

#ifdef TRACE
    wrterm("==> Entered s_quit");
#endif

    bcopy("221 ",buf, 4);
    len = mydomain(&buf[4], sizeof(buf)-4);
    bcopy(" Columbia MAILER BSMTP service done.", &buf[len+4], 36);
    (*S.repf)(buf, len+40);
    return final;
}

static s_state
s_verb(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_verb");
#endif

    if (l == 7 && xcmp(" ON", &s[4], 3) == 0) {
        S.verb = on;
        (*S.repf)(Verbonmsg,sizeof(Verbonmsg)-1);
    } else if (l == 8 && xcmp(" OFF", &s[4], 4) == 0) {
        S.verb = off;
        (*S.repf)(Verboffmsg,sizeof(Verboffmsg)-1);
    } else
        return s_syntax(st, s, l);
    return st;
}

static s_state
s_tick(st, s, l)
s_state st;
char *s;
int l;
{
    char buf[200];

#ifdef TRACE
    wrterm("==> Entered s_tick");
#endif

    if (l > 5) {
        bcopy("250 ",buf,4);
        bcopy(&s[5], &buf[4], l-5);
        bcopy(" ... that's the ticket.",&buf[4+l-5],23);
        (*S.repf)(buf,27+l-5);
    } else
        return s_syntax(st, s, l);
    return st;
}

static s_state
s_rply(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_rply");
#endif

    if (l == 9 && xcmp(" MAIL", &s[4], 5) == 0) {
        S.rply = mail;
        (*S.repf)("250 Error replies will be sent as mail.",39);
    } else if (l == 9 && xcmp(" SMTP", &s[4], 5) == 0) {
        S.rply = smtp;
        (*S.repf)("250 Error replies will be sent as smtp.",39);
    } else
        return s_syntax(st, s, l);
    return st;
}

static s_state
s_read(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_read");
#endif

    if (l > 4 && xcmp("D", &s[4], 1) == 0) {
        (*S.repf)("200 :READ card ignored.", 23);
        return st;
    } else
        return s_badc(st, s, l);
}

static s_state
s_oseq(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_oseq");
#endif

    (*S.repf)("500 Command out of sequence", 27);
    return error;
}
static s_state
s_badc(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_badc");
#endif

    (*S.repf)("500 Invalid command", 19);
    return error;
}
static s_state
s_errr(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_errr");
#endif

    (*S.repf)("500 Command ignored due to a previous error.",44);
    return error;
}
static s_state
s_nosp(st, s, l)
s_state st;
char *s;
int l;
{
    char buf[200];
    int len;

#ifdef TRACE
    wrterm("==> Entered s_nosp");
#endif

    bcopy("421 ", buf, 4);
    len = mydomain(&buf[4], sizeof(buf)-4);
    bcopy(" Service not available (internal error).", &buf[len+4], 40);
    (*S.repf)(buf, len+43);
    return error;
}

static s_state
s_syntax(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_syntax");
#endif

    (*S.repf)("501 Parameter syntax error.", 27);
    return error;
}

static s_state
s_syntax553(st, s, l)
s_state st;
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered s_syntax553");
#endif

    (*S.repf)("553 Mailbox syntax error.", 25);
    return error;
}

static s_state
s_maxr(st, s, l)
s_state st;
char *s;
int l;
{
    char buf[200];

#ifdef TRACE
    wrterm("==> Entered s_maxr");
#endif

    wtr("SMT",63,0);
    (*S.repf)("554 Mail aborted.  Maximum hop count exceeded.",46);
    return error;
}
/*
 * _aparse: call assembler aparse function.
 */
extern aparse();
static struct {
    int *mb;
    struct fieldd *fd;
    int *sp[2];
} a_plist;
static
_aparse(f)
struct fieldd *f;
{

#ifdef TRACE
    wrterm("==> Entered _aparse");
#endif

    a_plist.mb = &mailblok;
    a_plist.fd = f;
    a_plist.sp[0] = &mailblok.stackp[0];
    a_plist.sp[1] = &mailblok.stackp[1];
    return acall(aparse, 0, &a_plist);
}

/*
 * parse_data_hdr:  Parse the mail header within the DATA to fill in
 *  the MAILKTAB table since many of the header fields are used by
 *  the various outgoing exits (e.g. Subject: used by bulletin board
 *  posting exit).  Then, clobber the FROM: and TO: tbentrys with the
 *  ones we derived from the SMTP MAIL FROM and RCPT TO commands.
 *
 * xxx Maybe later, we can compare the MAIL FROM: with the FROM: in
 * the DATA header and provide a delivery advice message if they don't
 * match.
 */
extern hdrscan();
static s_state
parse_data_hdr(st,s,l)
char *s;
int l;
{
    int i, blen, resid = S.hdrbytes + 1;   /* (for the final x15) */
    char *bp;
    struct {
        int *mb;
    } pl;
    struct tbentry *te;

#ifdef TRACE
    wrterm("==> Entered parse_data_hdr");
#endif

    if ((mailblok.mailhbuf[0] =
                 (int)salloc(mailblok.mailhbuf[1] = resid)) <= 0)
      return s_nosp(st, s, l);
    for (i = 0, bp = (char *)mailblok.mailhbuf[0];
          i < mailblok.mailhrec; i++) {
        int rc;

        blen = resid;
        if ((rc = fsread(&S.data_file,bp,&blen)) != 0) {
            wtr("SMT",50,2,"ca","fsread#2","d",rc);
            fsclose(&S.data_file);
            return st;                   /* ignore parse failures */
        }
        bp += blen;
        *bp++ = 0x15;                    /* newline */
        resid -= (blen + 1);
    }
    *bp = 0x15;                          /* final newline for EOH */
    fsclose(&S.data_file);
    pl.mb = &mailblok;
    acall(hdrscan,0,&pl);             /* parse the header */
                                     /* side effect: allocs mailktab */
    /* if FROM or TO are missing from the mailktab, then plug in
       SMTP FROM and TO so user exits can find them. */
    if (mailblok.mailktab) {         /* but only if it got allocated */
        if (ctbluk(&te,mailblok.mailktab,"from",4) != TBMATCH)
          ctbadd(mailblok.mailktab,&S.from_tbe);    /* so add one */
        if (ctbluk(&te,mailblok.mailktab,"to",2) != TBMATCH)
          ctbadd(mailblok.mailktab,&S.rcpt_tbe);
    }
    return st;
}

/*
 * find_routd:  Return pointer to ROUTD given hostname string
 *
 * The search sequence proceeds in the following order until a
 * match is found or the entire sequence fails:
 *
 *   (1)  Look up full host string in MAILRTAB (OUTGOING table)
 *
 *   (2)  Strip any attached RSCS-Domain
 *        If any RSCS-domains have been coded, do a right to left
 *        compare and strip on a match
 *
 *   (3)  Look for the (shorter) hostname in MAILRTAB (OUTGOING table)
 *
 *   (4)  Look for it in MAILDTAB (DOMAINS table)
 *
 *   (5)  If MOPTAUN (Allow-Unknown-Nodes) true look for *DEFAULT*
 *
 *   (5A) If original string (the one used for the lookup in step 1)
 *        has no dot in it (i.e. no domain name originally)  then
 *        look for *DEFAULT* in MAILRTAB (OUTGOING table)
 *
 *   (5B) If original string (the one used for the lookup in step 1)
 *        has a dot in it (i.e. a domain name originally)  then
 *
 *        (5B.1) If an RSCS-Domain match was found look for
 *               *DEFAULT*ROUTE*.rscsdomain in MAILDTAB (DOMAINS table)
 *
 *        (5B.2) Look for *DEFAULT* in MAILDTAB (DOMAINS table)
 *
 *
 *  Returns ptr to ROUTD or NULL if not found.
 */
_fnroutd(s, l)     /* assembly-language callable stub */
char *s;
int l;
{

#ifdef TRACE
    wrterm("==> Entered _fnroutd");
#endif

    return find_routd(s, l);
}

static struct routd *
find_routd(s, l)
char *s;
int l;
{
    struct tbentry *te;
    int ol = l;              /* original length */
    char defrt[255];         /* *DEFAULT*ROUTE* string */
#ifdef DEBUG_FIND_ROUTD
    char dbgmsg[133];
#endif

#ifdef TRACE
    wrterm("==> Entered find_routd");
#endif

#ifdef DEBUG_FIND_ROUTD
    bcopy("==> find_routd: >", dbgmsg, 17);
    bcopy(s, &dbgmsg[17], l);
    bcopy("<", &dbgmsg[17+l], 2);
    wrterm(dbgmsg);
#endif

    /* (1) see if it is in MAILRTAB */
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Checking MAILRTAB");
#endif
    if (ctbluk(&te, mailblok.mailrtab, s, l) == TBMATCH) {
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Found in MAILRTAB");
#endif
        return te->tbe_dsp;        /* table dispatch value is *routd */
    }

    /* (2) see if it ends in .<RSCS-domain> */
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Attempting RSCS domain removal");
#endif
    if (mailblok.rscsdom[0] != 0) {
        int ri;
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Checking RSCS domains");
#endif
        for (ri = 0; mailblok.rscsdom[ri] != 0 &&
              ri < MAXRSCSDOM; ri++) {
           int rl = mailblok.rscsdom[ri][0];
           char *rp = &mailblok.rscsdom[ri][1];

           if (rl < l && xcmp(rp, &s[l-rl], rl) == 0) {
               l -= rl;           /* chop of the RSCS-domain */
#ifdef DEBUG_FIND_ROUTD
              bcopy("==> find_routd: >", dbgmsg, 17);
              bcopy(rp, &dbgmsg[17], rl);
              bcopy("< dropped", &dbgmsg[17+rl], 10);
              wrterm(dbgmsg);
           }
           else {
              bcopy("==> find_routd: >", dbgmsg, 17);
              bcopy(rp, &dbgmsg[17], rl);
              bcopy("< no match", &dbgmsg[17+rl], 11);
              wrterm(dbgmsg);
#endif
           }
        }
#ifdef DEBUG_FIND_ROUTD
        bcopy("==> find_routd: >", dbgmsg, 17);
        bcopy(s, &dbgmsg[17], l);
        bcopy("<", &dbgmsg[17+l], 2);
        wrterm(dbgmsg);
#endif
    }

    /* (3) see if it is in MAILRTAB */
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Checking MAILRTAB");
#endif
    if (ctbluk(&te, mailblok.mailrtab, s, l) == TBMATCH) {
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Found in MAILRTAB");
#endif
        return te->tbe_dsp;        /* table dispatch value is *routd */
    }

    /* (4) see if it is in MAILDTAB */
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Checking MAILDTAB");
#endif
    if (ctbluk(&te, mailblok.maildtab, s, ol) == TBMATCH) {
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Found in MAILDTAB");
#endif
        return te->tbe_dsp;
    }

    /* (5) see if MOPTAUN and *DEFAULT* is in MAILDTAB/MAILRTAB */
    if (mailblok.mailopts&MOPTAUN) {
#ifdef DEBUG_FIND_ROUTD
        wrterm("==> find_routd: Allow unknown nodes set");
#endif
       if (strnindex(s, '.', ol) < 0) {
       /* (5A) No '.' in original address string */
#ifdef DEBUG_FIND_ROUTD
           wrterm("==> find_routd: Checking MAILRTAB with *DEFAULT*");
#endif
           if (ctbluk(&te, mailblok.mailrtab, "*DEFAULT*", 9)
                                                   == TBMATCH) {
#ifdef DEBUG_FIND_ROUTD
           wrterm("==> find_routd: Using *DEFAULT* in OUTGOING table");
#endif
           return te->tbe_dsp;
           }
       } else {
       /* (5B) Have a '.' in original address string */
#ifdef DEBUG_FIND_ROUTD
           wrterm("==> find_routd: Checking MAILDTAB with *DEFAULT*");
#endif
           if (l != ol) {
               /* (5B.1)  Stripped an RSCS-Domain */
               bcopy("*DEFAULT*ROUTE*", defrt, 15);
               bcopy(s+l,defrt,ol-l);
#ifdef DEBUG_FIND_ROUTD
               bcopy("==> find_routd: Using ", dbgmsg, 22);
               bcopy(defrt, dbgmsg+22, ol-l+15);
               bcopy(" in DOMAINS table", dbgmsg+22+(ol-l+15), 18);
               wrterm(dbgmsg);
#endif
               if (ctbluk(&te, mailblok.maildtab, defrt, ol-l+15)
                                                       == TBMATCH) {
#ifdef DEBUG_FIND_ROUTD
                   bcopy("==> find_routd: Using ", dbgmsg, 22);
                   bcopy(defrt, dbgmsg+22, ol-l+15);
                   bcopy(" in DOMAINS table", dbgmsg+22+(ol-l+15), 18);
                   wrterm(dbgmsg);
#endif
                   return te->tbe_dsp;
               }
           }
           /* (5B.2) Try for *DEFAULT* */
           if (ctbluk(&te, mailblok.maildtab, "*DEFAULT*", 9)
                                                       == TBMATCH) {
#ifdef DEBUG_FIND_ROUTD
               wrterm(
                 "==> find_routd: Using *DEFAULT* in DOMAINS table");
#endif
               return te->tbe_dsp;
           }
       }
    }

    /* give up */
#ifdef DEBUG_FIND_ROUTD
    wrterm("==> find_routd: Not found");
#endif
    return 0;
}

/*
 * log_errors: get the error messages from the userlog file and
 *  write them into the SMTP reply file. This replaces call on MAILRET.
 */
log_errors()
{
    int rc,buflen,rblen;
    char buf[80], rbuf[200];

#ifdef TRACE
    wrterm("==> Entered log_errors");
#endif

    bcopy("554 Mail not delivered to some or all recipients:",
          rbuf,rblen=49);
    fsclose(mailblok.mailulfs);        /* close user error log */
    mailblok.mailulfs->fscbitno =
                        mailblok.mailulfs->fscbaitn = 0;  /* point */
    do {
        buflen = sizeof(buf);
        switch (rc = fsread(mailblok.mailulfs,buf,&buflen)) {
        case 0:
           rbuf[3] = '-';     /* continue prev. message */
           (*S.repf)(rbuf,rblen); /* and dump it out */
           while (buf[buflen-1] == ' ' && buflen > 0)
             --buflen;        /* trim trailing blanks */
           rbuf[3] = ' ';
           bcopy(buf,&rbuf[4],buflen);
           rblen = buflen + 4;
           break;
        case 12:              /* EOF */
           fsclose(mailblok.mailulfs);
           (*S.repf)(rbuf,rblen);
           break;
        default:
           wtr("SMT",50,2,"ca","fsread#3","d",rc);
           rbuf[3] = '-';
           (*S.repf)(rbuf,rblen);
           (*S.repf)("554 Mailer problem: can't read error log.",
                     41);
           break;
        }
    } while (rc == 0);
    bcopy("ERASE   ",mailblok.mailulfs->fscbcomm,8);
    svc202(mailblok.mailulfs);          /* erase userlog file */
}

/*
 * add_received:  Add "Received:" stamp to DATA file.  Format is:
 *
 *  if from a trusted mailer:
 *    Received: from <domain> by <lclhost1>(Mailer <mailvers>)
 *     with BSMTP id <mailftxt>; <arpadate>
 *
 *  if not from a trusted mailer and not a charlatan:
 *  Received: from <domain>(<mailuser>) by <lclhost1>(Mailer <mailvers>)
 *     with BSMTP id <mailftxt>; <arpadate>
 *
 *  if from a charlatan:
 *    Received: from <domain>(<mailuser>@<mailnode>)
 *     by <lclhost>(Mailer <mailvers>) with BSMTP id <mailftxt>;
 *     <arpadate>
 *
 *  Also adds an X-Delivery-Notice if S.fakefrom.
 *
 */
static char Deliv[] =
  "X-Delivery-Notice:  SMTP MAIL FROM does not correspond to sender.";

static
add_received()
{
    char buf[256];
    int len;
    extern arpadate();

#ifdef TRACE
    wrterm("==> Entered add_received");
#endif

    if (S.fakefrom)
      foldwrite(&S.data_file,Deliv,sizeof(Deliv)-1);
    bcopy("Received: from ",buf,len=15);
    bcopy(S.domain, &buf[len], S.dl); len += S.dl;
    if (!S.trusted) {          /* not trusted;  put in the vmid */
        buf[len++] = ' '; buf[len++] = '(';
        bcopy(mailblok.mailuser,&buf[len],mailblok.mailulen);
        len += mailblok.mailulen;
        if (S.charlatan) {     /* charlatan: add the node too */
            buf[len++] = '@';
            bcopy(mailblok.mailnode,&buf[len],mailblok.mailnlen);
            len += mailblok.mailnlen;
        }
        buf[len++] = ')';
    }
    bcopy(" by ",&buf[len],4); len += 4;
    if (mailblok.lclhost[0] != 0) {
        bcopy(&mailblok.lclhost[0][1],&buf[len],
              (int)mailblok.lclhost[0][0]);
        len += mailblok.lclhost[0][0];
    } else {
        bcopy(mailblok.cpustr,&buf[len],mailblok.cpuslen);
        len += mailblok.cpuslen;
        if (mailblok.rscsdom[0] != 0) {
            bcopy(&mailblok.rscsdom[0][1],&buf[len],
                  (int)mailblok.rscsdom[0][0]);
            len += mailblok.rscsdom[0][0];
        }
    }
    bcopy(" (Mailer ",&buf[len],9); len += 9;
    bcopy(mailblok.mailvers,&buf[len],8); len += 8;
    while (buf[len-1] == ' ') --len;
    bcopy(") with BSMTP id ",&buf[len],16); len += 16;
    bcopy(mailblok.mailftxt,&buf[len],4); len += 4;
    buf[len++] = ';';  buf[len++] = ' ';
    acall(arpadate,0,&buf[len]); len += 29;
    foldwrite(&S.data_file, buf, len);
}

/*
 * foldwrite: Write to file after folding long lines.
 */
static
foldwrite(f,buf,len)
struct fscb *f;
char *buf;
int len;
{
    int i,l,l2;

#ifdef TRACE
    wrterm("==> Entered foldwrite");
#endif

    i = 0;
    while (len > 0) {
        if (len > 80) {        /* fold the line (search back for */
            int j;             /* blank and leave it for next time) */
            for (l = 80, j = i+79; buf[j] != ' ' && j > i; j--,l--) ;
            if (--l <= 0) l = 80;  /* xxx */
        } else
          l = len;

        len -= l;
        l2 = l;
        S.hdrbytes += l + 1;
        mailblok.mailhrec++, mailblok.mailhlin++;
        fswrite(f,&buf[i],&l2);
        i += l;
    }
}

/*
 * count_received:  return number of received: lines in header.
 */
static
count_received()
{
    struct tbentry *te;
    struct fieldd *f;
    int i;

#ifdef TRACE
    wrterm("==> Entered count_received");
#endif

    if (ctbluk(&te,mailblok.mailktab,"received",8) != TBMATCH)
      return 0;
    for (i = 0, f = (struct fieldd *)te->tbe_dsp;
         FIEXTN(f) != 0; i++, f = FIEXTN(f))
      ; /* count extensions */
    return i;
}
_cntrcvd()               /* assembler-called stub */
{

#ifdef TRACE
    wrterm("==> Entered _cntrcvd");
#endif

    return count_received();
}

/*
 * SMTPREP: Process a BSMTP reply file.  If there are any error codes,
 *  notify the sender (if it can be figured out).  If the outbound file
 *  was not sent in VERB mode,  then there is no way to know who the
 *  MAIL FROM user is without doing a lot more work than I feel like.
 *
 * NOTE: It's probably better just to let the remote BSMTP server send
 * error mail directly to the user than waste time with BSMTP replies.
 * But,  for the sake of completeness and interoperability with other
 * BSMTP implemenations,  this code had to be written.
 */
/* global reply parser state information */
static struct {
    char error;                 /* an error response was parsed */
    char parserr;               /* error parsing reply file */
    char *mailfrom;             /* ptr to mail from: string */
    int mflen;                  /* length of above */
    struct fscb repfile;        /* fscb for the disk copy of reply */
} R;

smtprep()
{
    char rbuf[133];
    int rlen;

#ifdef TRACE
    wrterm("==> Entered smtprep");
#endif

    r_init();
    cpcmd("CLOSE 00C",9);
    if (cpcmd("READY 00C",9)) return 12;
    while ((rlen = rdcard(rbuf,(int)mailblok.mailsfb.sfbrecsz)) >= 0) {
        rbuf[rlen] = '\0';
        if (r_input(rbuf, rlen) != 0) {
            R.parserr++;
            break;
        }
    }
    cpcmd("CLOSE 00C",9);
    r_finish();
    return 4;
}

/*
 * r_init: 1) Initialize data structures.
 *         2) Init file to copy BSMTP reply to.
 */
r_init()
{

#ifdef TRACE
    wrterm("==> Entered r_init");
#endif

    wtr("SMT",57,0);
    bzero(&R,sizeof(R));
    fserase("smtp.reply");
    fsinit(&R.repfile,"smtp.reply");
    R.repfile.fscbfv = 'V';
}

/*
 * r_finish: 1) Close BSMTP reply file.
 *           2) See if there was an error and, if mailfrom is known,
 *              generate error mail.
 *           3) Erase BSMTP reply file.
 *  (Future stuff to add:  Acknowledgements)
 */
r_finish()
{

#ifdef TRACE
    wrterm("==> Entered r_finish");
#endif

    fsclose(&R.repfile);
    if (!R.parserr && R.error && R.mailfrom)
      err_mail(R.mailfrom,R.mflen,0);
    fserase("smtp.reply");
    wtr("SMT",58,0);
}

/*
 * r_input: parse smtp reply messages and look for error codes.
 *   Also, keep an eye out for a 050 MAIL FROM to see who to report
 *   the errors to.  Format of all messages should be:
 *   01234
 *   nnnb<response>
 *   Where 'nnn' must be digits and 'b' is either blank or '-' to
 *   indicate a continuation.
 *   This routine also copies each reply record to a disk file to
 *   be used in the event that an error code is parsed.
 */
r_input(rbuf, rlen)
char *rbuf;
int rlen;
{
    int rc, l;

#ifdef TRACE
    wrterm("==> Entered r_input");
#endif

    if (mailblok.mailopts & MOPTVL)      /* verbose log */
      wrterm(rbuf);
    if (rlen < 3 || !isdigit(rbuf[0]) || !isdigit(rbuf[1])
        || !isdigit(rbuf[2])) {
        wtr("SMT",60,0);
        return 8;               /* response contains garbage */
    }

    l = rlen;
    if (rc = fswrite(&R.repfile,rbuf,&l))
      wtr("SMT",50,2,"ca","fswrite ","d",rc);

    /* use the response code to select what operation to perform */
    switch (rbuf[0]) {
      case '0':                 /* look for 050 MAIL FROM:<...> */
        if (rbuf[1] == '5' && rbuf[2] == '0' && rlen >= 16
            && xcmp("MAIL FROM:<",&rbuf[4],11) == 0) {
                if (R.mailfrom) { /* already seen one */
                    wtr("SMT",61,0);
                    return 12;
                }
                R.mailfrom = (char *) salloc(rlen-15);
                if (!R.mailfrom) {  /*  no space */
                    wtr("SMT",3,1,"ca","r_input ");
                    return 16;
                }
                bcopy(&rbuf[15],R.mailfrom,R.mflen = rlen-16);
                R.mailfrom[rlen-15] = '\0';
        }
        break;
      case '1':                 /* normal responses */
      case '2':
      case '3':
        break;
      case '4':                 /* error responses */
      case '5':
        R.error++;
        break;
      default:
        return 8;               /* garbage */
    }
    return 0;
}

/*
 * err_mail:  Send BSMTP error mail.  Called both from the SMTP
 *  request parser and the SMTP reply parser.  In both cases,
 *  the SMTP transaction log is in "smtp.reply".
 */
static
err_mail(rcpt,rl,fp)
char *rcpt;         /* whom to send it to */
int rl;             /* length */
struct fscb *fp;    /* fscb for data or zero for none */
{
    char buf[200],myfrom[100],*domp;
    int bl,rc,fl,dl;
    extern arpadate();
    struct fscb repfile;

#ifdef TRACE
    wrterm("==> Entered err_mail");
#endif

    /* build my from address */
    myfrom[0] = '<';
    bcopy(mailblok.mailvmid,&myfrom[1],8);
    for (fl = 8; myfrom[fl] == ' '; fl--) ; /* trim blanks */
    fl++;
    myfrom[fl++] = '@';
    fl += dl = mydomain(domp = &myfrom[fl],sizeof(myfrom)-fl);
    myfrom[fl++] = '>';
    cpcmd("SPOOL 00D *",11);
    cpcmd("TAG DEV 00D --- returned mail file ---",38);
    bcopy("HELO ",buf,bl = 5);
    bcopy(domp,&buf[bl],dl); bl += dl;
    punch(buf,bl);
    bcopy("MAIL FROM:",buf,bl = 10);
    bcopy(myfrom,&buf[bl],fl); bl += fl;
    punch(buf,bl);
    bcopy("RCPT TO:<",buf,bl = 9);
    bcopy(rcpt,&buf[bl],rl); bl += rl;
    buf[bl++] = '>';
    punch(buf,bl);
    punch("DATA",4);
    bcopy("Date:                              ",buf,bl = 29+6);
    acall(arpadate,0,&buf[6]);        /* max result len is 29 */
    punch(buf,bl);
    bcopy("From: Network Mailer ",buf,bl=21);
    bcopy(myfrom,&buf[bl],fl);  bl += fl;
    punch(buf,bl);
    bcopy("To: ",buf,bl=4);
    bcopy(rcpt,&buf[bl],rl); bl += rl;
    punch(buf,bl);
    punch("Subject: mail delivery error",28);
    punch(" ",1);
    punch("Batch SMTP transaction log follows:",35);
    punch(" ",1);
    /* copy smtp.reply to punch */
    fsinit(&repfile,"smtp.reply");
    repfile.fscbfv = 'V';
    do {
        bl = sizeof(buf);
        switch (rc = fsread(&repfile, buf, &bl)) {
          case 0:
            punch(buf, bl);
            break;
          case 12:          /* EOF */
            fsclose(&repfile);
            break;
          default:          /* unexpected error */
            wtr("SMT",50,2,"ca","fsread#4","d",rc);
            break;
        }
    } while (rc == 0);
    if (fp) {
        punch(" ",1);
        punch("Original message follows:",25);
        punch(" ",1);
        buf[0] = '.';      /* in case doubling is needed */
        do {
            bl = sizeof(buf) - 1;
            switch (rc = fsread(fp, &buf[1], &bl)) {
              case 0:
                if (buf[1] == '.')
                  punch(buf,bl+1);
                else
                  punch(&buf[1],bl);
                break;
              case 12:   /* EOF */
                fsclose(fp);
                break;
              default:
                wtr("SMT",50,2,"ca","fsread#5","d",rc);
                break;
             }
          } while (rc == 0);
    }
    punch(".",1);
    punch("QUIT",4);
    cpcmd("CLOSE 00D NAME ERROR MAIL",26);
    wtr("SMT",59,0);
}

/*
 * is_mailer: returns true if fieldd points to a registered
 *  mailer (including myself).  compares user@node to outgoing/domain,
 *  incoming, and mailvmid@cpustr.
 *  1. Convert and pad the user to 8 characters for comparisons.
 *  2. Look up routd for given domain and compare RTUSER to userid
 *  3. If not that, compare RTNODE/user to CPUSTR/MAILVMID.
 *  4. If not that, use RTNODE and user to check MAILVTAB.
 *  5. If not that, use '*DEFAULT*' and user to check MAILVTAB.
 *  6. If not that, then return false:  not a mailer.
 *  If there are any bogus data structures, return true to play it
 *  safe (the idea is to prevent loops).
 */
static
is_mailer(f)
struct fieldd *f;
{
    union {                               /* vald key */
        struct { char node[8], user[8]; } part;
        char both[16];
    } key;
    struct mbox *a = (struct mbox *)f->fipars;
    struct tbentry te;
    struct routd *r;
    char *us,*hs;
    int ul,hl,i;

#ifdef TRACE
    wrterm("==> Entered is_mailer");
#endif

    if (a == 0)
      return 1;                          /* is a mailer */
    for (i = 0; i < sizeof(key.both); i++)
      key.both[i] = ' ';                 /* init key to blanks */
    /* 1. Convert and pad the userid */
    us = TOKPTR(MBOXLCL(a)->noddown);
    ul = MBOXLCL(a)->noddown->toklen;
    if (ul > 8)                          /* not a mailer since userid */
      return 0;                          /* is longer than a VMID */
    bcopy(us,key.part.user,ul);               /* copy in the userid */
    /* 2. Look up the routd. */
    hs = TOKPTR(MBOXDML(a)->noddown);
    hl = MBOXDML(a)->noddown->toklen;
    if ((r = find_routd(hs, hl)) == 0)
      return 1;
    if (xcmp(key.part.user,r->rtuser,8)==0)/*user matches mailer id*/
      return 1;
    /* 3. compare rtnode to cpustr and rtuser to mailvmid */
    bcopy(r->rtuser,key.part.user,8);            /* fill in the key */
    if (xcmp(mailblok.cpustr,key.part.node,8)==0 &&
        xcmp(mailblok.mailvmid,key.part.user,8)==0)
      return 1;
    /* 4. look in mailvtab */
    if (ctbluk(&te, mailblok.mailvtab, &key, 16) == TBMATCH)
      return 1;
    /* 5. look in mailvtab for '*default* userid' */
    bcopy("*DEFAULT*",key.part.node,8);
    if (ctbluk(&te, mailblok.mailvtab, &key, 16) == TBMATCH)
      return 1;
    /* 6. must not be a mailer! */
    return 0;
}

/*
 * err_loop: send error mail to maintainer.
 */
static
err_loop(f)
struct fscb *f;
{
    char buf[25];
    int bl;

#ifdef TRACE
    wrterm("==> Entered err_loop");
#endif

    wtr("SMT",28,0);
    bcopy(mailblok.maintid,buf,bl=8);
    while (buf[bl-1] == ' ') --bl;
    buf[bl++] = '@';
    bcopy(mailblok.maintnod,&buf[bl],8); bl += 8;
    while (buf[bl-1] == ' ') --bl;
    err_mail(buf,bl,f);
}

static
punch(buf,buflen)               /* punch a card and logout if error */
char *buf;                      /* since the only possible error is */
int buflen;                     /* device not ready which means spool */
{                               /* system is SNAFU */
    int rc;

#ifdef TRACE
    wrterm("==> Entered punch");
#endif

    if (buflen > 80) buflen = 80;  /* xxx */
    if (rc = punchc(buf, buflen) != 0) {
        wtr("SMT",50,2,"ca","punchc  ","d",rc);
        cpcmd("SP PUN PURGE",12); /* don't send a partial file */
        cpcmd("LOGOUT",6);      /* can't do much if spool is full */
    }
}
