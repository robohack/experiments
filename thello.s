# SYSCALL ARGS
# rdi rsi rdx rcx r8 r9

.section	.text

	.globl _start

_start:
	andq $-16, %rsp		# clear the 4 least significant bits of stack pointer to align it

	mov $4, %rax		# SYS_write
	mov $1, %rdi
	mov $hello, %rsi
	mov hello_len, %dl	# Note: does not clear upper bytes. Use movzxb (move zero extend) for that
	syscall

	mov $1, %rax		# SYS_exit
	xor %rdi, %rdi
	syscall

.section .data

hello:
	.ascii "Hello world!\n"
hello_len:
	.byte . - hello

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm " fn " ; " "as -o " fn ".o " fn ".s && ld -Bstatic -o "fn " /usr/lib/sysident.o " fn ".o && ./" fn))
# End:
#
