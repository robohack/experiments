#!/usr/pkg/bin/perl

$TRIALS = 10000000;
for ($i = 1; $i <= $TRIALS; $i++) {
    rand(1);
}
print $TRIALS, 'awk';

