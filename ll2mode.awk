#! /usr/bin/awk -f

BEGIN {
	owner = "";
	group = "";
	other = "";
	n["rwx"]="07";
	n["rw-"]="06";
	n["r-x"]="05";
	n["r--"]="04";
	n["-wx"]="03";
	n["-w-"]="02";
	n["--x"]="01";
	n["---"]="00";
}

{
	mode = 0;
	owner = substr($1, 2, 3);
	group = substr($1, 5, 3);
	other = substr($1, 8, 3);
	if (substr(owner, 3, 1) == "s") {
		sub(/s/, "x", owner);
		mode += 2048;			# 04000
	} else if (substr(owner, 3, 1) == "S") {
		sub(/S/, "-", owner);
		mode += 2048;			# 04000
	}
	if (substr(group, 3, 1) == "s") {
		sub(/s/, "x", group);
		mode += 1024;			# 02000
	} else if (substr(group, 3, 1) == "S") {
		sub(/S/, "-", group);
		mode += 1024;			# 02000
	}
	if (substr(other, 3, 1) == "t") {
		sub(/t/, "x", other);
		mode += 512;			# 01000
	} else if (substr(other, 3, 1) == "T") {
		sub(/T/, "-", other);
		mode += 512;			# 01000
	}
	mode += n[owner] * (2^6);
	mode += n[group] * (2^3);
	mode += n[other];
	printf("0%o\n", mode);
}
