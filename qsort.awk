#! /usr/bin/awk -f

# NOTE:  array 'A' must be indexed by ordinal integer numbers, starting at 1

function qsort(A, left, right,
	       i, last)				# internal operands
{
	if (left >= right)			# do nothing if array contains
		return;				# less than two elements
	swap(A, left, left + int((right - left + 1) * rand()));
	last = left;				# A[left] is now partition element
	for (i = left+1; i <= right; i++) {
		if (A[i] < A[left])
			swap(A, ++last, i);
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

BEGIN {
#	inplist[1] = "bbb";	# 22;
#	inplist[2] = "ddd"; 	# 44;
#	inplist[3] = "aaa";	# 11;
#	inplist[4] = "eee";	# 55;
#	inplist[5] = "ccc";	# 33;
#	NR = 5;
}

{
	# slurp....
	inplist[NR] = $0;
}

END {
	for (i = 1; i <= NR; i++) {
		printf("intpu item # %d has value = %s\n", i, inplist[i]);
	}

 	qsort(inplist, 1, NR)

	printf("\n");
	for (i = 1; i <= NR; i++) {
		printf("result item # %d has value = %s\n", i, inplist[i]);
	}


	delete inplist;
}
