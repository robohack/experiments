# 1 "tgcc-string-concat.c"
# 1 "/home/building/woods/src//"
# 1 "<built-in>"
# 1 "<command line>"
# 1 "tgcc-string-concat.c"
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */




/*
 * Macro to test if we're using a GNU C compiler of a specific vintage
 * or later, for e.g. features that appeared in a particular version
 * of GNU C.  Usage:
 *
 *	#if __GNUC_PREREQ__(major, minor)
 *	...cool feature...
 *	#else
 *	...delete feature...
 *	#endif
 */
# 59 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.7 2008/10/26 06:57:30 mrg Exp $	*/




/* No arch-specific cdefs. */
# 60 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 2 3 4

# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs_elf.h" 1 3 4
/*	$NetBSD: cdefs_elf.h,v 1.30 2008/07/21 15:22:19 lukem Exp $	*/

/*
 * Copyright (c) 1995, 1996 Carnegie-Mellon University.
 * All rights reserved.
 *
 * Author: Chris G. Demetriou
 *
 * Permission to use, copy, modify and distribute this software and
 * its documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 *
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND
 * FOR ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 *
 * Carnegie Mellon requests users of this software to return to
 *
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 *
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
# 63 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs_elf.h" 3 4
/* Do not use __weak_extern, use __weak_reference instead */
# 62 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 2 3 4
# 76 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 3 4
/*
 * The __CONCAT macro is used to concatenate parts of symbol names, e.g.
 * with "#define OLD(foo) __CONCAT(old,foo)", OLD(foo) produces oldfoo.
 * The __CONCAT macro is a bit tricky -- make sure you don't put spaces
 * in between its arguments.  __CONCAT can also concatenate double-quoted
 * strings produced by the __STRING macro, but this only works with ANSI C.
 */
# 129 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 3 4
/*
 * Used for internal auditing of the NetBSD source tree.
 */






/*
 * The following macro is used to remove const cast-away warnings
 * from gcc -Wcast-qual; it should be used with caution because it
 * can hide valid errors; in particular most valid uses are in
 * situations where the API requires it, not to cast away string
 * constants. We don't use *intptr_t on purpose here and we are
 * explicit about unsigned long so that we don't have additional
 * dependencies.
 */


/*
 * The following macro is used to remove the volatile cast-away warnings
 * from gcc -Wcast-qual; as above it should be used with caution
 * because it can hide valid errors or warnings.  Valid uses include
 * making it possible to pass a volatile pointer to memset().
 * For the same reasons as above, we use unsigned long and not intptr_t.
 */


/*
 * GCC2 provides __extension__ to suppress warnings for various GNU C
 * language extensions under "-ansi -pedantic".
 */




/*
 * GCC1 and some versions of GCC2 declare dead (non-returning) and
 * pure (no side effects) functions using "volatile" and "const";
 * unfortunately, these then cause warnings under "-ansi -pedantic".
 * GCC2 uses a new, peculiar __attribute__((attrs)) style.  All of
 * these work for GNU C++ (modulo a slight glitch in the C++ grammar
 * in the distribution version of 2.5.5).
 */
# 230 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 3 4
/*
 * C99 defines the restrict type qualifier keyword, which was made available
 * in GCC 2.92.
 */
# 242 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 3 4
/*
 * C99 defines __func__ predefined identifier, which was made available
 * in GCC 2.95.
 */
# 277 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 3 4
/*
 * A barrier to stop the optimizer from moving code or assume live
 * register values. This is gcc specific, the version is more or less
 * arbitrary, might work with older compilers.
 */






/*
 * GNU C version 2.96 adds explicit branch prediction so that
 * the CPU back-end can hint the processor and also so that
 * code blocks can be reordered such that the predicted path
 * sees a more linear flow, thus improving cache behavior, etc.
 *
 * The following two macros provide us with a way to use this
 * compiler feature.  Use __predict_true() if you expect the expression
 * to evaluate to true, and __predict_false() if you expect the
 * expression to evaluate to false.
 *
 * A few notes about usage:
 *
 *	* Generally, __predict_false() error condition checks (unless
 *	  you have some _strong_ reason to do otherwise, in which case
 *	  document it), and/or __predict_true() `no-error' condition
 *	  checks, assuming you want to optimize for the no-error case.
 *
 *	* Other than that, if you don't know the likelihood of a test
 *	  succeeding from empirical or other `hard' evidence, don't
 *	  make predictions.
 *
 *	* These are meant to be used in places that are run `a lot'.
 *	  It is wasteful to make predictions in code that is run
 *	  seldomly (e.g. at subsystem initialization time) as the
 *	  basic block reordering that this affects can often generate
 *	  larger code.
 */
# 324 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 3 4
/*
 * Macros for manipulating "link sets".  Link sets are arrays of pointers
 * to objects, which are gathered up by the linker.
 *
 * Object format-specific code has provided us with the following macros:
 *
 *	__link_set_add_text(set, sym)
 *		Add a reference to the .text symbol `sym' to `set'.
 *
 *	__link_set_add_rodata(set, sym)
 *		Add a reference to the .rodata symbol `sym' to `set'.
 *
 *	__link_set_add_data(set, sym)
 *		Add a reference to the .data symbol `sym' to `set'.
 *
 *	__link_set_add_bss(set, sym)
 *		Add a reference to the .bss symbol `sym' to `set'.
 *
 *	__link_set_decl(set, ptype)
 *		Provide an extern declaration of the set `set', which
 *		contains an array of the pointer type `ptype'.  This
 *		macro must be used by any code which wishes to reference
 *		the elements of a link set.
 *
 *	__link_set_start(set)
 *		This points to the first slot in the link set.
 *
 *	__link_set_end(set)
 *		This points to the (non-existent) slot after the last
 *		entry in the link set.
 *
 *	__link_set_count(set)
 *		Count the number of entries in link set `set'.
 *
 * In addition, we provide the following macros for accessing link sets:
 *
 *	__link_set_foreach(pvar, set)
 *		Iterate over the link set `set'.  Because a link set is
 *		an array of pointers, pvar must be declared as "type **pvar",
 *		and the actual entry accessed as "*pvar".
 *
 *	__link_set_entry(set, idx)
 *		Access the link set entry at index `idx' from set `set'.
 */





/*
 * Return the number of elements in a statically-allocated array,
 * __x.
 */


/* __BIT(n): nth bit, where __BIT(0) == 0x1. */



/* __BITS(m, n): bits m through n, m < n. */



/* find least significant bit that is set */
# 2 "tgcc-string-concat.c" 2





