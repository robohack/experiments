 /* -*- Mode: flex; -*- */

%{
#include <stdio.h>
%}

 /* definitions... */

MONTH		(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)
DAYNO		[1-9][0-9]?
HOUR		(0|1)[0-9]|2[0-4]
MINPS		[0-5][0-9]

WORD		[a-zA-Z0-9][-_a-zA-Z0-9]*

NUMBER		[1-9][0-9]*

TS_SYSLOG	{MONTH}\ +{DAYNO}\ +{HOUR}:{MINPS}:{MINPS}
TS_TAI64	[0-9A-Fa-f]{24}

 /* SYSLOG_VERBOSE      "<"({WORD}|{NUMBER})"."({WORD}|{NUMBER})">" */
SYSLOG_VERBOSE      "<"{WORD}"."{WORD}">"

 /* all words but "sshguard" provided that posix regex don't support negation nor intersection:
  * 1) all words of 2 to 7 characters or 8-* chars
  * 2) words of 7 chars different to "sshguard" (^s.* | s^s.* | ss^h.*
  */
PROCESSNAME ([-_a-zA-Z0-9]{2,7})|([-_a-zA-Z0-9]{9,})|([-_a-rt-zA-RT-Z0-9][-_a-zA-Z0-9]{7})|([sS][-_a-rt-zA-RT-Z0-9][-_a-zA-Z0-9]{6})|([sS]{2}[-_a-gi-zA-gI-Z0-9][-_a-zA-Z0-9]{5})|([sS]{2}[hH][-_a-fh-zA-FH-Z0-9][-_a-zA-Z0-9]{4})|([sS]{2}[hH][gG][-_a-tv-zA-TV-Z0-9][-_a-zA-Z0-9]{3})|([sS]{2}[hH][gG][uU][b-zB-Z0-9][-_a-zA-Z0-9]{2})|([sS]{2}[hH][gG][uU][-_aA][-_a-qs-zA-QS-Z0-9][-_a-zA-Z0-9])|([sS]{2}[hH][gG][uU][-_aA][rR][-_a-ce-zA-CE-Z0-9])

 /* IPv4 address (used in IPv6 address too, for IPv4 encapsulation) */
IPV4        ((25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]?|0)(\.(25[0-5]|2[0-4][0-9]|1[0-9]{2}|[1-9][0-9]?|0)){3})
 /* IPv6 addresses including compressed variants (RFC 2373) */
IPV6        (::|:(:[0-9a-fA-F]{1,4}){1,7}|[0-9a-fA-F]{1,4}:([0-9a-fA-F]{1,4}:(:[0-9a-fA-F]{1,4}){1,5}|(:[0-9a-fA-F]{1,4}){1,6})|([0-9a-fA-F]{1,4}:){3}([0-9a-fA-F]{1,4}:(:[0-9a-fA-F]{1,4}){1,3}|(:[0-9a-fA-F]{1,4}){1,4})|([0-9a-fA-F]{1,4}:){5}([0-9a-fA-F]{1,4}:[0-9a-fA-F]{0,4}:[0-9a-fA-F]{1,4}|(:[0-9a-fA-F]{1,4}){1,2})|([0-9a-fA-F]{1,4}:){1,7}:)
 /* an IPv4 packed in IPv6 as IPv4-mapped IPv6 address */
IPV4MAPPED6 ((:(:0{1,4}){0,4}|0{1,4}:(:0{1,4}){1,3}|(0{1,4}:){2}(0{1,4}:0{0,4}:0{1,4}|(:0{1,4}){1,2})|(0{1,4}:){1,4}):[fF]{4}:(((2[0-4]|1[0-9]|[1-9])?[0-9]|25[0-5])\.){3}((2[0-4]|1[0-9]|[1-9])?[0-9]|25[0-5]))

HOSTADDR    localhost|([-a-zA-Z0-9]+\.)+[a-zA-Z]+|{IPV4}|{IPV6}|{IPV4MAPPED6}

%%

 /* rules... */


 /*
  * syslog banner, eg "Nov 22 09:58:58 freyja sshd[94637]: "
  * tokenized as: timestamp hostname procname(subname) [pid]:
  *  both the subname and pid parts can be missing
  *
  * return SYSLOG_BANNER_PID or SYSLOG_BANNER depending on the presence of [pid]
  */
 /*
  * example from 'syslog -v' with SYSLOG_VERBOSE matching:
  *
  * Jun 19 12:17:19 <4.6>more sshd[6336]: Failed password for root from 43.229.52.70 port 48814 ssh2
  */

 /* handle entries with PID and without PID from processes other than sshguard */
{TS_SYSLOG}\ +{SYSLOG_VERBOSE}?({WORD}|{HOSTADDR})\ +{PROCESSNAME}"["{NUMBER}"]: "	{
	printf("SYSLOG_BANNER_PID:  '%s'\n", yytext);
    }

{TS_SYSLOG}\ +{SYSLOG_VERBOSE}?({WORD}|{HOSTADDR})\ +({PROCESSNAME}":")?	{
	printf("SYSLOG_BANNER_PID:  '%s'\n", yytext);
    }

%%

int main(int, char *[]);

int
main(int argc,
     char *argv[])
{
	++argv, --argc;  /* skip over program name */

	if (argc > 0) {
		yyin = fopen(argv[0], "r");
	} else {
		yyin = stdin;
	}

	yylex();

	exit(0);
}
