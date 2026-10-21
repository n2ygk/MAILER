# bsmtp.lpt

<!-- Page 1 -->

<pre>











                    <strong>BATCH</strong> <strong>SIMPLE</strong> <strong>MAIL</strong> <strong>TRANSFER</strong> <strong>PROTOCOL</strong>




















                             E. Alan Crosswell

            Columbia University Center for Computing Activities
















         Prepared for presentation at BITNET programmers&#x27; meeting


                             13 September 1982
</pre>

---

<!-- Page 2 -->

<pre>
                                                                          1


<strong>1.</strong> <strong>Acknowledgements</strong>

  The following paper is based on electronic mail discussions on <u>BITNET</u> and

original ideas for a Batch SMTP implementation developed by Mike O&#x27;Dell  on

<u>ARPAnet</u>.  Thanks to Greg Minshall for making Mike&#x27;s ideas known.



<strong>2.</strong> <strong>Introduction</strong>

  SMTP [RFC821]  is  an  interactive  protocol  that  defines a transaction

between two mail transport servers.  One is defined to be  the  sender  and

the other the recipient.


             1      2
  Using  RSCS , UUCP , or any other store-and-forward transfer system where

a <u>file</u> is the smallest unit of transmission it  is  clearly  impossible  to

maintain  such  a full-duplex transaction.  In RSCS, only files may be used

to communicate between service machines and no guarantee can be made of the

order  of arrival of one file with respect to another.  Also, communication

with other than VM systems is required.  Systems connected  via  HASP  SML,

NJE/NJI and other links exist (MVS, Unix, Tops-20, etc.).



  Why  then  should SMTP be used?  Maybe it shouldn&#x27;t.  However, since ARPA

supports it, it  will  be  well  defined  and  understood  by  the  largest

internet.    Also,  it  seems relatively straightforward to create programs

that will take a Batch SMTP (BSMTP) file and pass  it  to  a  SMTP  system,


_______________

  1
   IBM VM/370 Remote Spooling Communications Subsystem

  2
   Unix to Unix Communication Program
</pre>

---

<!-- Page 3 -->

<pre>
                                                                          2


simulating a real SMTP hookup.



<strong>3.</strong> <strong>Batch</strong> <strong>SMTP</strong> <strong>operation</strong>

  BSMTP uses seperate files for whole sections of a transaction rather than

each line.  The idea is to create a list of commands that would  be  issued

assuming  all  previous commands had received OK replies.  This transaction

log could then be sent to the receiver BSMTP server which  reads  the  file

and logs its responses.  The response log is then returned, etc.



  An  example of an SMTP transaction is in in figure SMTPFIG and an example

of the equivalent BSMTP transaction is in figures BSMTP1 and BSMTP2 below.


    S: HELO CUVMB.BITNET                   ! begin session
    R: 250 CUVMA.DECNET                    ! acknowledgement and ID
    S: MAIL FROM:&lt;EACUS@CUVMB.BITNET&gt;      ! request to send mail
    R: 250 OK                              ! receiver agrees to accept
    S: RCPT TO:&lt;AC2US@CUVMA.BITNET&gt;        ! recipient #1 is named
    R: 250 OK                              ! receiver will do it
    S: RCPT TO:&lt;FOO@CUVMA.BITNET&gt;          ! recipient #2 is named
    R: 551 No such user                    ! denied this one
    S: DATA                                ! ready to send data
    R: Start mail input, end with record containing only &quot;.&quot;
    Date: Monday, 13-Sep-82 10:32 EST
    From: Alan Crosswell &lt;EACUS@CUVMB&gt;
    Subject: Hello
    To: AC2US@CUVMA

    Hello there....
    .


                    <strong>Figure</strong> <strong>1:</strong>  Sample SMTP transaction
</pre>

---

<!-- Page 4 -->

