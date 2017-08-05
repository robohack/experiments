#!/bin/sh

# using eval and an extra file descriptor to pass a variable values
# from a sub-shell  (both break for ksh93)

cmd=date

set $( { ( cmd_out=$($cmd); echo "cmd_out='$cmd_out' cmd_exit=$?" >&3; ) | logger -t "$cmd"; } 3>&1 )

eval $*

echo "via set: $cmd='$cmd_out', exit='$cmd_exit'"
echo ""

eval $( { ( cmd2_out=$($cmd); echo "cmd2_out='$cmd2_out' cmd2_exit=$?" >&3; ) | logger -t "$cmd"; } 3>&1 )

echo "via pipe: $cmd='$cmd2_out', exit='$cmd2_exit'"
echo ""
