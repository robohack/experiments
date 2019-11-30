#
# Command line variables override global variables and environment variables!
#
# N.B.:  You CANNOT set Make variables on the command line if they must be
# adjusted within a Makefile -- they can only be set in the environment!
# Variables set on the command line are effectively always set last, after all
# Makefiles and environment variables have been read and processed.
#
# This is according to POSIX IEEE 1003.1:
#
#	Macro definitions shall be taken from the following sources, in the
#	following logical order, before the makefile(s) are read.
#
#	 1. Macros specified on the make utility command line, in the order
#	    specified on the command line.
#
#	 2. Macros defined by the MAKEFLAGS environment variable, in the order
#	    specified in the environment variable.
#
#	 3. The contents of the environment, excluding the MAKEFLAGS and SHELL
#	    variables and including the variables with null values.
#
#	 4. Macros defined in the inference rules built into make.  Macro
#	    definitions from these sources shall not override macro definitions
#	    from a lower-numbered source.
#
#	Macro definitions from a single source shall override previous macro
#	definitions from the same source.
#
#	Macros defined in the makefile(s) shall override macro definitions that
#	occur before them in the makefile(s) and macro definitions from source
#	4.  If the '-e' option is not specified, macros defined in the
#	makefile(s) shall override macro definitions from source 3.  Macros
#	defined in the makefile(s) shall not override macro definitions from
#	source 1 or source 2.
#
# N.B.:  So, lack of "+=" and "?=" in POSIX Make is a very serious drawback!
#
VAR ?= default
VAR += added

.PHONY: all
all:
	@echo VAR = ${VAR}

#
# make -f tmake-cmd-line-vars.mk VAR=cmdln
# VAR=environ make -f tmake-cmd-line-vars.mk
# VAR=environ make -f tmake-cmd-line-vars.mk VAR=cmdln
#

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-nondirectory (buffer-file-name)))) (concat "VAR=environ " (default-value 'compile-command) " -f " fn))
# End:
#