<pre>
                                                                          3


    HELO CUVMB.BITNET
    MAIL FROM:&lt;EACUS@CUVMB.BITNET&gt;
    RCPT TO:&lt;AC2US@CUVMA.BITNET&gt;
    RCPT TO:&lt;FOO@CUVMA.BITNET&gt;
    DATA
    Date: Monday, 13-Sep-82 10:32 EST
    From: Alan Crosswell &lt;EACUS@CUVMB&gt;
    Subject: Hello
    To: AC2US@CUVMA

    Hello there....
    .
    QUIT


                <strong>Figure</strong> <strong>2:</strong>  BSMTP: File from CUVMB to CUVMA


    220 CUVMA.BITNET Batch Simple Mail Transfer Service Ready
    250 CUVMA.BITNET
    250 OK
    250 OK
    551 No such user
    354 Start mail input.  End with &quot;.&quot;
    250 OK
    221 CUVMA.BITNET Service closing transmission...


                <strong>Figure</strong> <strong>3:</strong>  BSMTP: File from CUVMA to CUVMB



<strong>4.</strong> <strong>Modifications</strong> <strong>to</strong> <strong>SMTP</strong> <strong>syntax</strong>

  How does the server on CUVMB in the preceding example know which file  in

its  reader  is the reply to which file it sent?  There&#x27;s no way to be sure

unless some sort of extra identification is imbedded in  the  files.    The

following  proposed additions to the SMTP commands would solve this and the

problem of coordinating processing of replies:
</pre>

---

<!-- Page 5 -->

<pre>
                                                                          4


<strong>4.1.</strong> <strong>Ticket</strong> <strong>command</strong>


    <strong>TICK</strong> &lt;ticket-id&gt;



  The <u>Ticket</u> command is used to mark a  file  with  additional  identifying

information.    This would be a unique message indentifier generated by the

sending BSMTP system.  By use of the <u>ticket</u>, the BSMTP sender would be able

to identify a later BSMTP reply.


<strong>4.2.</strong> <strong>Verbose</strong> <strong>command</strong>


    <strong>VERB</strong>    <strong>ON</strong>
            <strong>OFF</strong>




  The <u>Verbose</u> command solves the problem of coordinating BSMTP transactions

further by specifying that all commands  received  by  the  receiver  BSMTP

system  be  echoed in its reply along with its usual reply codes (VERB ON).

Data other than that on command lines need  not  be  echoed,  so  the  file

overhead is not substantial.


<strong>4.3.</strong> <strong>Advantages</strong>

  Use  of  <u>Ticket</u> and <u>Verbose</u> has several advantages, The <u>ticket</u> provides a

unique identifier that the sender can use to keep track of the files it has

shipped.   When a reply arrives, the sender simply looks up the information

it stored when it generated the <u>ticket</u> and uses the information  to  inform

any  interested parties (the person who sent the mail) of the status of the

request.
</pre>

---

<!-- Page 6 -->

<pre>
                                                                          5


  The use of <u>verbose</u> takes the burden off the sender to remember and  later

synchronize  the  commands  it sent with the replies it receives.  Assuming

the sender trusts the replying BSMTP system, it  has  all  the  information

needed  in the proper order.  This information could very easily be &quot;piped&quot;

into a program that simulates the SMTP full-duplex interaction.



  A very simple sender implementation would not even have to keep track  of

<u>tickets</u> as long as the <u>verbose</u> information is supplied to it.



