/*
 * Fowler/Noll/Vo hash
 *
 * The basis of the hash algorithm was taken from an idea sent by email to the
 * IEEE Posix P1003.2 mailing list from Phong Vo (kpv@research.att.com) and
 * Glenn Fowler (gsf@research.att.com).  Landon Curt Noll (chongo@toad.com)
 * later improved on their algorithm.
 *
 * The magic is in the interesting relationship between the special prime
 * 16777619 (2^24 + 403) and 2^32 and 2^8.
 *
 * This hash produces the fewest collisions of any function that we've seen so
 * far, and works well on both numbers and strings.
 *
 * PUBLIC: u_int32_t __ham_func5 __P((DB *, const void *, u_int32_t));
 */
u_int32_t
__ham_func5(dbp, key, len)
	DB *dbp;
	const void *key;
	u_int32_t len;
{
	const u_int8_t *k, *e;
	u_int32_t h;

	if (dbp != NULL)
		COMPQUIET(dbp, NULL);

	k = key;
	e = k + len;
	for (h = 0; k < e; ++k) {
		h *= 16777619;
		h ^= *k;
	}
	return (h);
}
