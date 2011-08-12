#! /usr/bin/awk 

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
		RANDOM = int(1 + rand() * d);
		if (RANDOM in lines) {
			print lines[RANDOM];
			delete lines[RANDOM];
			++e;
		}
	}
}
