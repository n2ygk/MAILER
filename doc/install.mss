@comment{Installation instructions for Mailer V1.0  -  Monday, 24 May 1982}
@make(manual)
@string(Version="1.24")
@commandstring(M="@i<MAILER>")
@case[device, imprint10 "@style(singlesided)",
	      postscript "@style(singlesided)@modify(example,size -2)",
	      x9700 "@style(doublesided)",
	      lpt "@style(singlesided)@style(CarriageControl Fortran)",
	      ln03 "@style(singlesided)"]
@style(filedate="8 March 1952")
@style(hyphenbreak off)
@style(singlesided)

@begin(titlepage)
@begin(TitleBox)
@majorheading(Columbia VM MAILER)
@heading(Version @value(version))

@majorheading(Installation and Maintenance Guide)

24 May 1982
(last updated: @value(filedate))

E. Alan Crosswell
EACUS@@CUVMA.COLUMBIA.EDU
Scholarly Information Center
Columbia University in the City of New York
@end(TitleBox)

@begin(ResearchCredit)
@center<Copyright (C) 1982, 1987 The Trustees of Columbia University
in the City of New York>
@end(ResearchCredit)
@case[device, x9700 "@blankpage(1)"]
@end(TitlePage)
@set(page=1)
@pageheading(immediate, center="@M Installation and Maintenance")
@pagefooting(immediate, center="- @value(page) -")

@section(Introduction)

This document describes the installation, operation and maintenance
procedures for the Columbia University VM @M.  Please read sections
@ref(COPYR) and @ref(TERMS) on the terms under which @M is provided to
you and destroy the files that have been provided if you are unable to
abide by them.

Section @REF(RESPONS), below, briefly gives a job description for the
person or persons required to maintain @M at your site.  Sections
following that go into the details of installing @M the first time and
maintaining it and the data it needs to operate on BITNET.  The
information in these sections is written with the assumption that the
reader is conversant with the basic concepts of VM/SP and RSCS
networking.  No attempt is made to define terms.

@M was developed by E. Alan Crosswell, an employee of Columbia
University in the City of New York.  It is the property of the
Trustees of Columbia University in the City of New York.  Some
portions of the code have been contributed by @M users on BITNET
(their names are cited within the source code).  @M is written in IBM
370 Assembly language and C.

@subsection(Copyright Notice)@label(COPYR)
Copyright (c) 1982, 1987, The Trustees of Columbia University in the
City of New York.

Permission is granted to any individual or institution to use, copy,
or redistribute this software so long as it is not sold for profit,
provided that this notice and the original copyright notices are
retained.  Columbia University makes no representations about the
suitability of this software for any purpose.  It is provided "as is"
without express or implied warranty.

May be used but not sold without permission.

@subsection(Additional Terms for Waterloo C Library Object Code)@label(TERMS)
Some C programs provided with this distribution make use of the
Waterloo C Runtime Library, a component of Waterloo C, a licensed
software product of WATCOM Products, Inc. of Waterloo, Ontario,
Canada.  WATCOM has granted us permission to distribute the object
code for the runtime library to "sublicensees" so that sites without
Waterloo C may use @M with the following terms:
@begin(enumerate)
That we include limitations on WATCOM's liability, limitations of warranty,
and restrictions on further distribution similar to those included
in our Waterloo C license agreement.

We must maintain records of all copies of the library object code
distributed to sublicensees.
@end(enumerate)

No liability is assumed by WATCOM Products, Inc. for any special,
direct, indirect or consequential damages, losses, costs, fees or
expenses of any nature or kind.

You may freely give @M source code to others but may not redistribute
the @M object code module (MAILER MODULE) or C object code library
(CLIB TXTLIB) as they contain Waterloo C library object code.

In the event that Columbia University's license with WATCOM 
for Waterloo C is terminated,  you will be required to destroy any
copies of @M object code that contain Waterloo C library object
code.

@subsection(Support)

@M is supplied with documentation, parameter files, and source and/or
object code depending on the type of distribution you have requested.
If you have source code and make substantial modifications that are
of a generally useful nature,  please send those modifications back
to us so that they may be considered for inclusion in future versions.

@M is distributed over the BITNET computer network free of charge to
any member site.  Columbia University provides @M on an as-is basis
and will not provide any support.  You are expected to have the local
expertise required to install and maintain @M or to obtain such
expertise elsewhere.  (There are a large number of people on BITNET
that have this expertise and who may be willing to help you.)

@section(@M Maintainer and Postmaster Job Description)@label(RESPONS)
There are actually two jobs involved in running a mail service.
One is that of maintaining and updating the mail software and
databases and the other is to perform the duties of Postmaster.

The @M maintainer's responsibilities include:
@begin(enumerate)
Installation and configuration of the initial @M distribution.

Occassional installation of @M software updates (typically on the
order of once or twice a year but sometimes more frequently).

Monthly acquisition and installation of the BITNET database used by
@M.  This database is maintained by the BITNET Network Information
Center (BITNIC) and is distributed monthly over the network (or
sometimes by tape during periods of network instability).  This
maintenance is related to the other monthly BITNET node maintenance
work (e.g. updating the RSCS DIRECT).
@end(enumerate)
None of these responsibilities tend to be particularly time-consuming
although it depends on the individual's familiarity with VM and would
best be performed (or supervised) by a systems programmer.  There is
the potential for an incorrect @M installation to crash your VM system
(by filling up CP SPOOL space) or to "clobber" the network (by causing
mail loops).

The Postmaster's responsibilities include:
@begin(enumerate)
Answering local users' questions about the mail system.

