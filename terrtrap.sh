#!/bin/ksh

trap '
	rc=$?;
	if ((ERRNO > 0)); then
		EMSG="; errno: $ERRNO"
	else
		EMSG=""
	fi;
	echo "${0#-}: last command exit code: $rc$EMSG"
' ERR

trap 'echo exit handler for $$: $? 1>&2' 0


set -e
echo "${0#-}: triggering failure with errexit enabled"
false

echo "why are we still here????"

echo "normal exit: $rc"
exit $rc
