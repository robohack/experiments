main()
{
	float f, g;
	unsigned short u, v;
	short s, t;
	u = 250;
	s = 250;
	t = 100;
	v = 100;
	f = t - s;
	g = v - u;
	printf("f = %f, g = %f\n", f, g);
	/*
	 * SysVr2 PCC on VAX and 3B20 printed:
	 *
	 *	f = -150.000000, g = 65386.000000
	 *
	 * instead of:
	 *
	 *	f = -150.000000, g = -150.000000
	 *
	 * Chris Torek:
	 *
	 * Actually, the answer depends on two (or three) things:  If you
	 * implement classic C, the answer is some positive number.  The type
	 * of (u_short-u_short) is the type of
	 * (expand(u_short)-expand(u_short)) which is (u_int-u_int) which is
	 * u_int.  65368 is a typical value.  If you implement pANS C, the
	 * answer depends on one thing:  The type of (u_short-u_short) is
	 * either u_int or int.  If sizeof(int)>sizeof(short), expand(u_short)
	 * is int, otherwise it is u_int.  I consider this `feature' one of the
	 * major botches in the pANS (although something like it is necessary
	 * to get unsigned chars to produce int values without casts).  Now, on
	 * a 3B, sizeof(int) > sizeof(short), so the answer depends on whether
	 * your compiler implements classic C (65386.0) or pANS C (-150.0).
	 * But on a PDP-11, sizeof(int)==sizeof(short), so the answer is
	 * 65368.0 in both classic C and pANS C.
	 *
	 * -- 
	 * In-Real-Life: Chris Torek, Univ of MD Comp Sci Dept (+1 301 454 7163)
	 *
	 * Oops: since sizeof(u_int)==4 and the range is 0..4294967295, a
	 * `classic C' compiler should produce 4294967146.0, not 65368.0.
	 * Thanks to Chris Aoki at Sun for pointing this out, and to Donn
	 * Seeley for further noting that some bits will then `rub off' if this
	 * is stored in a 32-bit float (so that you might get, say,
	 * 4294967040.0).  (Donn also noticed a few typos: 65386 for 65368.)
	 *
	 * (In general, conversion from unsigned to float should be done using
	 * machine instructions of the form:
	 *
	 *         convert signed to float
	 *         if result < 0, add pow(2.0,32.0)
	 * e.g., on a vax,
	 *                 cvtlf   r1,r0
	 *                 jgeq    0f
	 *                 addf2   $0f4294967296.0,r0
	 *         0:      ...
	 * )
	 *
	 * -- 
	 * In-Real-Life: Chris Torek, Univ of MD Comp Sci Dept (+1 301 454 7163)
	 */
}
