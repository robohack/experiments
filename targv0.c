	char *argv0 = "progname";

	argv0 = (argv0 = strrchr(argv[0], '/')) ? argv0 + 1 : argv[0];
