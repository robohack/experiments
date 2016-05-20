#!/bin/sh

# warning:  this would be foiled by filenames with '|' chars

find . -maxdepth 1 -type f -print |
while read file; do
    egrep -h -o "[[:alnum:]][[:alnum:]_-]*" "$file" |
    tr [A-Z] [a-z] |
    sed "s|^|$file\||"
done |
sort -t '|' -k 2 |
uniq |
awk -F '|' '{
    if (lw != $2) {
        print fc " " lw;
        fc = 0;
    }
    lw = $2;
    fc++;
}'
