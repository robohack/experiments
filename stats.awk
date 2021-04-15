#!/usr/bin/awk -f

#
# variance = SUM[for i = 0..count of ((value[i] - mean)^2)] / (count - 1)
# stddev = sqrt(variance)
#
# From Numerical Recipies (by Press et al):
#
#	There is a long story about why the denominator is N-1 rather
#	than N.  If you have never heard that story, you may consult any
#	good statistics text...  We might also comment that if the
#	difference between N and N-1 ever matters to you, then you are
#	probably up to no good anyway -- e.g., trying to substantiate a
#	questionable hypothesis with marginal data.
#
# Apparently the /N version is called the "population stddev" and the /(N-1)
# version is called the "sample stddev".
#
# We can derive a direct formula for calculating the variance and stddev in
# one pass by keeping track of just sum of the values and the sum of the values
# squared, as well as of course the count of values:
#
#               (count * (SUM[values^2])) - (SUM[values]^2)
#    variance =	-------------------------------------------
#                           count * (count - 1)
#
#    stddev = sqrt(variance)
#

BEGIN {
	column = 1;
	sum = 0.0;
	sum_sq = 0.0;
	mean = 0.0;
	variance = 0.0;
	stddev = 0.0;
}

{
	sum += $column + 0.0;
	sum_sq += ($column + 0.0) * ($column + 0.0);
}

END {
	mean = sum / NR;
 	variance = (((NR * sum_sq) - (sum * sum)) / (NR * (NR - 1)));
	stddev = sqrt(variance);
	printf("count=%d sum=%f, sum_sq=%f\n", NR, sum, sum_sq);
	printf("mean=%f\n", mean);
	printf("variance=%f\n", variance);
	printf("#0 stddev=%f\n", stddev);
	#
	# this variation may not overflow as easily:
	#
	stddev = sqrt(((sum_sq / NR) - (mean * mean)) * NR / (NR - 1));
	printf("#1 stddev=%f\n", stddev);
	#
	# OR, someone else suggests (also gets same results):
	#
	# std_deviation = sqrt((sum_squares - (sum_values^2 / count_values)) / (count_values - 1))
	#
	stddev = sqrt((sum_sq - (sum * sum / NR)) / (NR - 1));
	printf("#2 stddev=%f\n", stddev);
	#
	# which we can also make less likely to overflow:
	#
	stddev = sqrt((sum_sq - (sum / NR * sum)) / (NR - 1));
	printf("#3 stddev=%f\n", stddev);
}
