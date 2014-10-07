#!/bin/sh

ch=$1

# swap the chars and newlines, then count the lines

tr ${ch}\\n \\n${ch} | wc -l
