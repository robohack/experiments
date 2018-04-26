#
# find all files matching pattern and copy them to a new directory
#
# POSIX only:
#
find . -iname "*foobar*" -exec cp -- {} ~/foo/bar \;