__asm(".pushsection " ".copyright" "\n" ".asciz \"" "@(#) Copyright (c) 1991, 1993\n" "	The Regents of the University of California.  All rights reserved.\n" "\"\n" ".popsection");



# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/stdlib.h" 1 3 4
/*	$NetBSD: stdlib.h,v 1.85 2008/06/21 00:58:00 gmcgarry Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)stdlib.h	8.5 (Berkeley) 5/19/95
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 38 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/stdlib.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/featuretest.h" 1 3 4
/*	$NetBSD: featuretest.h,v 1.8 2005/12/11 12:25:20 christos Exp $	*/

/*
 * Written by Klaus Klein <kleink@NetBSD.org>, February 2, 1998.
 * Public domain.
 *
 * NOTE: Do not protect this header against multiple inclusion.  Doing
 * so can have subtle side-effects due to header file inclusion order
 * and testing of e.g. _POSIX_SOURCE vs. _POSIX_C_SOURCE.  Instead,
 * protect each CPP macro that we want to supply.
 */

/*
 * Feature-test macros are defined by several standards, and allow an
 * application to specify what symbols they want the system headers to
 * expose, and hence what standard they want them to conform to.
 * There are two classes of feature-test macros.  The first class
 * specify complete standards, and if one of these is defined, header
 * files will try to conform to the relevant standard.  They are:
 *
 * ANSI macros:
 * _ANSI_SOURCE			ANSI C89
 *
 * POSIX macros:
 * _POSIX_SOURCE == 1		IEEE Std 1003.1 (version?)
 * _POSIX_C_SOURCE == 1		IEEE Std 1003.1-1990
 * _POSIX_C_SOURCE == 2		IEEE Std 1003.2-1992
 * _POSIX_C_SOURCE == 199309L	IEEE Std 1003.1b-1993
 * _POSIX_C_SOURCE == 199506L	ISO/IEC 9945-1:1996
 * _POSIX_C_SOURCE == 200112L	IEEE Std 1003.1-2001
 *
 * X/Open macros:
 * _XOPEN_SOURCE		System Interfaces and Headers, Issue 4, Ver 2
 * _XOPEN_SOURCE_EXTENDED == 1	XSH4.2 UNIX extensions
 * _XOPEN_SOURCE == 500		System Interfaces and Headers, Issue 5
 * _XOPEN_SOURCE == 520		Networking Services (XNS), Issue 5.2
 * _XOPEN_SOURCE == 600		IEEE Std 1003.1-2001, XSI option
 *
 * NetBSD macros:
 * _NETBSD_SOURCE == 1		Make all NetBSD features available.
 *
 * If more than one of these "major" feature-test macros is defined,
 * then the set of facilities provided (and namespace used) is the
 * union of that specified by the relevant standards, and in case of
 * conflict, the earlier standard in the above list has precedence (so
 * if both _POSIX_C_SOURCE and _NETBSD_SOURCE are defined, the version
 * of rename() that's used is the POSIX one).  If none of the "major"
 * feature-test macros is defined, _NETBSD_SOURCE is assumed.
 *
 * There are also "minor" feature-test macros, which enable extra
 * functionality in addition to some base standard.  They should be
 * defined along with one of the "major" macros.  The "minor" macros
 * are:
 *
 * _REENTRANT
 * _ISOC99_SOURCE
 * _LARGEFILE_SOURCE		Large File Support
 *		<http://ftp.sas.com/standards/large.file/x_open.20Mar96.html>
 */
# 39 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/stdlib.h" 2 3 4


# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 1 3 4
/*	$NetBSD: types.h,v 1.80 2008/02/26 13:37:09 simonb Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.4 (Berkeley) 1/21/94
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/featuretest.h" 1 3 4
/*	$NetBSD: featuretest.h,v 1.8 2005/12/11 12:25:20 christos Exp $	*/

/*
 * Written by Klaus Klein <kleink@NetBSD.org>, February 2, 1998.
 * Public domain.
 *
 * NOTE: Do not protect this header against multiple inclusion.  Doing
 * so can have subtle side-effects due to header file inclusion order
 * and testing of e.g. _POSIX_SOURCE vs. _POSIX_C_SOURCE.  Instead,
 * protect each CPP macro that we want to supply.
 */

/*
 * Feature-test macros are defined by several standards, and allow an
 * application to specify what symbols they want the system headers to
 * expose, and hence what standard they want them to conform to.
 * There are two classes of feature-test macros.  The first class
 * specify complete standards, and if one of these is defined, header
 * files will try to conform to the relevant standard.  They are:
 *
 * ANSI macros:
 * _ANSI_SOURCE			ANSI C89
 *
 * POSIX macros:
 * _POSIX_SOURCE == 1		IEEE Std 1003.1 (version?)
 * _POSIX_C_SOURCE == 1		IEEE Std 1003.1-1990
 * _POSIX_C_SOURCE == 2		IEEE Std 1003.2-1992
 * _POSIX_C_SOURCE == 199309L	IEEE Std 1003.1b-1993
 * _POSIX_C_SOURCE == 199506L	ISO/IEC 9945-1:1996
 * _POSIX_C_SOURCE == 200112L	IEEE Std 1003.1-2001
 *
 * X/Open macros:
 * _XOPEN_SOURCE		System Interfaces and Headers, Issue 4, Ver 2
 * _XOPEN_SOURCE_EXTENDED == 1	XSH4.2 UNIX extensions
 * _XOPEN_SOURCE == 500		System Interfaces and Headers, Issue 5
 * _XOPEN_SOURCE == 520		Networking Services (XNS), Issue 5.2
 * _XOPEN_SOURCE == 600		IEEE Std 1003.1-2001, XSI option
 *
 * NetBSD macros:
 * _NETBSD_SOURCE == 1		Make all NetBSD features available.
 *
 * If more than one of these "major" feature-test macros is defined,
 * then the set of facilities provided (and namespace used) is the
 * union of that specified by the relevant standards, and in case of
 * conflict, the earlier standard in the above list has precedence (so
 * if both _POSIX_C_SOURCE and _NETBSD_SOURCE are defined, the version
 * of rename() that's used is the POSIX one).  If none of the "major"
 * feature-test macros is defined, _NETBSD_SOURCE is assumed.
 *
 * There are also "minor" feature-test macros, which enable extra
 * functionality in addition to some base standard.  They should be
 * defined along with one of the "major" macros.  The "minor" macros
 * are:
 *
 * _REENTRANT
 * _ISOC99_SOURCE
 * _LARGEFILE_SOURCE		Large File Support
 *		<http://ftp.sas.com/standards/large.file/x_open.20Mar96.html>
 */
# 43 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4

/* Machine type dependent parameters. */
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/types.h" 1 3 4
/*	$NetBSD: types.h,v 1.60 2008/10/26 06:57:30 mrg Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	7.5 (Berkeley) 3/9/91
 */







# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 41 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/types.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/featuretest.h" 1 3 4
/*	$NetBSD: featuretest.h,v 1.8 2005/12/11 12:25:20 christos Exp $	*/

/*
 * Written by Klaus Klein <kleink@NetBSD.org>, February 2, 1998.
 * Public domain.
 *
 * NOTE: Do not protect this header against multiple inclusion.  Doing
 * so can have subtle side-effects due to header file inclusion order
 * and testing of e.g. _POSIX_SOURCE vs. _POSIX_C_SOURCE.  Instead,
 * protect each CPP macro that we want to supply.
 */

/*
 * Feature-test macros are defined by several standards, and allow an
 * application to specify what symbols they want the system headers to
 * expose, and hence what standard they want them to conform to.
 * There are two classes of feature-test macros.  The first class
 * specify complete standards, and if one of these is defined, header
 * files will try to conform to the relevant standard.  They are:
 *
 * ANSI macros:
 * _ANSI_SOURCE			ANSI C89
 *
 * POSIX macros:
 * _POSIX_SOURCE == 1		IEEE Std 1003.1 (version?)
 * _POSIX_C_SOURCE == 1		IEEE Std 1003.1-1990
 * _POSIX_C_SOURCE == 2		IEEE Std 1003.2-1992
 * _POSIX_C_SOURCE == 199309L	IEEE Std 1003.1b-1993
 * _POSIX_C_SOURCE == 199506L	ISO/IEC 9945-1:1996
 * _POSIX_C_SOURCE == 200112L	IEEE Std 1003.1-2001
 *
 * X/Open macros:
 * _XOPEN_SOURCE		System Interfaces and Headers, Issue 4, Ver 2
 * _XOPEN_SOURCE_EXTENDED == 1	XSH4.2 UNIX extensions
 * _XOPEN_SOURCE == 500		System Interfaces and Headers, Issue 5
 * _XOPEN_SOURCE == 520		Networking Services (XNS), Issue 5.2
 * _XOPEN_SOURCE == 600		IEEE Std 1003.1-2001, XSI option
 *
 * NetBSD macros:
 * _NETBSD_SOURCE == 1		Make all NetBSD features available.
 *
 * If more than one of these "major" feature-test macros is defined,
 * then the set of facilities provided (and namespace used) is the
 * union of that specified by the relevant standards, and in case of
 * conflict, the earlier standard in the above list has precedence (so
 * if both _POSIX_C_SOURCE and _NETBSD_SOURCE are defined, the version
 * of rename() that's used is the POSIX one).  If none of the "major"
 * feature-test macros is defined, _NETBSD_SOURCE is assumed.
 *
 * There are also "minor" feature-test macros, which enable extra
 * functionality in addition to some base standard.  They should be
 * defined along with one of the "major" macros.  The "minor" macros
 * are:
 *
 * _REENTRANT
 * _ISOC99_SOURCE
 * _LARGEFILE_SOURCE		Large File Support
 *		<http://ftp.sas.com/standards/large.file/x_open.20Mar96.html>
 */
# 42 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/types.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/int_types.h" 1 3 4
/*	$NetBSD: int_types.h,v 1.10 2005/12/24 20:07:10 perry Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)types.h	7.5 (Berkeley) 3/9/91
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 38 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/int_types.h" 2 3 4

/*
 * 7.18.1 Integer types
 */

/* 7.18.1.1 Exact-width integer types */

typedef signed char __int8_t;
typedef unsigned char __uint8_t;
typedef short int __int16_t;
typedef unsigned short int __uint16_t;
typedef int __int32_t;
typedef unsigned int __uint32_t;




/* LONGLONG */
typedef long long int __int64_t;
/* LONGLONG */
typedef unsigned long long int __uint64_t;




/* 7.18.1.4 Integer types capable of holding object pointers */

typedef int __intptr_t;
typedef unsigned int __uintptr_t;
# 43 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/types.h" 2 3 4







/* NB: This should probably be if defined(_KERNEL) */





typedef unsigned long paddr_t;
typedef unsigned long psize_t;

typedef unsigned long vaddr_t;
typedef unsigned long vsize_t;


typedef int pmc_evid_t;
typedef __uint64_t pmc_ctr_t;
typedef int register_t;

typedef volatile unsigned char __cpu_simple_lock_t;

/* __cpu_simple_lock_t used to be a full word. */





/* The x86 does not have strict alignment requirements. */
# 46 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4

# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/ansi.h" 1 3 4
/*	$NetBSD: ansi.h,v 1.22 2008/10/26 06:57:30 mrg Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ansi.h	8.2 (Berkeley) 1/4/94
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 38 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/ansi.h" 2 3 4

# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/int_types.h" 1 3 4
/*	$NetBSD: int_types.h,v 1.10 2005/12/24 20:07:10 perry Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)types.h	7.5 (Berkeley) 3/9/91
 */
# 40 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/ansi.h" 2 3 4

/*
 * Types which are fundamental to the implementation and may appear in
 * more than one standard header are defined here.  Standard headers
 * then use:
 *	#ifdef	_BSD_SIZE_T_
 *	typedef	_BSD_SIZE_T_ size_t;
 *	#undef	_BSD_SIZE_T_
 *	#endif
 */
# 69 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/ansi.h" 3 4
/*
 * mbstate_t is an opaque object to keep conversion state, during multibyte
 * stream conversions.  The content must not be referenced by user programs.
 */
typedef union {
 __int64_t __mbstateL; /* for alignment */
 char __mbstate8[128];
} __mbstate_t;
# 48 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/int_types.h" 1 3 4
/*	$NetBSD: int_types.h,v 1.10 2005/12/24 20:07:10 perry Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)types.h	7.5 (Berkeley) 3/9/91
 */
# 49 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4


# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/ansi.h" 1 3 4
/*	$NetBSD: ansi.h,v 1.12 2008/04/28 20:24:10 martin Exp $	*/

/*-
 * Copyright (c) 2000, 2001, 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jun-ichiro itojun Hagino and by Klaus Klein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/int_types.h" 1 3 4
/*	$NetBSD: int_types.h,v 1.10 2005/12/24 20:07:10 perry Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)types.h	7.5 (Berkeley) 3/9/91
 */
# 36 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/ansi.h" 2 3 4

