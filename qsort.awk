#! /usr/bin/awk -f

# NOTE:  array 'A' must be indexed by ordinal integer numbers, starting at
# left, ending at right (warning:  take care with indexing!)

function qsort(A, left, right,
	       i, last)				# internal operands
{
	if (left >= right) {			# do nothing if array contains
		return;				# less than two elements
	}
	swap(A, left, left + int(rand() * (right - left + 1)));
	last = left;				# A[left] is now partition element
	for (i = left + 1; i <= right; i++) {
		if (A[i] < A[left]) {		# using "<" as a compare "function"
			swap(A, ++last, i);
		}
	}
	swap(A, left, last);
	# sort left half...
	qsort(A, left, last - 1);
	# sort right half...
	qsort(A, last + 1, right);
}

function swap(A, i, j,
	      t)				# internal operand
{
	t = A[i];
	A[i] = A[j];
	A[j] = t;
}

# in Gawk 3.2 and newer length(A) does this too, but not nawk
#
function alen(A,			# array name
	      i, n)			# local variables
{
	n = 0;
	for (i in A) {
		n++;
	}
	return n;
}

BEGIN {
	if (DEBUG == 0 && DEBUG == "") {
		NDEBUG = 1;
	}
#	inplist[0] = "bbb";	# 22;
#	inplist[1] = "ddd"; 	# 44;
#	inplist[2] = "aaa";	# 11;
#	inplist[3] = "eee";	# 55;
#	inplist[4] = "ccc";	# 33;
#	NR = 5;
}

{
	# slurp.... (note:  NR starts at 1)
	inplist[NR-1] = $0;
}

END {
	if (NDEBUG == 0) {
		printf("read %d lines into array of %d items\n", NR, alen(inplist));
		for (i = 0; i < NR; i++) {
			printf("intput item # %d has value = %s\n", i, inplist[i]);
		}
	}
 	qsort(inplist, 0, NR - 1)

	printf("\n");
	for (i = 0; i < NR; i++) {
		if (NDEBUG == 0) {
			printf("result item # %d has value = %s\n", i, inplist[i]);
		} else {
			printf("%s\n", inplist[i]);
		}
	}

	delete inplist;
}
