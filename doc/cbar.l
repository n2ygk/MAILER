.TH CBAR LOCAL
.SH NAME
cbar \- add change bars to a document
.SH SYNOPSIS
.B cbar
[
.I bar-on bar-off
]

.SH DESCRIPTION
.B Cbar
inserts text-formatter commands into diff -e output to turn
on and off change bars.  This can then be fed back into 
.B ed(1)
along with the original document to produce a new document
that contains change bars wherever the new and old version of
a document have differences.

.I bar-on
and
.I bar-off
may be specified as the commands to turn on and off change bars,
respecively.  If not specified, 
.B @CBON
and 
.B @CBOFF
(for Scribe) are used.

.SH EXAMPLE
.nf
diff -b -e old.mss new.mss | cbar | ed - old.mss >bars.mss
.fi
.SH SEE ALSO
diff(1), ed(1)

.SH BUGS
.B ed(1)
commands on stdin must appear in decreasing line number order
so that lines at the end of the file get edited first and lines at
the beginning get edited last (diff -e does this).
This restriction exists because additional lines are
added to the file which will mess up the line numbering scheme if
done in lowest-to-highest order.
