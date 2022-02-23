#!/bin/sh
#
# test shell function definition
#

echo ""
echo "defun_func_test:"

defun ()
{
	_param=$1
	# N.B.:  quoting is HARD!
	_defun_newfunc="newfunc () { echo ${_param}: 'this is from newfunc!'  \"argv[1]=\$1\"; }"
	eval ${_defun_newfunc}
	unset _defun_newfunc _param

	echo "from_inside:"
	newfunc INSIDE
}

defun HELLO

echo "from_outside:"
newfunc OUTSIDE

# Change the leading "sh" to whatever other shell you want to test...
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "sh ./" (file-name-nondirectory (buffer-file-name)))
# End:
