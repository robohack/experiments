/*
 * The most even distribution of random numbers between 0.0 and 1.0 can be had
 * by passing a random integer
 */
double
Rand_to_Float(uintmax_t n)
{
	double value;
	const uintmax_t maxRange = 1 << DBL_MANT_DIG;

	n &= maxRange - 1;
	value = n * 1.0 / maxRange;

	return value;
}
