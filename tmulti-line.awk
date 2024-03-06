#!/usr/bin/awk -f

#     A pattern may consist of two patterns separated by a comma; in this case,
#     the action is performed for all lines from an occurrence of the first
#     pattern though an occurrence of the second.

# For example the following will print just the headers of all the messages in
# an "mbox" file:

'/^From /, /^$/'

# Local Variables:
# eval: (make-local-variable 'compile-command)
# compile-command: (concat "awk -f " (file-name-nondirectory (buffer-file-name)) " /var/mail/)
# End:
