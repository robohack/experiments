// Minimum / maximum of integers
// http://bits.stephan-brumme.com

// references:
// unknown

// code:
int select(int x, int y, int ifXisSmaller, int ifYisSmaller)
{
  int diff  = x - y;
  // sets bit31 to 0xFFFFFFFF if x<y, else 0x00000000
  int bit31 = diff >> 31;

  // return ifXisSmaller if x is smaller than y, else y
  return (bit31 & (ifXisSmaller ^ ifYisSmaller)) ^ ifYisSmaller;
}

int minimum(int x, int y)
{
  // if x<y then return x, else y
  return select(x,y,x,y);
}

int maximum(int x, int y)
{
  // if x<y then return y, else x
  return select(x,y,y,x);
}

int minimumSimple(int x, int y)
{
  if (x < y)
    return x;
  else
    return y;
}

// assembler:
#ifdef _MSC_VER
__forceinline void maximumAsm() // stripped down to core algorithm to avoid overhead of parameter pushes/pops
{
  // compiler: Visual C++ 2008
  // in: ecx - x
  // in: edx - y
  // out: eax - result
  __asm
  {
    ; diff  = x - y
    mov eax, ecx
    sub eax, edx
    ; bit31 = diff >> 31
    sar eax, 31
    ; return (bit31 & (y ^ x)) ^ x
    xor ecx, edx
    and eax, ecx
    xor eax, ecx
  }
}

__forceinline void minimumAsm() // stripped down to core algorithm to avoid overhead of parameter pushes/pops
{
  // compiler: Visual C++ 2008
  // in: ecx - x
  // in: edx - y
  // out: eax - result
  __asm
  {
    ; diff  = x - y
    mov eax, ecx
    sub eax, edx
    ; bit31 = diff >> 31
    sar eax, 31
    ; return (bit31 & (x ^ y)) ^ y
    xor ecx, edx
    and eax, ecx
    xor eax, edx
  }
}

__forceinline void minimumSimpleAsm() // stripped down to core algorithm to avoid overhead of parameter pushes/pops
{
  // compiler: Visual C++ 2008
  // in: ecx - x
  // in: eax - y
  // out: eax - result
  __asm
  {
    cmp ecx, eax
    jge $done
    mov eax, ecx
  $done:
  }
}
#endif


// restrictions:
// - designed for 32 bits
// - behavior of right shift is not clearly defined, may not work on certain platforms / compilers

// explanation:
// A universal helper function select (line 1) sets bit31 (line 5) to either 0x00000000 or 0xFFFFFFFF.
// Then a special property of XOR is used: a XOR b XOR b = a;
// In plain English, applying XOR twice with the same number eliminates this number.
//
// If bit31 is 0x00000000, line 8 becomes (0x00000000 AND (ifXisSmaller XOR ifYisSmaller)) XOR ifYisSmaller
// and since (zero AND something) = zero, only zero XOR ifYisSmaller is left which is ifYisSmaller.
//
// if bit31 is 0xFFFFFFFF, line 8 becomes (0xFFFFFFFF AND (ifXisSmaller XOR ifYisSmaller)) XOR ifYisSmaller
// and since (allBitsSet AND something) = something, (ifXisSmaller XOR ifYisSmaller) XOR ifYisSmaller
// is left, which is ifXisSmaller as explained earlier.
//
// The select routine can be used to implement a branch-free version of min (line 11) and max (line 17).
//
// Even though more instructions are needed than if written using a simple "if", modern processor
// are much faster because the presented code is branch-free and avoids mis-predictions and pipeline stalls.

// validation:
#include <stdio.h>
#ifdef _MSC_VER
#include <intrin.h>
#endif

int main(int, char**)
{
  // Microsoft Visual C++ performance measurement
#ifdef _MSC_VER
  printf("performance test ...\n");

  const size_t maxLoop = 100000;
  const size_t unroll  = 10;

  unsigned long long start = __rdtsc();
  for (size_t i = maxLoop; i != 0; i--)
  {
    // unroll 10 times to keep loop overhead to a minimum
    maximumAsm(); maximumAsm();
    maximumAsm(); maximumAsm();
    maximumAsm(); maximumAsm();
    maximumAsm(); maximumAsm();
    maximumAsm(); maximumAsm();
  }
  unsigned long long elapsed = __rdtsc() - start;
  printf("max: %I64d ticks => about %.3f ticks per call\n", elapsed, elapsed/(maxLoop*float(unroll)));

  start = __rdtsc();
  for (size_t i = maxLoop; i != 0; i--)
  {
    // unroll 10 times to keep loop overhead to a minimum
    minimumAsm(); minimumAsm();
    minimumAsm(); minimumAsm();
    minimumAsm(); minimumAsm();
    minimumAsm(); minimumAsm();
    minimumAsm(); minimumAsm();
  }
  elapsed = __rdtsc() - start;
  printf("min: %I64d ticks => about %.3f ticks per call\n", elapsed, elapsed/(maxLoop*float(unroll)));

  start = __rdtsc();
  for (size_t i = maxLoop; i != 0; i--)
  {
    // unroll 10 times to keep loop overhead to a minimum
    minimumSimpleAsm(); minimumSimpleAsm();
    minimumSimpleAsm(); minimumSimpleAsm();
    minimumSimpleAsm(); minimumSimpleAsm();
    minimumSimpleAsm(); minimumSimpleAsm();
    minimumSimpleAsm(); minimumSimpleAsm();
  }
  volatile int a = minimumSimple(start, elapsed);
  elapsed = __rdtsc() - start;
  printf("minSimple: %I64d ticks => about %.3f ticks per call\n", elapsed, elapsed/(maxLoop*float(unroll)));
#endif

  // analyze all numbers from 0 to 1^32 - 1
  printf("verifying ");
  unsigned int totalX = 0xFFFFFFFF;
           int x      = 0;
           int y      = 0x7FFFFFFF;
  unsigned int errors = 0;

  // exhaustive search is impossible for (2^32)^2 combinations
  do
  {
    int min = x < y ? x : y;
    int max = x > y ? x : y;

    if (minimum(x,y) != min)
    {
      printf("failed ! x=%u,y=%u has wrong minimum, expected %u but got %u.\n", x, y, min, minimum(x,y));
      errors++;
    }
    if (maximum(x,y) != max)
    {
      printf("failed ! x=%u,y=%u has wrong maximum, expected %u but got %u.\n", x, y, max, maximum(x,y));
      errors++;
    }

    // progress meter, the whole validation takes about a minute on my computer
    if ((x & 0x07FFFFFF) == 0)
      printf(".");

    x++; y--;
  } while (x != 0);

  printf(" %u errors.\n", errors);
  return errors;
}

// performance:*4
// - constant time, data independent
//
// + Intel Pentium D:
// - maximum: approx. 3 cycles per number
// - minimum: approx. 3 cycles per number
// - minimumSimple: approx. 3.5 cycles per number
//
// + Intel Core 2:
// - maximum: approx. 2.5 cycles per number
// - minimum: approx. 2.5 cycles per number
// - minimumSimple: approx. 2.5 cycles per number
//
// + Intel Core i7:
// - maximum: approx. 3 cycles per number
// - minimum: approx. 3 cycles per number
// - minimumSimple: approx. 2.25 cycles per number
