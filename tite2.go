// a weird compiler error

package main

import (
	"fmt"
)

// ITE() -- perhaps the best equivalent to C's "c?a:b" ternary operator
//
func ITE(condition bool, a interface{}, b interface{}) interface{} {
	if condition {
		return a
	}
	return b
}

type comparableStruct struct {
	boo  bool
	foo  int
	bar  float32
	none complex64
	name string
}

var ITETests = []struct {
	ifTrue  interface{}
	ifFalse interface{}
}{
	{
		ifTrue:  true,
		ifFalse: false},
	{
		ifTrue:  "true",
		ifFalse: "false"},
	{
		ifTrue:  1,
		ifFalse: 0},
	{
		ifTrue:  0.1,
		ifFalse: 0.0},
	{
		ifTrue:  1.e+1,
		ifFalse: 1.e+0},
	{
		ifTrue:  1.e+1i,
		ifFalse: 1.e+0i},
	{
		ifTrue:  '⊨',  // char appears in file as 0xe2 0x8a 0xa8
		ifFalse: '⊭'}, // char appears in file as 0xe2 0x8a 0xad

        // The above two lines cause this error _SOMETIMES_.  I have one
        // *_test.go file where they do not cause any error.
	//
	// go build tite2.go
	// # command-line-arguments
	// ./tite2.go:49:3: prepwrite: bad off=0 siz=-1000000000 s="".statictmp_13
	// ./tite2.go:49:3: WriteInt: bad integer size: -1000000000
	// ./tite2.go:50:3: prepwrite: bad off=0 siz=-1000000000 s="".statictmp_14
	// ./tite2.go:50:3: WriteInt: bad integer size: -1000000000
	//
        // Go 1.11 on both macOS and NetBSD/amd64

	{
		ifTrue:  comparableStruct{true, 1, 1.0, 1.0i, "true"},
		ifFalse: comparableStruct{false, 0, 0.0, 0.0i, "false"}}}

func TestITE() {
	for _, res := range ITETests {
		var out interface{}

		fmt.Printf("Testing %q,%q\n", res.ifTrue, res.ifFalse)
		out = ITE(true, res.ifTrue, res.ifFalse)
		if out != res.ifTrue {
			fmt.Printf("expected %q, got %q\n", res.ifTrue, out)
		}
		out = ITE(false, res.ifTrue, res.ifFalse)
		if out != res.ifFalse {
			fmt.Printf("expected %q, got %q\n", res.ifFalse, out)
		}
	}
}

func main() {
	TestITE()
}
