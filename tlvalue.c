gcc_cast_as_lvalue_p()
{
	void *ptr = 0;

	/* the result of a cast expression is not an lvalue! */
#if defined(__GCC__) && (__GNUC__ <= 3)
	(unsigned char *) ptr += 1;
#else
	ptr = (unsigned char *) ptr + 1;
#endif
}
