#! /bin/awk -f
BEGIN{
TRIALS=10000000
for (i=1;i<=TRIALS;i++) {}
print TRIALS, "awk"
}
