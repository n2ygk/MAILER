/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1985 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
/*
 * declarations of useful entries in MAILBLOK CSECT.  Used in
 * conjunction with Waterloo C's Global Base Register facility
 * and the CGLUE AUX file which all C functions must be compiled
 * with for it to work.  The CGLUE ASSEMBLE FUNC macro sets up the
 * Global Base Register (r10) with the address of MAILBLOK.
 *
 * Alan Crosswell, 11/85
 */
#include "cmsbloks.h"
#include "structs.h"
#include "tbluk.h"

static struct mailblok {
    int mailhbuf[2];         /* addr, len of header buffer           */
    int stackorg[2];         /* free storage stack origin            */
    int stack[2];            /* variable stack start                 */
    int stackp[2];           /* current stack pointer                */
    struct tbtab *mailrtab;  /* Addr of routing table                */
    struct tbtab *mailstab;  /* SPECIAL table anchor                 */
    struct tbtab *mailktab;  /* Msg header keyword table             */
    struct tbtab *mailvtab;  /* Verification table                   */
    struct tbtab *mailptab;  /* User parms table                     */
    int mailhrec;            /* Number of physical records in header */
    int mailhlin;            /* Number of logical records in header  */
    int mailrcnt;            /* Number of recipients processed       */
    int mailrcnb;            /* Number of recipinet blocks procesed  */
    int mailrmax;            /* Maximum number of recipients seen    */
    int mailrtot;            /* Total number of recipients seen      */
    int mailicnt;            /* Incoming file count                  */
    int mailstmx;            /* Maximum used stack address           */
    struct actadr *mailchn;  /* Action address list                  */
    struct fieldd *mailfld;  /* FIELDD anchro                        */
    struct fieldd *mailsfld; /* File sender FIELDD                   */
    int mailcmdl;            /* Stacked MAILCMD2 commands            */
    int mailfile;            /* Binary spool file number             */
    char mailftxt[4];        /* Character spool file number          */
    char mailopts;           /* Mailer runtime options               */
#define MOPTDEF 0x80         /* Supply defaults for omitted fields   */
#define MOPTAUN 0x40         /* Allow unknown nodes to send me mail  */
#define MOPTAC  0x20         /* Allow charlatans                     */
#define MOPTVL  0x10         /* Verbose console logging              */
    char mailflg1;           /* Current mail file flags              */
#define MFLGDEST 0x80        /* A '*DESTINATION:' card is present    */
#define MFLG1LCL 0x40        /* Mail source is local user            */
#define MFLG1GTW 0x20        /* Mail source is gateway               */
#define MFLG1EDI 0x10        /* Header has been edited               */
#define MFLG1REJ 0x08        /* Rejected by RSCS                     */
#define MFLG1HDS 0x04        /* Header has been scanned OK           */
#define MFLG1DOT 0x02        /* Quote lone dots when punching        */
#define MFLG1RES 0x01        /* File has been resent                 */
    char mailcmd;            /* Command flags                        */
#define MCMDCMD2 0x20        /* MAILCMD2 has been called             */
    char mailstat;           /* MAILER status flag byte              */
#define MSTINWT  0x80        /* In WAIT state                        */
#define MSTQSC   0x40        /* QUIESCENT                            */
    char cpustr[8];          /* RSCS node name */
    char rscsname[8];        /* rscs vmid */
    char mailvmid[8];        /* mail vmid */
    char mailtime[16];       /* time of last start/reload */
    char mailvers[28];       /* mailer version, assembly date */
                             /*  0 -  7 Version id                   */
                             /*  8 - 15 Assembly date                */
                             /* 16 - 16 Blank                        */
                             /* 17 - 24 HH:MM:00                     */
                             /* 25 - 27 Blank                        */
    int haltrc;              /* Return code from STOP command        */
    char mailnode[8];        /* Origin node name                     */
    char mailuser[8];        /* Origin userid                        */
    char mailrtfn[18];       /* Profile fileid                       */
                             /*  0 -  7 'MAILER  '                   */
                             /*  8 - 15 'PROFILE '                   */
                             /* 16 - 17 '* '                         */
    char mailulfn[18];       /* Log fileid                           */
                             /*  0 -  7 'MAILER  '                   */
                             /*  8 - 15 'LOG     '                   */
                             /* 16 - 17 '* '                         */
    char align1[2];          /* For alignment with ASM               */
    struct sfblok mailsfb;   /* SFBLOK for current reader file       */
    char align2[12];         /* For alignment with ASM               */
    char mailtag[130];       /* Tag text                             */

    double mailiucb[5];      /* IUCV external interrupt buffer       */
    int mailiucp;            /* IUCV *MSG pathid                     */

    int cmdecb;              /* Incoming command ECB                 */
    int stopecb;             /* Stop command ECB                     */
    int reldecb;             /* RELOAD command ECB                   */

    char spare1[200];        /* Spare                                */

    /* Commonly used constants */

