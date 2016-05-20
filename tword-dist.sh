for file in mail/*; do egrep -h -o '\w+' -- "$file" | awk '{print "'$file' " $0;}'; done | sort -k 2 | awk '{print $2}' | uniq -c

for file in *??*; do egrep -h -o '\w+' -- "$file" | sed "s|^|$file\||"; done | sort -t '|' -k 2 | sed 's/^[^|]*|//' | uniq -c
