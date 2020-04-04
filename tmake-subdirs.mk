#!/usr/bin/make -f
#
# provide aliases for subdirectory targets
#
SUBDIR= mod_x1 mod_x2 tex_x3 rmd_x4

.include <bsd.own.mk>

.if !defined(MAKEDIRTARGET)
# Only NetBSD has MAKEDIRTARGET by default...
MAKEDIRTARGETENV?=
MAKEDIRTARGET=\
	@_makedirtarget() { \
		dir="$$1"; shift; \
		target="$$1"; shift; \
		case "$${dir}" in \
		/*)	this="$${dir}/"; \
			real="$${dir}" ;; \
		.)	this="${_THISDIR_}"; \
			real="${.CURDIR}" ;; \
		*)	this="${_THISDIR_}$${dir}/"; \
			real="${.CURDIR}/$${dir}" ;; \
		esac; \
		show=$${this:-.}; \
		echo "$${target} ===> $${show%/}$${1:+	(with: $$@)}"; \
		cd "$${real}" \
		&& ${MAKEDIRTARGETENV} ${MAKE} _THISDIR_="$${this}" "$$@" $${target}; \
	}; \
	_makedirtarget
.endif

# if the command-line target is "xN" then make just in the matching "DIR_xN"
# subdirectory
#
.for dir in ${SUBDIR}
${dir:C/^.*_//}: ${dir} .PHONY
	${MAKEDIRTARGET} ${dir} all
.endfor

.include <bsd.subdir.mk>
