	#
	#	thello.s: "Hello world!" in x86_64 assembly language
	#
	# this example was mostly stolen from:  https://polprog.net/blog/netbsdasmprog/
	#
.section	.text

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
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn " ; " "as -o " fn ".o " fn ".s && ld -Bstatic -o "fn " /usr/lib/sysident.o " fn ".o && ./" fn))
# End:
#
