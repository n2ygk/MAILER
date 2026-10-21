# install.lpt

<!-- Page 1 -->

<pre>










                            <strong>COLUMBIA</strong> <strong>VM</strong> <strong>MAILER</strong>


                               <strong>VERSION</strong> <strong>1.24</strong>




                    <strong>INSTALLATION</strong> <strong>AND</strong> <strong>MAINTENANCE</strong> <strong>GUIDE</strong>



                                24 May 1982

                        (last updated: 18 May 1987)



                             E. Alan Crosswell

                         EACUS@CUVMA.COLUMBIA.EDU

                       Scholarly Information Center

                Columbia University in the City of New York















       Copyright (C) 1982, 1987 The Trustees of Columbia University
                          in the City of New York
</pre>

---

<!-- Page 2 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>1.</strong> <strong>Introduction</strong>

  This  document  describes  the  installation,  operation  and maintenance

procedures for the Columbia University VM <u>MAILER</u>.    Please  read  sections

COPYR  and  TERMS  on  the  terms under which <u>MAILER</u> is provided to you and

destroy the files that have been provided if you are  unable  to  abide  by

them.



  Section RESPONS, below, briefly gives a job description for the person or

persons required to maintain <u>MAILER</u> at your site.  Sections following  that

go  into the details of installing <u>MAILER</u> the first time and maintaining it

and the data it needs to operate on  BITNET.    The  information  in  these

sections  is written with the assumption that the reader is conversant with

the basic concepts of VM/SP and RSCS networking.  No  attempt  is  made  to

define terms.



  <u>MAILER</u>  was  developed  by  E.  Alan  Crosswell,  an employee of Columbia

University in the City of New York.  It is the property of the Trustees  of

Columbia  University  in  the  City of New York.  Some portions of the code

have been contributed by <u>MAILER</u> users on  BITNET  (their  names  are  cited

within  the  source  code).  <u>MAILER</u> is written in IBM 370 Assembly language

and C.


<strong>1.1.</strong> <strong>Copyright</strong> <strong>Notice</strong>

  Copyright (c) 1982, 1987, The Trustees of Columbia University in the City

of New York.



  Permission  is  granted to any individual or institution to use, copy, or
                                   - 1 -
</pre>

---

<!-- Page 3 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


redistribute this software so long as it is not sold for  profit,  provided

that this notice and the original copyright notices are retained.  Columbia

University makes no representations about the suitability of this  software

for  any  purpose.    It  is  provided  &quot;as  is&quot; without express or implied

warranty.



  May be used but not sold without permission.


<strong>1.2.</strong> <strong>Additional</strong> <strong>Terms</strong> <strong>for</strong> <strong>Waterloo</strong> <strong>C</strong> <strong>Library</strong> <strong>Object</strong> <strong>Code</strong>

  Some C programs provided with this distribution make use of the  Waterloo

C  Runtime  Library, a component of Waterloo C, a licensed software product

of WATCOM Products, Inc. of Waterloo, Ontario, Canada.  WATCOM has  granted

us  permission  to  distribute  the  object code for the runtime library to

&quot;sublicensees&quot; so that sites without Waterloo C may  use  <u>MAILER</u>  with  the

following terms:


   1. That  we  include limitations on WATCOM&#x27;s liability, limitations
      of warranty, and restrictions on further distribution similar to
      those included in our Waterloo C license agreement.

   2. We  must  maintain  records  of all copies of the library object
      code distributed to sublicensees.



  No liability is assumed by WATCOM Products, Inc. for any special, direct,

indirect  or  consequential damages, losses, costs, fees or expenses of any

nature or kind.



  You may freely give <u>MAILER</u> source code to others but may not redistribute

the  <u>MAILER</u>  object  code  module  (MAILER MODULE) or C object code library

(CLIB TXTLIB) as they contain Waterloo C library object code.
                                   - 2 -
</pre>

---

<!-- Page 4 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


  In the event that Columbia University&#x27;s license with WATCOM for  Waterloo

C  is  terminated,  you  will  be  required to destroy any copies of <u>MAILER</u>

object code that contain Waterloo C library object code.


<strong>1.3.</strong> <strong>Support</strong>

  <u>MAILER</u> is supplied with documentation, parameter files, and source and/or

object  code  depending on the type of distribution you have requested.  If

you have source code and make  substantial  modifications  that  are  of  a

generally useful nature, please send those modifications back to us so that

they may be considered for inclusion in future versions.



  <u>MAILER</u> is distributed over the BITNET computer network free of charge  to

any member site.  Columbia University provides <u>MAILER</u> on an as-is basis and

will not provide any support.  You are expected to have the local expertise

required  to  install  and  maintain  <u>MAILER</u>  or  to  obtain such expertise

elsewhere.  (There are a large number of people on BITNET  that  have  this

expertise and who may be willing to help you.)



<strong>2.</strong> <strong><u>MAILER</u></strong> <strong>Maintainer</strong> <strong>and</strong> <strong>Postmaster</strong> <strong>Job</strong> <strong>Description</strong>

  There  are  actually two jobs involved in running a mail service.  One is

that of maintaining and updating the mail software and  databases  and  the

other is to perform the duties of Postmaster.



  The <u>MAILER</u> maintainer&#x27;s responsibilities include:


   1. Installation   and   configuration   of   the   initial   <u>MAILER</u>
      distribution.

   2. Occassional installation of <u>MAILER</u> software  updates  (typically
                                   - 3 -
</pre>

---

<!-- Page 5 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


      on  the  order  of  once  or  twice  a  year  but sometimes more
      frequently).

   3. Monthly acquisition and installation of the BITNET database used
      by  <u>MAILER</u>.    This database is maintained by the BITNET Network
      Information Center (BITNIC) and is distributed monthly over  the
      network   (or  sometimes  by  tape  during  periods  of  network
      instability).  This maintenance is related to the other  monthly
      BITNET node maintenance work (e.g. updating the RSCS DIRECT).


None  of  these  responsibilities  tend  to  be particularly time-consuming

although it depends on the individual&#x27;s familiarity with VM and would  best

be  performed  (or  supervised)  by  a  systems  programmer.   There is the

