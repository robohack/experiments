#include <stdbool.h>

unsigned long millis(void);

void set_start(void);

bool bad_interval(void);
bool good_interval(void);

unsigned long bmptime;

/*
 * The Wrong Way:
 */
void bad_set_end(void);

void
bad_set_end()
{
	bmptime = millis() + 4000;
}

bool
bad_interval()
{
	if (millis() >= bmptime) {
		return true;
	}

	return false;
}


/*
 * Russel Nelson (on G+):
 *
 * What if bmptime is very very close to 2^32-1 milliseconds?  In order to ever
 * meet that test and set it back down low again, you'd have to hit just that
 * small range of milliseconds.  That's 49 days, by the way.  So it would
 * definitely work for 49 days after being rebooted, but might not work for
 * another 49 days or another or another.
 *
 * The right way to do that unsigned test is like this
 */
void good_set_start(void);

void
good_set_start()
{
	bmptime = millis();
}

bool
good_interval()
{
	if (millis() - bmptime >= 4000) {
		return true;
	}

	return false;
}

/*
 * Note:  two’s complement arithmetic ensures the delta result will be correct
 * even if the counter overflows (and thus wraps around to zero again) unless it
 * wraps around so much that it passes through the original “old value” you
 * subtract from it.  So, you just need to calculate the delta often enough to
 * do so at least twice in the time it takes the counter to overflow.  Set your
 * tick resolution and counter width appropriately.
 *
 *	extern uint32_t LastTime;
 *
 *	uint32_t temp = GetTickCount();
 *	uint32_t delta = temp – LastTime
 *	LastTime = temp;
 *
 */


/*
 * Care and feeding of unsigned int for index values
 */

typedef struct list {
	void *val;
} list_t;

unsigned int list_len(list_t *);
void *list_at_base_zero(unsigned int);	/* as in array subscripting... */

void fiddle(void *);

void udo_fwd(list_t *);
void udo_bwd(list_t *);

void
udo_fwd(list_t *l)
{
	unsigned int i;

	for (i = 0; i < list_len(l); i++) {
		void *val = list_at_base_zero(i);

		fiddle(val);
	}

	return;
}

/*
 * N.B.:  Obviously the need to compare with the modulo-2 overflow value would
 * be eliminated if we didn't want 'i' to be an index-from-zero and instead we
 * were able to write the satement so that the test reads "while the list is
 * NOT empty":
 *
 * 	for (i = list_len(l); i > 0; i--)
 */

void
udo_bwd(list_t *l)
{
	unsigned int i;

	for (i = list_len(l) - 1;
#if 0
	     i >= 0;			/* xxx obviously always true */
#else
	     i != ~0U;			/* also covers case where list_len() == 0 */
#endif
	     --i) {
		void *val = list_at_base_zero(i);

		fiddle(val);
	}

	return;
}