typedef char * __caddr_t; /* core address */
typedef __uint32_t __gid_t; /* group id */
typedef __uint32_t __in_addr_t; /* IP(v4) address */
typedef __uint16_t __in_port_t; /* "Internet" port number */
typedef __uint32_t __mode_t; /* file permissions */
typedef __int64_t __off_t; /* file offset */
typedef __int32_t __pid_t; /* process id */
typedef __uint8_t __sa_family_t; /* socket address family */
typedef unsigned int __socklen_t; /* socket-related datum length */
typedef __uint32_t __uid_t; /* user id */
typedef __uint64_t __fsblkcnt_t; /* fs block count (statvfs) */
typedef __uint64_t __fsfilcnt_t; /* fs file count */
# 52 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4


typedef __int8_t int8_t;




typedef __uint8_t uint8_t;




typedef __int16_t int16_t;




typedef __uint16_t uint16_t;




typedef __int32_t int32_t;




typedef __uint32_t uint32_t;




typedef __int64_t int64_t;




typedef __uint64_t uint64_t;



typedef __uint8_t u_int8_t;
typedef __uint16_t u_int16_t;
typedef __uint32_t u_int32_t;
typedef __uint64_t u_int64_t;

# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/endian.h" 1 3 4
/*	$NetBSD: endian.h,v 1.29 2000/03/17 00:09:20 mycroft Exp $	*/

# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 1 3 4
/*	$NetBSD: endian.h,v 1.26 2007/07/20 15:07:15 christos Exp $	*/

/*
 * Copyright (c) 1987, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)endian.h	8.1 (Berkeley) 6/11/93
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/featuretest.h" 1 3 4
/*	$NetBSD: featuretest.h,v 1.8 2005/12/11 12:25:20 christos Exp $	*/

/*
 * Written by Klaus Klein <kleink@NetBSD.org>, February 2, 1998.
 * Public domain.
 *
 * NOTE: Do not protect this header against multiple inclusion.  Doing
 * so can have subtle side-effects due to header file inclusion order
 * and testing of e.g. _POSIX_SOURCE vs. _POSIX_C_SOURCE.  Instead,
 * protect each CPP macro that we want to supply.
 */

/*
 * Feature-test macros are defined by several standards, and allow an
 * application to specify what symbols they want the system headers to
 * expose, and hence what standard they want them to conform to.
 * There are two classes of feature-test macros.  The first class
 * specify complete standards, and if one of these is defined, header
 * files will try to conform to the relevant standard.  They are:
 *
 * ANSI macros:
 * _ANSI_SOURCE			ANSI C89
 *
 * POSIX macros:
 * _POSIX_SOURCE == 1		IEEE Std 1003.1 (version?)
 * _POSIX_C_SOURCE == 1		IEEE Std 1003.1-1990
 * _POSIX_C_SOURCE == 2		IEEE Std 1003.2-1992
 * _POSIX_C_SOURCE == 199309L	IEEE Std 1003.1b-1993
 * _POSIX_C_SOURCE == 199506L	ISO/IEC 9945-1:1996
 * _POSIX_C_SOURCE == 200112L	IEEE Std 1003.1-2001
 *
 * X/Open macros:
 * _XOPEN_SOURCE		System Interfaces and Headers, Issue 4, Ver 2
 * _XOPEN_SOURCE_EXTENDED == 1	XSH4.2 UNIX extensions
 * _XOPEN_SOURCE == 500		System Interfaces and Headers, Issue 5
 * _XOPEN_SOURCE == 520		Networking Services (XNS), Issue 5.2
 * _XOPEN_SOURCE == 600		IEEE Std 1003.1-2001, XSI option
 *
 * NetBSD macros:
 * _NETBSD_SOURCE == 1		Make all NetBSD features available.
 *
 * If more than one of these "major" feature-test macros is defined,
 * then the set of facilities provided (and namespace used) is the
 * union of that specified by the relevant standards, and in case of
 * conflict, the earlier standard in the above list has precedence (so
 * if both _POSIX_C_SOURCE and _NETBSD_SOURCE are defined, the version
 * of rename() that's used is the POSIX one).  If none of the "major"
 * feature-test macros is defined, _NETBSD_SOURCE is assumed.
 *
 * There are also "minor" feature-test macros, which enable extra
 * functionality in addition to some base standard.  They should be
 * defined along with one of the "major" macros.  The "minor" macros
 * are:
 *
 * _REENTRANT
 * _ISOC99_SOURCE
 * _LARGEFILE_SOURCE		Large File Support
 *		<http://ftp.sas.com/standards/large.file/x_open.20Mar96.html>
 */
# 38 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 2 3 4

/*
 * Definitions for byte order, according to byte significance from low
 * address to high.
 */
# 51 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 3 4
/* C-family endian-ness definitions */

# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/ansi.h" 1 3 4
/*	$NetBSD: ansi.h,v 1.12 2008/04/28 20:24:10 martin Exp $	*/

/*-
 * Copyright (c) 2000, 2001, 2002 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jun-ichiro itojun Hagino and by Klaus Klein.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
# 54 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 55 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 1 3 4
/*	$NetBSD: types.h,v 1.80 2008/02/26 13:37:09 simonb Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.4 (Berkeley) 1/21/94
 */
# 56 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 2 3 4


typedef __in_addr_t in_addr_t;




typedef __in_port_t in_port_t;




__uint32_t htonl(__uint32_t) __attribute__((__const__));
__uint16_t htons(__uint16_t) __attribute__((__const__));
__uint32_t ntohl(__uint32_t) __attribute__((__const__));
__uint16_t ntohs(__uint16_t) __attribute__((__const__));






# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/endian_machdep.h" 1 3 4
/*	$NetBSD: endian_machdep.h,v 1.3 2006/01/30 21:52:38 dsl Exp $	*/
# 79 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 2 3 4

/*
 * Define the order of 32-bit words in 64-bit words.
 */
# 95 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 3 4
/*
 *  Traditional names for byteorder.  These are defined as the numeric
 *  sequences so that third party code can "#define XXX_ENDIAN" and not
 *  cause errors.
 */







# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/bswap.h" 1 3 4
/*      $NetBSD: bswap.h,v 1.4 2008/10/26 06:57:30 mrg Exp $      */

/* Written by Manuel Bouyer. Public domain */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/byte_swap.h" 1 3 4
/*	$NetBSD: byte_swap.h,v 1.15 2008/04/28 20:23:24 martin Exp $	*/

/*-
 * Copyright (c) 1998 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Charles M. Hannum.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 1 3 4
/*	$NetBSD: types.h,v 1.80 2008/02/26 13:37:09 simonb Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.4 (Berkeley) 1/21/94
 */
# 36 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/byte_swap.h" 2 3 4





static __inline __uint32_t __byte_swap_u32_variable(__uint32_t);
static __inline __uint32_t
__byte_swap_u32_variable(__uint32_t x)
{
 __asm volatile (
     "bswap %1"
     : "=r" (x) : "0" (x));
 return (x);
}


