#!/bin/sh
#
# test shell functions
#

bar=none
ebar=enone
export ebar
lbar=nope

# every truly POSIX-compliant shell implements the built-in
# command 'local', but then there's ksh (even #93!)
#
if type local >/dev/null 2>&1; then
	: a decent shell
else
	# Ksh93:
	#
	#	typeset vname[=value]
	#
	#	When invoked inside a function defined with the
	#	function name syntax, a new instance of the variable
	#	'vname' is created, and the variable's value and type
	#	are restored when the function completes.
	#
	# sadly this does not work for functions defined as func() {}
	# (and that's the only portable way to define a function!)
	#
	# I used to like Ksh -- now I'm glad it's only an inspiration...
	#
	alias local='typeset'
fi

testit ()
{
	local lbar

	bar=some
	ebar=esome
	lbar=lsome
}

echo bar=$bar
echo ebar=$ebar
echo lbar=$lbar
testit
if [ $bar != some ]; then
	echo OOPS!!! $bar=$bar
else
	echo bar=$bar
fi
if [ $ebar != esome ]; then
	echo OOPS!!! $ebar=$ebar
else
	echo ebar=$ebar
fi
if [ $lbar = lsome ]; then
	echo OOPS!!! $lbar=$lbar
else
	echo lbar=$lbar
fi
