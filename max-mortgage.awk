#!/usr/bin/awk -f

BEGIN {
	if (income == 0 && income == "") {
		income = 80000.00;
	}
	if (proptax == 0 && proptax == "") {
		proptax = 4000.00;
	}

	if (heat == 0 && heat == "") {
		heat = 175.00;
	}
	heat *= 12;		# monthly heating payments

	if (debt == 0 && debt == "") {
		debt = 600.00;
	}
	debt *= 12;		# monthly CC payments (or savings)

	if (second == 0 && second == "") {
		second = 0.00;
	}
	second *= 12;		# monthly 2nd mort payments

	if (apr == 0 && apr == "") {
		apr = 3.14;
	}
	loanRate = apr / 100;		# annual percentage rate
	if (amort == 0 && amort == "") {
		amort = 30;		# years
	}
	if (pctdown == 0 && pctdown == "") {
		pctdown = 20;
	}

	compound = 2.0 / 12;
	monTime = amort * 12;

	yrRate = loanRate / 2;
	rdefine = ((1.0 + yrRate) ^ compound) - 1.0;

	purchcompound = (1.0 + rdefine) ^ monTime;

	# gross debt service rate of 32%
	maxgdsr = 32.0 / 100;
	GDSPAY = (maxgdsr * income) - proptax - heat - second;

	# total debt service rate of 42%
	maxtdsr = 42.0 / 100;
	TDSPAY = (maxtdsr * income) - proptax - heat - second - debt;

	PAYMENT = (GDSPAY < TDSPAY) ? GDSPAY / 12.0 : TDSPAY / 12.0;
	MORTGAGE = (0 + ((PAYMENT * (purchcompound - 1.0)) / rdefine)) / purchcompound;

	printf("Max monthly payment:\t$ %10.02f / month\n", PAYMENT);
	printf("\n");
	printf("Maximum mortgage:\t$ %7d.00\n", MORTGAGE);
	printf("plus %d%% downpayment:\t$ %7d.00\n", pctdown, (MORTGAGE * pctdown / 100.0));
	printf("                 \t  %10s\n", "==========");
	printf("Maximum purchase:\t$ %7d.00\n", MORTGAGE + (MORTGAGE * pctdown / 100.0));

	exit;
}

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "./" (file-name-nondirectory (buffer-file-name)) " -v apr=3.03")
# End:
