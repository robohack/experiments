#!/bin/sh

# The correct/best way to handle lists of files which might contain
# whitespace or metacharacters, etc.
#
# WARNING:  Note though that filenames still can't include any control
# characters and be handled safely!  At minimum the kernel should not
# permit control characters in filenames, as per:
#
#	http://www.dwheeler.com/essays/fixing-unix-linux-filenames.html
#
# In theory the kernel could/should also prevent (or escape?)
# filenames that aren’t a valid UTF-8 encoding.

# first eliminate space from IFS....
#
Sifs=$IFS
IFS=$(printf '\n\t')

pattern='*.el'

for file in $(find . -name "${pattern}" -print) ; do
	#
	# IMPORTANT: always quote any appearance of "${file}"
	#
	case "${file}" in
	-*)
		echo "./${file}"
		;;
	*)
		echo "${file}"
		;;
	esac
done

IFS=$Sifs
