#!/bin/sh

# n.b.:  if you are waiting for a process _and_ responding to signals,
# you won't know if the wait succeeded (i.e. the child has exited), or
# if it was the "wait" statement itself which was interrupted.

while wait $pid; test $? -gt 128; do
	kill -0 $pid 2> /dev/null || break;
done