potential for an incorrect <u>MAILER</u> installation to crash your VM system  (by

filling  up  CP  SPOOL  space) or to &quot;clobber&quot; the network (by causing mail

loops).



  The Postmaster&#x27;s responsibilities include:


   1. Answering local users&#x27; questions about the mail system.

   2. Responding to queries from outsiders (typically requesting  help
      in reaching a particular member of your site&#x27;s community).

   3. Diagnosing  miscellaneous  mail  system  errors.    For example,
      <u>MAILER</u> sends &quot;error mail&quot; that can&#x27;t be automatically dealt with
      to the Postmaster.



  The  Postmaster should read his or her mail frequently (many do it one or

more times daily).  The degree of work involved  in  being  the  Postmaster

depends  on  how  much  service  you  want to provide to your community and

outsiders.  For example, you  may  be  willing  to  spend  a  lot  of  time

answering requests for the electronic address of Professor Smith or you may

prefer to suggest that the requestor simply call  Professor  Smith  on  the

telephone to find out her userid.  At Columbia, the daily Postmaster duties
                                   - 4 -
</pre>

---

<!-- Page 6 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


are peformed by a  trained  student  with  student  training,  back-up  and

difficult questions handled by a full-time staff member.



<strong>3.</strong> <strong>Inventory</strong> <strong>of</strong> <strong>Installation</strong> <strong>Materials</strong>

  The  following  items have been supplied to you.  Please verify that they

are all present.  Section ADDTL-FILES describes additional files  you  will

need to obtain in order to use <u>MAILER</u> on BITNET.


<strong>3.1.</strong> <strong>Object</strong> <strong>Code</strong> <strong>Distribution</strong>


<strong>&quot;INSTALL</strong> <strong>LISTING&quot;</strong>     this document.


<strong>&quot;MCONFIG</strong> <strong>EXEC&quot;</strong>        used to configure Mailer.


<strong>&quot;MAILER</strong> <strong>TXTLIB&quot;</strong>       library of exit routines.


<strong>&quot;MAILER</strong> <strong>MODULE&quot;</strong>       main   program.    Contains  licensed  software  (see
                      section 1.2, above).


<strong>&quot;MAILER</strong> <strong>HELPCMS&quot;</strong>      general command description.


<strong>&quot;MAILER</strong> <strong>HELPPRIV&quot;</strong>     privileged command description.


<strong>&quot;PROFILE</strong> <strong>EXEC&quot;</strong>        to run <u>MAILER</u> as an  AUTOLOG&#x27;d  disconnected  virtual
                      machine.


<strong>&quot;SITEOPTS</strong> <strong>EXEC&quot;</strong>       to  set  site-specific  options.  The default options
                      will probably be fine for you.


<strong>&quot;PLM</strong> <strong>LISTING&quot;</strong>         <u>MAILER</u> Program Logic Manual (usually not up to date).


<strong>&quot;MG</strong> <strong>MODULE&quot;</strong>           program  to  facilitate  automatic  regeneration   of
                      &quot;MAILER  PROFILE&quot;  from  &quot;MAILER MTPLATE&quot; and &quot;MAILER
                      MAILLCL&quot;.  Contains licensed software.   See  section
                      1.2, above.
                                   - 5 -
</pre>

---

<!-- Page 7 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>3.2.</strong> <strong>Source</strong> <strong>Code</strong> <strong>Distribution</strong> <strong>A</strong> <strong>--</strong> <strong>for</strong> <strong>sites</strong> <strong>with</strong> <strong>Waterloo</strong> <strong>C</strong>

  The  source  code  distribution  is  sent to you as the contents of three

disks and is free of the Waterloo C library restrictions.  This  means  you

will need a Waterloo C compiler to modify and rebuild <u>MAILER</u>.


   1. Base source.

   2. Updates and CNTRL files.

   3. Miscellaneous execs and maintenance tools.


<strong>3.3.</strong> <strong>Source</strong> <strong>Code</strong> <strong>Distribution</strong> <strong>B</strong> <strong>--</strong> <strong>for</strong> <strong>sites</strong> <strong>without</strong> <strong>Waterloo</strong> <strong>C</strong>

  This distribution contains everything in source distribution A as well as

the following files:


<strong>&quot;MAILGEN</strong> <strong>TXTLIB&quot;</strong>      Library of TEXTs  for  all  <u>MAILER</u>  assembler  and  C
                      sources.


<strong>&quot;CLIB</strong> <strong>TXTLIB&quot;</strong>         Waterloo  C  runtime  library.   This is bound by the
                      WATCOM sublicense terms.



<strong>4.</strong> <strong>Additional</strong> <strong>Files</strong> <strong>Needed</strong> <strong>for</strong> <strong>BITNET</strong>

  Besides the files provided with <u>MAILER</u>, you will need to  obtain  several

other  files  from  BITNIC  which  are  part of the database of mail system

information.  You will need to obtain a new copy of these files  each  time

they become available (once monthly).


<strong>&quot;XMAILER</strong> <strong>NAMES&quot;</strong>       The subset of the node information database necessary
                      to generate a new MAILER PROFILE.


<strong>&quot;MAILER</strong> <strong>NAMES&quot;</strong>        &quot;Mail-only&quot;  host  names  which   represent   systems
                      reachable by <u>MAILER</u> which are not actual BITNET nodes
                      (and are therefore not in the nodes database).


                                   - 6 -
</pre>

---

<!-- Page 8 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>&quot;DOMAIN</strong> <strong>NAMES&quot;</strong>        Information about Internet domains.



<strong>5.</strong> <strong>Your</strong> <strong><u>MAILER</u></strong> <strong>subscription</strong> <strong>and</strong> <strong>registration</strong>

  Distribution of <u>MAILER</u> updates was formerly based on  a  subscription  in

the  node information database maintained by the BITNET Network Information

Center (BITNIC).  This same database is used to register the  existence  of

your  <u>MAILER</u>  virtual  machine  so  that  other mailers know about it.  The

&quot;:inform<u>X</u>&quot; and &quot;:info<u>X</u>&quot; tags were used for your <u>MAILER</u> subscription.    The

&quot;:mailer&quot;   tag   registers  your  node&#x27;s  <u>MAILER</u>.    Updated  registration

information is distributed once monthly by BITNIC in  the  <strong>&quot;XMAILER</strong>  <strong>NAMES&quot;</strong>

file (see section 4).



  The &quot;:mailer&quot; tag is still used to register your <u>MAILER</u>, but Eric Thomas&#x27;

Revised  LISTSERV  is  now  the  system  used  to  maintain   your   <u>MAILER</u>

subscription.    All  previously &quot;:info/:inform&quot;-subscribed users have been

moved to LISTSERV.


<strong>5.1.</strong> <strong>Subscription</strong> <strong>via</strong> <strong>LISTSERV</strong>

  To maintain your &quot;subscription&quot; for <u>MAILER</u> bug  fixes  and  enhancements,

you  need  to subscribe to the &quot;MAILDIST&quot; mailing list provided by LISTSERV

at CUVMA.  By default, your subscription to this list  will  only  get  you

mail  announcing  new <u>MAILER</u> file shipments.  To get the files as well, you

will want to change your subscription options to include files.    In  this

manner, you can hear about <u>MAILER</u> changes without actually getting the code

distribution if someone else at your site has that responsibility.



  To subscribe to the <u>MAILER</u> distribution (by the  time  you  receive  this
                                   - 7 -
</pre>

---

<!-- Page 9 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


document  you  are  already  subscribed)  send  the  following command as a

message or mail to LISTSERV at CUVMA:


    SUBSCRIBE MAILDIST your name



  To get the code distribution as well as announcements, after subscribing,

send:


    SET MAILDIST FILES



  If  you  maintain the <u>MAILER</u> for multiple machines, you need only do this

for one of them.  This will keep  the  flow  of  unneccessary  files  to  a

minimum.


<strong>5.2.</strong> <strong>Registration</strong> <strong>:mailer</strong> <strong>tag</strong>

  Finally,  to  tell the rest of us that you have a <u>MAILER</u>, you need to set

up  the  <strong>:mailer</strong>  field.    Initially,  when  you  first  get  <u>MAILER</u>   (or

beforehand), the field should read:


    :mailer.<u>vmid</u> DEFRT 1


where  <u>vmid</u>  is  the userid of your <u>MAILER</u> machine.  This will allow you to

send mail via other sites&#x27; <u>Mailers</u> to try things out.  Normally, you  would

need  to  wait  until the new <strong>&quot;XMAILER</strong> <strong>NAMES&quot;</strong> file has been distributed and

other sites have had time to update their Profiles.    If  you  send  to  a

<u>MAILER</u>  site  and get return mail saying &quot;Validity check failed,&quot; then that

site hasn&#x27;t updated its Profile yet.  (See section MESSAGES for a  list  of

the various messages generated by <u>MAILER</u>.)


                                   - 8 -
</pre>

---

<!-- Page 10 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


  Since  this  has  become  more  and  more of a problem as the size of the

network has grown and the lag time between distributions of new versions of

<strong>&quot;XMAILER</strong>  <strong>NAMES&quot;</strong>  has  increased,  the  initial  <strong>&quot;MAILER</strong>  <strong>PROFILE&quot;</strong>  that is

generated by MCONFIG for your initial <u>MAILER</u>  installation  is  set  up  to

bypass  all  other  mailers.   This is not an ideal solution since you lose

some of the benefits of running <u>MAILER</u>, but at least you won&#x27;t get a lot of

&quot;Validity  check&quot;  mail  returned.    Once  a  new <strong>&quot;XMAILER</strong> <strong>NAMES&quot;</strong> has been

distributed that lists your site as having a Mailer (or  if  your  site  is

already  listed  that  way) then you can regenerate the <strong>&quot;MAILER</strong> <strong>PROFILE&quot;</strong> as

described below.



  Once you have put <u>MAILER</u> into production use (i.e. it  is  always  logged

on) you will want to change the field to read:


    :mailer.<u>vmid</u>


This  will  cause  other <u>Mailers</u> to send mail for your site via your <u>MAILER</u>

rather than via direct RSCS routing.



<strong>6.</strong> <strong>Installation</strong> <strong>Procedure</strong>


                        1
   1. Create a VM userid  for the <u>MAILER</u>  disconnected  machine.    It
      should  have  a  virtual  storage  size  of 1M and a 191 disk of


_______________

  1
   Please don&#x27;t change the userid without first updating your node database
entry  and waiting for it to be redistributed to all <u>MAILER</u> sites.  You may
end up having to run two copies of <u>MAILER</u> during the transition period; one
under  the  old  name and one under the new name to avoid any disruption in
mail traffic.
                                   - 9 -
</pre>

---

<!-- Page 11 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


      adequate size to hold  the  largest  mail  file  you  anticipate
      receiving.    If  you give it the appropriate CP privilege class
      for the <strong>MSGNOH</strong> command then it will use  that  command  to  send
      messages.  If not, it will use <strong>MSG</strong>.

   2. Obtain   write   access  to  a  CMS  minidisk  to  which  <u>MAILER</u>
      disconnected virtual machine will  have  read  access.    It  is
      recommend that this be other than the A-disk.  <u>MAILER</u> requires a
      writeable A-disk for various temporary files it  creates.    The
      disk  that the <u>MAILER</u> parameter files and TXTLIB reside on would
      best be owned by the <u>MAILER</u> maintainer.  This will allow ease of
      future updates without interrupting normal operations.

   3. Copy the supplied files to the disk.

   4. Obtain access to a disk containing the files listed in section 4
      (or put them on the same disk <u>MAILER</u> is on).

   5. Run <strong>MCONFIG</strong>.  It is self-explanatory.

   6. <u>MAILER</u> is now ready to run.   The  <strong>&quot;PROFILE</strong>  <strong>EXEC&quot;</strong>  supplied  is
      designed for AUTOLOG use.  The best way to run <u>MAILER</u> is to have
      <u>VMUTIL</u> or <u>AUTOLOG1</u> AUTOLOG it.  You may however log on  directly
      and start it that way.



