c
c hello world in FORTRAN(77)
c
      program hello
        implicit none
        write(*,*) 'Hello world!'
      end

c
c Local Variables:
c eval: (make-local-variable 'compile-command)
c compile-command: (let ((fn (file-name-sans-extension (file-name-nondirectory (buffer-file-name))))) (concat "rm -f " fn "; " (default-value 'compile-command) " " fn " FC=fort77 CPPFLAGS=-I/usr/pkg/include LDFLAGS=-L/usr/pkg/lib && ./" fn))
c End:
c
