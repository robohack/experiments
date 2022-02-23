#!/bin/sh

DATE="date"
DATER0="${DATE} -r 0"
CAT=cat
CATVT="${CAT} -vt"

STDC="%c"
ISO8601='%Y-%m-%dT%H:%M:%SZ'

TAB=`printf '\t'`

TZ=0
export TZ

DATEALL ()
{
	${DATER0} "+foo${TAB}${ISO8601}${TAB}none"
}

DATEP ()
{
	${DATER0} "$@"
}

echo
echo 'a run of "${DATER0}", "${DATER0}" plus fmt, and the functions "DATEALL" and "DATEP":'
echo

${DATER0}
${DATER0} +"foo${TAB}${ISO8601}${TAB}none"
DATEALL
DATEP +"foo${TAB}${ISO8601}${TAB}none"

echo
echo "now each through a pipe to '${CATVT}':"
echo

${DATER0} | ${CATVT}
${DATER0} +"foo ${ISO8601} none" | ${CATVT}
${DATER0} +"foo${TAB}${ISO8601}${TAB}none" | ${CATVT}
DATEALL | ${CATVT}
DATEP +"foo${TAB}${ISO8601}${TAB}none" | ${CATVT}

echo
echo "now through a pipe to 'while read':"
echo

${DATER0} | while read txt more; do
		echo "Read: ${txt}"
		if [ -n "$more" ]; then
			echo "read: got more: '$more' (as expected)"
		fi
	done
# XXX wasn't this supposed to fail somehow?!?!?!?
${DATER0} +"foo ${ISO8601} none" | \
	while read foo txt none more; do
		if [ "${foo}" != "foo" ]; then
			echo "read: field splitting failed: '${foo}' != 'foo'"
		fi
		if [ "${none}" != "none" ]; then
			echo "read: field splitting failed: '${none}' != 'none'"
		fi
		echo "Read: '${txt}'"
		if [ -n "$more" ]; then
			echo "read: got more: '$more' (as expected)"
		fi
	done
# XXX wasn't this supposed to fail too?!?!?!?
${DATER0} +"foo${TAB}${ISO8601}${TAB}none" | \
	while read foo txt none more; do
		if [ "${foo}" != "foo" ]; then
			echo "read: field splitting failed: '${foo}' != 'foo'"
		fi
		if [ "${none}" != "none" ]; then
			echo "read: field splitting failed: '${none}' != 'none'"
		fi
		echo "Read: '${txt}'"
		if [ -n "$more" ]; then
			echo "read: got more: '$more' (NOT EXPECTED!)"
		fi
	done
DATEALL | \
	while read foo txt none more; do
		if [ "${foo}" != "foo" ]; then
			echo "read: field splitting failed: '${foo}' != 'foo'"
		fi
		if [ "${none}" != "none" ]; then
			echo "read: field splitting failed: '${none}' != 'none'"
		fi
		echo "Read: '${txt}'"
		if [ -n "$more" ]; then
			echo "read: got more: '$more' (NOT EXPECTED!)"
		fi
	done
DATEP +"foo${TAB}${ISO8601}${TAB}none" | \
	while read foo txt none more; do
		if [ "${foo}" != "foo" ]; then
			echo "read: field splitting failed: '${foo}' != 'foo'"
		fi
		if [ "${none}" != "none" ]; then
			echo "read: field splitting failed: '${none}' != 'none'"
		fi
		echo "Read: '${txt}'"
		if [ -n "$more" ]; then
			echo "read: got more: '$more' (NOT EXPECTED!)"
		fi
	done

# Change the leading "sh" to whatever other shell you want to test...
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "sh ./" (file-name-nondirectory (buffer-file-name)))
# End:
