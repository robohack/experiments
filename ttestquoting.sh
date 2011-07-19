#! /bin/sh

# try each of these
#
empty=''
empty='('
empty='! -gt'
empty='!'
empty='-o -a'

#
# "new" way
#
if [ -z "${empty}" ]; then
	echo new: empty
else
	echo new: not empty
fi

#
# old way (quotes on "X" only for readability)
#
if [ "X${empty}" = "X" ]; then
	echo old: empty
else
	echo old: not empty
fi

#
# buggy way (sometimes a syntax error)
#
if [ "${empty}" = "" ]; then
	echo buggy: empty
else
	echo buggy: not empty
fi

#
# wrong "new" way
#
if [ -z ${empty} ]; then
	echo wrong new: empty
else
	echo wrong new: not empty
fi

#
# wrong old way (sometimes a syntax error, sometimes wrong result)
#
if [ ${empty} = "" ]; then
	echo wrong old: empty
else
	echo wrong old: not empty
fi
