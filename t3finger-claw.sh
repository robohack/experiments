#!/bin/sh

# derived from http://blackskyresearch.net/try.sh.txt
# Isaac (.ike) Levy

yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 1; }
try() { "$@" || die "cannot $*"; }

# using it
#try cd /some/place
#try tar xzvfp /another/place/stuff.tbz

true
