/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1985 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
/*
 * structs.h:  C versions of various Mailer data structures.
 *   Keep this file in sync with MAILMACS MACRO.
 *
 * A lot of these structs are really dumb looking.  This is because
 * they reflect assembler DSECTs which did things like put a length
 * in one byte and a pointer in the other 3 bytes of a word.  Due to
 * the language syntax,  it also means a whole bunch of macros are
 * defined herein which cast a 3-byte pointer to it's actual type.
 *
 * Alan Crosswell 12/85
 */

struct fieldd {
    struct fieldd *finext;
    struct {
        unsigned f_flg1:8;
        unsigned f_extn:24;
    } s1;
#define FINEXT(f) ((f)->finext)
#define FIEXTN(f) ((struct fieldd *)(f)->s1.f_extn)
#define fiextn s1.f_extn
#define fiflg1 s1.f_flg1
#define FIEXTFLG 0x80
#define FIINVFLG 0x40
#define FIEDIFLG 0x20
    struct {
        unsigned f_ecnt:8;
        unsigned f_tok:24;
    } s2;
#define FITOK(f) ((struct tokend *)(f)->s2.f_tok)
#define fitok s2.f_tok
#define fiecnt s2.f_ecnt
    unsigned fipars;
    char *fistr;
    char *fiend;
};

struct tokend {
    struct tokend *toknext;
#define TOKNEXT(t) ((struct tokend *)(t)->toknext)
    struct tokend *tokprev;
    char *tokptr;
#define TOKPTR(t) ((char *)(t)->tokptr)
    int toklen;
    char toktype;
#define TOKATM  'A'
#define TOKQSTR '"'
#define TOKCMA  ","
#define TOKLTP  "("
#define TOKRTP  ")"
#define TOKLTB  "<"
#define TOKRTB  ">"
#define TOKAT   "@"
    char unused1;
    char unused2;
    char unused3;
};

struct mbox {
    struct mbox *mbxnext;
    struct mbox *mbxprev;
    struct node *mbxrtl;
#define MBOXRTL(a) ((struct node *)(a)->mbxrtl)
    int mbxrcnt;
    struct node *mbxlcl;
#define MBOXLCL(a) ((struct node *)(a)->mbxlcl)
    int mbxlcnt;
    struct node *mbxdml;
#define MBOXDML(a) ((struct node *)(a)->mbxdml)
    int mbxdcnt;
};

struct node {
    struct node *nodnext;
    struct node *nodprev;
    struct tokend *noddown;
};

struct routd {
    char rtroute[8];
    char rtuser[8];
    char rtprog[8];
    char rtparm[8];
    char rttype[2];
    char rtflags;
#define RTP1 0x80
#define RTP2 0x40
#define RTP3 0x20
    char rtrsvd;
    int *rtaddr;
    int rtmsgcnt;
    int rtmsgfai;
    int rtmsglen;
    int rtmsgmax;
};

struct actadr {
    struct actadr *actnext;
    struct mbox *actmbox;
    struct routd *actrt;
    struct actadr *actchn;
};

struct vald {
    char vauser[8];
    char vaprog[8];
    char vaparm[8];
    int *vaaddr;
    int vamsgcnt;
    int vamsgfai;
    int vamsglen;
    int vamsgmax;
};

struct trcent {
    char trctype[8];         /* Entry type                           */
    char trcroutn[8];        /* Name of routine making entry         */
    int trcaddr;             /* Address at which entry made          */
    char trcdata[44];        /* Use defined by entry type            */

};
