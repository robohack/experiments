#!/bin/ksh

# or a POSIX shell with arrays....

# First save all the cmd-line params in an array:
#
set -A ORIG_ARGS "$@"
#
# Proper Ksh allows:
#
#varname=("$@")
#
# now we can reuse them as desired by expanding the array elements:
#
#exec foo "${ORIG_ARGS[@]}"

# cool trick, in ksh93, but probably not pdksh:
#
# Shift an array:
#
#	set -A name "${name[@]:1}"