<strong>7.</strong> <strong>Overview</strong> <strong>of</strong> <strong><u>MAILER</u></strong> <strong>Operation</strong>

  This  is  a  brief  description of what <u>MAILER</u> does.  When first started,

<u>MAILER</u> reads the Mailer Profile, allocates various tables,  and  loads  any

user  exits that are not built in.  It then goes into a wait state, waiting

for the arrival  of  a  reader  file,  an  SMSG  interrupt,  or  a  console

interrupt.



  If  the  interrupt was an SMSG or from console typein, the command parser

is invoked to process the user command.  For reader interrupts,  each  file

in the reader is read and processed, usually resulting in the generation of

outgoing punch spool files.



  A valid reader file can be one of:
                                  - 10 -
</pre>

---

<!-- Page 12 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


   1. An RFC822 mail file.    A  mail  file  is  a  PUNCH  spool  file
      containing a header seperated by a blank record from the text of
      the message.  The syntax specification for the header format  is
      derived  from  the <u>Standard</u> <u>for</u> <u>the</u> <u>Format</u> <u>of</u> <u>ARPA</u> <u>Internet</u> <u>Text</u>
      <u>Messages</u>, DARPA Request for Comments #822.  NOTE: Only a  subset
      of  RFC822  is  supported  by <u>MAILER</u>.  Specifically, it fails to
      properly parse nested parentheses and route-lists.  The  various
      Internet   RFC&#x27;s   are   available  as  <strong>&quot;RFCxxx</strong>  <strong>STANDARD&quot;</strong>  from
      NICSERVE@BITNIC.

   2. A Batch Simple Mail Transfer Protocol (BSMTP) request.  A  BSMTP
      request is differentiated from an RFC822 mail file by having the
      first record start with the text &quot;HELO &quot;.   This  is  the  first
      command  of an SMTP dialog and looks sufficiently different from
      a mail file to be easily identified.

   3. A reply to a BSMTP request previsouly generated.  A BSMTP  reply
      is  identified  by  having  the first record start with 3 digits
      (e.g. &quot;220&quot;).



  For both mail files and BSMTP requests, <u>MAILER</u> does essentially the  same

thing.  It parses the mail or BSMTP request to obtain a list of recipients.

In the case of BSMTP, this is simple -- each recipient is identified  by  a

RCPT  command.  For RFC822 mail, it is a little more difficult to determine

<u>which</u> addresses should be acted upon.    This  is  one  reason  that  BSMTP

support  was  added  to <u>MAILER</u>.  Another reson is that BSMTP has become the

de-facto standard for interoperation of various mail systems on BITNET.



  Various table entries in the Mailer Profile  determine  exactly  how  the

mail  is  sent to the list of recipients.  Specifically, the INCOMING table

lists which other mailers your <u>MAILER</u> knows about and will trust.    Anyone

not  listed  in  the INCOMING table must send a mail file with a Sender: or

From: field that is consistent with the file&#x27;s RSCS origin node and userid.



  The function of <u>MAILER</u> is described  in  further  detail  in  the  <u>MAILER</u>

                                  - 11 -
</pre>

---

<!-- Page 13 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<u>Program</u> <u>Logic</u> <u>Manual</u>, &quot;PLM LISTING.&quot;



<strong>8.</strong> <strong>MAILER</strong> <strong>PROFILE</strong>

  The  <strong>&quot;MAILER</strong> <strong>PROFILE&quot;</strong> CMS file contains the parameters used to initialize

the <u>MAILER</u> routing tables and set other variables and option  flags.    You

may  want  to  add  or remove names from the list of special userids not to

receive mail.  See section MG, below, for the procedures for modifying  and

automatically regenerating the &quot;MAILER PROFILE&quot;.



  You  can  specify  a different name for the Profile when invoking <u>MAILER</u>.

The syntax is:


    MAILER [<u>fn</u> [<u>ft</u> [<u>fm</u>]]]


The default values  for  <u>fn</u>,  <u>ft</u>,  and  <u>fm</u>  are  <strong>MAILER</strong>,  <strong>PROFILE</strong>,  and  <strong>*</strong>,

respectively.    If,  for  example, your system has shared dasd between two

cpu&#x27;s (as ours does), your &quot;PROFILE EXEC&quot; can invoke <u>MAILER</u> with  different

Profile  names  depending  on  the cpuid.  This allows you to have a single

disk containing the <u>MAILER</u> program, txtlib, and profiles.



  The parts of the Mailer Profile are described below:


<strong>8.1.</strong> <strong>Variable</strong> <strong>Declarations</strong>

  You may declare a variable at any time within the Profile by entering:


    <u>variable</u>_<u>name</u>=<u>value</u>


For example:


                                  - 12 -
</pre>

---

<!-- Page 14 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


    rscsname=RSCS



  All variables declared this way are then available to any  user  exit  to

obtain their value.  Some variable names have special meaning to <u>MAILER</u>:


RSCSname        VMID  of  the  RSCS/Networking server.  Files received from
                this userid are assumed to be from the RSCS network.    The
                file&#x27;s  TAG  text is parsed for the network origin node and
                userid.  Files destined to the RSCS  network  are  sent  to
                this VMID.


RSCSalt         VMID of a secondary RSCS/Networking server.  This is useful
                if you need to run two different RSCS&#x27;s  for  some  reason.
                <u>MAILER</u>   will   always  send  files  to  the  primary  RSCS
                (<u>rscsname</u>), but will accept files from this RSCS  as  being
                from the network.


ThisNode        RSCS node name for the machine <u>MAILER</u> is running on.


RSCS-Domain     The  default  domain name to concatenate with the RSCS node
                name when domain-style names are needed.


Local-Domain    An optional domain name for the machine <u>MAILER</u>  is  running
                on  if  you want something different from the concatenation
                of the RSCS node name and <u>RSCS</u>-<u>Domain</u>.


MaintUser       VMID of the <u>MAILER</u> maintainer.  This  user  is  allowed  to
                send  privileged commands via CP SMSG and will also receive
                files that <u>MAILER</u> has trouble with.


MaintNode       RSCS node name of the <u>MAILER</u> maintainer.  For best results,
                this should be the same as <u>ThisNode</u>.


Supply-Default-Fields
                May have the value True or False, but leave it false  since
                its functionality has not been implemeneted.


Allow-Unknown-Nodes
                May have the value True or False.   If  True,  then  <u>MAILER</u>
                will  attempt  to deliver mail to hosts that are not listed
                                  - 13 -
</pre>

---

<!-- Page 15 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


                in it&#x27;s tables.  Useful when RSCS route  updates  get  done
                before  <u>MAILER</u> Profile updates.  When False, can be used to
                restrict the list of sites that you may exchange mail with.


Allow-Charlatans
                If  true,  BSMTP requests are allowed to give a domain name
                on the HELO command and/or  a  FROM  address  on  the  MAIL
                command  which  do  not  appear  to  correspond to the RSCS
                origin.  For a charlatan HELO, the &quot;Received:&quot; header  line
                lists   that   actual   RSCS  origin  in  the  comment  (in
                parentheses) following the &quot;from domain.&quot;  For a  charlatan
                FROM, an &quot;X-Delivery-Notice:&quot; is added to the mail header.


Msgcmd          The  CP  message  command  to use instead of MSGNOH or MSG.
                Used only by the LOCAL exit.


Msgprefix       The prefix other than &quot;*&quot; to  preceed  messages  with  Used
                only by the LOCAL exit.


Verbose-Logging If  true,  all  BSMTP commands and responses are printed on
                the console.


<strong>8.2.</strong> <strong>ORIGINS</strong> <strong>Table</strong>

  Entries in the ORIGINS table are keyed by the VM userid.  Any reader file

from  a  user  listed  in the table causes the associated ORIGINS user exit

program to be invoked prior to any other processing of the file.  The  list

of available ORIGINS exits is listed in section ORIGINS-EXITS below.


<strong>8.3.</strong> <strong>INCOMING</strong> <strong>Table</strong>

  Entries  in  the INCOMING table are keyed by RSCS origin node and userid.

After a file is received and its mail header has been parsed, the  INCOMING

user  exit  program is invoked.  A typical use of an INCOMING exit is to do

validation of mail  to  prevent  forged  &quot;From:&quot;  userids.    The  list  of

available INCOMING exits is listed in section INCOMING-EXITS below.



                                  - 14 -
</pre>

---

<!-- Page 16 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>8.4.</strong> <strong>OUTGOING</strong> <strong>Table</strong>

  Entries  in  the  OUTGOING  table  are  keyed  by  host  name.   For this

discussion, <u>host</u> <u>name</u> is defined as everything to the right of the  at-sign

