#! /usr/bin/awk -f

BEGIN {
	income = 80000.00;
	proptax = 500.00;
	heat = 200.00 * 12;		# monthly heating payments
	debt = 300.00 * 12;		# monthly CC payments
	second = 0.00 * 12;		# monthly 2nd mort payments

	RATE = 7.05 / 100;		# annual percentage rate
	amort = 25;			# years

	compound = 2.0 / 12;
	monTime = amort * 12;

	yrRate = RATE / 2;
	rdefine = ((1.0 + yrRate) ^ compound) - 1.0;

	purchcompound = (1.0 + rdefine) ^ monTime;

	maxgdsr = 32.0 / 100;
	GDSPAY = (maxgdsr * income) - proptax - heat - second;

	maxtdsr = 42.0 / 100;
	TDSPAY = (maxtdsr * income) - proptax - heat - second - debt;

	PAYMENT = (GDSPAY < TDSPAY) ? GDSPAY / 12.0 : TDSPAY / 12.0;
	MORTGAGE = (0 + ((PAYMENT * (purchcompound - 1.0)) / rdefine)) / purchcompound;

	printf("Max monthly payment:\t$ %.02f\n", PAYMENT);
	printf("Maximum mortgage:\t$ %d.00\n", MORTGAGE);

	exit;
}

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "./" (file-name-nondirectory (buffer-file-name)))
# End:
