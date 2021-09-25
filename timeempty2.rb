#!/usr/pkg/bin/ruby

trials = 0
for trial in 1...10000000
	trials += 1
end

print "ruby ", trials, " ", trial, "\n"
