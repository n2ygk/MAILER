/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1985 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
/*
 * CMS control block definitions.
 */
#ifndef _CMSBLOKS
#define _CMSBLOKS 1
#ifndef _TYPES
#include "types.h"
#endif _TYPES

struct sfblok {
    struct sfblok *sfbpnt;
    unsigned sfbstart;
    char sfbuser[8];
    char sfborig[8];
    unsigned sfbrecno;
    u_short sfbrecsz;
    u_short sfbfilid;
    u_char sfbflag;
#define SFBINUSE 0x80
#define SFBRECOK 0x40
#define SFBUHOLD 0x20
#define SFBDUMP  0x10
#define SFBOPEN  0x08
#define SFBSHOLD 0x04
#define SFBEOF   0x02
#define SFBRECER 0x01
    u_char sfbtype;
    u_short sfbmisc1;
    unsigned sfbrecs;
    char sfbfname[12];
    char sfbftype[12];
    char sfbdate[8];
    char sfbtime[8];
    unsigned sfblast;
    u_short sfbcopy;
    u_char sfbclas;
    u_char sfbflag2;
#define SFBHOLD  0x80
#define SFBNOHLD 0x40
#define SFBFLNMT 0x20
#define SFBREQUE 0x20
#define SFBRSTRT 0x10
#define SFBTICER 0x08
#define SFBPURGE 0x04
#define SFBFIRST 0x02
#define SFBMON   0x01
    char sfbdist[8];
    char sfbflash[4];
    u_char sfbstcpy;
    u_char sfbflag3;
#define SFBLDBEG 0x80
#define SFBLDMID 0x40
#define SFBFCB   0x20
#define SFBSEEN  0x02
#define SFBXFER  0x01
    u_char sfbckpmp;
    u_char sfbrsvd2;
};

struct fscb {
    union {
        double aligned;
        char fscb_comm[8];
    } fscb_u;
#define fscbcomm fscb_u.fscb_comm
    char fscbfn[8];
    char fscbft[8];
    char fscbfm[2];
    u_short fscbitno;
    char *fscbbuf;
    unsigned fscbsize;
    char fscbfv;
    char fscbflg;
#define FSCBITAV 0x40
#define FSCBEPL  0x20
#define FSCBRCAV 0x01
    u_short fscbnoit;
    unsigned fscbnord;
    unsigned fscbaitn;
    unsigned fscbanit;
    int fscbwptr;
    int fscbrptr;
};
#endif _CMSBLOKS
