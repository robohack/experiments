	# ELF note to identify the binary as a native NetBSD program
	##  XXX this could be replaced by linking with /usr/lib/sysident.o
.section ".note.netbsd.ident", "a"
	.long	2f-1f		# length of name (@2: - @1:)
	.long	4f-3f		# length of the description (@4: - @3:)
	.long	1		# "type"
1:	.asciz "NetBSD"		# name
2:	.p2align 2		# padding
3:	.long	400000000	# description (__NetBSD_Version__ from <sys/param.h>)
4:	.p2align 2		# padding

.section	.text
	.globl _start

_start:
	andq $-16, %rsp		# clear the 4 least significant bits of stack pointer to align it
	## The kernel expects the syscall number in rax
	## and the first parameter in rdi
	mov $1, %rdi 		# exit status (1), the first parameter, goes in rdi
	mov $1, %rax 		# syscall ID for SYS_exit, in rax (see <sys/syscall.h>)
	int $0x80		# aka amd64 "syscall"

#
# XXX N.B.:  `asm-mode' does not handle `asm-comment-char' very well.  It must be
#	set *before* asm-mode is invoked, which cannot be done with Local Variables!
#
# Local Variables:
# asm-comment-char: ?\#
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn " ; " "as -o " fn ".o " fn ".s && ld -Bstatic -o "fn " " fn ".o && ./" fn))
# End:
#
