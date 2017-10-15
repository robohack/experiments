// -*- mode: go -*-

package minfys

import (
	"fmt"
	"testing"
)

// Example
var (
	xA = Intervals{{10, 15}, {30, 35}, {20, 25}}
	xB = Interval{5, 32}
	xD = Intervals{{5, 9}, {16, 19}, {26, 29}}
	xR = Intervals{}
)

func BenchmarkExample(b *testing.B) {
	b.ReportAllocs()
	a := make(Intervals, len(xA))
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		copy(a, xA)
		xR = a.Difference(xB)
	}
	b.StopTimer()
	if fmt.Sprint(xD) != fmt.Sprint(xR) {
		b.Fatal(xD, xR)
	}
}
