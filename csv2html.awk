#! /usr/bin/awk -f

# awk script to create HTML tables from CSV files

BEGIN {
	FS = ",";
	CAPTION = "Some Table caption";
	print "<table border cellspacing=0 cellpadding=5>";
	print "<caption>" CAPTION "</caption>";
}
{
	x = 1;
	print "<tr>";
	while (x <= NF) {
		if (NR == 1) {
			print "\t <th>" $x "</th>";
		} else {
			print  "\t <td>" $x "</td>";
		}
		++x;
	}
	print "</tr>";
}
END {
	print "</table>";
}
