/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1987 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
#define CCNET
static char version[] = "MG version 2.06";
/*
 * MG: Generate mailer tables from NAMES/NODES files.
 *     Uses the :mailer, :nick, :alias, :netsoft tags.
 *
 * INPUT FILES:
 *
 *      XMAILER NAMES    - subset of needed fields from huge database
 *      MAILER  NAMES    - mail-only nodes.
 *       (or their EARN equivalents)
 *      DOMAIN  NAMES    - domains
 *      <node>  MAILLCL
 *      <node>  MTPLATE
 *
 * OUTPUT FILE:
 *
 *      <node> PROFILE
 *
 * Format of :nick tag is
 *           :nick.hostname
 *
 * Format of :mailer tag is
 *            :mailer.[vmid[@node] [exit type [parm]]]
 * If :mailer is blank,  then set up a default outgoing entry of:
 *    hostname hostname ? DEFRT 1
 * If :mailer.vmid is given then set up:
 *    hostname hostname vmid BSMTP 3
 * If :mailer.vmid@node then:
 *    hostname node vmid BSMTP 3
 * else, use the given info to fill in the [exit type [parm]] part.
 *
 * When :mailer is non-blank and has no @node, generate an incoming
 *   entry of:  hostname vmid
 *
 * Specifying the "-i" switch tells MG to replace all entries
 * that would have BSMTP as the outgoing exit with DEFRT.  This
 * is used for initial mailer distributions  to allow for the lag
 * time in updates to * NAMES.
 *
 * You can now include "literal" parameters in the NAMES or MAILLCL
 * files.  A literal is a token in quotes that gets replaced by a
 * variable name.  This gets around the 8 character limit on parms.
 * This will be most useful for the BSMTP exit.
 */

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#ifdef WC30
#include <string.h>
#endif WC30
#include "names.h"

/* Configuration constants */

#define MAXREPS 400          /* max size of MAILLCL file */
#define MAXNAMES 10          /* max number of names files used */

struct mailfld {
    char *host;
    char *node;
    char *vmid;
    char *exit;
    char *type;
    char *parm;
}
r;

struct mailfld default1 = {
    "", "", "", "BSMTP", "3", "" };

struct mailfld default2 = {
    "", "", "?", "DEFRT", "1", "" };

struct names namelist[] = {
    { "nick" },
#define NICK    0
    { "alias" },
#define ALIAS   1
    { "mailer" },
#define MAILER  2
    { "netsoft" },
#define NETSOFT 3
    { NULL }
};

char *names[MAXNAMES+2] = {    /* list of names files to use */
    "xmailer.names",
    "mailer.names",
    NULL
};

char *domnames[MAXNAMES+2] = { /* list of domain names files */
    "domain.names",
     NULL
};

char outtab[] = "mailgen.cmsut1";
char intab[]  = "mailgen.cmsut2";
char domtab[]  = "mailgen.cmsut3";
char littab[]  = "mailgen.cmsut4";
char repfile[18];
char *repfpat = "%.8s.maillcl";
char outfile[60];
char *outfpat = "%.8s.profile (recfm f lrecl 80";
char tpfile[18];
char *tpfpat = "%.8s.mtplate";
int iniflag = 0;

char *token();
#define strcmp(s,t) Xstrcmp(s,t)

extern int namenode;

main(argc,argv)
int argc;
char **argv;
{
    int rc;
    char *profname;
    FILE *litf;

    namenode = 1;       /* tell names library that :node is OK */

    for (; argc > 1 && *argv[1]  == '-'; --argc, ++argv)
      switch(argv[1][1]) {
        case 'i':         /* initial installation */
        case 'I':
          iniflag++;
          break;
        default:
          usage();
      }
    if (argc < 2) usage();
    profname = argv[1];
    /*
     * see if the user wants to override the default list of mailer or
     * domain NAMES files.  Mailer names files are indicate simply by
     * the name.  Domain names file have a "-d" arg in front.
     */
    if (argc > 2) {
        int ni, di;

        for (ni = di = 0, argc -= 2, argv = &argv[2];
                          argc > 0; argc--, argv++) {
            if (strcmp("-d", *argv) == 0) { /* domain names file */
                if (--argc <= 0 || di >= MAXNAMES) break;
                domnames[di++] = *++argv;
                domnames[di] = NULL;
            } else {
                if (ni >= MAXNAMES) break;
                names[ni++] = *argv;
                names[ni] = NULL;
            }
        }
    }

    sprintf(repfile,repfpat,profname);
    sprintf(outfile,outfpat,profname);
    sprintf(tpfile,tpfpat,profname);
    if ((litf = fopen(littab,"w")) == NULL) {
        fprintf(stderr,"Couldn't create %s\n", littab);
        exit(1);
    }
    generate(names, outtab, intab, litf);   /* work on mailer names */
    generate(domnames, domtab, NULL, litf); /* work on domain names */
    fclose(litf);
    merge();
    unlink(intab);
    unlink(outtab);
    unlink(littab);
    unlink(domtab);
    exit(0);
}
usage()
{
    printf(
 "Usage: mg [-i] <node> [<names_file> ...] [-d <domains_file>...]\n");
    exit(1);
}

