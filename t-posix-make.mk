.POSIX:
all: x plus subs err
x:
	@echo "Hello,"; false; echo "World"
plus:
	@echo "a command (not executed with -n)"
	+@echo "a command prefixed by '+' executes even with -n"
	@echo "another command (not executed with -n)"
subs:
	@echo make -n plus
	@${.MAKE} -f ${MAKEFILE} -n plus
	@echo make -n -j1 plus
	@${.MAKE} -f ${MAKEFILE} -n -j1 plus
err:
	@(echo Now we expect an error...; exit 1)
	@echo "Oops! you shouldn't see this!"

# 
# from: http://lists.freebsd.org/pipermail/freebsd-standards/2004-May/000569.html
# 
# The
# 	make x
# 
# command should therefore generate two lines:
# 
# 	Hello,
# 	World
# 
# with no error on a 'make' command that is POSIX-compliant.
# 
# The
# 	make -n plus
# 
# command should print
# 
# 	echo a command
# 	echo "a command prefixed by '+' executes even with -n"
# 	a command prefixed by '+' executes even with -n
# 	echo another command
# 
# to show that it is not executing the first and last echo command, but
# is executing the middle one.
# 
# The
# 	make err
# 
# command should print
# 
# 	Now we expect an error...
# 
# and exit with an error code 1.
# 
# Running all of the tests may require adding a few command-line
# arguments if the paricular version of make does not support setting
# the .MAKE or MAKEFILE macros such as this:
# 
# 	make .MAKE=/usr/bin/make MAKEFILE=Makefile
# 


#
# Note that an action which consists of a more complex shell command,
# such as one using a loop, and maybe also one which uses a
# conditional command, should explicitly use 'set -e', if desired, to
# ensure that errors during a sub-command are detected immediately.
#


#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-nondirectory (buffer-file-name)))) (concat (default-value 'compile-command) " -f " fn))
# End:
#
