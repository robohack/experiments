# round(), from the GNU Awk manual
#
function round(x, ival, aval, fraction)
{
	# integer part, int() just truncates
	ival = int(x);
	# see if there is a fractional part
	# xxx N.B.:  this may not work if x is the result of a float calculation
	if (ival == x) {
		return ival;
	}
	if (x < 0) {
		aval = -x;
		# absolute value
		ival = int(aval);
		fraction = aval - ival;
		if (fraction >= 0.5) {
			return int(x) - 1;	# -2.5 --> -3
		} else {
			return int(x);		# -2.3 --> -2
		}
	} else {
		fraction = x - ival;
		if (fraction >= 0.5) {
			return ival + 1;
		} else {
			return ival;
		}
	}
}
