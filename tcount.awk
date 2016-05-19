#!/bin/sh

awk '
    BEGIN {
        eights = 0;
        fives = 0;
        threes = 0;
    }
    /8888/ {
        eightln[eights] = $0;
        eights++;
    }
    /5555/ {
        fiveln[fives] = $0;
        fives++;
    }
    /3333/ {
        threeln[threes] = $0;
        threes++;
    }
    # ... and so on
    END {
        printf("%d 8888\n", eights);
        printf("%d 5555\n", fives);
        printf("%d 3333\n", threes);
        for (i = 0; i < eights; i++) {
            print eightln[i];
        }
        print "=========="
        for (i = 0; i < fives; i++) {
            print fiveln[i];
        }
        print "=========="
        for (i = 0; i < threes; i++) {
            print threeln[i];
        }
   }
' << __EOF__
This 23 line has eight 8888
This 11 line has three 3333
need 12 to separate eight and three 3333
eight 32 is greater than three 8888
three 13 is less than eight 3333
three 14 is printed more than eight 3333
__EOF__
