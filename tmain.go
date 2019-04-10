//
// Package main showing how to test command-line code
//
// build:	go vet tmain.go && go build tmain.go
// test:	go test -v tmain.go tmain_test.go
//
// see also go.mod
//
package main

import (
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path/filepath"
	"time"

	// original code at https://github.com/natefinch/lumberjack
	// under the v2.0 branch, so with module support it will be:
	//"github.com/natefinch/lumberjack/v2"
	"gopkg.in/natefinch/lumberjack.v2"
)

const VERSION = "v1.1"

type AppSettings struct {
	// useful flags
	version    bool
	debug      bool
	help       bool
	testMain   bool
	makeItFail bool

	// optional parameters
	logFile string

	// required parameters
	required string
}

var Settings AppSettings

var (
	// currentTime exists so it can be mocked out by tests.
	currentTime = time.Now
	debug       *log.Logger
)

// ITE() -- perhaps the best equivalent to C's "c?a:b" ternary operator
//
// (called ITE() instead of IIf() [aka ImmediateIf] because the latter looks too
// odd in CamelCase; and IIF() is too easy to mistake as IFF: "if and only if")
//
func ITE(
	condition bool,
	a interface{},
	b interface{},
) interface{} {
	if condition {
		return a
	}
	return b
}

// override flag.Usage for 2
func usage() {
	fmt.Fprintf(os.Stderr, "Usage of %s:\n", os.Args[0])
	flag.PrintDefaults()
	if Settings.help {
		os.Exit(0)
	} else {
		os.Exit(2)
	}
}

func version() {
	fmt.Fprintf(os.Stdout, "Version: %s\n", VERSION)
	os.Exit(0)
}

func init() {
	flag.Usage = usage

	flag.BoolVar(&Settings.testMain, "test-main-exit",
		false,
		"Internal flag for testing clean exit from main()")

	flag.BoolVar(&Settings.makeItFail, "make-it-fail",
		false,
		"Internal flag for testing failure exit from main()")

	flag.BoolVar(&Settings.version, "V",
		false,
		"Print release identification")

	flag.BoolVar(&Settings.debug, "d",
		false,
		"Turn on debug output and log to stderr.")

	flag.BoolVar(&Settings.help, "h",
		false,
		"Print this help.")

	// optional parameters

	flag.StringVar(&Settings.logFile, "L",
		"",
		"Activity log file.")

	// some required parameters

	flag.StringVar(&Settings.required, "required",
		"",
		"Required Parameter.")

	// by default the debug.() logger goes to /dev/null
	//
	debug = log.New(ioutil.Discard, "", log.LstdFlags)
}

func exit(
	err error,
	ecode int,
	esc string,
) {
	if err != nil {
		log.Printf("%s: error: %v", esc, err)
		os.Exit(ecode) // allows a "fatal" exit with zero...
	}
	debug.Println("All OK!")
}

func main() {
	var err error
	var lw *lumberjack.Logger = nil

	defer func() {
		exit(nil, 0, "")
	}()

	flag.Parse()

	if Settings.version {
		version()
	}

	// basic logging setup (after flag.Parse())....
	//
	// always use microseconds for all log entries
	log.SetFlags(log.LstdFlags | log.Lmicroseconds)
	if Settings.logFile != "" {
		Settings.logFile, err = filepath.Abs(Settings.logFile)
		if err != nil {
			exit(err, 2, Settings.logFile)
		}
		// the extra parens trick gofmt into leaving the nice spaces in
		// place around the bitwise operators
		lf, err := os.OpenFile(Settings.logFile,
			(os.O_APPEND | os.O_CREATE | os.O_WRONLY), 0644)
		if err != nil {
			exit(err, 1, Settings.logFile)
		}
		lf.Close()
		if err != nil {
			exit(err, 2, Settings.logFile)
		}
		lw = &lumberjack.Logger{
			Filename: Settings.logFile,
			MaxAge:   90, // days
			MaxSize:  10, // megabytes (default 100)
		}
		log.SetOutput(lw)
		// always use UTC (and microseconds) in the log file
		log.SetFlags(log.LstdFlags | log.Lmicroseconds | log.LUTC)
	}
	if Settings.debug {
		if lw != nil {
			log.Println("Adding logging to stderr")
			// reset log.() to also write logs to stderr
			// (n.b.:  it keeps its use of UTC)
			log.SetOutput(io.MultiWriter(os.Stderr, lw))
		}
		// reset debug.() to actually write, to stderr
		// (debug output might make more sense in local time)
		debug = log.New(os.Stderr, "DEBUG:  ", log.LstdFlags|log.Lmicroseconds)
		debug.Println("Logging and debug to terminal",
			// xxx sigh, how to convince gofmt to leave spaces
			// around the '+' (concat) operator???
			ITE(Settings.logFile != "", ("and "+Settings.logFile), ""))
	}
	if Settings.testMain {
		debug.Println("Testing main() for clean exit")
		// we want this to return normally from main(), so we must jump
		// past checks for required parameters and any other sanity
		// checks, plus avoid doing anything functional
		goto end
	}

	// check required parameters

	if Settings.required == "" {
		log.Println("required parameter not given!")
		usage()
	}

	if Settings.makeItFail != true {
		debug.Println("makeItFail is NOT set!")
	} else {
		debug.Println("makeItFail is set...")
	}

	// main logic starts from a sub-function just to allow goto
	// (goto must not jump over new variable definitions)

	doit()

end:

	debug.Printf("Done!")
}

func doit() {
	// OK, on with the show!

	if Settings.makeItFail {
		log.Fatal("failing it!")
	}

	debug.Printf("doing it!")
}

//
// Local Variables:
// eval: (make-local-variable 'compile-command)
// compile-command: "go vet tmain.go && go build tmain.go"
// End:
//