static __inline __uint16_t __byte_swap_u16_variable(__uint16_t);
static __inline __uint16_t
__byte_swap_u16_variable(__uint16_t x)
{
 __asm volatile ("rorw $8, %w1" : "=r" (x) : "0" (x));
 return (x);
}


# 9 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/bswap.h" 2 3 4


# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/bswap.h" 1 3 4
/*      $NetBSD: bswap.h,v 1.13 2008/06/27 22:19:43 mlelstv Exp $      */

/* Written by Manuel Bouyer. Public domain */





# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 10 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/bswap.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 1 3 4
/*	$NetBSD: types.h,v 1.80 2008/02/26 13:37:09 simonb Exp $	*/

/*-
 * Copyright (c) 1982, 1986, 1991, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 * (c) UNIX System Laboratories, Inc.
 * All or some portions of this file are derived from material licensed
 * to the University of California by American Telephone and Telegraph
 * Co. or Unix System Laboratories, Inc. and are reproduced herein with
 * the permission of UNIX System Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)types.h	8.4 (Berkeley) 1/21/94
 */
# 11 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/bswap.h" 2 3 4

# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/bswap.h" 1 3 4
/*      $NetBSD: bswap.h,v 1.4 2008/10/26 06:57:30 mrg Exp $      */

/* Written by Manuel Bouyer. Public domain */
# 13 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/bswap.h" 2 3 4


/* Always declare the functions in case their address is taken (etc) */




__uint16_t bswap16(__uint16_t) __asm("__bswap16") __attribute__((__const__));
__uint32_t bswap32(__uint32_t) __asm("__bswap32") __attribute__((__const__));

__uint64_t bswap64(__uint64_t) __attribute__((__const__));




/* machine/byte_swap.h might have defined inline versions */
# 12 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/bswap.h" 2 3 4
# 108 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 2 3 4

/*
 * Macros for network/external number representation conversion.
 */
# 136 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 3 4
/*
 * Macros to convert to a specific endianness.
 */
# 188 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 3 4
/*
 * Routines to encode/decode big- and little-endian multi-octet values
 * to/from an octet stream.
 */
# 203 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 3 4
static __inline __attribute__((__unused__)) void be16enc(void *dst, __uint16_t u) { u = (__builtin_constant_p(((__uint16_t)(u))) ? ((__uint16_t) (((((__uint16_t)(u)) & 0xff00) >> 8) | ((((__uint16_t)(u)) & 0x00ff) << 8))) : __byte_swap_u16_variable((__uint16_t)(u))); __builtin_memcpy(dst, &u, sizeof(u)); }
static __inline __attribute__((__unused__)) void be32enc(void *dst, __uint32_t u) { u = (__builtin_constant_p(((__uint32_t)(u))) ? ((__uint32_t) (((((__uint32_t)(u)) & 0xff000000) >> 24) | ((((__uint32_t)(u)) & 0x00ff0000) >> 8) | ((((__uint32_t)(u)) & 0x0000ff00) << 8) | ((((__uint32_t)(u)) & 0x000000ff) << 24))) : __byte_swap_u32_variable((__uint32_t)(u))); __builtin_memcpy(dst, &u, sizeof(u)); }
static __inline __attribute__((__unused__)) void be64enc(void *dst, __uint64_t u) { u = (__builtin_constant_p(((__uint64_t)(u))) ? ((__uint64_t) (((((__uint64_t)(u)) & 0xff00000000000000ull) >> 56) | ((((__uint64_t)(u)) & 0x00ff000000000000ull) >> 40) | ((((__uint64_t)(u)) & 0x0000ff0000000000ull) >> 24) | ((((__uint64_t)(u)) & 0x000000ff00000000ull) >> 8) | ((((__uint64_t)(u)) & 0x00000000ff000000ull) << 8) | ((((__uint64_t)(u)) & 0x0000000000ff0000ull) << 24) | ((((__uint64_t)(u)) & 0x000000000000ff00ull) << 40) | ((((__uint64_t)(u)) & 0x00000000000000ffull) << 56))) : bswap64((__uint64_t)(u))); __builtin_memcpy(dst, &u, sizeof(u)); }
static __inline __attribute__((__unused__)) void le16enc(void *dst, __uint16_t u) { u = (u); __builtin_memcpy(dst, &u, sizeof(u)); }
static __inline __attribute__((__unused__)) void le32enc(void *dst, __uint32_t u) { u = (u); __builtin_memcpy(dst, &u, sizeof(u)); }
static __inline __attribute__((__unused__)) void le64enc(void *dst, __uint64_t u) { u = (u); __builtin_memcpy(dst, &u, sizeof(u)); }
# 220 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/endian.h" 3 4
static __inline __attribute__((__unused__)) __uint16_t be16dec(const void *buf) { __uint16_t u; __builtin_memcpy(&u, buf, sizeof(u)); return (__builtin_constant_p(((__uint16_t)(u))) ? ((__uint16_t) (((((__uint16_t)(u)) & 0xff00) >> 8) | ((((__uint16_t)(u)) & 0x00ff) << 8))) : __byte_swap_u16_variable((__uint16_t)(u))); }
static __inline __attribute__((__unused__)) __uint32_t be32dec(const void *buf) { __uint32_t u; __builtin_memcpy(&u, buf, sizeof(u)); return (__builtin_constant_p(((__uint32_t)(u))) ? ((__uint32_t) (((((__uint32_t)(u)) & 0xff000000) >> 24) | ((((__uint32_t)(u)) & 0x00ff0000) >> 8) | ((((__uint32_t)(u)) & 0x0000ff00) << 8) | ((((__uint32_t)(u)) & 0x000000ff) << 24))) : __byte_swap_u32_variable((__uint32_t)(u))); }
static __inline __attribute__((__unused__)) __uint64_t be64dec(const void *buf) { __uint64_t u; __builtin_memcpy(&u, buf, sizeof(u)); return (__builtin_constant_p(((__uint64_t)(u))) ? ((__uint64_t) (((((__uint64_t)(u)) & 0xff00000000000000ull) >> 56) | ((((__uint64_t)(u)) & 0x00ff000000000000ull) >> 40) | ((((__uint64_t)(u)) & 0x0000ff0000000000ull) >> 24) | ((((__uint64_t)(u)) & 0x000000ff00000000ull) >> 8) | ((((__uint64_t)(u)) & 0x00000000ff000000ull) << 8) | ((((__uint64_t)(u)) & 0x0000000000ff0000ull) << 24) | ((((__uint64_t)(u)) & 0x000000000000ff00ull) << 40) | ((((__uint64_t)(u)) & 0x00000000000000ffull) << 56))) : bswap64((__uint64_t)(u))); }
static __inline __attribute__((__unused__)) __uint16_t le16dec(const void *buf) { __uint16_t u; __builtin_memcpy(&u, buf, sizeof(u)); return (u); }
static __inline __attribute__((__unused__)) __uint32_t le32dec(const void *buf) { __uint32_t u; __builtin_memcpy(&u, buf, sizeof(u)); return (u); }
static __inline __attribute__((__unused__)) __uint64_t le64dec(const void *buf) { __uint64_t u; __builtin_memcpy(&u, buf, sizeof(u)); return (u); }
# 3 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/endian.h" 2 3 4
# 99 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4


typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;
typedef unsigned long u_long;

typedef unsigned char unchar; /* Sys V compatibility */
typedef unsigned short ushort; /* Sys V compatibility */
typedef unsigned int uint; /* Sys V compatibility */
typedef unsigned long ulong; /* Sys V compatibility */


typedef __uint64_t u_quad_t; /* quads */
typedef __int64_t quad_t;
typedef quad_t * qaddr_t;

/*
 * The types longlong_t and u_longlong_t exist for use with the
 * Sun-derived XDR routines involving these types, and their usage
 * in other contexts is discouraged.  Further note that these types
 * may not be equivalent to "long long" and "unsigned long long",
 * they are only guaranteed to be signed and unsigned 64-bit types
 * respectively.  Portable programs that need 64-bit types should use
 * the C99 types int64_t and uint64_t instead.
 */

typedef __int64_t longlong_t; /* for XDR */
typedef __uint64_t u_longlong_t; /* for XDR */

typedef __int64_t blkcnt_t; /* fs block count */
typedef __uint32_t blksize_t; /* fs optimal block size */


typedef __fsblkcnt_t fsblkcnt_t; /* fs block count (statvfs) */




typedef __fsfilcnt_t fsfilcnt_t; /* fs file count */




/* We don't and shouldn't use caddr_t in the kernel anymore */

typedef __caddr_t caddr_t; /* core address */
# 154 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 3 4
typedef __int64_t daddr_t; /* disk address */


typedef __uint32_t dev_t; /* device number */
typedef __uint32_t fixpt_t; /* fixed point number */


typedef __gid_t gid_t; /* group id */



typedef int idtype_t; /* type of the id */
typedef __uint32_t id_t; /* group id, process id or user id */
typedef __uint64_t ino_t; /* inode number */
typedef long key_t; /* IPC key (for Sys V IPC) */


typedef __mode_t mode_t; /* permissions */



typedef __uint32_t nlink_t; /* link count */


typedef __off_t off_t; /* file offset */




typedef __pid_t pid_t; /* process id */


typedef __int32_t lwpid_t; /* LWP id */
typedef quad_t rlim_t; /* resource limit */
typedef __int32_t segsz_t; /* segment size */
typedef __int32_t swblk_t; /* swap offset */


typedef __uid_t uid_t; /* user id */



typedef int mqd_t;

typedef unsigned long cpuid_t;

typedef int psetid_t;
# 240 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 3 4
/*
 * These belong in unistd.h, but are placed here too to ensure that
 * long arguments will be promoted to off_t if the program fails to
 * include that header or explicitly cast them to off_t.
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 250 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4

__off_t lseek(int, __off_t, int);
int ftruncate(int, __off_t);
int truncate(const char *, __off_t);






/* Major, minor numbers, dev_t's. */
# 270 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 3 4
typedef unsigned long clock_t;




typedef unsigned int size_t;





typedef int ssize_t;




typedef long time_t;




typedef int clockid_t;




typedef int timer_t;




typedef int suseconds_t;




typedef unsigned int useconds_t;




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/fd_set.h" 1 3 4
/*	$NetBSD: fd_set.h,v 1.2 2005/12/11 12:25:20 christos Exp $	*/

/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)types.h	8.4 (Berkeley) 1/21/94
 */




# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/cdefs.h" 1 3 4
/*	$NetBSD: cdefs.h,v 1.69 2008/08/17 00:23:02 gmcgarry Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Berkeley Software Design, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)cdefs.h	8.8 (Berkeley) 1/9/95
 */
# 38 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/fd_set.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/featuretest.h" 1 3 4
/*	$NetBSD: featuretest.h,v 1.8 2005/12/11 12:25:20 christos Exp $	*/

/*
 * Written by Klaus Klein <kleink@NetBSD.org>, February 2, 1998.
 * Public domain.
 *
 * NOTE: Do not protect this header against multiple inclusion.  Doing
 * so can have subtle side-effects due to header file inclusion order
 * and testing of e.g. _POSIX_SOURCE vs. _POSIX_C_SOURCE.  Instead,
 * protect each CPP macro that we want to supply.
 */

/*
 * Feature-test macros are defined by several standards, and allow an
 * application to specify what symbols they want the system headers to
 * expose, and hence what standard they want them to conform to.
 * There are two classes of feature-test macros.  The first class
 * specify complete standards, and if one of these is defined, header
 * files will try to conform to the relevant standard.  They are:
 *
 * ANSI macros:
 * _ANSI_SOURCE			ANSI C89
 *
 * POSIX macros:
 * _POSIX_SOURCE == 1		IEEE Std 1003.1 (version?)
 * _POSIX_C_SOURCE == 1		IEEE Std 1003.1-1990
 * _POSIX_C_SOURCE == 2		IEEE Std 1003.2-1992
 * _POSIX_C_SOURCE == 199309L	IEEE Std 1003.1b-1993
 * _POSIX_C_SOURCE == 199506L	ISO/IEC 9945-1:1996
 * _POSIX_C_SOURCE == 200112L	IEEE Std 1003.1-2001
 *
 * X/Open macros:
 * _XOPEN_SOURCE		System Interfaces and Headers, Issue 4, Ver 2
 * _XOPEN_SOURCE_EXTENDED == 1	XSH4.2 UNIX extensions
 * _XOPEN_SOURCE == 500		System Interfaces and Headers, Issue 5
 * _XOPEN_SOURCE == 520		Networking Services (XNS), Issue 5.2
 * _XOPEN_SOURCE == 600		IEEE Std 1003.1-2001, XSI option
 *
 * NetBSD macros:
 * _NETBSD_SOURCE == 1		Make all NetBSD features available.
 *
 * If more than one of these "major" feature-test macros is defined,
 * then the set of facilities provided (and namespace used) is the
 * union of that specified by the relevant standards, and in case of
 * conflict, the earlier standard in the above list has precedence (so
 * if both _POSIX_C_SOURCE and _NETBSD_SOURCE are defined, the version
 * of rename() that's used is the POSIX one).  If none of the "major"
 * feature-test macros is defined, _NETBSD_SOURCE is assumed.
 *
 * There are also "minor" feature-test macros, which enable extra
 * functionality in addition to some base standard.  They should be
 * defined along with one of the "major" macros.  The "minor" macros
 * are:
 *
 * _REENTRANT
 * _ISOC99_SOURCE
 * _LARGEFILE_SOURCE		Large File Support
 *		<http://ftp.sas.com/standards/large.file/x_open.20Mar96.html>
 */
