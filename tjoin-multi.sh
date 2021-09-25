#!/bin/sh

f1=$(mktemp -t jdata)
f2=$(mktemp -t jdata)
f3=$(mktemp -t out)

trap "RC=$?; rm -f $f1 $f2 $f3*; exit $RC" 0 1 2 3 15

sort > $f1 <<__EOF__
ADA
ADAM
BRUCE
GARY
__EOF__

tab=$(printf '\t')

sort -t "${tab}" -k 1,1 > $f2 <<__EOF__
AB${tab}1
ABA${tab}2
ABB${tab}3
ADA${tab}4
ADA${tab}5
EVE${tab}6
EVE${tab}7
EVE${tab}8
GARY${tab}9
GARY${tab}10
__EOF__

cat > $f3 <<EOF
ADA${tab}4
ADA${tab}5
GARY${tab}9
GARY${tab}10
EOF

join -t "${tab}" -j 1 $f1 $f2 | diff $f3 -
