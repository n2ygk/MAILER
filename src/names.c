/*-------------------------------------------------------------------*
 *    MAILER -- Copyright (c) 1982,1987 Columbia University.         *
 *  Program Property of the Trustees of Columbia University in the   *
 *  City of New York.                                                *
 *-------------------------------------------------------------------*/
/*
 * names.c:  Routines to deal with VM/SP NAMES files.
 *
 * namefind(file, names, func) - look in NAMES file "file" for the
 *      list of tags to match/fill in in the "names" array and call
 *      func once for each one found or until func returns non-zero.
 *
 * nextname(filep, names) - gets next NAMES file entry and fills in
 *      requested values from the names array.
 *
 * extern int namenode when set non-zero causes ":node" to be used as
 * an alias for :nick.  This makes it usable with the EARN NODES files.
 *
 * Alan Crosswell,  Columbia University Center for Computing Activities
 */

#include <stdio.h>
#include <ctype.h>
#include "names.h"
#define MAXDATA 4096

#define assert(X,Y) if(!(X)) error("assertion failed: %s\n",Y)

 /* use case-insensitve compare instead: */
#define strcmp(A,B) Xstrcmp(A,B)
int namenode = 1;

namefind(file, names, func)
char *file;
struct names names[];
int (*func)();
{
    FILE *filep;
    struct names *ncopy;
    int i, nnames;

    if ((filep  = fopen(file, "r")) == NULL)
      return -1;

    /* find size of 'names' and copy tags portion of it. */
    for (nnames = 0; names[nnames].n_tag != NULL; nnames++);
    ++nnames;
    ncopy = calloc(nnames, sizeof (struct names));
    for (i = 0; i < nnames; i++)
      ncopy[i].n_tag = names[i].n_tag;
    while (nextname(filep, ncopy)) {    /* look for matches */
        int matchflag = 0;

        for (i = 0; i < nnames-1; i++) {
            if (names[i].n_val != NULL) {
                matchflag = 0;
#ifdef DEBUG2
                printf("comparing %s: '%s' to '%s'\n",names[i].n_tag,
                       names[i].n_val, ncopy[i].n_val);
#endif DEBUG2
                if (strcmp(names[i].n_val, ncopy[i].n_val) == 0)
                  ++matchflag;
                else
                  break;
            }
        }
        if (matchflag)
          if (!(*func)(ncopy))
            break;
    }
    free(ncopy);
}

/*
 * nextname: find next name entry and fill in requested value fields.
 * Name entries are delimited by their ":nick" tags.
 * Return true until EOF and then return false.
 *
 * Format of a names file is:
 *  :nick.something (more tags)
 *   ...
 *  :nick.next thing
 *
 */

nextname(fp, nm)
FILE *fp;
struct names nm[];
{
    int c;
    static char dbuf[MAXDATA];
    static char *db, *maxdb = &dbuf[MAXDATA-1];
    char *curtag, *curdata;
    static enum {intag, indata, initial, nickseen} state = initial;
    static int firsttime = 1;
    static int cardcol = 0;
    int leading;
    struct names *n;
#ifdef DEBUG
    static char cbuf[256], *cb;
#endif DEBUG

    if (feof(fp)) {
        state = initial;
        firsttime = 1;
        cardcol = 0;
        return 0;
    }

    for (;;) {
        if (c == EOF) {
          cardcol = 1;
          return 1;
        }
        c = getc(fp);
        cardcol++;
        if ((cardcol == 1) && (c == '*')) {
#ifdef DEBUG
          cbuf[0] = '*';
          cb = &cbuf[1];
#endif DEBUG
          while ((c != '\n') && (c != EOF)) {
            c = getc(fp);
#ifdef DEBUG
            *cb++ = c;
#endif DEBUG
          }
#ifdef DEBUG
          *cb++ = '\0';
          printf("%s\n",cbuf);
#endif DEBUG
          cardcol = 0;
          continue;
        }
        switch(state) {
          case initial:
            leading = 1;
            switch (c) {
              case ':':
                     state = intag;
                     curtag = db = dbuf;
                     break;
              case ' ':
              case '\t':
                     break;
              case '\n':
                     cardcol = 0;
                     break;
              case EOF:
                     return 0;
              default:
                     error("Not a NAMES file.");
                     break;
            }
#ifdef DEBUG
            printf("initial\n");
#endif DEBUG
            break;
          case nickseen:        /* :nick was last thing seen */
            if (c == EOF) return 0;
            strcpy(dbuf,"nick");
            curtag = dbuf;
            curdata = db = &dbuf[5];
            *db++ = c;
            state = indata;
#ifdef DEBUG
            printf("nickseen\n");
#endif DEBUG
            break;
          case intag:
#ifdef DEBUG
            printf("intag\n");
#endif DEBUG
            switch(c) {
              case '.':                 /* end of tag */
              case '\n':
              case EOF:
                if (c == '\n') cardcol=0;
                assert((db < maxdb),"db < maxdb");
                *db++ = '\0';
                curdata = db;
                state = indata;
#ifdef DEBUG
                printf("tag is \"%s\"\n", curtag);
#endif DEBUG
                if (strcmp(curtag,"nick") == 0
                    || (namenode && strcmp(curtag,"node") == 0)) {
#ifdef DEBUG
                    printf(":nick found\n");
#endif DEBUG
/* xxx */           state = nickseen;
                    if (firsttime)
                      firsttime = 0;
                    else {
                        return 1;
                    }
                }
                break;
              default:
                if (c == EOF) break;
                assert((db < maxdb),"db < maxdb");
                *db++ = c;
#ifdef DEBUG
                printf("'%c' ",c);
#endif DEBUG
                break;
            }
            break;
          case indata:
#ifdef DEBUG
            printf("indata\n");
#endif DEBUG
            switch (c) {
              case '\n':
                leading = 1;            /* leading blanks flag */
                cardcol = 0;
                assert((db < maxdb),"db < maxdb");
                *db++ = ' ';
#ifdef DEBUG
                printf("'\\n' ");
#endif DEBUG
                break;
              case ' ':
              case '\t':
                if (!leading) {
                    assert((db < maxdb), "db < maxdb");
                    *db++ = ' ';
                }
                break;
              case ':':
              case EOF:
                leading = 0;
                while (isspace(*--db)); /* rtrim */
                ++db;
                assert((db < maxdb),"db < maxdb");
                *db++ = '\0';
#ifdef DEBUG
                printf("curdata is \"%s\"\n", curdata);
#endif DEBUG

                for (n = &nm[0]; n->n_tag != NULL; n++)
                  if (strcmp(n->n_tag, curtag) == 0) {
                      n->n_val = curdata;
#ifdef DEBUG2
                      printf("::%s.%s\n", n->n_tag, n->n_val);
#endif DEBUG2
                      break;
                  }
                curtag = db;
                state = intag;
                break;
              default:
                if (c == EOF) break;
                leading = 0;
                assert((db < maxdb),"db < maxdb");
                *db++ = c;
#ifdef DEBUG
                printf("'%c' ",c);
#endif DEBUG
                break;
            }
            break;
          default:
            error("Unknown state");
            break;
        } /* switch */
    } /* for */
}

error(s,a1,a2,a3,a4,a5)
char *s;
char *a1,*a2,*a3,*a4,*a5;
{
    fprintf(stderr,s,&a1);
    exit(1);
}
