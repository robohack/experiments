#!/bin/sh

yell() { echo "$0: $*" >&2; }
die() { yell "$*"; exit 111; }
try() { "$@" || die "cannot $*"; }

# using it
#try cd /some/place
#try tar xzvfp /another/place/stuff.tbz

true
