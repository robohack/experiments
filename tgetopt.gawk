#!/bin/sh
# -*- awk -*-

# Magic to allow an AWK script to use the first "gawk" in $PATH:
#
some_arbitrary_awk_variable=0 "eval" "exec /usr/bin/env" "gawk -f $0 -- ${@}"

# The real AWK code starts here...

# XXX this can only work in GNU AWK since it is the only one which stops
# processing its own command-line options after the (last?) '-f'.

# getopt --- do C library getopt(3) function in GNU AWK
#
# arnold@gnu.ai.mit.edu
# Public domain
#
# Initial version: March, 1991
# Revised: May, 1993

# External variables:
#    Optind -- index of ARGV for first non-option argument
#    Optarg -- string value of argument to current option
#    Opterr -- if non-zero, print our own diagnostic
#    Optopt -- current option letter

# Returns
#    -1     at end of options
#    ?      for unrecognized option
#    <c>    a character representing the current option

# Private Data
#    _opti  index in multi-flag option, e.g., -abc
function getopt(argc, argv, options,    optl, thisopt, i)
{
    optl = length(options)
    if (optl == 0)        # no options given
        return -1

    if (argv[Optind] == "--") {  # all done
        Optind++
        _opti = 0
        return -1
    } else if (argv[Optind] !~ /^-[^: \t\n\f\r\v\b]/) {
        _opti = 0
        return -1
    }
    if (_opti == 0)
        _opti = 2
    thisopt = substr(argv[Optind], _opti, 1)
    Optopt = thisopt
    i = index(options, thisopt)
    if (i == 0) {
        if (Opterr)
            printf("%c -- invalid option\n",
                                  thisopt) > "/dev/stderr"
        if (_opti >= length(argv[Optind])) {
            Optind++
            _opti = 0
        } else
            _opti++
        return "?"
    }
    if (substr(options, i + 1, 1) == ":") {
        # get option argument
        if (length(substr(argv[Optind], _opti + 1)) > 0)
            Optarg = substr(argv[Optind], _opti + 1)
        else
            Optarg = argv[++Optind]
        _opti = 0
    } else
        Optarg = ""
    if (_opti == 0 || _opti >= length(argv[Optind])) {
        Optind++
        _opti = 0
    } else
        _opti++
    return thisopt
}


BEGIN {
	# global variables
	#
	VERSION = "1.0";
	debug = 0;
	param = 0;

	argv0 = ARGV[0];
	sub(/.*\//, "", argv0);	# basename()

	# getopt() setup...
	#
	Opterr = 0;		# getopt() will not print errors....
	Optind = 1;		# start at the beginning, i.e. not argv[0]

	while ((c = getopt(ARGC, ARGV, "dp:v")) != -1) {
		if (c == "d") {
			# XXX in The One True AWK the interpreter gets the '-d'!
			printf("%s: debug enabled\n", argv0);
			debug = 1;
		} else if (c == "p") {
			if (length(Optarg) == 0) {
				printf("%s: option -%s requires an argument\n", argv0, Optopt);
				exit 2;
			}
			if (debug > 0) {
				print "match: " match(Optarg, /^[0-9$]*$/) " RSTART: " RSTART " RLENGTH: " RLENGTH
			}
			if (match(Optarg, /^[0-9]*$/) == 0) {
				printf("%s: option -%s requires an integer\n", argv0, Optopt);
				exit 2;
			}
			printf("%s: parameter for -%s: %d\n", argv0, Optopt, Optarg);
			param = Optarg;
		} else if (c == "v") {
			printf("%s: version: %s\n", argv0, VERSION);
			exit 0;
		} else if (c == "?") {
			# XXX unsupported in many known versions, but not the above
			printf("%s: invalid option -%s\n", argv0, Optopt);
			exit 2;
		} else if (c == ":") {
			# XXX unsupported in most known versions (incl. above)
			# (see length() check for -p)
			printf("%s: missing parameter for -%s\n", argv0, Optopt);
			exit 2;
		} else {
			printf("%s: ");
		}
		
	}

	# after getopt() is done, we have to clear out all the elements of
	# ARGV from one to Optind, so that gawk will not try to process the
	# command line options as file names
	#
	for (i = 1; i < Optind; i++)
		ARGV[i] = "";
	
	for (; Optind < ARGC; Optind++) {
		printf("ARGV[%d] = '%s'\n", Optind, ARGV[Optind]);
		ARGV[Optind] = "";
	}

	# AWKs will normally exit if there's no other pattern+action code blocks
	# than BEGIN{}, but the magic 'var=0 "eval"' line breaks this rule so:
	#
	exit 0;
}

{
	# the middle.
}

END {
	# the end.
}
