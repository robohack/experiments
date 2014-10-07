# to replace Bash's "[[" operator
#
fnmatch () { case "$2" in $1) return 0 ;; *) return 1 ;; esac ; }
