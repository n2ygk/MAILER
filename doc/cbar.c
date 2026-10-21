static char *RCSid = "$Header$";
/* 
 * cbar [bar-on bar-off]
 *
 * Inserts change bar commands around diff -e output.  
 *
 * Alan Crosswell, Columbia University
 *
 * Tacks on a "1,$p" at the end so that you can do something like this:
 *
 *  diff -b -e old.mss updated.mss | cbar | ed - old.mss >updated-bar.mss
 *
 * Input looks like this:
 *------------------------------
 * 746,747d
 * 138a
 * NOTE: Change this for LISTSERV.
 * .
 * 89c
 * followed by a blank or hyphen (e.g. "220 ").
 * .
 *------------------------------
 * Output looks like:
 *------------------------------
 * 746,747d
 * 138,138a
 * @CBON
 * NOTE: Change this for LISTSERV.
 * @CBOFF
 * .
 * 91,91c
 * @CBON
 * followed by a blank or hyphen (e.g. "220 ").
 * @CBOFF
 * .
 * 1,$p
 *------------------------------
 *
 * NOTE:  It is imperitave that the line numbers appear in highest-first
 *  order since extra lines are being added.
 *
 * The default change bar commands are those used by Scribe (@CBON, @CBOFF).
 * You may provide your own (e.g. cbar ".mc |" ".mc" for nroff).
 *
 * $Log$
 * 
 */
#include <stdio.h>

static char *cbon = "@CBON";
static char *cboff = "@CBOFF";

main(argc,argv)
int argc;
char **argv;
{
    char buf[1024];
    enum {ed_cmd, in_data} state = ed_cmd;
    
    if (argc == 3) {
	cbon = argv[1];
	cboff = argv[2];
    } else if (argc != 1) {
	fprintf(stderr,"Usage: cbar [bar_on bar_off]\n");
	exit(1);
    }

    while(gets(buf) != NULL) {
	switch (state) {
	  case ed_cmd:
	    if (buf[strlen(buf)-1] == 'd') {
		state = ed_cmd;	/* 'd' has no data */
		puts(buf);
	    } else {
		state = in_data; /* 'a' and 'c' do */
		printf("%s\n%s\n", buf, cbon);
	    }
	    break;

	  case in_data:
	    if (buf[0] == '.' && buf[1] == '\0') { /* end of data */
		state = ed_cmd;
		printf("%s\n.\n", cboff);
	    } else
	      puts(buf);
	    break;
	} /* end switch(state) */
    } /* end while */
    puts("1,$p");
}
