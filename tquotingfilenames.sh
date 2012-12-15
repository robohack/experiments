#! /bin/sh

# The correct way to  if filenames can't include control characters:

IFS=$(printf '\n\t'`)
for file in `find . -name '*.el'` ; do
	if [ ! -f "${file}c" ] ; then
		echo "$file"
	fi
done
