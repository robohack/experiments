#include <assert.h>
#include <CoreServices/CoreServices.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <unistd.h>

/*
 * very fast timing on Darwin (Mach) (Mac OS X) 
 *
 * From:  http://developer.apple.com/library/mac/qa/qa1398/_index.html 
 *
 * Note: For this code to work, Mach "absolute time" must be equivalent to
 * CoreServices "AbsoluteTime".  This equivalence was previously undocumented,
 * although it is true on all shipping versions of Mac OS X and is expected to
 * remain true on all future versions.
 *
 * Also, apparently on Snow Leopard on x86_64) the absolute time values are
 * actually in nanoseconds and so don't actually require any conversion at all.
 *
 * It is said that mach_absolute_time() is very fast -- does it use the
 * commpage?
 */

uint64_t GetPIDTimeInNanoseconds(void);

uint64_t
GetPIDTimeInNanoseconds()
{
	uint64_t        start;
	uint64_t        end;
	uint64_t        elapsed;
	uint64_t        elapsedNano;
	static mach_timebase_info_data_t    sTimebaseInfo;

	/* Start the clock. */
	start = mach_absolute_time();

	/*
	 * Call getpid().  This will produce inaccurate results because we're
	 * only making a single system call.  For more accurate results you
	 * should call getpid multiple times and average the results.
	 */
	(void) getpid();

	/* Stop the clock. */
	end = mach_absolute_time();

	/* Calculate the duration. */
	elapsed = end - start;

	/* Convert to nanoseconds. */

	/*
	 * If this is the first time we've run, get the timebase.  We can use
	 * denom == 0 to indicate that sTimebaseInfo is uninitialised because
	 * it makes no sense to have a zero denominator is a fraction.
	 */
	if (sTimebaseInfo.denom == 0) {
		(void) mach_timebase_info(&sTimebaseInfo);
	}

	/*
	 * Do the maths to convert from AbsoluteTime to nanoseconds.
	 *
	 * We hope that the multiplication doesn't overflow; the price you pay
	 * for working with integers!
	 */
	elapsedNano = elapsed * sTimebaseInfo.numer / sTimebaseInfo.denom;

	return elapsedNano;
}


int	main(void);

int
main()
{
	printf("getpid() = %llu ns\n", (unsigned long long int) GetPIDTimeInNanoseconds());

	exit(0);
}
