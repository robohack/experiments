#!/bin/sh

var="12 bar"
empty=""

if [ ${empty:-"0"} -gt 10 ]; then
	echo empty=${empty} is greater than 10
fi
if [ ${none:-"0"} -gt 10 ]; then
	echo none=${none} is greater than 10
fi
# Here test (i.e. '[') will barf with "[: bar: unexpected operator"
if [ ${var:-'0'} -gt 10 ]; then
	echo var=${var} is greater than 10
fi
# Here test (i.e. '[') gives the more meaningful: "[: '12 bar': bad number"
if [ "${var:-'0'}" -gt 10 ]; then
	echo var=${var} is greater than 10
fi
