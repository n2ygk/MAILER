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

extern mailblok;
extern int mailhbuf[2];         /* addr, len of header buffer */
extern char upcase[256];        /* the upper-case translate table */
extern struct sfblok mailsfb;   /* sfblok for current reader file */
extern int stackp[2];           /* stack pointer, length */
extern struct fscb *mailulfs;   /* error log fscb pointer */
extern char *lcldom;            /* local-domain string */
extern char *rscsdom;           /* rscs-domain string */
extern char cpustr[8];          /* rscs node name */
extern int cpuslen;             /*  length */
extern char rscsname[8];        /* rscs vmid */
extern char mailvmid[8];        /* mail vmid */
extern char mailtime[16];       /* time of last start/reload */
extern char mailvers[16];       /* mailer version, assembly date */
extern char mailnode[8];        /* origin node name */
extern int mailnlen;            /*  length */
extern char mailuser[8];        /* origin userid */
extern int mailulen;            /*  length */
extern struct actadr *mailchn;  /* action address list */
extern struct fieldd *mailfld;
extern struct fieldd *mailsfld;
extern struct tbtab *mailktab;
extern struct tbtab *maildtab;
extern struct tbtab *mailrtab;
extern struct tbtab *mailvtab;
extern struct tbtab *mailptab;
extern int mailhrec;
extern int mailhlin;
extern char mailftxt[];
extern char mailopts;
#define MOPTDEF 0x80
#define MOPTAUN 0x40
#define MOPTAC  0x20
#define MOPTVL  0x10
extern char mailflg1;
#define MFLGDEST 0x80
#define MFLG1LCL 0x40
#define MFLG1GTW 0x20
#define MFLG1EDI 0x10
#define MFLG1REJ 0x08
#define MFLG1HDS 0x04
#define MFLG1DOT 0x02
#define MFLG1RES 0x01
extern char mailflg2;
#define MFLG2SMT 0x80
#define MFLG2VLF 0x40
#define MFLG2NOR 0x20
#define MFLG2NLF 0x10
#define MFLG2NOH 0x08
extern int maxrcvd;
/* extern struct fscb ulfscb */
/* extern struct trcent *trtblst  */
/* extern struct trcent *trtblcur */
/* extern struct trcent *trtblend */
extern char maintid[8];         /* maintainer's userid */
extern char maintnod[8];        /* maintainer's node name */
extern char maintid2[8];        /* maintainer's userid */
extern char maintno2[8];        /* maintainer's node name */
extern char maintid3[8];        /* maintainer's userid */
extern char maintno3[8];        /* maintainer's node name */
extern char maintid4[8];        /* maintainer's userid */
extern char maintno4[8];        /* maintainer's node name */
extern char maintid5[8];        /* maintainer's userid */
extern char maintno5[8];        /* maintainer's node name */
extern char maintid6[8];        /* maintainer's userid */
extern char maintno6[8];        /* maintainer's node name */
extern char maintid7[8];        /* maintainer's userid */
extern char maintno7[8];        /* maintainer's node name */
extern char maintid8[8];        /* maintainer's userid */
extern char maintno8[8];        /* maintainer's node name */
extern char maintid9[8];        /* maintainer's userid */
extern char maintno9[8];        /* maintainer's node name */
