#! /bin/sh

if [ $# = 0 ]; then
	set --	/usr/share/dict/american \
		/usr/share/dict/british \
		/usr/share/dict/eign \
		/usr/share/dict/propernames \
		/usr/share/dict/stop \
		/usr/share/dict/web2 \
		/usr/share/dict/web2a \
		/usr/share/dict/words \
		/usr/share/dict/words.cracklib
fi

echo $*

# hmmmm.... this is unexpected:  all shells evaluate ${$#} as $$

eval echo the last parameter is: \${$#}
