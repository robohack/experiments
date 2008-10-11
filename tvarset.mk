.for c in ${COMPILERS}
via_for_loop+=		${c}
.endfor

# Try to overwrite the variable.
#
COMPILERS=		from the Makefile

# Check the value of the variable.
#
via_shell_2!=		echo ${COMPILERS:Q}
via_assignment_2:=	${COMPILERS}
via_for_loop_2:=	# empty
.for c in ${COMPILERS}
via_for_loop_2+=	${c}
.endfor

# The truth comes out ...
#
all:
	@echo "before:"
	@echo "  via_shell: "${via_shell:Q}
	@echo "  via_assignment: "${via_assignment:Q}
	@echo "  via_for_loop: "${via_for_loop:Q}
	@echo ""
	@echo "after:"
	@echo "  via_shell: "${via_shell_2:Q}
	@echo "  via_assignment: "${via_assignment_2:Q}
	@echo "  via_for_loop: "${via_for_loop_2:Q}
	@echo ""
	@echo "direct use:"
	@echo "  "${COMPILERS:Q}