<strong>5.</strong> <strong>Example</strong> <strong>of</strong> <strong>BSMTP</strong> <strong>operations</strong>

  A complete example would be as follows:


   1. The CUVMB mailer receives a file from a local user to be sent to
      a user at CUVMA.

   2. A unique <u>ticket</u> is created and a file is punched to CUVMA.   The
      <u>ticket</u> is stored for later recall.

   3. The CUVMA mailer receives the file from CUVMB.

   4. Mail is sent to the CUVMA user.

   5. A  <u>verbsose</u>  reply  file  is generated which contains the <u>ticket</u>
      supplied by CUVMB and is punched to the CUVMB mailer.

   6. The CUVMB mailer receives the file.

   7. The file is recognized as a reply rather than an  original  file
      sent  from  CUVMA  by  the  presence  of reply codes rather than
      commands.

   8. Each code 250 line is read and skipped.

   9. Each code 050 line (the echoed  command)  is  processed  to  the
      degree  desired.    In  this case, the <strong>TICK</strong> is found and used to
      access the <u>ticket</u> that was previously saved.

  10. Other code 050 lines and their replies  are  processed  and  the
      appropriate  action  is  taken (i.e. <u>EACUS</u> is informed that user
      <u>FOO</u> was not found).
</pre>

---

<!-- Page 7 -->

<pre>
                                                                          6


    HELO CUVMB.BITNET
    VERB ON
    TICK 0001
    MAIL FROM:&lt;EACUS@CUVMB.BITNET&gt;
    RCPT TO:&lt;AC2US@CUVMA.BITNET&gt;
    RCPT TO:&lt;FOO@CUVMA.BITNET&gt;
    DATA
    Date: Monday, 13-Sep-82 10:32 EST
    From: Alan Crosswell &lt;EACUS@CUVMB&gt;
    Subject: Hello
    To: AC2US@CUVMA

    Hello there....
    .
    QUIT


                <strong>Figure</strong> <strong>4:</strong>  BSMTP: file generated by sender


    220 CUVMA.BITNET Batch Simple Mail Transfer Service Ready
    250 CUVMA.BITNET
    050 VERB ON
    250 OK
    050 TICK 0001
    250 OK
    050 MAIL FROM:&lt;EACUS@CUVMB.BITNET&gt;
    250 OK
    050 RCPT TO:&lt;AC2US@CUVMA.BITNET&gt;
    250 OK
    050 RCPT TO:&lt;FOO@CUVMA.BITNET&gt;
    551 No such user
    050 DATA
    354 Start mail input.  End with &quot;.&quot;
    250 OK
    050 QUIT
    221 CUVMB.BITNET closing transmission...


               <strong>Figure</strong> <strong>5:</strong>  BSMTP: file returned by recipient



<strong>6.</strong> <strong>Compatibility</strong> <strong>with</strong> <strong>SMTP</strong>

  The <u>Ticket</u> and <u>Verbose</u> commands and 050 reply code are additions  on  top

of  the current SMTP specification.  Assuming that they would be ignored by

an SMTP process, compatibility is maintained.  The <u>Verbose</u> command has even

been suggested as a debugging aid for SMTP implementers.
</pre>

---

<!-- Page 8 -->

<pre>
                                                                          7


                                <strong>REFERENCES</strong>
</pre>

---

<!-- Page 9 -->

<pre>
                                                                          i


<strong>Table</strong> <strong>of</strong> <strong>Contents</strong>

   1. Acknowledgements                                                    1
   2. Introduction                                                        1
   3. Batch SMTP operation                                                2
   4. Modifications to SMTP syntax                                        3
       4.1. Ticket command                                                4
       4.2. Verbose command                                               4
       4.3. Advantages                                                    4
   5. Example of BSMTP operations                                         5
   6. Compatibility with SMTP                                             6
</pre>

---

<!-- Page 10 -->

<pre>
                                                                         ii


<strong>List</strong> <strong>of</strong> <strong>Figures</strong>

   <strong>Figure</strong> <strong>1:</strong>   Sample SMTP transaction                                    2
   <strong>Figure</strong> <strong>2:</strong>   BSMTP: File from CUVMB to CUVMA                            3
   <strong>Figure</strong> <strong>3:</strong>   BSMTP: File from CUVMA to CUVMB                            3
   <strong>Figure</strong> <strong>4:</strong>   BSMTP: file generated by sender                            6
   <strong>Figure</strong> <strong>5:</strong>   BSMTP: file returned by recipient                          6
</pre>
