#
# demo that .PATH has to exist before prerequistes are found therein
#
OBJDIR = objs

.PATH: $(OBJDIR)

all: $(OBJDIR) .WAIT test

# note the first time around .PATH won't be effective
#
test: a.o b.o   # simulate linking
	@echo Linking target: $@, prerequisites: $>

.SUFFIXES: .c .o
.c.o:
	gcc -o $(OBJDIR)/$@ -c $<

$(OBJDIR):
	-mkdir $@

#
# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (let ((fn (file-name-nondirectory (buffer-file-name)))) (concat "rm -rf objs; touch a.c b.c; make -f " fn " && echo '# now make again:' && make -f " fn "; rm -rf objs a.c b.c"))
# End:
#
