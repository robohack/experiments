//
//	some exmple tests for a "package main"
//
// test: go test -v tmain.go tmain_test.go
//
package main

import (
	"testing"

	"os"
	"os/exec"
	"syscall"
)

// ToDo:  Consider use of "github.com/rendon/testcli"

// This is the first test function that's called, and it is also called
// recursively by the other TestMain*() functions below
//
// n.b.  this is not TestMain() -- that is special in pkg "testing"
//
func TestMainProgram(t *testing.T) {
	// because main() changes places...
	orig_cwd, _ := os.Getwd()
	defer func() {
		os.Chdir(orig_cwd)
	}()
	oldArgs := os.Args
	defer func() { os.Args = oldArgs }()
	testType := os.Getenv("TEST")
	// testType must match the names of the TestMain* functions belowa
	switch testType {
	case "TestMainHelp":
		os.Args = []string{testType, "-h"}
	case "TestMainUsage":
		os.Args = []string{testType}
	case "TestMainVersion":
		os.Args = []string{testType, "-V"}
	case "TestMainMakeItFail":
		os.Args = []string{testType, "--required=zanzibar", "--make-it-fail", "-d"}
	case "TestMainLog":
		os.Args = []string{testType, "--test-main-exit", "-d", "-L", testType + ".log"}
	default:
		// when this test function itself runs, do nothing but log...
		os.Args = []string{"TestMainProgram", "-test-main-exit", "-d"}
	}
	// just call main to do the work
	main()
}

func myTestMainProgramSetup(t *testing.T) *exec.Cmd {
	// only run TestMainProgram() this time (with all other args)
	cmd := exec.Command(os.Args[0], "-test.run=TestMainProgram")
	// communicate desired sub-test to TestMainProgram()
	cmd.Env = append(os.Environ(), "TEST="+t.Name())

	return cmd
}

func myTestCleanup() {
	// XXX ToDo cleanup!

	// e.g. could check the log entry in "testMain.log"
}

// recursive calls to TestMainProgram with environment variable to select the
// desired test mode

func TestMainUsage(t *testing.T) {
	cmd := myTestMainProgramSetup(t)
	//
	// uncomment to see the help output in the test run (but note
	// it will now include test args too!)
	//
	//cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
	err := cmd.Run()
	e, ok := err.(*exec.ExitError)
	if err == nil {
		t.Fatalf("%s: exited OK, wanted non-zero exit status", t.Name())
	}
	if ok && e.Success() != true && e.Sys().(syscall.WaitStatus).ExitStatus() == 2 {
		t.Logf("%s: %v", t.Name(), err)
		myTestCleanup()
		return
	}
	t.Fatalf("%s: %v, wanted exit status %d", t.Name(), err, 2)
}

func TestMainVersion(t *testing.T) {
	cmd := myTestMainProgramSetup(t)
	cmd.Stdout, cmd.Stderr = os.Stdout, os.Stderr
	err := cmd.Run()
	if err == nil {
		return
	}
	t.Fatalf("%s: %v, wanted exit status %d", t.Name(), err, 0)
}

func TestMainHelp(t *testing.T) {
	cmd := myTestMainProgramSetup(t)
	err := cmd.Run()
	if err == nil {
		return
	}
	t.Fatalf("%s: %v, wanted exit status %d", t.Name(), err, 0)
}

func TestMainMakeItFail(t *testing.T) {
	cmd := myTestMainProgramSetup(t)
	cmd.Stdout, cmd.Stderr, cmd.Stdin = os.Stdout, os.Stderr, os.Stdin
	err := cmd.Run()
	e, ok := err.(*exec.ExitError)
	if err == nil {
		t.Fatalf("%s: exited OK, wanted non-zero exit status", t.Name())
	}
	if ok && e.Success() != true && e.Sys().(syscall.WaitStatus).ExitStatus() == 1 {
		t.Logf("%s: %v", t.Name(), err)
		myTestCleanup()
		return
	}
	t.Fatalf("%s: %v, wanted exit status %d", t.Name(), err, 1)
}

func TestMainLog(t *testing.T) {
	cmd := myTestMainProgramSetup(t)
	cmd.Stdout, cmd.Stderr, cmd.Stdin = os.Stdout, os.Stderr, os.Stdin
	err := os.Remove(t.Name() + ".log")
	if err != nil && os.IsNotExist(err) != true {
		t.Fatalf("%s: %v", t.Name(), err)
	}
	file, err := os.Open(t.Name() + ".log")
	if file != nil || err == nil {
		t.Fatalf("%s: %s still exists!", t.Name(), t.Name()+".log")
	}
	if os.IsNotExist(err) != true {
		t.Fatalf("%s: %v", t.Name(), err)
	}
	file.Close()
	err = cmd.Run()
	if err == nil {
		return
	}
	t.Fatalf("%s: %v, wanted exit status %d", t.Name(), err, 0)
	file, err = os.Open(t.Name() + ".log")
	if file == nil || err != nil {
		t.Fatalf("%s: %v", t.Name(), err)
	}
	file.Close()
}

// more tests, for internal functions...

// this is a rather silly minimal test -- it does not really go very far to
// properly exercise and validate the "interface" feature of ITE()
//
type comparableStruct struct {
	boo    bool
	foo    int
	bar    float32
	none   complex64
	name   string
	symbol rune
}

var ITETests = []struct {
	ifTrue  interface{}
	ifFalse interface{}
}{
	{
		ifTrue:  true,
		ifFalse: false,
	},
	{
		ifTrue:  "true",
		ifFalse: "false",
	},
	{
		ifTrue:  1,
		ifFalse: 0,
	},
	{
		ifTrue:  0.1,
		ifFalse: 0.0,
	},
	{
		ifTrue:  1.e+1,
		ifFalse: 1.e+0,
	},
	{
		ifTrue:  1.e+1i,
		ifFalse: 1.e+0i,
	},
	{
		// XXX these should be rune literals ('⊨') as in the
		// comparableStruct example below, but there is a compiler bug
		// before 1.12 which prevents them from being used in an
		// interface initializer like this
		ifTrue:  "⊨",
		ifFalse: "⊭",
	},
	{
		ifTrue:  comparableStruct{true, 1, 1.0, 1.0i, "true", '⊨'},
		ifFalse: comparableStruct{false, 0, 0.0, 0.0i, "false", '⊭'},
	},
}

func TestITE(t *testing.T) {
	for _, res := range ITETests {
		var out interface{}

		out = ITE(true, res.ifTrue, res.ifFalse)
		if out != res.ifTrue {
			t.Errorf("expected %q, got %q", res.ifTrue, out)
		}
		out = ITE(false, res.ifTrue, res.ifFalse)
		if out != res.ifFalse {
			t.Errorf("expected %q, got %q", res.ifFalse, out)
		}
	}
}

//
// Local Variables:
// eval: (make-local-variable 'compile-command)
// compile-command: "go test -v tmain.go tmain_test.go"
// End:
//
