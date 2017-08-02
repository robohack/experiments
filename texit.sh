#!/bin/sh

{ echo "Hello"; false } | cat
echo $?
