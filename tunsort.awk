#!/usr/bin/awk -f

BEGIN {
	srand();
	d = 0;
	e = 0;
	lines[0] = "";
}
{
	lines[++d]=$0;
}
END {
	while (1) {
		if (e == d) {
			break;
		}
		rnum = int(1 + rand() * d);
		if (rnum in lines) {
			print lines[rnum];
			delete lines[rnum];
			++e;
		}
	}
}
