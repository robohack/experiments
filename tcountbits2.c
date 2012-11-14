#ifndef _BITCOUNT_H_
#define _BITCOUNT_H_

/* Return the Hamming Wieght of val, i.e. the number of 'on' bits. */
int bitcount( unsigned int );

/* List of available bitcount algorithms.
 * onTheFly:    Calculate the bitcount on demand.
 *
 * lookupTalbe: Uses a small lookup table to determine the bitcount.  This
 * method is on average 3 times as fast as onTheFly, but incurs a small
 * upfront cost to initialize the lookup table on the first call.
 *
 * strategyCount is just a placeholder. 
 */
enum strategy { onTheFly, lookupTable, strategyCount };

/* String represenations of the algorithm names */
extern const char *strategyNames[];

/* Choose which bitcount algorithm to use. */
void setStrategy( enum strategy );

#endif

#include <limits.h>

#if 0
#include "bitcount.h"
#endif

/* The number of entries needed in the table is equal to the number of unique
 * values a char can represent which is always UCHAR_MAX + 1*/
static unsigned char _bitCountTable[UCHAR_MAX + 1];
static unsigned int _lookupTableInitialized = 0;

static int _defaultBitCount( unsigned int val ) {
    int count;

    /* Starting with:
     * 1100 - 1 == 1011,  1100 & 1011 == 1000
     * 1000 - 1 == 0111,  1000 & 0111 == 0000
     */
    for ( count = 0; val; ++count )
        val &= val - 1;

    return count;
}

/* Looks up each byte of the integer in a lookup table.
 *
 * The first time the function is called it initializes the lookup table.
 */
static int _tableBitCount( unsigned int val ) {
    int bCount = 0;

    if ( !_lookupTableInitialized ) {
        unsigned int i;
        for ( i = 0; i != UCHAR_MAX + 1; ++i )
            _bitCountTable[i] =
                ( unsigned char )_defaultBitCount( i );

        _lookupTableInitialized = 1;
    }

    for ( ; val; val >>= CHAR_BIT )
        bCount += _bitCountTable[val & UCHAR_MAX];

    return bCount;
}

static int ( *_bitcount ) ( unsigned int ) = _defaultBitCount;

const char *strategyNames[] = { "onTheFly", "lookupTable" };

void setStrategy( enum strategy s ) {
    switch ( s ) {
    case onTheFly:
        _bitcount = _defaultBitCount;
        break;
    case lookupTable:
        _bitcount = _tableBitCount;
        break;
    case strategyCount:
        break;
    }
}

/* Just a forwarding function which will call whichever version of the
 * algorithm has been selected by the client 
 */
int bitcount( unsigned int val ) {
    return _bitcount( val );
}

#ifdef _BITCOUNT_EXE_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* Use the same sequence of pseudo random numbers to benmark each Hamming
 * Weight algorithm.
 */
void benchmark( int reps ) {
    clock_t start, stop;
    int i, j;
    static const int iterations = 1000000;

    for ( j = 0; j != strategyCount; ++j ) {
        setStrategy( j );

        srand( 257 );

        start = clock(  );

        for ( i = 0; i != reps * iterations; ++i )
            bitcount( rand(  ) );

        stop = clock(  );

        printf
            ( "\n\t%d psudoe-random integers using %s: %f seconds\n\n",
              reps * iterations, strategyNames[j],
              ( double )( stop - start ) / CLOCKS_PER_SEC );
    }
}

int main( void ) {
    int option;

    while ( 1 ) {
        printf( "Menu Options\n"
            "\t1.\tPrint the Hamming Weight of an Integer\n"
            "\t2.\tBenchmark Hamming Weight implementations\n"
            "\t3.\tExit ( or cntl-d )\n\n\t" );

        if ( scanf( "%d", &option ) == EOF )
            break;

        switch ( option ) {
        case 1:
            printf( "Please enter the integer: " );
            if ( scanf( "%d", &option ) != EOF )
                printf
                    ( "The Hamming Weight of %d ( 0x%X ) is %d\n\n",
                      option, option, bitcount( option ) );
            break;
        case 2:
            printf
                ( "Please select number of reps ( in millions ): " );
            if ( scanf( "%d", &option ) != EOF )
                benchmark( option );
            break;
        case 3:
            goto EXIT;
            break;
        default:
            printf( "Invalid option\n" );
        }

    }

 EXIT:
    printf( "\n" );

    return 0;
}

#endif
