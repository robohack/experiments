#!/bin/sh

indent_print()
{
	local i=0
	while [ $i -lt $1 ]; do
		printf "\t"
		i=$((i + 1))
	done
	echo "$2"
}

walk_tree()
{
	local oldifs bn lev pr pmat
	if [ $# -lt 3 ]; then
		if [ $# -lt 2 ]; then
			pmat=".*"
		else
			pmat="$2"
		fi
		walk_tree "$1" "$pmat" 0
		return
	fi
	lev=$3
	[ -d "$1" ] || return
	oldifs=$IFS
	IFS="
"
	for el in $1/*; do
		bn=$(basename "$el")
		if [ -d "$el" ]; then
			indent_print $lev "$bn/"
			pr=$( walk_tree "$el" "$2" $((lev + 1)) )
			echo "$pr"
		else
			case $bn in
			$2)
				indent_print $lev "$bn"
				;;
			esac
		fi
	done
	IFS=$oldifs
}

# example:  find all the *.sh files
#
walk_tree "$1" "*.sh"
