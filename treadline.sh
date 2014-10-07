#!/bin/sh

#
# safest way to read a whole line into a variable
#
IFS= read var << EOF
$(foo)
EOF
