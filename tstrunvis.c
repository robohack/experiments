#include <sys/types.h>
#include <vis.h>

main()
{
	char foo[100];

	strunvis(foo, "abcdef");
}
