	#
	#	thello.s: "Hello world!" in x86_64 assembly language
	#
	# this example was mostly stolen from:  https://polprog.net/blog/netbsdasmprog/
	#
	# See also:  https://github.com/ibara/echo
	#
.section	.text

	# N.B.:  note the inclusion of /usr/lib/sysident.o in the link loader
	# command.

	# Finding the true name of the default "ENTRY" symbol in the
	# documentation is basically impossible.
	# 
	# ld(1) gives no hints whatsoever.
	# 
	# ld.info for GNU LD suggests the default entry point might be "start".
	# 
	# Indeed in the code (ld/ldlang.c) we can see that a string
	# entry_symbol_default is set to "start".  But that's not used in
	# practice, so is not the name of the symbol we have to provide here!
	# 
	# However, there's also a default "internal" linker script, and of
	# course it can contain an "ENTRY()" line, and indeed the default
	# scripts for elf64-x86-64 on NetBSD/amd64 do just that as follows:
	#
	#	ENTRY(_start)
	# 
	# These scripts are built into ld(1), though one matching the default
	# can be found in /usr/libdata/ldscripts/elf_x86_64.xc, which was
	# generated at build time as the output of a script found in
	# src/external/gpl3/binutils/dist/ld/scripttempl/elf.sc)
	# 
	# One can view the script used by the linker by adding "--verbose" to
	# the ld command-line options.
	# 
	# However note also that ld.info describes an algorithm for setting the
	# entry address, and so in a simple example like this we don't even need
	# to define an entry symbol since one of the options is to use "the
	# address of the first byte of the `.text' section, if present".
	#
	.globl _start

_start:
	andq $-16, %rsp		# clear the 4 least significant bits of stack pointer to align it

	# from src/sys/arch/x86/x86/syscall.c:
	#	ifdef __x86_64__
	#	/*
	#	 * The first 6 syscall args are passed in rdi, rsi, rdx, r10, r8 and r9
	#	 * (rcx gets copied to r10 in the libc stub because the syscall
	#	 * instruction overwrites %cx) and are together in the trap frame
	#	 * with space following for 4 more entries.
	#	 */
	#
	mov $4, %rax		# SYS_write
	mov $1, %rdi
	mov $hello_str, %rsi
	mov hello_len, %dl	# Note: does not clear upper bytes. Use movzxb (move zero extend) for that
	syscall

	mov $1, %rax		# SYS_exit
	xor %rdi, %rdi
	syscall

.section	.data

hello_str:
	.ascii "Hello world!\n"
hello_len:
	.byte . - hello_str

#
# XXX N.B.:  `asm-mode' does not handle `asm-comment-char' very well.  It must be
#	set *before* asm-mode is invoked, which cannot be done with Local Variables!
#
# Local Variables:
# asm-comment-char: ?\#
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn " ; " "as -o " fn ".o " fn ".s && ld -Bstatic -o "fn " /usr/lib/sysident.o " fn ".o && ./" fn))
# End:
#
