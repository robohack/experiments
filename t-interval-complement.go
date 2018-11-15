// -*- mode: go -*-

// see also the associated "*_test.go" file

// build with "go build t-interval-complement.go"
// test with "go test -v -bench . t-interval-complement.go t-interval-complement_test.go"

package minfys

import (
	"sort"
)

type Interval struct {
	Start int
	End   int
}

type Intervals []Interval

func (ivs Intervals) Len() int           { return len(ivs) }
func (ivs Intervals) Swap(i, j int)      { ivs[i], ivs[j] = ivs[j], ivs[i] }
func (ivs Intervals) Less(i, j int) bool { return ivs[i].Start < ivs[j].Start }

func (i *Interval) OverlapsOrAdjacent(j Interval) bool {
	return i.End+1 >= j.Start && j.End+1 >= i.Start
}

func (i *Interval) Merge(j Interval) bool {
	if !i.OverlapsOrAdjacent(j) {
		return false
	}
	if j.Start < i.Start {
		i.Start = j.Start
	}
	if j.End > i.End {
		i.End = j.End
	}
	return true
}

func (ivs Intervals) Merge(iv Interval) Intervals {
	ivs = append(ivs, iv)
	merged := make(Intervals, 0, len(ivs))
	for _, iv := range ivs {
		for i := 0; i < len(merged); {
			if iv.Merge(merged[i]) {
				merged = append(merged[:i], merged[i+1:]...)
			} else {
				i++
			}
		}
		merged = append(merged, iv)
	}
	return merged
}

func (ivs Intervals) MergeUsingSort(iv Interval) Intervals {
	ivs = append(ivs, iv)
	sort.Sort(ivs)
	merged := make(Intervals, 0, len(ivs))
	merged = append(merged, ivs[0])
	for i := 1; i < len(ivs); i++ {
		last := len(merged) - 1
		if !merged[last].Merge(ivs[i]) {
			merged = append(merged, ivs[i])
		}
	}
	return merged
}

func (a Intervals) Difference(b Interval) Intervals {
	// If A and B are sets, then the relative complement of A in B
	// is the set of elements in B but not in A.
	// The relative complement of A in B is denoted B ∖  A:
	//     B \ A = {x ∈ A | x ∉ B}
	//     B \ A = B ∩ A'
	//
	// For example. d = a\b,
	//     a: [{10, 15}, {30, 35}, {20, 25}]
	//     b: {5,32}
	//     d: [{5,9}, {16,19}, {26,29}]
	// The elements of set a are non-overlapping, non-adjacent,
	// and unsorted intervals.

	if len(a) <= 0 {
		return Intervals{b}
	}

	var d Intervals
	for ; len(a) > 0; a = a[1:] {
		for i := 1; i < len(a); i++ {
			if a[i].Start < a[0].Start {
				a[i], a[0] = a[0], a[i]
			}
		}

		if b.Start < a[0].Start {
			if b.End < a[0].Start {
				d = append(d, b)
				break
			}
			d = append(d, Interval{b.Start, a[0].Start - 1})
			b.Start = a[0].Start
		}
		if b.End <= a[0].End {
			break
		}
		if b.Start <= a[0].End {
			b.Start = a[0].End + 1
		}
		if len(a) == 1 {
			if b.Start <= a[0].End {
				b.Start = a[0].End + 1
			}
			d = append(d, b)
			break
		}
	}
	return d
}