# 39 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/fd_set.h" 2 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/int_types.h" 1 3 4
/*	$NetBSD: int_types.h,v 1.10 2005/12/24 20:07:10 perry Exp $	*/

/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	from: @(#)types.h	7.5 (Berkeley) 3/9/91
 */
# 40 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/fd_set.h" 2 3 4

/*
 * Implementation dependent defines, hidden from user space. X/Open does not
 * specify them.
 */

typedef __int32_t __fd_mask;

/* bits per mask */




/*
 * Select uses bit masks of file descriptors in longs.  These macros
 * manipulate such bit fields (the filesystem macros use chars).
 * FD_SETSIZE may be defined by the user, but the default here should
 * be enough for most uses.
 */




typedef struct fd_set {
 __fd_mask fds_bits[(((256) + ((((unsigned int)sizeof(__fd_mask) * 8)) - 1)) / (((unsigned int)sizeof(__fd_mask) * 8)))];
} fd_set;
# 84 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/fd_set.h" 3 4
/*
 * Expose our internals if we are not required to hide them.
 */
# 312 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4


typedef struct kauth_cred *kauth_cred_t;

typedef int pri_t;
# 351 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 3 4
# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/pthread_types.h" 1 3 4
/*	$NetBSD: pthread_types.h,v 1.13 2008/08/02 19:46:30 matt Exp $	*/

/*-
 * Copyright (c) 2001, 2008 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Nathan J. Williams.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */




/*
 * We use the "pthread_spin_t" name internally; "pthread_spinlock_t" is the
 * POSIX spinlock object. 
 */
typedef __cpu_simple_lock_t pthread_spin_t;

/*
 * Copied from PTQ_HEAD in pthread_queue.h
 */






struct pthread_queue_struct_t { struct __pthread_st *ptqh_first; struct __pthread_st **ptqh_last; };
typedef struct pthread_queue_struct_t pthread_queue_t;

struct __pthread_st;
struct __pthread_attr_st;
struct __pthread_mutex_st;
struct __pthread_mutexattr_st;
struct __pthread_cond_st;
struct __pthread_condattr_st;
struct __pthread_spin_st;
struct __pthread_rwlock_st;
struct __pthread_rwlockattr_st;
struct __pthread_barrier_st;
struct __pthread_barrierattr_st;

typedef struct __pthread_st *pthread_t;
typedef struct __pthread_attr_st pthread_attr_t;
typedef struct __pthread_mutex_st pthread_mutex_t;
typedef struct __pthread_mutexattr_st pthread_mutexattr_t;
typedef struct __pthread_cond_st pthread_cond_t;
typedef struct __pthread_condattr_st pthread_condattr_t;
typedef struct __pthread_once_st pthread_once_t;
typedef struct __pthread_spinlock_st pthread_spinlock_t;
typedef struct __pthread_rwlock_st pthread_rwlock_t;
typedef struct __pthread_rwlockattr_st pthread_rwlockattr_t;
typedef struct __pthread_barrier_st pthread_barrier_t;
typedef struct __pthread_barrierattr_st pthread_barrierattr_t;
typedef int pthread_key_t;

struct __pthread_attr_st {
 unsigned int pta_magic;

 int pta_flags;
 void *pta_private;
};

/*
 * ptm_owner is the actual lock field which is locked via CAS operation.
 * This structure's layout is designed to compatible with the previous
 * version used in SA pthreads.
 */

/*
 * If __SIMPLE_UNLOCKED != 0 and we have to pad, we have to worry about
 * endianness.  Currently that isn't an issue but put in a check in case
 * something changes in the future.
 */




struct __pthread_mutex_st {
 unsigned int ptm_magic;
 pthread_spin_t ptm_errorcheck;

 __uint8_t ptm_pad1[3];

 pthread_spin_t ptm_interlock; /* unused - backwards compat */

 __uint8_t ptm_pad2[3];

 volatile pthread_t ptm_owner;
 pthread_t * volatile ptm_waiters;
 unsigned int ptm_recursed;
 void *ptm_spare2; /* unused - backwards compat */
};
# 135 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/pthread_types.h" 3 4
struct __pthread_mutexattr_st {
 unsigned int ptma_magic;
 void *ptma_private;
};





struct __pthread_cond_st {
 unsigned int ptc_magic;

 /* Protects the queue of waiters */
 pthread_spin_t ptc_lock;
 pthread_queue_t ptc_waiters;

 pthread_mutex_t *ptc_mutex; /* Current mutex */
 void *ptc_private;
};
# 165 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/pthread_types.h" 3 4
struct __pthread_condattr_st {
 unsigned int ptca_magic;
 void *ptca_private;
};




struct __pthread_once_st {
 pthread_mutex_t pto_mutex;
 int pto_done;
};



struct __pthread_spinlock_st {
 unsigned int pts_magic;
 pthread_spin_t pts_spin;
 int pts_flags;
};





/* PTHREAD_SPINLOCK_INITIALIZER is an extension not specified by POSIX. */





struct __pthread_rwlock_st {
 unsigned int ptr_magic;

 /* Protects data below */
 pthread_spin_t ptr_interlock;

 pthread_queue_t ptr_rblocked;
 pthread_queue_t ptr_wblocked;
 unsigned int ptr_nreaders;
 volatile pthread_t ptr_owner;
 void *ptr_private;
};
# 221 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/pthread_types.h" 3 4
struct __pthread_rwlockattr_st {
 unsigned int ptra_magic;
 void *ptra_private;
};




struct __pthread_barrier_st {
 unsigned int ptb_magic;

 /* Protects data below */
 pthread_spin_t ptb_lock;

 pthread_queue_t ptb_waiters;
 unsigned int ptb_initcount;
 unsigned int ptb_curcount;
 unsigned int ptb_generation;

 void *ptb_private;
};




struct __pthread_barrierattr_st {
 unsigned int ptba_magic;
 void *ptba_private;
};
# 352 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/types.h" 2 3 4
# 42 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/stdlib.h" 2 3 4


# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/machine/ansi.h" 1 3 4
/*	$NetBSD: ansi.h,v 1.22 2008/10/26 06:57:30 mrg Exp $	*/

/*-
 * Copyright (c) 1990, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)ansi.h	8.2 (Berkeley) 1/4/94
 */
# 45 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/stdlib.h" 2 3 4







typedef int wchar_t;



typedef struct {
 int quot; /* quotient */
 int rem; /* remainder */
} div_t;

typedef struct {
 long quot; /* quotient */
 long rem; /* remainder */
} ldiv_t;




typedef struct {
 /* LONGLONG */
 long long int quot; /* quotient */
 /* LONGLONG */
 long long int rem; /* remainder */
} lldiv_t;



typedef struct {
 quad_t quot; /* quotient */
 quad_t rem; /* remainder */
} qdiv_t;



# 1 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/sys/null.h" 1 3 4
/*	$NetBSD: null.h,v 1.7.86.1 2010/03/21 03:45:33 snj Exp $	*/
# 86 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/stdlib.h" 2 3 4






extern size_t __mb_cur_max;



__attribute__((__noreturn__)) void _Exit(int);
__attribute__((__noreturn__)) void abort(void);
__attribute__((__pure__)) int abs(int);
int atexit(void (*)(void));
double atof(const char *);
int atoi(const char *);
long atol(const char *);


/* also in search.h */
void *bsearch(const void *, const void *, size_t, size_t,
    int (*)(const void *, const void *));

void *calloc(size_t, size_t);
div_t div(int, int);
__attribute__((__noreturn__)) void exit(int);
void free(void *);
 char *getenv(const char *);
__attribute__((__pure__)) long
  labs(long);
ldiv_t ldiv(long, long);
void *malloc(size_t);
void qsort(void *, size_t, size_t, int (*)(const void *, const void *));
int rand(void);
void *realloc(void *, size_t);
void srand(unsigned);
double strtod(const char * __restrict, char ** __restrict);
long strtol(const char * __restrict, char ** __restrict, int);
unsigned long
  strtoul(const char * __restrict, char ** __restrict, int);
int system(const char *);

/* These are currently just stubs. */
int mblen(const char *, size_t);
size_t mbstowcs(wchar_t * __restrict, const char * __restrict, size_t);
int wctomb(char *, wchar_t);
int mbtowc(wchar_t * __restrict, const char * __restrict, size_t);
size_t wcstombs(char * __restrict, const wchar_t * __restrict, size_t);





/*
 * IEEE Std 1003.1c-95, also adopted by X/Open CAE Spec Issue 5 Version 2
 */


int rand_r(unsigned int *);



/*
 * X/Open Portability Guide >= Issue 4
 */

double drand48(void);
double erand48(unsigned short[3]);
long jrand48(unsigned short[3]);
void lcong48(unsigned short[7]);
long lrand48(void);
long mrand48(void);
long nrand48(unsigned short[3]);
unsigned short *
  seed48(unsigned short[3]);
void srand48(long);

int putenv(const char *);



/*
 * X/Open Portability Guide >= Issue 4 Version 2
 */


long a64l(const char *);
char *l64a(long);

char *initstate(unsigned long, char *, size_t);
long random(void);
char *setstate(char *);
void srandom(unsigned long);




char *mkdtemp(char *);
int mkstemp(char *);
char *mktemp(char *)



 ;

int setkey(const char *);

char *realpath(const char *, char *);

int ttyslot(void);

void *valloc(size_t); /* obsoleted by malloc() */

int grantpt(int);
int unlockpt(int);
char *ptsname(int);


/*
 * ISO C99
 */


/* LONGLONG */
long long int atoll(const char *);
/* LONGLONG */
long long int llabs(long long int);
/* LONGLONG */
lldiv_t lldiv(long long int, long long int);
/* LONGLONG */
long long int strtoll(const char * __restrict, char ** __restrict, int);
/* LONGLONG */
unsigned long long int
  strtoull(const char * __restrict, char ** __restrict, int);
float strtof(const char * __restrict, char ** __restrict);
long double strtold(const char * __restrict, char ** __restrict);


/*
 * The Open Group Base Specifications, Issue 6; IEEE Std 1003.1-2001 (POSIX)
 */


int setenv(const char *, const char *, int);

int unsetenv(const char *) __asm("__unsetenv13");


int posix_openpt(int);
int posix_memalign(void **, size_t, size_t);


/*
 * Implementation-defined extensions
 */







void *alloca(size_t);


__uint32_t arc4random(void);
void arc4random_stir(void);
void arc4random_addrandom(u_char *, int);
char *getbsize(int *, long *);
char *cgetcap(char *, const char *, int);
int cgetclose(void);
int cgetent(char **, const char * const *, const char *);
int cgetfirst(char **, const char * const *);
int cgetmatch(const char *, const char *);
int cgetnext(char **, const char * const *);
int cgetnum(char *, const char *, long *);
int cgetset(const char *);
int cgetstr(char *, const char *, char **);
int cgetustr(char *, const char *, char **);
void csetexpandtc(int);

int daemon(int, int);
 char *devname(dev_t, __mode_t);
# 278 "/rest/build/woods/once/netbsd-5-i386-ppro-destdir/usr/include/stdlib.h" 3 4
int humanize_number(char *, size_t, __int64_t, const char *, int, int);
int dehumanize_number(const char *, __int64_t *);

dev_t getdevmajor(const char *, __mode_t);
int getloadavg(double [], int);

int getenv_r(const char *, char *, size_t);

void cfree(void *);

int heapsort(void *, size_t, size_t, int (*)(const void *, const void *));
int mergesort(void *, size_t, size_t,
     int (*)(const void *, const void *));
int radixsort(const unsigned char **, int, const unsigned char *,
     unsigned);
int sradixsort(const unsigned char **, int, const unsigned char *,
     unsigned);

void setproctitle(const char *, ...)
     __attribute__((__format__(__printf__, 1, 2)));
const char *getprogname(void) __attribute__((const));
void setprogname(const char *);

quad_t qabs(quad_t);
quad_t strtoq(const char * __restrict, char ** __restrict, int);
u_quad_t strtouq(const char * __restrict, char ** __restrict, int);

 /* LONGLONG */
long long strsuftoll(const char *, const char *, long long, long long);
 /* LONGLONG */
long long strsuftollx(const char *, const char *, long long, long long,
       char *, size_t);

int l64a_r(long, char *, int);

size_t shquote(const char *, char *, size_t);
size_t shquotev(int, char * const *, char *, size_t);




qdiv_t qdiv(quad_t, quad_t);


# 12 "tgcc-string-concat.c" 2

int main(int, char *[]);

int
main(int argv __attribute__((__unused__)), char *argc[] __attribute__((__unused__)))
{
 exit(1);
 /* NOTREACHED */
}
