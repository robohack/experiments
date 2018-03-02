#!/bin/ksh

ip="192.168.240.240"

num=100

set -A iparr $(echo $ip | sed 's/\./ /g')

nip=$((iparr[0] << 24))
nip=$((nip | iparr[1] << 16))
nip=$((nip | iparr[2] << 8))
nip=$((nip | iparr[3]))

lnip=$((nip + num))

i=1

while [ $nip -lt $lnip ]; do
	let $((nip++))
	iparr[0]=$((nip >> 24))
	iparr[1]=$(((nip >> 16) & 255))
	iparr[2]=$(((nip >> 8) & 255))
	iparr[3]=$((nip & 255))
	printf "#%04d: %d.%d.%d.%d\n" $i ${iparr[0]} ${iparr[1]} ${iparr[2]} ${iparr[3]}
	let $((i++))
done