in  a  mail address.  For example, in the address <strong><u>EACUS</u></strong><strong>@</strong><strong><u>CUVMA</u></strong>, <strong>CUVMA</strong> is the

host   name   and,    in    the    address    <strong><u>John</u></strong><strong>_</strong><strong><u>Smith</u></strong><strong>@</strong><strong><u>Newcastle</u></strong><strong>.</strong><strong><u>MAILNET</u></strong>,

<strong>Newcastle.MAILNET</strong> is the hostname.



  When  mail  is  to  be  sent to a particular address, <u>MAILER</u> looks up the

hostname in the OUTGOING table and then  invokes  the  OUTGOING  user  exit

program.    If  the  hostname  can&#x27;t  be  found and the <u>Allow</u>-<u>Unknown</u>-<u>Nodes</u>

variable is <strong>True</strong>, then <u>MAILER</u> looks for the special OUTGOING entry with the

key  &quot;<strong>*DEFAULT*</strong>&quot;  and  invokes  the  user  exit  for  that entry.  (See the

description of the DOMAINS table, below, for an exception).   The  list  of

available OUTGOING exits is in section OUTGOING-EXITS, below.


<strong>8.5.</strong> <strong>DOMAINS</strong> <strong>Table</strong>

  The  DOMAINS  table  is  an  optional  table  that  has  the  same  basic

functionality as the OUTGOING table.  The difference is that the key for an

entry  in  the  DOMAINS table need not match the hostname exactly.  It need

only match the right-hand end.  When a DOMAINS  table  is  defined,  <u>MAILER</u>

first  searces  the  OUTGOING  table  and,  failing  to find a match there,

searches the DOMAINS table before finally giving  up  and  going  with  the

&quot;*DEFAULT*&quot; entry.  The DOMAINS and OUTGOING tables use the same exits.








                                  - 15 -
</pre>

---

<!-- Page 17 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>8.6.</strong> <strong>SPECIAL</strong> <strong>Table</strong>

  The  key  to  an  entry  in  the  SPECIAL  table  is a username, that is,

everything to the left of the at-sign in a mail address.  The SPECIAL table

can  be used to implement userid aliases of up to 255 characters in length,

mailing lists, automatic mail forwarding, etc.  Note that the SPECIAL table

is  never  searched  by  the <u>MAILER</u> proper.  It is only used if an OUTGOING

exit (like LOCAL) chooses to.  A list of SPECIAL user exits is  in  section

SPECIAL-EXITS.



<strong>9.</strong> <strong><u>MAILER</u></strong> <strong>User</strong> <strong>Exits</strong> <strong>and</strong> <strong>MAILER</strong> <strong>TXTLIB</strong>

  The  commonly  used  user  exit  programs  are built into <u>MAILER</u>.  Others

reside  in  MAILER  TXTLIB  and  are  dynamically  loaded  by  <u>MAILER</u>  when

referenced in one of the tables at initialization.


<strong>9.1.</strong> <strong>Outgoing</strong> <strong>Exits</strong>

  These exit names are used in the Profile OUTGOING and DOMAINS tables.  In

addition to these standard exits, several sites have implemented their  own

local exits for various functions such as gatewaying mail over &quot;home grown&quot;

or otherwise strange links to other systems.  Each exit has a  type  and  a

(sometimes optional) 8 character parameter.



  There  are  three types of OUTGOING exit which determine exactly when the

exit is invoked and with what information.   Some  exits  may  be  used  as

different  types  if  they are properly written although they generally are

written to be of a specific type.  The three types are:


1    Called once per recipient address.

                                  - 16 -
</pre>

---

<!-- Page 18 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


2    Called once for all recipients having the same server node/userid pair
     and exit name.


3    Same  as  type  2  but  complete  information  about all recipients is
     available.  This includes having different exit  parameters  available
     to the same exit (e.g. BSMTP parameters).



  The OUTGOING exits provide with <u>MAILER</u> are:


<strong>9.1.1.</strong> <strong>BITNET</strong>

  This obsolete type 2 exit sends a file to another RSCS node assumed to be

running <u>MAILER</u>. The file is sent to the server node/userid  listed  in  the

table.  BITNET has been superseded by the BSMTP exit.


<strong>9.1.2.</strong> <strong>BSMTP</strong>
                                                              2
  This type 3 exit sends a Batch Simple Mail Transfer Protocol  data stream

to  the server node/userid from the table.  A RCPT command is generated for

each recipient that caused the exit to be invoked.  The usual  domain  name

used   to  identify  the  sending  host  in  the  HELO  command  is  either

<u>Local</u>-<u>Domain</u> or the concatenation of <u>ThisNode</u> and <u>RSCS</u>-<u>Domain</u>.



  The optional parameter to BSMTP can be one of:


<u>var</u>             A variable name.  The value of the variable is used as  the
                HELO domain instead of the default described above.


<strong>@</strong><u>var</u>            A variable name preceded by an at-sign.  This is used to do
                BSMTP routing to another BSMTP  server  that  supports  the


_______________

  2
   a  store-and-forward  version  of  the  ARPANET  SMTP service defined by
J. Postel in DARPA Request for Comments #821
                                  - 17 -
</pre>

---

<!-- Page 19 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


                BSMTP   source   route.    <u>MAILER</u>  does  not  support  this
                construct, but others such as the Wiscnet SMTP  server  do.
                The value of <u>var</u> is inserted in the source route portion of
                each RCPT TO: address.


<strong>%</strong><u>var</u>            A variable name preceded by a percent-sign.  This  is  used
                to  perform  the  ubiquitous &quot;percent routing hack&quot; that is
                widely used but is not documented  in  any  mail  standard.
                The RCPT TO address is rewritten by changing the at-sign to
                a percent and appending the The value of <u>var</u> to the end  of
                the address.


Note  that  for  the  at-sign  and percent forms of the parameter, that the

variable name must be 7 characters or less.  For the other form,  the  name

must be 8 characters or less.



  Examples  of  the  four  flavors  of  BSMTP  (vanilla  and three types of

parameter) follow:


    via20B=@CU20B.COLUMBIA.EDU
    viaDEC=@DECWRL.DEC.COM
    funnydom=BLAH.BLAH.EDU
     ...
    OUTGOING:
    ;domain  node   userid   exit   type parameter

    CUVMA    CUVMA  MAILER   BSMTP  3
    CUTC20   CUVMA  SMTPSERV BSMTP  3    @via20B
    FROTZ    XYZZY  FUZZNET  BSMTP  3    funnydom
     ...

    DOMAINS:
    .DEC     WISCVM SMTPSERV BSMTP  3    %viaDEC
     ...



  Given the following sample addresses, these RCPT TO&#x27;s would be created:


JOE@CUVMA      RCPT TO:&lt;JOE@CUVMA&gt;

JOE@CUTC20     RCPT TO:&lt;@CU20B.COLUMBIA.EDU:JOE@CUTC20&gt;

                                  - 18 -
</pre>

---

<!-- Page 20 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


JOE@DEC123.DEC RCPT TO:&lt;JOE%DEC123.DEC@DECWRL.DEC.COM&gt;


<strong>9.1.3.</strong> <strong>DEFRT</strong>

  This type 1 exit sends a file directly to the  intended  recipient  on  a

remote  RSCS  node not running <u>MAILER</u>.  The file name is set to that of the

sender (truncated to 8 characters).  The file type is set  to  &quot;MAIL&quot;,  the

class to &quot;M&quot;, and the distribution code to the host name of the sender.



  DEFRT  will  generally  refuse  to  deliver  mail  to userids more than 8

characters long since this is the VM implementation  limit.    An  optional

parameter,  <strong>TRUNCATE</strong>, may be supplied which allows longer names.  The first

8 characters are used in this case.  This facility is meant to be  used  in

conjunction  with  VAX/VMS  systems  running  <u>jnet</u> and UNIX systems running

<u>UREP</u>.  DEFRT ignores the server node/userid information in the table entry.


<strong>9.1.4.</strong> <strong>LOCAL</strong>

  This type 1 exit sends mail to a local user on the system running <u>MAILER</u>.

It sets the file name, etc. as in DEFRT above.  It also sets the file&#x27;s TAG

to look like one RSCS would generate.  This allows a  modified  RDRLIST  or

RECEIVE exec to determine the origin of a remote file in the same manner as

for files from RSCS.



  The optional <strong>PROFS</strong> parameter will cause a  PROFS  Subject  record  to  be

appended  to  the end of the file so that PROFS mail may be used.  Kudos to

Peter Day of Emory University.  See also the PROFS SPECIAL exit.





                                  - 19 -
</pre>

---

<!-- Page 21 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>9.1.5.</strong> <strong>NOMAIL</strong>

  This type 2 exit is used for RSCS nodes that can&#x27;t handle mail.  It  logs

an  error  message  and  causes  the  mail  to  be  returned.  The required

parameter is the host name to give in the error mail.  It sends  mail  back

