#! /bin/ksh

# or a POSIX shell with arrays....

save_args()
{
	local ORIG_ARGS=("$@")
}

# now we can reuse them as desired:
#
#exec foo "${ORIG_ARGS[@]}"