Responding to queries from outsiders (typically requesting
help in reaching a particular member of your site's community).

Diagnosing miscellaneous mail system errors.  For example, @M sends
"error mail" that can't be automatically dealt with to the Postmaster.
@end(enumerate)

The Postmaster should read his or her mail frequently (many do it one
or more times daily).  The degree of work involved in being the
Postmaster depends on how much service you want to provide to your
community and outsiders.  For example, you may be willing to spend a
lot of time answering requests for the electronic address of Professor
Smith or you may prefer to suggest that the requestor simply call
Professor Smith on the telephone to find out her userid.  At Columbia,
the daily Postmaster duties are peformed by a trained student with
student training, back-up and difficult questions handled by a
full-time staff member.

@section(Inventory of Installation Materials)

The following items have been supplied to you.  Please verify
that they are all present.  Section @ref(ADDTL-FILES) describes 
additional files you will need to obtain in order to use @M on BITNET.

@subsection(Object Code Distribution)
@begin(description,leftmargin +22,indent -22)
@b<"INSTALL LISTING">@\this document.

@b<"MCONFIG EXEC">@\used to configure Mailer.
	
@b<"MAILER TXTLIB">@\library of exit routines.

@b<"MAILER MODULE">@\main program.  Contains licensed software (see
section @ref(TERMS), above).

@b<"MAILER HELPCMS">@\general command description.

@b<"MAILER HELPPRIV">@\privileged command description.

@b<"PROFILE EXEC">@\to run @M as an AUTOLOG'd disconnected virtual
machine.

@b<"SITEOPTS EXEC">@\to set site-specific options.  The default options will
probably be fine for you.

@b<"PLM LISTING">@\@M Program Logic Manual (usually not up to date).

@b<"MG MODULE">@\program to facilitate automatic
regeneration of "MAILER PROFILE" from "MAILER MTPLATE" and "MAILER MAILLCL".
Contains licensed software.  See section @ref(TERMS),  above.
@end(description)

@subsection(Source Code Distribution A -- for sites with Waterloo C)
The source code distribution is sent to you as the contents of three disks and
is free of the Waterloo C library restrictions.  This means you will need
a Waterloo C compiler to modify and rebuild @M.
@begin(enumerate)
Base source.

Updates and CNTRL files.

Miscellaneous execs and maintenance tools.
@end(enumerate)

@subsection(Source Code Distribution B -- for sites without Waterloo C)
This distribution contains everything in source distribution A as
well as the following files:
@begin(description,leftmargin +22,indent -22)
@b<"MAILGEN TXTLIB">@\Library of TEXTs for all @M assembler and C sources.

@b<"CLIB TXTLIB">@\Waterloo C runtime library.  This is bound by the WATCOM
sublicense terms.
@end(description)

@Section[Additional Files Needed for BITNET]@label<ADDTL-FILES>
Besides the files provided with @M,  you will need to obtain several
other files from BITNIC which are part of the database of mail system
information.  You will need to obtain a new copy of these files each
time they become available (once monthly).
@begin(description,leftmargin +22,indent -22)
@b<"XMAILER NAMES">@\The subset of the node information database necessary
to generate a new MAILER PROFILE.

@b<"MAILER NAMES">@\"Mail-only" host names which represent systems reachable
by @M which are not actual BITNET nodes (and are therefore not in the nodes
database).

@b<"DOMAIN NAMES">@\Information about Internet domains.
@end(description)

@section(Your @M subscription and registration)

Distribution of @M updates was formerly based on a subscription in the node
information database maintained by the BITNET Network Information
Center (BITNIC).  This same database is used to register the existence
of your @M virtual machine so that other mailers know about it.  The
":inform@i<X>" and ":info@i<X>" tags were used for your @M subscription.
The ":mailer" tag registers your node's @M.  Updated registration
information is distributed once monthly by BITNIC in the @b<"XMAILER NAMES">
file (see section @ref<ADDTL-FILES>).

The ":mailer" tag is still used to register your @M, but Eric Thomas'
Revised LISTSERV is now the system used to maintain your @M subscription.
All previously ":info/:inform"-subscribed users have been moved to LISTSERV.

@subsection(Subscription via LISTSERV)
To maintain your "subscription" for @M bug fixes and enhancements, you
need to subscribe to the "MAILDIST" mailing list provided by LISTSERV
at CUVMA.  By default, your subscription to this list will only get
you mail announcing new @M file shipments.  To get the files as well,
you will want to change your subscription options to include files.
In this manner, you can hear about @M changes without actually getting
the code distribution if someone else at your site has that responsibility.

To subscribe to the @M distribution (by the time you receive this document
you are already subscribed) send the following command as a message or
mail to LISTSERV at CUVMA:
@begin(example)
SUBSCRIBE MAILDIST your name
@end(example)

To get the code distribution as well as announcements, after subscribing,
send:
@begin(example)
SET MAILDIST FILES
@end(example)

If you maintain the @M for multiple machines, you need only
do this for one of them.  This will keep the flow of unneccessary
files to a minimum.

@subsection(Registration :mailer tag)
Finally,  to tell the rest of us that you have a @M,  you need
to set up the @b<:mailer> field.  Initially,  when you first get @M
(or beforehand),  the field should read:
@begin(example)
:mailer.@i<vmid> DEFRT 1
@end(example)
where @i<vmid> is the userid of your @M machine.  This will
allow you to send mail via other sites' @i<Mailers> to try things out.
Normally,  you would need to wait until the new @b<"XMAILER NAMES"> file has
been distributed and other sites have had time to update their Profiles.
If you send to a @M site and get return mail saying "Validity
check failed,"  then that site hasn't updated its Profile yet.  (See
section @REF<MESSAGES> for a list of the various messages generated
by @M.)