    char revtab[256];        /* Reverse string translate table       */
    char skipblks[256];      /* Skip blanks translate table          */
    char traptab[256];       /* General 1 or 2 character trap table  */
    char upcase[256];        /* Upper-case translate table           */

    /* Common routine declarations */

    int apunhdr;             /* Routine to punch headers             */
    int apuntxt;             /* Routine to punch text                */
    int apuncls;             /* Routine to close punch               */
    int atbluk;              /* Table lookup                         */
    int amonwtr;             /* Log writer                           */
    int aarpadat;            /* Current date in RFC822 format        */
    int aunixsca;            /* Unix-style argument scanner          */
    int ardhdr;              /* Get next header record               */
    int ardtxt;              /* Get next text record                 */
    int patch1[34];          /* Patch space                          */
    int maxrcvd;             /* Max. number of Received: headers     */
    int mailulen;            /* Length of MAILUSER                   */
    int mailnlen;            /* Length of MAILNODE                   */
    struct fscb *mailulfs;   /* error log fscb pointer               */
    char *oldldom;           /* Spare (was -> Local-Domain           */
    int cpuslen;             /* Length of CPUSTR                     */
    char *oldrdom;           /* Spare (was -> Rscs-Domain            */
    struct tbtab *maildtab;  /* Domains table pointer                */
    struct tbtab *mailotab;  /* Origins table pointer                */
    struct fieldd *maildfld; /* File date FIELDD                     */
    char rscsalt[8];         /* Alternat RSCS id                     */
    char siteopts;           /* Site-specific options                */
#define SMAINE  0x80         /* U. Maine "FORM MAIL"                 */
#define SNXFER  0x40         /* Don't use SFBLOK SFBXFER flag        */
    char rescnt;             /* Count of Resent- field groups        */
    char mailflg2;           /* More flags                           */
#define MFLG2SMT 0x80        /* Current mail file is BSMTP-generated */
#define MFLG2VLF 0x40        /* Validate failed                      */
#define MFLG2NOR 0x20        /* No Received: stamp wanted            */
#define MFLG2NLF 0x10        /* MAIL FROM:<>                         */
#define MFLG2NOH 0x08        /* MAIL FROM:<userid> (i.e. no host)    */
#define MFLG2BLD 0x04        /* BUILDLST incomplete (some mail to    */
                             /*     send but some destiantions not   */
                             /*     found)                           */
    char reserved;           /* Reserved                             */
    struct fscb ulfscb;      /* MAILER USERLOG fscb                  */
    struct trcent *trtblst;  /* Start of trace table                 */
    struct trcent *trtblcur; /* Current trace table entry            */
    struct trcent *trtblend; /* End of trace table                   */
    int align4;              /* For alignment with ASM               */
    char maintid[8];         /* maintainer's userid                  */
    char maintnod[8];        /* maintainer's node name               */
    char maintid2[8];        /* maintainer's userid                  */
    char maintno2[8];        /* maintainer's node name               */
    char maintid3[8];        /* maintainer's userid                  */
    char maintno3[8];        /* maintainer's node name               */
    char maintid4[8];        /* maintainer's userid                  */
    char maintno4[8];        /* maintainer's node name               */
    char maintid5[8];        /* maintainer's userid                  */
    char maintno5[8];        /* maintainer's node name               */
    char maintid6[8];        /* maintainer's userid                  */
    char maintno6[8];        /* maintainer's node name               */
    char maintid7[8];        /* maintainer's userid                  */
    char maintno7[8];        /* maintainer's node name               */
    char maintid8[8];        /* maintainer's userid                  */
    char maintno8[8];        /* maintainer's node name               */
    char maintid9[8];        /* maintainer's userid                  */
    char maintno9[8];        /* maintainer's node name               */
    char endlist[16];        /* End of list flag                     */
#define MAXLCLHOST 9
    char *lclhost[MAXLCLHOST]; /* local-domain strings */
#define MAXRSCSDOM 9
    char *rscsdom[MAXRSCSDOM]; /* rscs-domain strings */
} ;

/* MAILBLOK is created by MAILBLOK ASSEMBLE file during MAILER gen   */

extern struct mailblok mailblok;

/* UPDATES APPLIED AT 17:29:13 ON 19 SEP 1989
* * * * * * * * * * * * * * * * AUXR2$H * * * * * * * * * * * * * * * *
R200008    Allow multiple Local-Hosts.
*         MAILBLOK R200008  D1 V2-MNT  10/05/88   10:22:31
R200009    Allow multiple Rscs-Domains.
*         MAILBLOK R200009  D1 V2-MNT  10/05/88   22:37:12
R202014    Make MAILBLOK globally available to all code
*          This will remove dependence on Waterloo AUX file usage for
*          increased portability of C code
*         MAILBLOK R202014  D1 V2-MNT  02/21/89   14:49:56
R202016H   Fix MAILER007E RC=0002 during RELOAD processing.
*         MAILBLOK R202016H D1 V2-MNT  02/17/89   17:05:41
*/
