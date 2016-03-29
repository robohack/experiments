float
Rand_to_Float(unsigned n)
{
	const unsigned maxRange = 1 << 24; n &= maxRange - 1; float value = n * 1.0 / maxRange; return value; } 
