#! /bin/sh
:

i=0
while [ "$i" -lt 10 ] ; do
	f=$(awk 'END {printf("%04d\n", '$i');}' < /dev/null)
	touch -t 196807040343 a.$f
	touch -t 203412251801 b.$f
	touch 196807040343 c.$f
	touch 203412251801 d.$f
	i=$(expr $i + 1)
	sleep 1
done
