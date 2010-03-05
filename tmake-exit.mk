all: all-1 all-2

all-1:
	true;	\
	false;	\
	true;	\
	echo "you should not see this! (but you likely will)"

all-2:
	true;
	false;
	true;
	echo "you should not see this either!"