Since this has become more and more of a problem as the size of the
network has grown and the lag time between distributions of new
versions of @B("XMAILER NAMES") has increased, the initial @b("MAILER
PROFILE") that is generated by MCONFIG for your initial @M
installation is set up to bypass all other mailers.  This is not an
ideal solution since you lose some of the benefits of running @M,
but at least you won't get a lot of "Validity check" mail returned.
Once a new @b("XMAILER NAMES") has been distributed that lists your
site as having a Mailer (or if your site is already listed that way)
then you can regenerate the @b("MAILER PROFILE") as described below.

Once you have put @M into production use (i.e. it is always
logged on) you will want to change the field to read:
@begin(example)
:mailer.@i<vmid>
@end(example)
This will cause other @i<Mailers> to send mail for your site via your
@M rather than via direct RSCS routing.

@section(Installation Procedure)
@begin(enumerate)
Create a VM userid@foot[Please
don't change the userid without first updating your node database
entry and waiting for it to be redistributed to all @M sites.  You may
end up having to run two copies of @M during the transition period;
one under the old name and one under the new name to avoid any
disruption in mail traffic.] for the @M disconnected machine.  It should have a
virtual storage size of 1M and a 191 disk of adequate size to hold the largest
mail file you anticipate receiving.  If you give it the appropriate CP
privilege class for the @b<MSGNOH> command then it will use that
command to send messages.  If not, it will use @b<MSG>.  

Obtain write access to a CMS minidisk to which @M disconnected virtual
machine will have read access.  It is recommend that this be other
than the A-disk.  @M requires a writeable A-disk for various temporary
files it creates.  The disk that the @M parameter files and TXTLIB
reside on would best be owned by the @M maintainer.  This will allow
ease of future updates without interrupting normal operations.

Copy the supplied files to the disk.

Obtain access to a disk containing the files listed in section
@REF<ADDTL-FILES> (or put them on the same disk @M is on).

Run @b<MCONFIG>.  It is self-explanatory.

@M is now ready to run.  The @B<"PROFILE EXEC"> supplied is
designed for AUTOLOG use.  The best way to run @M is
to have @i<VMUTIL> or @i<AUTOLOG1> AUTOLOG it.  You may however log on directly
and start it that way.
@end(enumerate)

@section(Overview of @M Operation)
This is a brief description of what @M does.  When
first started, @M reads the Mailer Profile, allocates
various tables, and loads any user exits that are not built in.  It
then goes into a wait state, waiting for the arrival of a reader file,
an SMSG interrupt, or a console interrupt.

If the interrupt was an SMSG or from console typein,  the command
parser is invoked to process the user command.  For reader interrupts,
each file in the reader is read and processed,  usually resulting
in the generation of outgoing punch spool files.

A valid reader file can be one of:  
@begin(enumerate)
An RFC822 mail file.  A mail file is a PUNCH spool file containing a header
seperated by a blank record from the text of the message.  The syntax
specification for the header format is derived from the @i<Standard
for the Format of ARPA Internet Text Messages>,  DARPA Request for
Comments #822.  NOTE: Only a subset of RFC822 is supported by @M.
Specifically,  it fails to properly parse nested parentheses and
route-lists.  The various Internet RFC's are available as 
@b<"RFCxxx STANDARD"> from NICSERVE@@BITNIC.

A Batch Simple Mail Transfer Protocol (BSMTP) request.  A BSMTP
request is differentiated from an RFC822 mail file by having the first
record start with the text "HELO ".  This is the first command
of an SMTP dialog and looks sufficiently different from a mail
file to be easily identified.

A reply to a BSMTP request previsouly generated.  A BSMTP
reply is identified by having the first record start with 3 digits
(e.g. "220").
@end(enumerate)

For both mail files and BSMTP requests,  @M does essentially
the same thing.  It parses the mail or BSMTP request to obtain a
list of recipients.  In the case of BSMTP,  this is simple -- each
recipient is identified by a RCPT command.  For RFC822 mail,  it is a little
more difficult to determine @i<which> addresses should be acted upon.
This is one reason that BSMTP support was added to @M.  Another
reson is that BSMTP has become the de-facto standard for interoperation
of various mail systems on BITNET.

Various table entries in the Mailer Profile determine exactly how the
mail is sent to the list of recipients.  Specifically, the INCOMING
table lists which other mailers your @M knows about and will trust.
Anyone not listed in the INCOMING table must send a mail file with a
Sender: or From: field that is consistent with the file's RSCS origin
node and userid.

The function of @M is described in further detail in the
@i<MAILER Program Logic Manual>,  "PLM LISTING."

@section(MAILER PROFILE)

The @B<"MAILER PROFILE"> CMS file contains the parameters used to
initialize the @M routing tables and set other variables and
option flags.  You may want to add or remove names from the list of
special userids not to receive mail.  See section @ref(MG), below, for
the procedures for modifying and automatically regenerating the
"MAILER PROFILE".

You can specify a different name for the Profile when invoking @M.
The syntax is:
@begin(example)
MAILER [@i<fn> [@i<ft> [@i<fm>]]]
@end(example)
The default values for @i<fn>, @i<ft>, and @i<fm> are @b<MAILER>, @b<PROFILE>,
and @b<*>,  respectively.  If, for example, your system has shared dasd 
between two cpu's (as ours does),  your "PROFILE EXEC" can invoke @M
with different Profile names depending on the cpuid.  This allows you to
have a single disk containing the @M program,  txtlib,  and profiles.

The parts of the Mailer Profile are described below:
@subsection(Variable Declarations)
You may declare a variable at any time within the Profile by entering:
@example(@i<variable_name>=@i<value>)
For example:
@example(rscsname=RSCS)

All variables declared this way are then available to any user exit
to obtain their value.  Some variable names have special meaning
to @M:
@begin(description)
RSCSname@\VMID of the RSCS/Networking server.  Files received from
this userid are assumed to be from the RSCS network.  The file's TAG
text is parsed for the network origin node and userid.  Files destined
to the RSCS network are sent to this VMID.

RSCSalt@\VMID of a secondary RSCS/Networking server.  This is useful
if you need to run two different RSCS's for some reason.  @M
will always send files to the primary RSCS (@i<rscsname>),  but will
accept files from this RSCS as being from the network.

ThisNode@\RSCS node name for the machine @M is running on.

RSCS-Domain@\The default domain name to concatenate with the RSCS node
name when domain-style names are needed.

Local-Domain@\An optional domain name for the machine @M is
running on if you want something different from the concatenation
of the RSCS node name and @i<RSCS-Domain>.

MaintUser@\VMID of the @M maintainer.  This user is allowed
to send privileged commands via CP SMSG and will also receive files
that @M has trouble with.

MaintNode@\RSCS node name of the @M maintainer.  For best
results,  this should be the same as @i<ThisNode>.

Supply-Default-Fields@\May have the value True or False,  but leave
it false since its functionality has not been implemeneted.

Allow-Unknown-Nodes@\May have the value True or False.  If True,
then @M will attempt to deliver mail to hosts that are
not listed in it's tables.  Useful when RSCS route updates get
done before @M Profile updates.  When False,  can be
used to restrict the list of sites that you may exchange mail
with.

Allow-Charlatans@\If true,  BSMTP requests are allowed to give
a domain name on the HELO command and/or a FROM address on the MAIL
command which do not appear to correspond to the RSCS origin.  For
a charlatan HELO,  the "Received:" header line lists that actual
RSCS origin in the comment (in parentheses) following the "from domain."
For a charlatan FROM, an "X-Delivery-Notice:" is added to the mail header.

Msgcmd@\The CP message command to use instead of MSGNOH or MSG.  Used
only by the LOCAL exit.

Msgprefix@\The prefix other than "*" to preceed messages with
Used only by the LOCAL exit.

Verbose-Logging@\If true,  all BSMTP commands and responses are printed
on the console.
@end(description)

@subsection(ORIGINS Table)
Entries in the ORIGINS table are keyed by the VM userid.  Any reader
file from a user listed in the table causes the associated ORIGINS
user exit program to be invoked prior to any other processing of
the file.  The list of available ORIGINS exits is listed in section
@ref(ORIGINS-EXITS) below.

@subsection(INCOMING Table)
Entries in the INCOMING table are keyed by RSCS origin node and userid.
After a file is received and its mail header has been parsed,  the
INCOMING user exit program is invoked.  A typical use of an INCOMING
exit is to do validation of mail to prevent forged "From:" userids.
The list of available INCOMING exits is listed in section @ref(INCOMING-EXITS)
below.

@subsection(OUTGOING Table)
Entries in the OUTGOING table are keyed by host name.  For this
discussion, @i<host name> is defined as everything to the right of the
at-sign in a mail address.  For example, in the address
@p<EACUS@@CUVMA>, @b<CUVMA> is the host name and, in the address
@p<John_Smith@@Newcastle.MAILNET>, @b<Newcastle.MAILNET> is the
hostname.  

When mail is to be sent to a particular address, @M looks up
the hostname in the OUTGOING table and then invokes the OUTGOING user
exit program.  If the hostname can't be found and the
@i<Allow-Unknown-Nodes> variable is @b<True>, then @M looks
for the special OUTGOING entry with the key "@b<*DEFAULT*>" and
invokes the user exit for that entry.  (See the description
of the DOMAINS table, below, for an exception).  The list of available OUTGOING
exits is in section @ref<OUTGOING-EXITS>, below.

@subsection(DOMAINS Table)
The DOMAINS table is an optional table that has the same basic
functionality as the OUTGOING table.  The difference is that the
key for an entry in the DOMAINS table need not match the hostname
exactly.  It need only match the right-hand end.  When a DOMAINS
table is defined,  @M first searces the OUTGOING table
and,  failing to find a match there,  searches the DOMAINS table
before finally giving up and going with the "*DEFAULT*" entry.
The DOMAINS and OUTGOING tables use the same exits.

@subsection(SPECIAL Table)
The key to an entry in the SPECIAL table is a username, that is,
everything to the left of the at-sign in a mail address.  The SPECIAL
table can be used to implement userid aliases of up to 255 characters
in length,  mailing lists,  automatic mail forwarding,  etc.
Note that the SPECIAL table is never searched by the @M proper.
It is only used if an OUTGOING exit (like LOCAL) chooses to.
A list of SPECIAL user exits is in section @ref<SPECIAL-EXITS>.

@section(@M User Exits and MAILER TXTLIB)

The commonly used user exit programs are built into @M.  Others reside
in MAILER TXTLIB and are dynamically loaded by @M when referenced in
one of the tables at initialization.

@subsection(Outgoing Exits)@label(OUTGOING-EXITS)
These exit names are used in the Profile OUTGOING and DOMAINS tables.
In addition to these standard exits, several sites have implemented
their own local exits for various functions such as gatewaying mail
over "home grown" or otherwise strange links to other systems.
Each exit has a type and a (sometimes optional) 8 character parameter.

There are three types of OUTGOING exit which determine exactly when
the exit is invoked and with what information.  Some exits may be used
as different types if they are properly written although they generally
are written to be of a specific type.  The three types are:
@begin(description,leftmargin +5,indent -5)
1@\Called once per recipient address.

2@\Called once for all recipients having the same server node/userid
pair and exit name.

3@\Same as type 2 but complete information about all recipients is
available.  This includes having different exit parameters available
to the same exit (e.g. BSMTP parameters).
@end(description)

The OUTGOING exits provide with @M are:

@paragraph[BITNET]
This obsolete type 2 exit sends a file to another RSCS node
assumed to be running @M. The file is sent to the server
node/userid listed in the table.  BITNET has been superseded
by the BSMTP exit.

@paragraph[BSMTP]
This type 3 exit sends a Batch Simple Mail Transfer Protocol
@foot(a store-and-forward version of the ARPANET SMTP service defined
by J. Postel in DARPA Request for Comments #821) data stream to
the server node/userid from the table.  A RCPT command is generated for
each recipient that caused the exit to be invoked.  The usual domain
name used to identify the sending host in the HELO command is either
@i<Local-Domain> or the concatenation of  @i<ThisNode> and @i<RSCS-Domain>.  

The optional parameter to BSMTP can be one of:
@begin(description)
@i<var>@\A variable name.  The value of the variable is used as the HELO
domain instead of the default described above.

@b<@@>@i<var>@\A variable name preceded by an at-sign.  This is used to
do BSMTP routing to another BSMTP server that supports the BSMTP
source route.  @M does not support this construct,  but others such
as the Wiscnet SMTP server do.  The value of @i<var> is inserted in
the source route portion of each RCPT TO: address.

@b<%>@i<var>@\A variable name preceded by a percent-sign.  This is used
to perform the ubiquitous "percent routing hack" that is widely used
but is not documented in any mail standard.  The RCPT TO address
is rewritten by changing the at-sign to a percent and appending the
The value of @i<var> to the end of the address.
@end(description)
Note that for the at-sign and percent forms of the parameter, that the
variable name must be 7 characters or less.  For the other form, the
name must be 8 characters or less.

Examples of the four flavors of BSMTP (vanilla and three types of parameter)
follow:
@begin(example)
via20B=@@CU20B.COLUMBIA.EDU
viaDEC=@@DECWRL.DEC.COM
funnydom=BLAH.BLAH.EDU
 ...
OUTGOING:
;domain  node   userid   exit   type parameter

CUVMA    CUVMA  MAILER   BSMTP  3
CUTC20   CUVMA  SMTPSERV BSMTP  3    @@via20B
FROTZ    XYZZY  FUZZNET  BSMTP  3    funnydom
 ...

DOMAINS:
.DEC     WISCVM SMTPSERV BSMTP  3    %viaDEC
 ...
@end(example)

Given the following sample addresses,  these RCPT TO's would be created:
@begin(format)
@tabclear
JOE@@CUVMA@ @ @ @ @ @ @^RCPT TO:<JOE@@CUVMA>

JOE@@CUTC20@\RCPT TO:<@@CU20B.COLUMBIA.EDU:JOE@@CUTC20>

JOE@@DEC123.DEC@\RCPT TO:<JOE%DEC123.DEC@@DECWRL.DEC.COM>
@end(format)

@paragraph[DEFRT]
This type 1 exit sends a file directly to the intended recipient
on a remote RSCS node not running @M.  The file name is set to
that of the sender (truncated to 8 characters).  The file type is set
to "MAIL", the class to "M",  and the distribution code to the host name
of the sender.

DEFRT will generally refuse to deliver mail to userids more than 8
characters long since this is the VM implementation limit.  An
optional parameter, @b<TRUNCATE>, may be supplied which allows longer
names.  The first 8 characters are used in this case.  This facility
is meant to be used in conjunction with VAX/VMS systems running
@i<jnet> and UNIX systems running @i<UREP>.  DEFRT ignores the server
node/userid information in the table entry.

@paragraph[LOCAL]
This type 1 exit sends mail to a local user on the system running
@M.  It sets the file name, etc. as in DEFRT above.  It also sets
the file's TAG to look like one RSCS would generate.  This allows a
modified RDRLIST or RECEIVE exec to determine the origin of a remote file
in the same manner as for files from RSCS.

The optional @b<PROFS> parameter will cause a PROFS Subject record to
be appended to the end of the file so that PROFS mail may be used.
Kudos to Peter Day of Emory University.  See also the PROFS SPECIAL
exit.

@paragraph[NOMAIL]
This type 2 exit is used for RSCS nodes that can't handle
mail.  It logs an error message and causes the mail to be returned.
The required parameter is the host name to give in the error mail.
It sends mail back that essentially says, "Mail service for @i<parameter>
is not available."

@paragraph[INTERNET]
This type 1 exit was written by Henry Nussbacher of the Weizmann
Institute, Rehovot, Israel and is based on the DEFRT exit as well as
the old BSMTP exit written by Arthur Ecock of the City University of
New York.  Reg Quinton and Jeffrey Honig have also contributed code to it.
INTERNET is used in the "*DEFAULT*" position of the Profile
OUTGOING table.  It searches for host names ending in ".ARPA",
".UUCP", ".DEC", and so on and routes via the appropriate gateway.
Much of INTERNET's capability is now in the mainline @M code (using
the DOMAINS table) and the BSMTP exit (using the at-sign and
percent-preceded parameters).

Specifying the optional @b<RETURN> parameter causes mail lacking
a domain-style recipient address (i.e. without dots) to be returned
with the error "Unknown Host" rather than attempting to send the
mail via RSCS (which will generally result in returned mail unless
RSCS knows of some node names that @M doesn't).

@paragraph[UTSMAIL]
This exit will send mail to users on a UTS guest running on the
same machine as @M.  It submits a UTS @i<uux> file which invokes
the @i<rmail(3)> program to enter the mail into the UTS mail system.
The required parameter is the vmid of the UTS guest.@foot[UTSMAIL has not
been tested with or modified to support UTS/V's @i<sendmail> system.]

@subsection(Incoming Exits)@label(INCOMING-EXITS)

@paragraph[RSCSVER]
This incoming exit is used to validate files from sites not running 
@M.  The sender name and host are checked to see if they match
the RSCS origin in the tag text.  This exit is used in the "*DEFAULT*"
position of the Incoming table and allows users on other systems not
running @M to use your @M to send mail.

@paragraph[(none)]
Not specifying an Incoming exit causes the built in validation
code to execute.  This is used to indicate the names of other trusted
Mailers (which are allowed to send a file with a "From:" or "Sender:" 
which differ from their vmid and RSCS node name).

@subsection(Special Exits)@label(SPECIAL-EXITS)
Special table exits are used to define local user name aliases like
POSTMASTER and/or mailing list names among other things.

@paragraph[FWDMAIL]
This exit provides mail forwarding.  The list of recipients
to forward to must be defined with a variable assignment in the Profile.
The variable name must be 8 characters or less and is supplied as the
required parameter.

@paragraph[RSMAIL]
This exit provides mail resending.  This is similar to forwarding
and the list of recipients is supplied in the same way as for FWDMAIL.
The difference between the two is that FWDMAIL creates a new mail header
while RSMAIL simply adds "Resent-To", "Resent-From", and "Resent-Cc"
fields to the original header.

An example of using FWDMAIL or RSMAIL follows:
@begin(example)
people=SMITH@@CUVMA,JONES@@CUVMB
alan=EACUS@@CUVMA
 ...
SPECIAL:
CROSSWELL  RSMAIL  1  alan
FOO-L      RSMAIL  1  people
@end(example)

@paragraph[DISTLIST]
DISTLIST uses BSMTP to re-distribute mail to a list of recipients
contained in a file named @b["@i<parameter> DISTLIST"].  The file
should be RECFM V and have one address per line.  This exit does not
edit or add anything to the mail header at all (other than having the
side effect of causing an additional "Received:" header to be added) and
could be very useful for distribution of large mailing lists.  The
only restriction on the use of this exit is that all the recipients
listed must be reachable with BSMTP.  That is, it must not matter that
the recipient's address does not appear in the "To:" line of the
RFC822 mail.  This restriction is satisfied for LOCAL and DEFRT distribution
as well as any mail server that is defined in OUTGOING/DOMAINS with
the BSMTP exit.  In other words,  if a recipient is reached with the
BITNET exit,  then the mail will most likely be returned to the sender
with an error of "No recipients for this node."

For a file named @B<"SAMPLE DISTLIST">:
@begin(example)
SMITH@@CUVMA
JONES@@CUVMB
@end(example)

An example profile entry would be:
@begin(example)
SPECIAL:
SOME-GROUP  DISTLIST 1 SAMPLE
@end(example)

@paragraph[VMMLOG]
This exit appends mail to a file in the format used by the Columbia
VMM mail program (written by Vace Kundakci) and is used at Columbia
to provide bulletin boards.  The required parameter specifies the
file name of the VMMLOG file.  Additionally,  the file mode of the
VMMLOG file may be specified as a variable named "@i<parameter>-mail-disk".
For example:
@begin(example)
bboard-mail-disk=G
...
SPECIAL:
bboard   VMMLOG   1 BBOARD
@end(example)

@paragraph[PROFS]
The PROFS exit does the same thing as the LOCAL OUTGOING exit with
the PROFS parameter.  You would use this exit when most users get
the standard LOCAL treatment but a specific list of users (which
you enumerate in the SPECIAL table) want mail delivered in PROFS format.

@paragraph[(none)]
Omitting the exit name causes the built in exit to be
invoked which disallows mail to the given user name.  Use this 
approach to prevent mail from being sent to specific users like
CMSBATCH and SYSTEM.


@subsection(Origins Exits)@label(ORIGINS-EXITS)
The Origins table lists exits to be invoked based on origin userid.
The exit is given control immediately after the file has been 
received and prior to any other processing has been performed.  This
is unlike the Incoming exit which is not invoked until the mail header
has been read in and parsed.  An Origins exit can be used to do
arbitrary processing of an input file based on origin userid. 
@begin(description)
@i<none>@\Leaving the exit name blank causes the built in exit to be
invoked which causes the received file to be purged and a message
sent to the originating userid.  This is useful to dissallow users
like CMSBATCH from sending any files to @M and guarantees that
no error mail files will be sent back to those users.

@end(description)
@section(@M Commands)

The @M accepts commands via CP SMSG or from the console.  Certain
commands are restricted to SMSGs from the Maintainer (as defined in 
@B<"MAILER PROFILE">) or console commands.  Other commands are available
to general users via SMSG.  SMSG replies are sent back to the issuer via
CP MSGNOH if the @M has the necessary privileges.  If not, CP MSG
is used.  Help on commands may be obtained by use of "?" in commands
and subcommands.  For example, "CP SMSG MAILER ?" will cause a list of
the valid general commands to be sent back.  

Privileged commands are described in "MAILER HELPPRIV".  These are 
designed for debugging purposes and to aid general maintenance.

@section(Maintenance)
New versions of @M and exits will be sent to you from time to time.
Additionally,  you are expected to keep your site's "MAILER PROFILE" up to
date.
@subsection(New versions)
New versions will be sent to you preceded by mail describing the reason
for the new version.  Right now,  these files are generally sent with
the Cornell CARD utility (which has been supplied to you).  To install
a new "MAILER MODULE",  simply rename the old "MAILER MODULE" (for backup
purposes),  CARD LOAD the new one and restart @M.  

TEXT files are sent out as replacements for members of "MAILER TXTLIB".
Use the appropriate TXTLIB DEL and TXTLIB ADD commands to install new
TEXT files.  See the @i<CMS Command and Macro Reference> for details.

@subsection("MAILER PROFILE" updates)@label(MG)
The @b<MG> program can be used to automate regeneration of your site's
"MAILER PROFILE" which must usually be done once a month when the latest
"NAMES" files become available.
Using MG,  you only have to make site-specific changes
once,  when you first install @M.

@paragraph(Files Used by MG)
MG uses four or more input files:
@begin(description,leftmargin +22,indent -22)
"* NAMES"@\The NAMES files previously described in section @ref(ADDTL-FILES).

"@i<name> MTPLATE"@\The template file used by MG to build "@i<name>
PROFILE".  This file looks just like the Profile with the exception
that the incoming, outgoing and domains table lines are replaced by the words
@b<%incoming>, @b<%outgoing> and @b<%domains>, respectively.  MG replaces these
lines with the appropriate information taken from "XMAILER NAMES",
"MAILER NAMES", "DOMAIN NAMES", and "@i<name> MAILLCL".  You can also use
"@b<%include> @i<fn.ft.fm>" to cause a file to be included in the
output Profile.

"@i<name> MAILLCL"@\This file consists of the lines in the OUTGOING and
DOMAINS tables that will be different for your site.  That is, the
entry or entries for the local system.  For example, for the site
CUVMB, the Outgoing line for CUVMB specifies the BITNET exit in all
other sites' Profiles and the LOCAL exit for CUVMB's Profile.  Entries
in "@i<name> MAILLCL" should follow the same syntax as lines in the
Outgoing table.  No comments or blank lines are permitted.
@end(description)

@i<Name>,  above,  is typically @b<MAILER>,  but you may use any convenient
name.
@paragraph(Invoking MG)
To run MG,  type:
@begin(example)
MG [-i] @i<name> [@i<alternate NAMES file names>]
@end(example)
@i<NAMES> file names preceded by a "-d" are used for the DOMAINS table.
Others are used for the OUTGOING and INCOMING tables.

The typical invocation of MG is:
@begin(example)
MG MAILER
@end(example)

An example of using a different Outgoing/Incoming NAMES file is:
@begin(example)
MG MAILER NEWBIT.NAMES.G MAILER.NAMES
@end(example)

Finally,  an example of using a different DOMAINS NAMES file is:
@begin(example)
MG MAILER -d NEWDOM.NAMES
@end(example)
[Note the use of OS-style "dot" notation for file names.]

You will normally not need the "-i" switch to MG which tells it
to set up an "initial distribution" profile -- that is,  a profile
for a Mailer that other Mailers do not yet have in their INCOMING tables.
The default list of NAMES files used by MG is:
"XMAILER.NAMES MAILER.NAMES -d DOMAIN.NAMES".

MG will also accept EARN-style "NODES" files which differ only slightly
from the BITNET "NAMES" files.  So,  if you prefer to use the EARN node
administration tools,  you may do so.

@paragraph(Examples of MG input files)
@begin(figure,group,break)
@bar()
@caption(Sample template file: "CUVMB MTPLATE")@tag(FIG-MTPLATE)
@begin(example)
; CUVMB MTPLATE - template used to build CUVMB PROFILE

rscsname=RSCS                ; name of rscs/networking vm userid
RSCS-Domain=.BITNET          ; the RSCS network is called BITNET
thisnode=CUVMB               ; local node name
Local-Domain=CUVMB.COLUMBIA.EDU ; this as well as CUVMB.BITNET
maintuser=EACUS              ; userid of maintainer
maintnode=CUVMB              ; node name of maintainer
supply-default-fields=FALSE  ; leave FALSE (not yet implemented)
allow-unknown-nodes=TRUE     ; allows nodes not in my table but
                             ; known to RSCS to exchange mail.
POSTMAST=EACUS@@CUVMA         ; Person for mail system questions
BITINFO=EACUS@@CUVMA          ; BITNET Info id
@hinge

; local origins whose files I want to throw away:

ORIGINS:
CMSBAT01
CMSBAT02
CMSBAT03
CMSBAT04
CMSBAT05
END ORIGINS

@hinge
INCOMING:
; Node   Userid   Exit
;------- -------- ---------
@b<%incoming>
*DEFAULT*      *DEFAULT*     RSCSVER   ; standard RSCS tag checking
END INCOMING
@hinge
; Table of mail nodes that I can send to.

OUTGOING:
; Alias name     Node     Userid   Exit     Type Parm
; ----------     -------- -------- -------- ---- --------
@b<%outgoing>
CUVMB.COLUMBIA.EDU   *       *     LOCAL     1           
*DEFAULT*        *DEFAULT*   ?     INTERNET  1
END OUTGOING
@hinge

; Domains

DOMAINS:
@b<%domains>
END DOMAINS
@hinge
; Local addresses to receive "special handling":

SPECIAL:
SYSTEM                        ; prevent punched decks
POSTMASTER RSMAIL 1 POSTMAST  ; Person for mail questions
BITINFO    RSMAIL 1 BITINFO   ; Person for BITinfo questions
END SPECIAL

@end(example)
@bar()
@end(figure)

@begin(figure,group,break)
@bar()

@caption(Sample local replacements file: "CUVMB MAILLCL")
@begin(example)
CUVMB     *      *      LOCAL   1
CUUTSA    *      *      UTSMAIL 1  UNIXA

@end(example)
@bar()
@end(figure)

@paragraph(Cookbook)
The cookbook approach to build your @M Profile with MG follows.
This is done for you by the MCONFIG EXEC.

The first time only:
@begin(enumerate)
Create a "@i<name> MTPLATE" file similar to the one in figure 
@ref(FIG-MTPLATE).

Create a "@i<name> MAILLCL" file with a single line in it:
@begin(example)
@i<SITENAME>   *      *    LOCAL   1
@end(example)

Get yourself added onto the list of people that are informed when
a new copy of the various NAMES files arrive at your site (or you can "poll"
NICSERVE@@BITNIC for new versions).  The files are huge,  so try to
find them on your system first before obtaining them from NICSERVE.
@end(enumerate)

Each time a new "XMAILER", "MAILER" or "DOMAIN NAMES" file becomes available:
@begin(enumerate)
Obtain read access to the NAMES files on any accessed disk.

Obtain read access to "@i<name> MTPLATE" and "@i<name> MAILLCL"
on any accessed disk.

Type: @example(MG @i<name>)

When MG is done,  a new file,  "@i<name> PROFILE A1" will have been created.

Compare "@i<name> PROFILE A1" to "MAILER PROFILE" on the disk that @M
reads when it starts up to check for any discrepancies.  You should see
only a list of new BITNET sites plus some sites that have put @M into
production use recently.

If you are satisfied with the new "@i<name> PROFILE", replace "MAILER
PROFILE" with it (after making a backup copy).  Occasionally, the data
provided by BITNIC is incorrect and @M will find a fatal error in the
new PROFILE so you will have to revert to the previous version.

Tell Mailer to re-access the disk reload (see the example below).
@end(enumerate)

The suggested way to install a new "MAILER PROFILE" while @M
is running is as follows:
@begin(enumerate)
Obtain write access to the minidisk containing the "MAILER PROFILE"
(assuming the @M has read-only access). 

Make the needed changes to the file.

Tell @M to reaccess the disk.  For example,  if the disk
is linked at 193 as the B disk,  issue the following commands
(User typein prefixed by "."):
@begin(example)
.CP SMSG MAILER CMS ACC 193 B
| CMS command accepted.
| R(00000) for: ACC 193 B 
.CP SMSG MAILER RELOAD
| Reload command accepted.
@end(example)
(These commands will only be accepted by the userid listed as the @M
maintainer in the profile.)
@end(enumerate)

@section(Diagnosing Problems)
@M breaks from time to time.  Typically,  some error will occur
from which it thinks it can't recover,  so it will exit.  This generally
leads to @M logging out if you use the standard PROFILE EXEC.
If this happens,  take a look at the
topmost file in @i<Mailer's> reader.  This is usually the culprit.
Most errors have to do with wild syntax in a file that causes @M
to crash due to some "impossible case" happening in the header parsing
module (which is admittedly a sloppy job).

Transfer this file away from @M and log it back in.  It will 
probably run just fine.  Take a look at the file and if you can't
figure out why it caused @M to break down,  then try asking someone
else on BITNET (such as the members of the XMAILER or MAIL-L mailing lists).

@section(Messages)@label(MESSAGES)

@M generates messages on the console, as messages sent to users within
mail reporting an error, and as BSMTP replies.  Most messages use the
standard CMS format of @i<CCCMMMNNNT> where @i<CCC> is "MAI" for @M,
@i<MMM> is the particular @M module issuing the message, @i<NNN> is
the message number, and @i<T> is one of three standard message types
and two additional types:
@begin(description)
I@\Informational.

W@\Warning.

E@\Error.

U@\Report error to user within error mail.

X@\Send RSCS MSG to mail sender (e.g. who the mail is "From:").
@end(description)

The message code is never put in error mail or sent as a CP
or RSCS MSG but is written to the console depending on the
CP EMSG setting.

@subsection(Message Codes)
The following messages with message numbers are generated by
@M proper and the various user exits:

@begin(description,leftmargin +4, indent -4)
MAILER000E Unable to obtain file '@i<fn ft fm>', RC=@i<nnnn>

MAILER001I CUCCA VM Network MAILER @i<version> @i<date> Copyright (c) 1982,1987 Columbia University

MAILER002E Routing file not lrecl 80 and recfm F.

MAILER003E Stack overflow in @i<routine>@\
@M did not have adequate virtual storage available to process the mail.
This may be caused by a garbage file in the reader,  a mail file with
a very large header, or the virtual machine size is too small.  

MAILER004I MAILER running on @i<node>

MAILER005I Initialization complete.

MAILER006I MAILER waiting for work.

MAILER007E MAILER stopping due to error @i<nn>@\
Look for a previous error message which indicates the specific nature of the
error.

MAILER008E DIAG 14 RC=@i<nnnn>@\
CP Diagnose X'14' failed.  This "never happens" except when the CP SPOOL
is in a mess.

MAILER009I Received file @i<spoolid> sender: @i<vmid> from: @i<node> 
<userid>

MAILER010E RDCARD RC=@i<nnnn>@\
See message 008, above and the @i<CMS Command and Macro Reference> for
the RDCARD macro return codes.

MAILER011I @i<nnnn> bytes of stack space.@\
After initializing (allocating tables and loading user exits), @M allocates
the rest of available contiguous virtual storage for use as a stack
for mail processing tasks.

MAILER012I '@i<name>' not in routing table.

MAILER013W Distrib exit '@i<name>' RC=@i<nnnn>@\
The named exit set a non-zero return code.  See if the exit generated
any error messages preceding this one.

MAILER014W Invalid entry in '@i<file name>', line @i<lineno>

MAILER015W Table add error @i<nn>, line @i<lineno>@\
An error code was returned from the function that adds entries to one
of the several tables @M uses.  See what line @i<lineno> contains and
hopefully that will tell you what went wrong.

MAILER016E @i<name> table is empty

MAILER017E User exit '@i<name>' load error @i<nn>@\
@i<Nn> is the return code from the CMS LOAD command.  @M dynamically
loads user exits by issuing a "LOAD @i<name> (ORIGIN @i<some address>).
These exits must not contain any unresolved external references (much
like RSCS exits).  (Exit developers should see the CMS PRELOAD command).

MAILER018I User exit '@i<name>' loaded at @i<hex address>@\
Useful for determining the source of abends.

MAILER019U Unknown network sender: @i<node> @i<userid>@\
A user not registered as a trusted mailer tried to send mail containing
other than his own node name and userid as the sender.  See also
mesage 021.

MAILER020U Outgoing mail has no FROM: or SENDER: field.@\
The RFC822 mail header is missing a required field.

MAILER021U FROM: or SENDER: inconsistent with spool file origin.@\
See message 019.

MAILER022E Invalid exit type '@i<type>' for '@i<exit name>'

MAILER023U Required DATE is not present.@\
See message 020.

MAILER024U Validity check failed.@\
See message 019.

MAILER025U Message syntax is unrecognizable.@\
Something other than a PUNCH file containg RFC822 mail, BSMTP, or
a BSMTP reply appeared in @M's reader.  The usual culprit is a
NETDATA file.

MAILER026U Internal error in @i<routine>

MAILER027U Unknown host: @i<name>...

MAILER028U Error loop detected.@\
@M has determined that further transmission of a piece of error mail
will result in it being sent to another mail server which would cause
a mail loop.  Instead,  the mail is sent to the maintainer to deal with.

MAILER029I Returning file to @i<user> AT @i<node>

MAILER030U Syntax error in field- @i<fieldname>

MAILER031U '@i<name>...' is too long to be a VM userid.@\

MAILER032U No such local user: @i<vmid>

MAILER033U Mail to @i<vmid> is not allowed.@\
@i<Vmid> is in the SPECIAL table.

MAILER034X Mail sent to @i<user> at @i<node>

MAILER035U File is not a PUNCH file.

MAILER036U RSCS rejected mail w/tag: @i<tag text>.... Is the hostname
misspelled?@\
A file generated by @M was apparently transferred back from RSCS,
indicating that a non-existent node was specificied.  Be careful
with this one since a user may also fool @M by transferring a mail
file that he has received after playing around with the TAG.

MAILER037U Another network mailer sent an invalid file.

MAILER038I Mailer stopping.

MAILER039I Mailer reloading.

MAILER040X Mail sent to @i<netname> node @i<name>

MAILER041U Mail service is not available for @i<name>@\
Message generated by NOMAIL exit.

MAILER042I Mail processing not active.@\
@M has been quiesced.  It will accept and respond to SMSG or console
commands but will not process any files that arrive in its reader.

MAILER043I @i<nnnn> recipient blocks.

MAILER044U No recipients for this node.@\
RFC822 mail was received from a remote sender (e.g. sent by another
@M with the BITNET exit@) that does not contain any addresses in the header
that are listed as having this @M as a server.  See the description of
the DISTLIST exit, above.

MAILER045E Can't ready reader 00C.  CP RC=@i<nnnn>@\
See message 008.

MAILER046U Auto-forwarding error.

MAILER047X Mail for @i<parm> auto-forwarded to @i<value>

MAILER048U Transferred files not allowed.  Was it a bad RSCS node or user 
CP TRANSFER?

MAILER049X File @i<vmid> @i<spoolid> rejected.@\
Generated by default ORIGINS exit.

MAILER050E @i<routine> RC=@i<nnnn>@\

MAILER051U Unknown domain: @i<domain>...

MAILER052X Mail sent to @i<nnnn> recipients in distribution list @i<name>

MAILER053U Error @i<nn> processing distribution list @i<name>

MAILER054I BSMTP request transaction start.

MAILER055I BSMTP request transaction end.

MAILER056I BSMTP reply sent to @i<node> @i<userid>

MAILER057I BSMTP reply transaction start.

MAILER058I BSMTP reply transaction end.

MAILER059I Error mail sent.

MAILER060E BSMTP reply syntax unrecognizable.  Reply ignored.

MAILER061E BSMTP reply contains multiple "050 MAIL FROM" lines.
Reply ignored.

MAILER062E A-disk not writeable.

MAILER063U Mail aborted.  Maximum hop count exceeded.@\
The BSMTP processing code counts "Received:" headers and refuses
to further send any mail that has more than 50 of them.  This
is an attempt to curtail mail loops caused by recursive distribution
lists, etc.

MAILER064W Duplicate table entry (line @i<nnnn>) ignored.@\
The entry on line @i<nnnn> of the profile has not been added to the
current table being defined because it was previsously entered
into the table.

MAILER065U File is corrupt: @i<file name>

MAILER066X Mail appended to @i<name> bulletin board.
@end(description)
@subsection(BSMTP Messages)
The following messages are generated by the BSMTP processor in
response to the command or condition indicated.  These
messages appear in the BSMTP reply (when sent) and on the console:

@begin(description, leftmargin +6, indent -6)
(initial)@\
@begin(description,leftmargin +4, indent -4)
220 @i[<domain>] Columbia MAILER @i[<version>] BSMTP service ready.
@end(description)

(any)@\
@begin(description,leftmargin +4, indent -4)
500 Command out of sequence.

500 Command ignored due to a previous error.

421 Service not available (internal error).@\
Typically caused by a failure to obtain needed virtual storage.

501 Parameter syntax error.

553 Mailbox syntax error.

554 Mail aborted.  Maximum hop count exceeded.
@end(description)

HELO@\
@begin(description,leftmargin +4, indent -4)
250 @i[<my domain>] Hello @i[<sender's domain>]

250 @i[<my domain>] You are a charlatan; @i[<sender's domain>] is bogus for
a file with RSCS origin @i[<node>] @i[<userid>]@\
However,  the mail will be allowed to continue as the allow-charlatans
variable is True.

501 @i[<sender's domain>] is bogus for a file with RSCS origin @i[<node>]
@i[<userid>]@\
Allow-Charlatans is False.
@end(description)

MAIL@\
@begin(description,leftmargin +4, indent -4)
250 @i[<mailbox>]... sender OK.

501 @i[<mailbox>] does not correpsond to RSCS origin of @i[<node>] @i[<userid>]
@end(description)

RCPT@\
@begin(description,leftmargin +4, indent -4)
250 @i[<mailbox>]... recipient OK.

550 Mailbox not found.@\
@M was unable to find an OUTGOING/DOMAINS table entry for the given
hostname.
@end(description)

DATA@\
@begin(description,leftmargin +4, indent -4)
354 Start mail input.  End with <crlf>.<crlf>@\
Everything up until a line with a lone dot is read in as the mail
data.  Possible response after the dot is received are:

250 Mail accepted for delivery. 

552 DATA aborted:  out of disk space.

451 DATA aborted:  CMS filesystem error.

554-Mail not delivered to some or all recipients:@\
This message is followed by one or more of the "U"-type messages listed
above,  each on a line starting with "554".
@end(description)

RSET@\
@begin(description,leftmargin +4, indent -4)
250 Service reset OK.
@end(description)

NOOP@\
@begin(description,leftmargin +4, indent -4)
250 Ok
@end(description)

QUIT@\
@begin(description,leftmargin +4, indent -4)
221 @i[<my domain>] Columbia MAILER BSMTP service done.
@end(description)

VERB@\
@begin(description,leftmargin +4, indent -4)
250 Verbose mode ON.

250 Verbose mode OFF.
@end(description)

TICK@\
@begin(description,leftmargin +4, indent -4)
250 @i[<ticket>]... That's the ticket.
@end(description)

RPLY@\
@begin(description,leftmargin +4, indent -4)
250 Error replies will be sent as mail.@\
Overrides whatever default is in effect and causes errors to be reported
to the user (person in the MAIL FROM) in mail.

250 Error replies will be sent as smtp.@\
Overrides whatever default in in effect and causes errors to be reported
to the mail server (spool file origin) in a BSMTP reply.
@end(description)

:REA@\
@begin(description,leftmargin +4, indent -4)
200 :READ card ignored.@\
Makes life easier for people who test @M with the CMS PUNCH command.
@end(description)

(others)@\
@begin(description,leftmargin +4, indent -4)
500 Invalid command.
@end(description)
@end(description)

@subsection(Miscellaneous Messages)

@begin(description,leftmargin +4, indent -4)
* New mail from @i[<address>]@\
Generated by LOCAL and PROFS exits and sent to the mail recipient
via CP MSGNOH, MSG, or whatever command is specified as Msgcmd.
The message is prefixed by "*" unless specified as Msgprefix.
@end(description)
