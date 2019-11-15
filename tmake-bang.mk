#
# tests related to http://austingroupbugs.net/view.php?id=337
#

bar != echo bar#baz

# Presume ASCII or UTF-8:
hash != printf '\043'
barbaz != echo bar$(hash)baz

XYZZY = fiddle-dee-dee
dollar = $$
VAR3 != printf '%s' '$(dollar)(XYZZY)'
GVAR3 = $(shell printf '%s' '$(dollar)XYZZY')

all:
	echo 'bar=<${bar}>'
	echo 'barbaz=<${barbaz}>'
	@printf '%s\n' 'VAR3=<$(VAR3)>'
	@printf '%s\n' 'GVAR3=<$(GVAR3)>'
#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-nondirectory (buffer-file-name)))) (concat (default-value 'compile-command) " -f " fn))
# End:
#
