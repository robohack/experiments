#!/bin/sh

# I'm not quite sure how or why this works, but we can use BSD sed's "-a" option
# to edit in place:

sed -ane 's/usr/USR/;	# do a substitution in the pattern space
	H;		# append a newline and the pattern space to the hold
	$!d;		# if this is not the last line, delete the pattern space
	g;		# replace the pattern space with the hold
	w '"$1"'	# write pattern space to the file' $1
