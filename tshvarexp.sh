#!/bin/sh

DATE=date
DATER="${DATE} -r 0"
CAT=cat
CATV="${CAT} -v"

ISO8601='%Y-%m-%dT%H:%M:%SZ'
TAB=$(printf '\t')

# Lesson:  Don't put code in a variable -- put it in a function!
DATEALL ()
{
	${DATE} -r 0 "+foo${TAB}${ISO8601}${TAB}none"
}
echo "plain '${DATE}' and ${DATER}"

TZ=0
export TZ

${DATE}
${DATER}
${DATER} +"foo${TAB}${ISO8601}${TAB}none"
DATEALL

echo "through a pipe to cat"
${DATE} | ${CAT}
${DATER} | ${CAT}
${DATER} | ${CATV}
${DATER} \
	+"foo${TAB}${ISO8601}${TAB}none" | \
	${CAT}
DATEALL | ${CAT}

echo "through a pipe to while"
${DATE} | while read txt; do
	echo "Read: ${txt}"
	done
${DATER} | while read txt; do
	echo "Read: ${txt}"
	done
${DATER} +"foo${TAB}${ISO8601}${TAB}none" | \
	while read foo txt none; do
		if [ "${foo}" != "foo" ]; then
			echo "read: field splitting failed: '${foo}' != 'foo'"
		fi
		if [ "${none}" != "none" ]; then
			echo "read: field splitting failed: '${none}' != 'none'"
		fi
		echo "Read: '${txt}'"
	done

DATEALL | \
	while read foo txt none; do
		if [ "${foo}" != "foo" ]; then
			echo "read: field splitting failed: '${foo}' != 'foo'"
		fi
		if [ "${none}" != "none" ]; then
			echo "read: field splitting failed: '${none}' != 'none'"
		fi
		echo "Read: '${txt}'"
	done