that essentially says, &quot;Mail service for <u>parameter</u> is not available.&quot;


<strong>9.1.6.</strong> <strong>INTERNET</strong>

  This  type  1  exit  was  written  by  Henry  Nussbacher  of the Weizmann

Institute, Rehovot, Israel and is based on the DEFRT exit as  well  as  the

old  BSMTP exit written by Arthur Ecock of the City University of New York.

Reg Quinton and Jeffrey Honig have also contributed code to it.    INTERNET

is  used  in  the  &quot;*DEFAULT*&quot;  position of the Profile OUTGOING table.  It

searches for host names ending in &quot;.ARPA&quot;, &quot;.UUCP&quot;, &quot;.DEC&quot;, and so  on  and

routes  via  the appropriate gateway.  Much of INTERNET&#x27;s capability is now

in the mainline <u>MAILER</u> code (using the DOMAINS table) and  the  BSMTP  exit

(using the at-sign and percent-preceded parameters).



  Specifying   the   optional   <strong>RETURN</strong>  parameter  causes  mail  lacking  a

domain-style recipient address (i.e. without dots) to be returned with  the

error  &quot;Unknown  Host&quot;  rather  than  attempting  to send the mail via RSCS

(which will generally result in returned mail unless  RSCS  knows  of  some

node names that <u>MAILER</u> doesn&#x27;t).


<strong>9.1.7.</strong> <strong>UTSMAIL</strong>

  This  exit  will  send  mail  to users on a UTS guest running on the same

machine as <u>MAILER</u>.  It submits a UTS <u>uux</u> file which  invokes  the  <u>rmail</u>(<u>3</u>)

program to enter the mail into the UTS mail system.  The required parameter

                                  - 20 -
</pre>

---

<!-- Page 22 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


                             3
is the vmid of the UTS guest.


<strong>9.2.</strong> <strong>Incoming</strong> <strong>Exits</strong>


<strong>9.2.1.</strong> <strong>RSCSVER</strong>

  This  incoming  exit  is  used  to  validate files from sites not running

<u>MAILER</u>.  The sender name and host are checked to see if they match the RSCS

origin  in  the tag text.  This exit is used in the &quot;*DEFAULT*&quot; position of

the Incoming table and allows users on other systems not running <u>MAILER</u>  to

use your <u>MAILER</u> to send mail.


<strong>9.2.2.</strong> <strong>(none)</strong>

  Not  specifying  an  Incoming exit causes the built in validation code to

execute.  This is used to indicate  the  names  of  other  trusted  Mailers

(which are allowed to send a file with a &quot;From:&quot; or &quot;Sender:&quot;  which differ

from their vmid and RSCS node name).


<strong>9.3.</strong> <strong>Special</strong> <strong>Exits</strong>

  Special table exits are used to  define  local  user  name  aliases  like

POSTMASTER and/or mailing list names among other things.











_______________

  3
   UTSMAIL has not been tested with or modified to support UTS/V&#x27;s <u>sendmail</u>
system.
                                  - 21 -
</pre>

---

<!-- Page 23 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>9.3.1.</strong> <strong>FWDMAIL</strong>

  This exit provides mail forwarding.  The list of recipients to forward to

must be defined with a variable assignment in the Profile.    The  variable

name  must  be  8  characters  or  less  and  is  supplied  as the required

parameter.


<strong>9.3.2.</strong> <strong>RSMAIL</strong>

  This exit provides mail resending.  This is similar to forwarding and the

list  of  recipients  is  supplied  in  the  same  way as for FWDMAIL.  The

difference between the two is that FWDMAIL creates a new mail header  while

RSMAIL  simply  adds  &quot;Resent-To&quot;, &quot;Resent-From&quot;, and &quot;Resent-Cc&quot; fields to

the original header.



  An example of using FWDMAIL or RSMAIL follows:


    people=SMITH@CUVMA,JONES@CUVMB
    alan=EACUS@CUVMA
     ...
    SPECIAL:
    CROSSWELL  RSMAIL  1  alan
    FOO-L      RSMAIL  1  people


<strong>9.3.3.</strong> <strong>DISTLIST</strong>

  DISTLIST uses BSMTP  to  re-distribute  mail  to  a  list  of  recipients

contained in a file named <strong>&quot;</strong><strong><u>parameter</u></strong> <strong>DISTLIST&quot;</strong>.  The file should be RECFM V

and have one address per line.  This exit does not edit or add anything  to

the  mail  header  at  all (other than having the side effect of causing an

additional &quot;Received:&quot; header to be added) and could  be  very  useful  for

distribution  of  large  mailing lists.  The only restriction on the use of

this exit is that all the recipients listed must be reachable  with  BSMTP.

                                  - 22 -
</pre>

---

<!-- Page 24 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


That is, it must not matter that the recipient&#x27;s address does not appear in

the &quot;To:&quot; line of the RFC822 mail.  This restriction is satisfied for LOCAL

and  DEFRT  distribution  as  well  as  any  mail server that is defined in

OUTGOING/DOMAINS with the BSMTP exit.  In other words, if  a  recipient  is

reached with the BITNET exit, then the mail will most likely be returned to

the sender with an error of &quot;No recipients for this node.&quot;



  For a file named <strong>&quot;SAMPLE</strong> <strong>DISTLIST&quot;</strong>:


    SMITH@CUVMA
    JONES@CUVMB



  An example profile entry would be:


    SPECIAL:
    SOME-GROUP  DISTLIST 1 SAMPLE


<strong>9.3.4.</strong> <strong>VMMLOG</strong>

  This exit appends mail to a file in the format used by the  Columbia  VMM

mail  program (written by Vace Kundakci) and is used at Columbia to provide

bulletin boards.  The required parameter specifies the  file  name  of  the

VMMLOG  file.    Additionally,  the  file  mode  of  the VMMLOG file may be

specified as a variable named &quot;<u>parameter</u>-mail-disk&quot;.  For example:


    bboard-mail-disk=G
    ...
    SPECIAL:
    bboard   VMMLOG   1 BBOARD






                                  - 23 -
</pre>

---

<!-- Page 25 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>9.3.5.</strong> <strong>PROFS</strong>

  The PROFS exit does the same thing as the LOCAL OUTGOING  exit  with  the

PROFS  parameter.  You would use this exit when most users get the standard

LOCAL treatment but a specific list of users (which you  enumerate  in  the

SPECIAL table) want mail delivered in PROFS format.


<strong>9.3.6.</strong> <strong>(none)</strong>

  Omitting  the  exit  name  causes  the  built in exit to be invoked which

disallows mail to the given user name.  Use this approach to  prevent  mail

from being sent to specific users like CMSBATCH and SYSTEM.


<strong>9.4.</strong> <strong>Origins</strong> <strong>Exits</strong>

  The  Origins table lists exits to be invoked based on origin userid.  The

exit is given control immediately after the  file  has  been  received  and

prior  to  any  other  processing  has  been performed.  This is unlike the

Incoming exit which is not invoked until the mail header has been  read  in

and  parsed.   An Origins exit can be used to do arbitrary processing of an

input file based on origin userid.


<u>none</u>            Leaving the exit name blank causes the built in exit to  be
                invoked  which  causes the received file to be purged and a
                message sent to the originating userid.  This is useful  to
                dissallow  users  like  CMSBATCH  from sending any files to
                <u>MAILER</u> and guarantees that no error mail files will be sent
                back to those users.



<strong>10.</strong> <strong><u>MAILER</u></strong> <strong>Commands</strong>

  The  <u>MAILER</u>  accepts  commands  via CP SMSG or from the console.  Certain

commands are restricted to SMSGs from the Maintainer (as defined in <strong>&quot;MAILER</strong>

<strong>PROFILE&quot;</strong>)  or  console  commands.   Other commands are available to general

                                  - 24 -
</pre>

---

<!-- Page 26 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


users via SMSG.  SMSG replies are sent back to the issuer via CP MSGNOH  if

the  <u>MAILER</u> has the necessary privileges.  If not, CP MSG is used.  Help on

commands may be obtained by use of &quot;?&quot; in commands and  subcommands.    For

example, &quot;CP SMSG MAILER ?&quot; will cause a list of the valid general commands

to be sent back.



  Privileged commands are  described  in  &quot;MAILER  HELPPRIV&quot;.    These  are

designed for debugging purposes and to aid general maintenance.



<strong>11.</strong> <strong>Maintenance</strong>

  New  versions  of <u>MAILER</u> and exits will be sent to you from time to time.

Additionally, you are expected to keep your site&#x27;s &quot;MAILER PROFILE&quot;  up  to

date.


<strong>11.1.</strong> <strong>New</strong> <strong>versions</strong>

  New  versions  will be sent to you preceded by mail describing the reason

for the new version.  Right now, these files are generally  sent  with  the

Cornell  CARD  utility  (which has been supplied to you).  To install a new

&quot;MAILER  MODULE&quot;,  simply  rename  the  old  &quot;MAILER  MODULE&quot;  (for  backup

purposes), CARD LOAD the new one and restart <u>MAILER</u>.



  TEXT  files  are sent out as replacements for members of &quot;MAILER TXTLIB&quot;.

Use the appropriate TXTLIB DEL and TXTLIB ADD commands to install new  TEXT

files.  See the <u>CMS</u> <u>Command</u> <u>and</u> <u>Macro</u> <u>Reference</u> for details.






                                  - 25 -
</pre>

---

<!-- Page 27 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>11.2.</strong> <strong>&quot;MAILER</strong> <strong>PROFILE&quot;</strong> <strong>updates</strong>

  The  <strong>MG</strong>  program  can  be  used  to  automate regeneration of your site&#x27;s

&quot;MAILER PROFILE&quot; which must usually be done once a month  when  the  latest

&quot;NAMES&quot;  files  become  available.    Using  MG,  you  only  have  to  make

site-specific changes once, when you first install <u>MAILER</u>.


<strong>11.2.1.</strong> <strong>Files</strong> <strong>Used</strong> <strong>by</strong> <strong>MG</strong>

  MG uses four or more input files:


&quot;* NAMES&quot;             The NAMES files previously described in section 4.


&quot;<u>name</u> MTPLATE&quot;        The template file used by MG to build &quot;<u>name</u> PROFILE&quot;.
                      This  file  looks  just  like  the  Profile  with the
                      exception that the  incoming,  outgoing  and  domains
                      table  lines  are  replaced  by  the words <strong>%incoming</strong>,
                      <strong>%outgoing</strong> and <strong>%domains</strong>, respectively.    MG  replaces
                      these  lines  with  the appropriate information taken
                      from &quot;XMAILER NAMES&quot;, &quot;MAILER NAMES&quot;, &quot;DOMAIN NAMES&quot;,
                      and  &quot;<u>name</u>  MAILLCL&quot;.    You  can  also use &quot;<strong>%include</strong>
                      <u>fn</u>.<u>ft</u>.<u>fm</u>&quot; to cause a  file  to  be  included  in  the
                      output Profile.


&quot;<u>name</u> MAILLCL&quot;        This  file  consists of the lines in the OUTGOING and
                      DOMAINS tables that will be different for your  site.
                      That  is,  the entry or entries for the local system.
                      For example, for the site CUVMB,  the  Outgoing  line
                      for  CUVMB  specifies  the  BITNET  exit in all other
                      sites&#x27;  Profiles  and  the  LOCAL  exit  for  CUVMB&#x27;s
                      Profile.  Entries in &quot;<u>name</u> MAILLCL&quot; should follow the
                      same syntax as lines  in  the  Outgoing  table.    No
                      comments or blank lines are permitted.



  <u>Name</u>, above, is typically <strong>MAILER</strong>, but you may use any convenient name.








                                  - 26 -
</pre>

---

<!-- Page 28 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>11.2.2.</strong> <strong>Invoking</strong> <strong>MG</strong>

  To run MG, type:


    MG [-i] <u>name</u> [<u>alternate</u> <u>NAMES</u> <u>file</u> <u>names</u>]


<u>NAMES</u> file names preceded by a &quot;-d&quot; are used for the DOMAINS table.  Others

are used for the OUTGOING and INCOMING tables.



  The typical invocation of MG is:


    MG MAILER



  An example of using a different Outgoing/Incoming NAMES file is:


    MG MAILER NEWBIT.NAMES.G MAILER.NAMES



  Finally, an example of using a different DOMAINS NAMES file is:


    MG MAILER -d NEWDOM.NAMES


[Note the use of OS-style &quot;dot&quot; notation for file names.]



  You will normally not need the &quot;-i&quot; switch to MG which tells it to set up

an  &quot;initial  distribution&quot; profile -- that is, a profile for a Mailer that

other Mailers do not yet have in their INCOMING tables.  The  default  list

of   NAMES   files   used   by  MG  is:    &quot;XMAILER.NAMES  MAILER.NAMES  -d

DOMAIN.NAMES&quot;.



  MG will also accept EARN-style &quot;NODES&quot; files which differ  only  slightly

                                  - 27 -
</pre>

---

<!-- Page 29 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


from  the  BITNET  &quot;NAMES&quot;  files.   So, if you prefer to use the EARN node

administration tools, you may do so.


<strong>11.2.3.</strong> <strong>Examples</strong> <strong>of</strong> <strong>MG</strong> <strong>input</strong> <strong>files</strong>


---------------------------------------------------------------------------


             <strong>Figure</strong> <strong>1:</strong>  Sample template file: &quot;CUVMB MTPLATE&quot;


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
    POSTMAST=EACUS@CUVMA         ; Person for mail system questions
    BITINFO=EACUS@CUVMA          ; BITNET Info id

    ; local origins whose files I want to throw away:

    ORIGINS:
    CMSBAT01
    CMSBAT02
    CMSBAT03
    CMSBAT04
    CMSBAT05
    END ORIGINS

    INCOMING:
    ; Node   Userid   Exit
    ;------- -------- ---------
    <strong>%incoming</strong>
    *DEFAULT*      *DEFAULT*     RSCSVER   ; standard RSCS tag checking
    END INCOMING








                                  - 28 -
</pre>

---

<!-- Page 30 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


    ; Table of mail nodes that I can send to.

    OUTGOING:
    ; Alias name     Node     Userid   Exit     Type Parm
    ; ----------     -------- -------- -------- ---- --------
    <strong>%outgoing</strong>
    CUVMB.COLUMBIA.EDU   *       *     LOCAL     1
    *DEFAULT*        *DEFAULT*   ?     INTERNET  1
    END OUTGOING

    ; Domains

    DOMAINS:
    <strong>%domains</strong>
    END DOMAINS
    ; Local addresses to receive &quot;special handling&quot;:

    SPECIAL:
    SYSTEM                        ; prevent punched decks
    POSTMASTER RSMAIL 1 POSTMAST  ; Person for mail questions
    BITINFO    RSMAIL 1 BITINFO   ; Person for BITinfo questions
    END SPECIAL



---------------------------------------------------------------------------


---------------------------------------------------------------------------




        <strong>Figure</strong> <strong>2:</strong>  Sample local replacements file: &quot;CUVMB MAILLCL&quot;


    CUVMB     *      *      LOCAL   1
    CUUTSA    *      *      UTSMAIL 1  UNIXA



---------------------------------------------------------------------------


<strong>11.2.4.</strong> <strong>Cookbook</strong>

  The cookbook approach to build your <u>MAILER</u> Profile with MG follows.  This

is done for you by the MCONFIG EXEC.



                                  - 29 -
</pre>

---

<!-- Page 31 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


  The first time only:


   1. Create a &quot;<u>name</u> MTPLATE&quot; file similar to the one in figure 1.

   2. Create a &quot;<u>name</u> MAILLCL&quot; file with a single line in it:


          <u>SITENAME</u>   *      *    LOCAL   1


   3. Get  yourself  added  onto  the list of people that are informed
      when a new copy of the various NAMES files arrive at  your  site
      (or you can &quot;poll&quot; NICSERVE@BITNIC for new versions).  The files
      are huge, so try to  find  them  on  your  system  first  before
      obtaining them from NICSERVE.



  Each  time  a  new  &quot;XMAILER&quot;,  &quot;MAILER&quot;  or  &quot;DOMAIN NAMES&quot; file becomes

available:


   1. Obtain read access to the NAMES files on any accessed disk.

   2. Obtain read access to &quot;<u>name</u> MTPLATE&quot; and &quot;<u>name</u> MAILLCL&quot;  on  any
      accessed disk.

   3. Type:


          MG <u>name</u>


   4. When  MG  is  done, a new file, &quot;<u>name</u> PROFILE A1&quot; will have been
      created.

   5. Compare &quot;<u>name</u> PROFILE A1&quot; to &quot;MAILER PROFILE&quot; on the  disk  that
      <u>MAILER</u>  reads  when it starts up to check for any discrepancies.
      You should see only a list of new BITNET sites plus  some  sites
      that have put <u>MAILER</u> into production use recently.

   6. If  you  are  satisfied  with  the  new  &quot;<u>name</u> PROFILE&quot;, replace
      &quot;MAILER  PROFILE&quot;  with  it  (after  making  a   backup   copy).
      Occasionally,  the  data  provided  by  BITNIC  is incorrect and
      <u>MAILER</u> will find a fatal error in the new PROFILE  so  you  will
      have to revert to the previous version.

   7. Tell  Mailer  to  re-access  the  disk  reload  (see the example
      below).

                                  - 30 -
</pre>

---

<!-- Page 32 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


  The suggested way to install a  new  &quot;MAILER  PROFILE&quot;  while  <u>MAILER</u>  is

running is as follows:


   1. Obtain  write  access  to  the  minidisk  containing the &quot;MAILER
      PROFILE&quot; (assuming the <u>MAILER</u> has read-only access).

   2. Make the needed changes to the file.

   3. Tell <u>MAILER</u> to reaccess the disk.  For example, if the  disk  is
      linked  at 193 as the B disk, issue the following commands (User
      typein prefixed by &quot;.&quot;):


          .CP SMSG MAILER CMS ACC 193 B
          | CMS command accepted.
          | R(00000) for: ACC 193 B
          .CP SMSG MAILER RELOAD
          | Reload command accepted.


      (These commands will only be accepted by the  userid  listed  as
      the <u>MAILER</u> maintainer in the profile.)



<strong>12.</strong> <strong>Diagnosing</strong> <strong>Problems</strong>

  <u>MAILER</u>  breaks  from time to time.  Typically, some error will occur from

which it thinks it can&#x27;t recover, so it will exit.  This generally leads to

<u>MAILER</u>  logging out if you use the standard PROFILE EXEC.  If this happens,

take a look at the topmost file in <u>Mailer</u>&#x27;<u>s</u> reader.  This  is  usually  the

culprit.    Most  errors  have to do with wild syntax in a file that causes

<u>MAILER</u> to crash due to some  &quot;impossible  case&quot;  happening  in  the  header

parsing module (which is admittedly a sloppy job).



  Transfer this file away from <u>MAILER</u> and log it back in.  It will probably

run just fine.  Take a look at the file and if you can&#x27;t figure out why  it

caused  <u>MAILER</u>  to break down, then try asking someone else on BITNET (such

as the members of the XMAILER or MAIL-L mailing lists).
                                  - 31 -
</pre>

---

<!-- Page 33 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>13.</strong> <strong>Messages</strong>

  <u>MAILER</u> generates messages on the  console,  as  messages  sent  to  users

within  mail  reporting  an error, and as BSMTP replies.  Most messages use

the standard CMS format of <u>CCCMMMNNNT</u> where <u>CCC</u> is &quot;MAI&quot; for <u>MAILER</u>, <u>MMM</u> is

the  particular  <u>MAILER</u>  module  issuing  the  message,  <u>NNN</u> is the message

number, and <u>T</u> is one of three standard message  types  and  two  additional

types:


I               Informational.


W               Warning.


E               Error.


U               Report error to user within error mail.


X               Send  RSCS  MSG  to  mail  sender  (e.g.  who  the  mail is
                &quot;From:&quot;).



  The message code is never put in error mail or sent as a CP or  RSCS  MSG

but is written to the console depending on the CP EMSG setting.


<strong>13.1.</strong> <strong>Message</strong> <strong>Codes</strong>

  The  following  messages  with  message  numbers  are generated by <u>MAILER</u>

proper and the various user exits:


MAILER000E Unable to obtain file &#x27;<u>fn</u> <u>ft</u> <u>fm</u>&#x27;, RC=<u>nnnn</u>


MAILER001I CUCCA VM Network MAILER <u>version</u>  <u>date</u>  Copyright  (c)  1982,1987
    Columbia University


MAILER002E Routing file not lrecl 80 and recfm F.
                                  - 32 -
</pre>

---

<!-- Page 34 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


MAILER003E Stack overflow in <u>routine</u>
    <u>MAILER</u> did not have adequate virtual storage available to  process  the
    mail.   This may be caused by a garbage file in the reader, a mail file
    with a very large header, or the virtual machine size is too small.


MAILER004I MAILER running on <u>node</u>


MAILER005I Initialization complete.


MAILER006I MAILER waiting for work.


MAILER007E MAILER stopping due to error <u>nn</u>
    Look  for  a previous error message which indicates the specific nature
    of the error.


MAILER008E DIAG 14 RC=<u>nnnn</u>
    CP  Diagnose  X&#x27;14&#x27;  failed.    This &quot;never happens&quot; except when the CP
    SPOOL is in a mess.


MAILER009I Received file <u>spoolid</u> sender: <u>vmid</u> from: <u>node</u> &lt;userid&gt;


MAILER010E RDCARD RC=<u>nnnn</u>
    See  message 008, above and the <u>CMS</u> <u>Command</u> <u>and</u> <u>Macro</u> <u>Reference</u> for the
    RDCARD macro return codes.


MAILER011I <u>nnnn</u> bytes of stack space.
    After  initializing  (allocating tables and loading user exits), <u>MAILER</u>
    allocates the rest of available contiguous virtual storage for use as a
    stack for mail processing tasks.


MAILER012I &#x27;<u>name</u>&#x27; not in routing table.


MAILER013W Distrib exit &#x27;<u>name</u>&#x27; RC=<u>nnnn</u>
    The named exit set a non-zero return code.  See if the  exit  generated
    any error messages preceding this one.


MAILER014W Invalid entry in &#x27;<u>file</u> <u>name</u>&#x27;, line <u>lineno</u>


MAILER015W Table add error <u>nn</u>, line <u>lineno</u>
    An error code was returned from the function that adds entries  to  one
                                  - 33 -
</pre>

---

<!-- Page 35 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


    of  the  several tables <u>MAILER</u> uses.  See what line <u>lineno</u> contains and
    hopefully that will tell you what went wrong.


MAILER016E <u>name</u> table is empty


MAILER017E User exit &#x27;<u>name</u>&#x27; load error <u>nn</u>
    <u>Nn</u>  is  the  return code from the CMS LOAD command.  <u>MAILER</u> dynamically
    loads user exits by issuing a &quot;LOAD <u>name</u> (ORIGIN <u>some</u> <u>address</u>).   These
    exits  must  not  contain any unresolved external references (much like
    RSCS exits).  (Exit developers should see the CMS PRELOAD command).


MAILER018I User exit &#x27;<u>name</u>&#x27; loaded at <u>hex</u> <u>address</u>
    Useful for determining the source of abends.


MAILER019U Unknown network sender: <u>node</u> <u>userid</u>
    A user not registered as a trusted mailer tried to send mail containing
    other than his own node name and userid as the sender.  See also mesage
    021.


MAILER020U Outgoing mail has no FROM: or SENDER: field.
    The RFC822 mail header is missing a required field.


MAILER021U FROM: or SENDER: inconsistent with spool file origin.
    See message 019.


MAILER022E Invalid exit type &#x27;<u>type</u>&#x27; for &#x27;<u>exit</u> <u>name</u>&#x27;


MAILER023U Required DATE is not present.
    See message 020.


MAILER024U Validity check failed.
    See message 019.


MAILER025U Message syntax is unrecognizable.
    Something  other  than  a  PUNCH file containg RFC822 mail, BSMTP, or a
    BSMTP reply appeared in <u>MAILER</u>&#x27;s  reader.    The  usual  culprit  is  a
    NETDATA file.


MAILER026U Internal error in <u>routine</u>


                                  - 34 -
</pre>

---

<!-- Page 36 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


MAILER027U Unknown host: <u>name</u>...


MAILER028U Error loop detected.
    <u>MAILER</u> has determined that further transmission of  a  piece  of  error
    mail  will  result  in it being sent to another mail server which would
    cause a mail loop.  Instead, the mail is sent to the maintainer to deal
    with.


MAILER029I Returning file to <u>user</u> AT <u>node</u>


MAILER030U Syntax error in field- <u>fieldname</u>


MAILER031U &#x27;<u>name</u>...&#x27; is too long to be a VM userid.


MAILER032U No such local user: <u>vmid</u>


MAILER033U Mail to <u>vmid</u> is not allowed.
    <u>Vmid</u> is in the SPECIAL table.


MAILER034X Mail sent to <u>user</u> at <u>node</u>


MAILER035U File is not a PUNCH file.


MAILER036U  RSCS  rejected  mail  w/tag:  <u>tag</u>  <u>text</u>....  Is  the   hostname
    misspelled?
    A file generated by <u>MAILER</u> was apparently transferred back  from  RSCS,
    indicating  that  a non-existent node was specificied.  Be careful with
    this one since a user may also fool <u>MAILER</u> by transferring a mail  file
    that he has received after playing around with the TAG.


MAILER037U Another network mailer sent an invalid file.


MAILER038I Mailer stopping.


MAILER039I Mailer reloading.


MAILER040X Mail sent to <u>netname</u> node <u>name</u>


                                  - 35 -
</pre>

---

<!-- Page 37 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


MAILER041U Mail service is not available for <u>name</u>
    Message generated by NOMAIL exit.


MAILER042I Mail processing not active.
    <u>MAILER</u>  has  been  quiesced.    It  will  accept and respond to SMSG or
    console commands but will not process any  files  that  arrive  in  its
    reader.


MAILER043I <u>nnnn</u> recipient blocks.


MAILER044U No recipients for this node.
    RFC822 mail was received from a remote sender  (e.g.  sent  by  another
    <u>MAILER</u> with the BITNET exit


MAILER045E Can&#x27;t ready reader 00C.  CP RC=<u>nnnn</u>
    See message 008.


MAILER046U Auto-forwarding error.


MAILER047X Mail for <u>parm</u> auto-forwarded to <u>value</u>


MAILER048U Transferred files not allowed.  Was it a bad RSCS node  or  user
    CP TRANSFER?


MAILER049X File <u>vmid</u> <u>spoolid</u> rejected.
    Generated by default ORIGINS exit.


MAILER050E <u>routine</u> RC=<u>nnnn</u>


MAILER051U Unknown domain: <u>domain</u>...


MAILER052X Mail sent to <u>nnnn</u> recipients in distribution list <u>name</u>


MAILER053U Error <u>nn</u> processing distribution list <u>name</u>


MAILER054I BSMTP request transaction start.


MAILER055I BSMTP request transaction end.
                                  - 36 -
</pre>

---

<!-- Page 38 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


MAILER056I BSMTP reply sent to <u>node</u> <u>userid</u>


MAILER057I BSMTP reply transaction start.


MAILER058I BSMTP reply transaction end.


MAILER059I Error mail sent.


MAILER060E BSMTP reply syntax unrecognizable.  Reply ignored.


MAILER061E BSMTP reply contains multiple &quot;050  MAIL  FROM&quot;  lines.    Reply
    ignored.


MAILER062E A-disk not writeable.


MAILER063U Mail aborted.  Maximum hop count exceeded.
    The BSMTP processing code counts &quot;Received:&quot;  headers  and  refuses  to
    further  send  any  mail  that  has  more  than 50 of them.  This is an
    attempt to curtail mail loops caused by recursive  distribution  lists,
    etc.


MAILER064W Duplicate table entry (line <u>nnnn</u>) ignored.
    The entry on line <u>nnnn</u> of the profile has not been added to the current
    table being defined because it was previsously entered into the table.


MAILER065U File is corrupt: <u>file</u> <u>name</u>


MAILER066X Mail appended to <u>name</u> bulletin board.


<strong>13.2.</strong> <strong>BSMTP</strong> <strong>Messages</strong>

  The  following  messages are generated by the BSMTP processor in response

to the command or condition indicated.  These messages appear in the  BSMTP

reply (when sent) and on the console:


(initial)


                                  - 37 -
</pre>

---

<!-- Page 39 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


      220 &lt;<u>domain</u>&gt; Columbia MAILER &lt;<u>version</u>&gt; BSMTP service ready.


(any)


      500 Command out of sequence.


      500 Command ignored due to a previous error.


      421 Service not available (internal error).
          Typically caused by a failure to obtain needed virtual storage.


      501 Parameter syntax error.


      553 Mailbox syntax error.


      554 Mail aborted.  Maximum hop count exceeded.


HELO


      250 &lt;<u>my</u> <u>domain</u>&gt; Hello &lt;<u>sender</u>&#x27;<u>s</u> <u>domain</u>&gt;


      250 &lt;<u>my</u> <u>domain</u>&gt; You are a charlatan; &lt;<u>sender</u>&#x27;<u>s</u> <u>domain</u>&gt; is bogus for a
          file with RSCS origin &lt;<u>node</u>&gt; &lt;<u>userid</u>&gt;
          However,  the  mail  will  be  allowed   to   continue   as   the
          allow-charlatans variable is True.


      501  &lt;<u>sender</u>&#x27;<u>s</u>  <u>domain</u>&gt;  is  bogus for a file with RSCS origin &lt;<u>node</u>&gt;
          &lt;<u>userid</u>&gt;
          Allow-Charlatans is False.


MAIL


      250 &lt;<u>mailbox</u>&gt;... sender OK.


      501 &lt;<u>mailbox</u>&gt; does not correpsond to RSCS origin of &lt;<u>node</u>&gt; &lt;<u>userid</u>&gt;


RCPT
                                  - 38 -
</pre>

---

<!-- Page 40 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


      250 &lt;<u>mailbox</u>&gt;... recipient OK.


      550 Mailbox not found.
          <u>MAILER</u> was unable to find an OUTGOING/DOMAINS table entry for the
          given hostname.


DATA


      354 Start mail input.  End with &lt;crlf&gt;.&lt;crlf&gt;
          Everything up until a line with a lone dot is read in as the mail
          data.  Possible response after the dot is received are:


      250 Mail accepted for delivery.


      552 DATA aborted:  out of disk space.


      451 DATA aborted:  CMS filesystem error.


      554-Mail not delivered to some or all recipients:
          This message is followed by one or more of the &quot;U&quot;-type  messages
          listed above, each on a line starting with &quot;554&quot;.


RSET


      250 Service reset OK.


NOOP


      250 Ok


QUIT


      221 &lt;<u>my</u> <u>domain</u>&gt; Columbia MAILER BSMTP service done.


VERB


      250 Verbose mode ON.
                                  - 39 -
</pre>

---

<!-- Page 41 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


      250 Verbose mode OFF.


TICK


      250 &lt;<u>ticket</u>&gt;... That&#x27;s the ticket.


RPLY


      250 Error replies will be sent as mail.
          Overrides whatever default is in effect and causes errors  to  be
          reported to the user (person in the MAIL FROM) in mail.


      250 Error replies will be sent as smtp.
          Overrides whatever default in in effect and causes errors  to  be
          reported to the mail server (spool file origin) in a BSMTP reply.


:REA


      200 :READ card ignored.
          Makes life easier for people who test <u>MAILER</u> with the  CMS  PUNCH
          command.


(others)


      500 Invalid command.


<strong>13.3.</strong> <strong>Miscellaneous</strong> <strong>Messages</strong>


* New mail from &lt;<u>address</u>&gt;
    Generated by LOCAL and PROFS exits and sent to the mail  recipient  via
    CP  MSGNOH,  MSG,  or  whatever  command  is  specified as Msgcmd.  The
    message is prefixed by &quot;*&quot; unless specified as Msgprefix.









                                  - 40 -
</pre>

---

<!-- Page 42 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>Table</strong> <strong>of</strong> <strong>Contents</strong>

   1. Introduction                                                        1
       1.1. Copyright Notice                                              1
       1.2. Additional Terms for Waterloo C Library Object Code           2
       1.3. Support                                                       3
   2. <u>MAILER</u> Maintainer and Postmaster Job Description                    3
   3. Inventory of Installation Materials                                 5
       3.1. Object Code Distribution                                      5
       3.2. Source Code Distribution A -- for sites with Waterloo C       6
       3.3. Source Code Distribution B -- for sites without Waterloo C    6
   4. Additional Files Needed for BITNET                                  6
   5. Your <u>MAILER</u> subscription and registration                           7
       5.1. Subscription via LISTSERV                                     7
       5.2. Registration :mailer tag                                      8
   6. Installation Procedure                                              9
   7. Overview of <u>MAILER</u> Operation                                       10
   8. MAILER PROFILE                                                     12
       8.1. Variable Declarations                                        12
       8.2. ORIGINS Table                                                14
       8.3. INCOMING Table                                               14
       8.4. OUTGOING Table                                               15
       8.5. DOMAINS Table                                                15
       8.6. SPECIAL Table                                                16
   9. <u>MAILER</u> User Exits and MAILER TXTLIB                                16
       9.1. Outgoing Exits                                               16
           9.1.1. BITNET                                                 17
           9.1.2. BSMTP                                                  17
           9.1.3. DEFRT                                                  19
           9.1.4. LOCAL                                                  19
           9.1.5. NOMAIL                                                 20
           9.1.6. INTERNET                                               20
           9.1.7. UTSMAIL                                                20
       9.2. Incoming Exits                                               21
           9.2.1. RSCSVER                                                21
           9.2.2. (none)                                                 21
       9.3. Special Exits                                                21
           9.3.1. FWDMAIL                                                22
           9.3.2. RSMAIL                                                 22
           9.3.3. DISTLIST                                               22
           9.3.4. VMMLOG                                                 23
           9.3.5. PROFS                                                  24
           9.3.6. (none)                                                 24
       9.4. Origins Exits                                                24
   10. <u>MAILER</u> Commands                                                   24
   11. Maintenance                                                       25
       11.1. New versions                                                25
       11.2. &quot;MAILER PROFILE&quot; updates                                    26
           11.2.1. Files Used by MG                                      26
           11.2.2. Invoking MG                                           27
           11.2.3. Examples of MG input files                            28
           11.2.4. Cookbook                                              29
                                   - i -
</pre>

---

<!-- Page 43 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


   12. Diagnosing Problems                                               31
   13. Messages                                                          32
       13.1. Message Codes                                               32
       13.2. BSMTP Messages                                              37
       13.3. Miscellaneous Messages                                      40















































                                  - ii -
</pre>

---

<!-- Page 44 -->

<pre>
                    <u>MAILER</u> Installation and Maintenance


<strong>List</strong> <strong>of</strong> <strong>Figures</strong>

   <strong>Figure</strong> <strong>1:</strong>   Sample template file: &quot;CUVMB MTPLATE&quot;                     28
   <strong>Figure</strong> <strong>2:</strong>   Sample local replacements file: &quot;CUVMB MAILLCL&quot;           29
















































                                  - iii -

</pre>
