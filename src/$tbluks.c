/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1985 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
/*
 * _tbluks(table_ptr, test_str, test_len, work_ptr)
 *
 * Sequential version of TBLUK.  Called from TBLUK when the table
 * options flag indicates a sequential table (the usual table is
 * sorted).  It should check the right-adjust match flag too,  but
 * since that's the only reason this routine was written,  it doesn't
 * for now.
 *
 * Patched to check for exact match or "." at or just before
 * start of match, to correct domain selection.
 * (Still assumes the right-adjust flag, plus domain use.)
 * *** Code courtesy of Mark Williamson (MARK@RICE).
 *
 * Since the assembler interface expects return values in R0 and R1
 * we play some games with the stack:  On entry,  the value of the
 * OS caller's R13 is on the stack immediately before the first
 * parameter.  Use the address of the first parameter -4 to find
 * R13 and then stuff the R0 and R1 values into the caller's save
 * area:
 *        -4 |  R13  |---->OS save area: stm 14,12,12(13)
 *           +-------+
 *         0 | arg 1 |
 *           +-------+
 *
 * Alan Crosswell, 11/85
 */
#include "types.h"
#include "cglue.h"
#include "mailblok.h"

extern int bcmp();

_tbluks(tp, sp, sl, wp)
struct tbtab *tp;
char *sp;
int sl;
struct {
    char tab[128];                /* place for upcase'd table entry */
    char tst[128];                /* place for upcase'd test entry */
} *wp;                            /* work area pointer */
{
    int i;
    register struct tbentry *te;
    struct ossave **stack = (struct ossave **)&tp - 1;

    if (sl < 0) {
        wrterm("*** Error *** _tbluks called with negative length");
        return TBNOT;
    }

    upcvt(sp,wp->tst,sl);         /* copy test string and upcase it */
    for (i = 0,te = tp->tbt_adr; i < tp->tbt_len; i++,te++) {
        register int tl = te->tbe_str->tbs_siz;

        if (tl > sl) continue;    /* table string must be a substring */
        upcvt(&te->tbe_str->tbs_str, wp->tab, tl);
        if (bcmp(&wp->tst[sl-tl],wp->tab, tl) == 0) {
          if ((wp->tab[0]=='.')  /* if table has dot */
           || (sl == tl)         /* or full-length match */
           || (wp->tst[sl-tl-1]=='.')) { /*or addr has dot*/
            (*stack)->r0 = te;      /* r0 gets address of tbentry */
            (*stack)->r1 = te->tbe_dsp; /* dispatch value */
            return TBMATCH;       /* found a match */
          }
        }
    }
    return TBNOT;
}

upcvt(src, dst, len)
char *src, *dst;
int len;
{
    while (len--)
        *dst++ = mailblok.upcase[*src++];  /* translate to upper case */
}

/*
 * ctbadd:  Call assembler tbadd function from C.
 */
extern tbadd();
ctbadd(tab_ptr, tbe_ptr)
struct tbtab *tab_ptr;
struct tbentry *tbe_ptr;
{
    struct {
        struct tbtab *tabp;
        struct tbentry *tbep;
        char *workp;
    } pl;
    char workarea[512];

    pl.tabp = tab_ptr;
    pl.tbep = tbe_ptr;
    pl.workp = workarea;
    return acall(tbadd,0,&pl);
}
