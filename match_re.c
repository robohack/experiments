/*
 * match_re - match a string against a regular expression using the POSIX API
 *
 * NOTE:  If icase is non-zero then the goal is to do case-insensitive matching
 * by default.
 *
 * RETURNS:
 *
 *	MATCH_MATCHED	- the string matched the RE
 *
 *	MATCH_NOMATCH	- no match.
 *
 *	MATCH_FAIL	- an error occured, error message written to paniclog
 */
int
match_re(key, exp, icase, start, beg, end, last)
    char *key;			/* string to match */
    char *exp;			/* RE to match against */
    unsigned int icase;		/* ignore case? */
    char **start;		/* optional ptr to beg of match */
    char **beg;			/* optional ptr to beg of sub-expression */
    char **end;			/* optional ptr to after sub-expression */
    char **last;		/* optional ptr to after match */
{
    regex_t preg;
    char re_errmsg[BUFSIZ];
    int reg_icase = icase ? REG_ICASE : 0;
    regmatch_t pmatch[2];
    int  rc;

    DEBUG3(DBG_DRIVER_HI, "match_re: matching '%v' against expression '%v'%s\n",
	   key, exp, icase ? ", ignoring case" : "");

    /* XXX convert to using the native PCRE API via compile_pcre_expr() and match_pcre_expr()... */
    if ((rc = regcomp(&preg, exp, (REG_EXTENDED | reg_icase | ((beg || end) ? 0 : REG_NOSUB))))) {
	(void) regerror(rc, &preg, re_errmsg, sizeof(re_errmsg));
	write_log(WRITE_LOG_PANIC, "match_re: bad regular expression '%v': %s",
		  exp, re_errmsg);
	regfree(&preg);
	return MATCH_FAIL;
    }
    /* zero indicates match */
    switch ((rc = regexec(&preg, key, (size_t) 2, pmatch, 0))) {
    case 0:
	rc = MATCH_MATCHED;
	/* if (pmatch[1].rm_so == -1) assert(pmatch[1].rm_eo == -1); */
	if (start) {
	    *start = key + pmatch[0].rm_so;
	}
	if (beg) {
	    *beg = key + ((pmatch[1].rm_so == -1) ? pmatch[0].rm_so : pmatch[1].rm_so);
	}
	if (end) {
	    *end = key + ((pmatch[1].rm_eo == -1) ? pmatch[0].rm_eo : pmatch[1].rm_eo);
	}
	if (last) {
	    *last = key + pmatch[0].rm_eo;
	}
	break;
    case REG_NOMATCH:
	rc = MATCH_NOMATCH;
	break;
    default:
	(void) regerror(rc, &preg, re_errmsg, sizeof(re_errmsg));
	write_log(WRITE_LOG_PANIC, "match_re: error matching '%v' with expression '%v': %s",
		  key, exp, re_errmsg);
	rc = MATCH_FAIL;		/* error during matching */
    }
    regfree(&preg);

    DEBUG3(DBG_DRIVER_HI, "match_re: '%v' %s expression '%v'\n", key, rc ? "matched" : "did not match", exp);

    return rc;
}
