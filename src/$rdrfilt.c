/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1985 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
/*
 * _rdrfilt - filter reader files.  New code as of version 1.22
 *
 * This routine peeks at the input reader file to see if it is
 * something other than a plain RFC822 mail file (such as BSMTP
 * or a BSMTP reply).  If it is plain mail,  then it returns a
 * 0 value which tells mailmain to continue as if this routine
 * had never been called.
 *
 * For other files,  this routine takes over the functionality
 * previously in mailmain and does the necessary processing.
 * A return value of 4 indicates successful completion which tells
 * mailmain to purge the input file.  Any other non-zero value
 * indicates a fatal error.
 *
 * Alan Crosswell, 12/85
 */

#include "types.h"
#include "mailblok.h"
#include "cfuncs.h"

extern bcmp();
extern smtpin();
extern smtprep();

#define RMAXRECL 132

_rdrfilt(mb)
int mb;                           /* mailblok ptr (not needed) */
{
    char rbuf[RMAXRECL];
    int rc, len;

    if ((rc = cpcmd("READY 00C",9)) != 0)
        return(rderr(rc));
    while (1) {
        if ((len = rdcard(rbuf, (int)mailblok.mailsfb.sfbrecsz)) < 0)
            return rderr(rc);
        if (len >= 5 && (bcmp(rbuf,":READ",5) == 0))
            continue;             /* skip over the :READ header */
        /* now check for various 1st cards:
         *  HELO for a BSMTP request
         *  xxx  (3 digits) for a BSMTP reply
         */
        if (len >= 4 && (xcmp(rbuf,"HELO",4) == 0)) {
            rc = smtpin();        /* pass control to smtp input code */
        } else if (len >= 4 && isdigit(rbuf[0]) && isdigit(rbuf[1])
                 && isdigit(rbuf[2])) {
            rc = smtprep();       /* pass control to smtp reply code */
        } else {                  /* just a plain mail file */
            rc = 0;               /* return like nothing happened */
        }
        cpcmd("CLOSE 00C",9);     /* close the reader file */
        return rc;
    }
}

static
rderr(rc)
int rc;
{
    wtr("RDR",45,1,"d",rc);
    return 12;
}

