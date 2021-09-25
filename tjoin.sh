#!/bin/sh

f1=$(mktemp -t jdata)
f2=$(mktemp -t jdata)

trap "RC=$?; rm -f $f1 $f2*; exit $RC" 0 1 2 3 15

# join on combined key columns #1 and #2, output only matching records
#
# (only 3 lines should be output, A,AB & C,CC & F,GG)

sed -e 's/ //g' -e 's/,/;/' > $f1 <<__EOF__
A, AB,100
 F,GG,99
B,BB,200
A,DF,50
C, CC,300
__EOF__

sed -e 's/ //g' -e 's/,/;/' > $f2 <<__EOF__
A,AB,10
C,CC,30
F, GG,11
B, XX,12
C, DD,40
D,DF, 4
__EOF__

sort -o $f1 $f1
sort -o $f2 $f2

# n.b.:  do not use exec -- see trap on 0 above

# XXX with support for multi-column keys the external key-combining is avoided
#join -o 1.1,1.2,1.3,2.3 -1 1,2 -2 1,2 $f1 $f2

join -t , -o 1.1,1.2,2.2 -1 1 -2 1 $f1 $f2 | sed 's/;/,/'
