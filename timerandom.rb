#! /usr/pkg/bin/ruby

trials = 0
while trials < 10000000
	rand(1)
	trials += 1
end

print "ruby ", trials, "\n"