generate(names, outtab, intab, litf)
char *names[];
char *outtab, *intab;
FILE *litf;
{
    FILE *outf;
    FILE *inf;
    FILE *namesf;
    int i;

    if ((outf = fopen(outtab,"w")) == NULL) {
        fprintf(stderr,"Couldn't create %s\n", outtab);
        exit(1);
    }
    if (intab == NULL)
        inf = NULL;
    else if ((inf = fopen(intab,"w")) == NULL) {
        fprintf(stderr,"Couldn't create %s\n", intab);
        exit(1);
    }

    for (i = 0; names[i] != NULL; i++) {
        if ((namesf = fopen(names[i], "r")) == NULL) {
            fprintf(stderr,"Couldn't open %s\n", names[i]);
            exit(1);
        }
        for (namelist[ALIAS].n_val = 0;
             nextname(namesf, namelist);
             namelist[ALIAS].n_val = 0) {
            char *alias = namelist[ALIAS].n_val;
            char *a;
            int tlen;

#ifdef CCNET
/* temporary CCnet kludge:  Site names end in .CCNET in the NAMES
 * file,  but the CCnet mailer on CU20B doesn't believe in the
 * .CCNET ending yet,  so strip it out here for now.
 */
            if ((tlen = strlen(namelist[NICK].n_val)) > 7)
              if (strcmp(&(namelist[NICK].n_val)[tlen-6],".CCNET") == 0)
                (namelist[NICK].n_val)[tlen-6] = '\0';
#endif CCNET
            /* parse and output an outgoing table entry */
            scanmailer(namelist[MAILER].n_val, namelist[NICK].n_val,
                       namelist[NETSOFT].n_val);
            put_outf(outf, &r, litf);

            /* maybe output an incoming table entry */
#define mailer namelist[MAILER].n_val
            if (inf != NULL && *mailer && *mailer != '?'
                && !strcmp(r.host,r.node))
              put_inf(inf, &r);
#undef mailer

            /* go thru list of aliases and generate outgoing entries */
            if (alias != NULL)
              for (a = token(&alias); *a != '\0'; a = token(&alias))
                if (strcmp(namelist[NICK].n_val, a) != 0) {
                    r.host = a;
                    put_outf(outf, &r, litf);
                }
        }
        fclose(namesf);
    }
    fclose(outf);
    if (inf != NULL) fclose(inf);
}

scanmailer(mailer,nick,netsoft)
char *mailer, *nick;
{
    char *i, *s;

    if (*mailer == '\0') {
        r = default2;               /* use DEFRT */
        r.host = r.node = nick;
        r.parm = "";
        return;
    }
    r = default1;
    if (iniflag) {            /* initial dist */
            r.exit = "DEFRT"; /* use DEFRT instead of BSMTP */
            r.type = "1";     /* called once per address */
        }
    r.host = r.node = nick;
    r.vmid = token(&mailer);        /* vmid is first token */
    for (i = r.vmid; *i != '\0' && *i != '@' && *i != ' '; i++);
    switch(*i) {
      case '\0':
        if (*mailer == '\0') return;
        break;
      case ' ':
        break;
      case '@':
        *i++ = '\0';
        r.node = i;
        break;
    }
    if (*mailer == '\0') return;
    r.exit = token(&mailer);
    if (*mailer == '\0') return;
    r.type = token(&mailer);
    if (*mailer == '\0') return;
    r.parm = token(&mailer);
}

put_outf(f, r, litf)
FILE *f;
struct mailfld *r;
FILE *litf;
{
    struct mailfld *outrep();
    static char pat[] = "%-16s %-8s %-8s %-8s %-4s %-8s\n";
    char *first,*last,*parm,parmbuf[100];
    int len,l1;
    static int litnum = 0;  /* counter for literal var names */

    r = outrep(r);          /* do any local replacement */
    parm = r->parm;
    /* check parm for a literal which needs to be replaced by a var */
    if ((first = index(r->parm,'"')) && (last = rindex(r->parm,'"'))) {
        if ((len = last - first - 1) < 0)  /* length of lit value */
          len = 0;
        l1 = first - r->parm;    /* length of part before lit */
        fprintf(litf,"$%04.4d=%*.*s\n",++litnum,len,len,first+1);
        sprintf(parmbuf,"%*.*s$%04.4d%s",l1,l1,r->parm,litnum,last+1);
        parm = parmbuf;
    }
    fprintf(f, pat, r->host, r->node, r->vmid, r->exit, r->type,
            parm);
}

put_inf(f, r)
FILE *f;
struct mailfld *r;
{
    static char pat[] = "%-8s %-8s\n";

