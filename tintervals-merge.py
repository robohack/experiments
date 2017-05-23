# -*- mode: python; python-indent: 8; indent-tabs-mode: t; smart-tabs-mode: nil -*- 

# from:  https://stackoverflow.com/a/20062829/816536

# If using python 3, uncomment the following:
#from functools import reduce

# In all of the following, the list of intervals must be sorted and
# non-overlapping.  We also assume that the intervals are half-open,
# so that x is in tp(start, end) iff start <= x and x < end.

def flatten(list_of_tps):
	"""Convert a list of intervals to a list of endpoints"""
	return reduce(lambda ls, ival: ls + [ival.start, ival.end],
	              list_of_tps,
	              [])

def unflatten(list_of_endpoints):
	"""Convert a list of endpoints, with an optional terminating sentinel,
	   into a list of intervals"""
	return [tp(list_of_endpoints[i], list_of_endpoints[i + 1])
	        for i in range(0, len(list_of_endpoints) - 1, 2)]

def merge(a_tps, b_tps, op):
	"""Merge two lists of intervals according to the boolean function op"""
	a_endpoints = flatten(a_tps)
	b_endpoints = flatten(b_tps)

	sentinel = max(a_endpoints[-1], b_endpoints[-1]) + 1
	a_endpoints += [sentinel]
	b_endpoints += [sentinel]

	a_index = 0
	b_index = 0

	res = []

	scan = min(a_endpoints[0], b_endpoints[0])
	while scan < sentinel:
		in_a = not ((scan < a_endpoints[a_index]) ^ (a_index % 2))
		in_b = not ((scan < b_endpoints[b_index]) ^ (b_index % 2))
		in_res = op(in_a, in_b)

		if in_res ^ (len(res) % 2): res += [scan]
		if scan == a_endpoints[a_index]: a_index += 1
		if scan == b_endpoints[b_index]: b_index += 1
		scan = min(a_endpoints[a_index], b_endpoints[b_index])

	return unflatten(res)

def interval_diff(a, b):
	return merge(a, b, lambda in_a, in_b: in_a and not in_b)

def interval_union(a, b):
	return merge(a, b, lambda in_a, in_b: in_a or in_b)

def interval_intersect(a, b):
	return merge(a, b, lambda in_a, in_b: in_a and in_b)
