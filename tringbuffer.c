/*
 * Virtual Stream form of FIFO:
 *
 * #define SIZE		2^n
 * struct FIFO {
 * 	value_t Elem[SIZE];
 *	unsigned int ReadPos;
 *	unsigned int WritePos;
 * };
 *
 * writing elements is Elem[WritePos % SIZE] = x; and updating the index is
 * WritePos++; (and analogous for ReadPos). In other words, you delay the
 * reduction modulo SIZE. For this to be efficient, you normally want to pick a
 * power of 2 for SIZE; this makes the wrapping computation cheap and will
 * automatically do the right thing if one of the positions ever
 * overflows. This leads to very straightforward, efficient code. The number of
 * items in the queue is WritePos - ReadPos; no case distinction, unsigned
 * arithmetic does the right thing. No trouble with the last element either (if
 * the queue is full, then "WritePos == ReadPos + SIZE" – no problem!).
 */
