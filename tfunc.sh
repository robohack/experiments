#! /bin/sh
#
# test shell functions
#

bar=none

testit ()
{
	bar=some
}

echo bar=$bar
testit
echo bar=$bar
