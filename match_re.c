/*
 * match_re - match a string against a regular expression using the POSIX API,
 * and return pointers to the matched portion of the string, as well as to the
 * first sub-expression.
 *
 * NOTE:  If icase is non-zero then the goal is to do case-insensitive matching
 * by default.
 *
 * RETURNS:
 *
 *	1	- the string matched the RE
 *
 *	0	- no match.
 *
 *	-1	- an error occured, location of error message written errorp
 */
int
match_re(key, exp, icase, startp, begp, endp, lastp, errorp)
	char *key;		/* string to match */
	char *exp;		/* RE to match against */
	unsigned int icase;	/* ignore case? */
	char **startp;		/* optional ptr to beg of match */
	char **begp;		/* optional ptr to beg of sub-expression */
	char **endp;		/* optional ptr to after sub-expression */
	char **lastp;		/* optional ptr to after match */
	char **errorp;		/* ptr to store location of error string */
{
	regex_t preg;
	char re_errmsg[BUFSIZ];
	regmatch_t pmatch[2];
	int  rc;
	
	if ((rc = regcomp(&preg,
			  exp,
			  (REG_EXTENDED |
			   ((icase) ? REG_ICASE : 0) |
			   ((begp || endp) ? 0 : REG_NOSUB))))) {
		(void) regerror(rc, &preg, re_errmsg, sizeof(re_errmsg));
		(void) asprintf(errorp, "match_re: bad regular expression '%v': %s",
				exp, re_errmsg);
		regfree(&preg);
		return -1;
	}
	/* zero indicates match */
	switch ((rc = regexec(&preg, key, (size_t) 2, pmatch, 0))) {
	case 0:
		rc = 1;
		if (startp) {
			*startp = key + pmatch[0].rm_so;
		}
		/* if (pmatch[1].rm_so == -1) assert(pmatch[1].rm_eo == -1); */
		if (begp) {
			*begp = key + ((pmatch[1].rm_so == -1) ? pmatch[0].rm_so : pmatch[1].rm_so);
		}
		if (endp) {
			*endp = key + ((pmatch[1].rm_eo == -1) ? pmatch[0].rm_eo : pmatch[1].rm_eo);
		}
		if (lastp) {
			*lastp = key + pmatch[0].rm_eo;
		}
		break;
	case REG_NOMATCH:
		rc = 0;
		break;
	default:
		(void) regerror(rc, &preg, re_errmsg, sizeof(re_errmsg));
		(void) asprintf(errorp, "match_re: error matching '%v' with expression '%v': %s",
				key, exp, re_errmsg);
		rc = -1;		/* error during matching */
	}
	regfree(&preg);

	return rc;
}
