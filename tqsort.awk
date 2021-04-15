#!/usr/bin/awk -f
#
# the following qsort implementation extracted from:
#
#	http://www.armory.com/~ftp/lib/awk/qsort
#
# @(#) qsort 1.2.1 2005-10-21
# 1990 john h. dubois iii (john@armory.com)
#
# qsortArbIndByValue():  Sort an array (with an arbitrary index) according to
#                        the values of its elements.
#
# Input variables:
#
# Arr[] is an array of values with arbitrary (associative) indices.
#
# Output variables:
#
# k[] is returned with numeric indices 1..n.  The values assigned to these
# indices are the indices of Arr[], ordered so that if Arr[] is stepped
# through in the order Arr[k[1]] .. Arr[k[n]], it will be stepped through in
# order of the values of its elements.
#
# Return value: The number of elements in the arrays (n).
#
# NOTES:
#
# Initialize 'k' before calling:  outlist[1] = 0;
#
# Full example for accessing results:
#
#	foolist["second"] = 2;
#	foolist["zero"] = 0;
#	foolist["third"] = 3;
#	foolist["first"] = 1;
#
#	outlist[1] = 0;
# 	n = qsortArbIndByValue(foolist, outlist)
#
#	for (i = 1; i <= n; i++) {
#		printf("item at %s has value %d\n" outlist[i], foolist[outlist[i]]);
#	}
#
## Delete 'k' after you're done using it:
#
# 	delete outlist;
#
function qsortArbIndByValue(Arr, k,
			    ArrInd, ElNum)
{
	ElNum = 0;
	for (ArrInd in Arr) {
		k[++ElNum] = ArrInd;
	}
	qsortSegment(Arr, k, 1, ElNum);
	return ElNum;
}
#
# qsortSegment(): Sort a segment of an array.
#
# Input variables:
#
# Arr[] contains data with arbitrary indices.
#
# k[] has indices 1..nelem, with the indices of Arr[] as values.
#
# Output variables:
#
# k[] is modified by this function.  The elements of Arr[] that are pointed to
# by k[start..end] are sorted, with the values of elements of k[] swapped
# so that when this function returns, Arr[k[start..end]] will be in order.
#
# Return value: None.
#
function qsortSegment(Arr, k, start, end,
		      left, right, sepval, tmp, tmpe, tmps)
{
	if ((end - start) < 1) {	# 0 or 1 elements
		return;
	}
	# handle two-element case explicitly for a tiny speedup
	if ((end - start) == 1) {
		if (Arr[tmps = k[start]] > Arr[tmpe = k[end]]) {
			k[start] = tmpe;
			k[end] = tmps;
		}
		return;
	}
	# Make sure comparisons act on these as numbers
	left = start + 0;
	right = end + 0;
	sepval = Arr[k[int((left + right) / 2)]];
	# Make every element <= sepval be to the left of every element > sepval
	while (left < right) {
		while (Arr[k[left]] < sepval) {
			left++;
		}
		while (Arr[k[right]] > sepval) {
			right--;
		}
		if (left < right) {
			tmp = k[left];
			k[left++] = k[right];
			k[right--] = tmp;
		}
	}
	if (left == right)
		if (Arr[k[left]] < sepval) {
			left++;
		} else {
			right--;
		}
	if (start < right) {
		qsortSegment(Arr, k, start, right);
	}
	if (left < end) {
		qsortSegment(Arr, k, left, end);
	}
}

BEGIN {
	inplist["second"] = 22;
	inplist["fourth"] = 44;
	inplist["zero"] = 0;
	inplist["third"] = 33;
	inplist["first"] = 11;

	# note how "for (idx in list)" order is unrelated to initialization
	# order, or any seemingly obvious relationship between index values, or
	# even the array values
	i = 1;
	for (idx in inplist) {
		printf("inplist item # %d has index name: %s value = %d)\n", i, idx, inplist[idx]);
		i++;
	}

	outlist[1] = 0;
 	n = qsortArbIndByValue(inplist, outlist)

	for (i = 1; i <= n; i++) {
		printf("outlist at %d has index name: %s (inplist[outlist[i]] = %d)\n", i, outlist[i], inplist[outlist[i]]);
	}

	delete foolist;
	delete outlist;
}
