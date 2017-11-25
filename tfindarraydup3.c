bool
has_duplicates(unsigned int *arr,
	       unsigned int m,
	       unsigned int n)
{
	bool has_dups = false;
	unsigned int i;
	unsigned int *p;

	/*
	 * O(m) in time, O(1) in space (for 'typeof(m) == typeof(*a) == int')
	 *
	 *  Whether a[] array has duplicates.
	 *
	 *  precondition: all values are in [n, n+m] range.
	 *
	 *  feature: It marks visited items using a sign bit.
	 */

	assert((INT_MIN - (INT_MIN - 1)) == 1);		/* check n == INT_MIN */

	for (p = a; p != &a[m]; ++p) {
		*p -= (n - 1);				/* [n, n+m] -> [1, m+1] */
		assert(*p > 0);
	}

	/* determine: are there duplicates? */
	for (i = 0; i < m; ++i) {
		unsigned int j = abs(a[i]) - 1;

		assert(j >= 0);
		assert(j < m);

		if (a[j] > 0) {
			a[j] *= -1;			/* mark */
		} else {
			/* have already seen it! */
			has_dups = true;
			break;
		}
	}

	/* restore the array */
	for (p = a; p != &a[m]; ++p) {
		if (*p < 0) {
			*p *= -1;			/* unmark */
		}
		/*
		 * [1, m+1] -> [n, n+m]
		 */
		*p += (n - 1);
	}

	return has_dups;
}

/*
 * Local Variables:
 * eval: (make-local-variable 'compile-command)
 * compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn "; " (default-value 'compile-command) " " fn " && ./" fn))
 * End:
 */