    fprintf(f, pat, r->node, r->vmid);
}

/*
 * outrep:  replace any matching entries from outgoing table
 */
struct mailfld *
outrep(r)
struct mailfld *r;
{
    static FILE *repf = NULL;
    static struct mailfld reptab[MAXREPS];
    static int nrep = 0, norep = 0;
    static char buf[512];
    struct mailfld *m;
    int len, i;
    char *p, *malloc();

    if (norep) return r;

    if (repf == NULL) {     /* 1st time thru */
        if ((repf = fopen(repfile,"r")) == NULL) {
            fprintf(stderr,"No replacements file (%s)\n", repfile);
            norep++;
            return r;
        }
        while (fgets(buf, sizeof(buf), repf)) {
            len = strlen(buf);
            buf[len-1] = '\0';      /* kill newline */
            if ((p = malloc(len)) == NULL) {
                fprintf(stderr,"Out of memory.\n");
                exit(1);
            }
            bcopy(buf, p, len);
            m = &reptab[nrep];
            if (nrep++ >= MAXREPS) {
                fprintf(stderr,"Table overflow.\n");
                exit(1);
            }
            m->host = token(&p);
            m->node = token(&p);
            m->vmid = token(&p);
            m->exit = token(&p);
            m->type = token(&p);
            m->parm = token(&p);
        }
        fclose(repf);
        }

    /* Search the table for matching host name */

    for (i = 0; i < nrep; i++)
      if (strcmp(reptab[i].host, r->host) == 0)
        return &reptab[i];
    return r;
}

merge()
{
    extern struct tm *localtim();
    extern char *asctime();
    FILE *tpf = fopen(tpfile,"r");
    FILE *outf = fopen(outfile,"w");
    char buf[512];

    if (tpf == NULL) {
        printf("%s: not found.\n", tpfile);
        return 1;
    }
    if (outf == NULL) {
        printf("%s: not found.\n", outfile);
        return 1;
    }
#ifdef WC30
    fprintf(outf,"; Generated by %s %s", version,
            asctime(localtime((time_t *)0)));
#else
    fprintf(outf,"; Generated by %s %s", version,
            asctime(localtime()));
#endif WC30

    while(fgets(buf, sizeof(buf), tpf)) {
        if (buf[0] == '%') {
            if (strncmp(&buf[1],"incoming",8) == 0)
              mergein(outf, intab, 1);
            else if (strncmp(&buf[1],"outgoing",8) == 0)
              mergein(outf, outtab, 1);
            else if (strncmp(&buf[1],"domains",7) == 0)
              mergein(outf, domtab, 1);
            else if (strncmp(&buf[1], "include", 7) == 0) {
                buf[strlen(buf)-1] = '\0';
                rtrim(buf);
                mergein(outf, &buf[9], 1);
            }
        }
        else
          fputs(buf, outf);
    }
    fprintf(outf,"; \"literal\" variables:\n");
    mergein(outf, littab, 0);       /* literals file */
    fclose(outf);
    return 0;
}

mergein(f,n,complain)
FILE *f;
char *n;
int complain;  /* complain about missing file */
{
    FILE *inf = fopen(n,"r");
    int c;

    if (inf == NULL) {
        if (complain) printf("%s: not found..n", n);
        return;
    }

    while ((c = getc(inf)) != EOF)
      putc(c,f);
    fclose(inf);
}

/*
 * token: Return a pointer to the next null-terminated token in a string
 *    of tokens delimited by white space.  Update the passed pointer
 *    so that subsequent calls will return subsequent tokens.
 */

char *
token(c)
register char **c;
{
    char *r;

    if (**c == '\0') return *c;
    while (isspace(**c) && **c != '\0') (*c)++; /* trim leading */
    r = *c;
    if (**c == '\0') return r;
    while (!isspace(**c) && **c != '\0') (*c)++;    /* skip over */
    if (**c == '\0') return r;
    **c = '\0';                 /* replace w/null */
    (*c)++;
    return r;
}

#define lower(c) (isupper(c)?tolower(c):c)

Xstrcmp(s, t) /* case independent strcmp() */
char *s, *t;
{
    for (; lower(*s) == lower(*t); s++, t++)
      if (*s == '\0')
        return 0;
    return lower(*s) - lower(*t);
}

rtrim(c)
char *c;
{
    register int l;

    for (l = strlen(c) - 1; l >= 0 && c[l] == ' '; --l)
      c[l] = '\0';
    return l+1;
}

/* strfind(haystack,needle)
   haystack is a string of blank-delimited words
   needle is a word to look for in haystack

   will return 0 if needle not found
   will return word number of needle in haystack if found
*/
int
strfind(s1,s2)
char *s1,*s2;
{
    char *s = s1;
    char *t;
    int i ;

    for(i=1;strcmp((t = token(&s)),s2) != 0;++i)
         if (*t == '\0'){
              i = 0;
              break;
         };
    return i;
}
