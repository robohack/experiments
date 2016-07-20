#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
	void* data;
	int weight;
} item;

uint64_t subsum(item *, unsigned int);
int main(int, char *[]);

uint64_t
subsum(item *x,
       unsigned int n)
{
	unsigned int i;
	int j, w, from, to, step, pos = 0, neg = 0;
	uint64_t bit, *buf;

	for (i = 0; i < n; i++) {
		if (x[i].weight >= 0) {
			pos += x[i].weight;
		} else {
			neg += x[i].weight;
		}
	}

	buf = calloc((unsigned int) (pos - neg) + 1, sizeof(uint64_t));
	buf -= neg;

	for (i = 0; i < n; i++) {
		w = x[i].weight;
		bit = (uint64_t)1 << i;

		if (w < 0) {
			from = neg, to = pos + 1, step = 1;
		} else {
			from = pos, to = neg - 1, step = -1;
		}

		for (j = from; j != to; j += step) {
			if (buf[j]) {
				buf[j + w] = buf[j] | bit;
			}
		}
		buf[w] = bit;

		if (buf[0]) {
			break;
		}
	}

	bit = buf[0];
	free(buf + neg);

	return bit;
}

int
main(int argc,
     char *argv[])
{
	item em[] = {
		{"alliance",	-624},	{"archbishop",	-915}, 	{"balm",	 397},
		{"bonnet",	 452},	{"brute",	 870},	{"centipede",	-658},
		{"cobol",	 362},	{"covariate",	 590},	{"departure",	 952},
		{"deploy",	  44},	{"diophantine",	 645},	{"efferent",	  54},
		{"elysee",	-326},	{"eradicate",	 376},	{"escritoire",	 856},
		{"exorcism",	-983},	{"fiat",	 170},	{"filmy",	-874},
		{"flatworm",	 503},	{"gestapo",	 915},	{"infra",	-847},
		{"isis",	-982},	{"lindholm",	 999},	{"markham",	 475},
		{"mincemeat",	-880},	{"moresby",	 756},	{"mycenae",	 183},
		{"plugging",	-266},	{"smokescreen",	 423},	{"speakeasy",	-745},
		{"vein",	 813}
	};

	unsigned int i;
	uint64_t v, ret = subsum(em, sizeof(em)/sizeof(em[0]));

	if (argc != 2) {
		printf("usage: %s N\n", getprogname());
		exit(1);
	}

	if (ret == (uint64_t) atoi(argv[1])) {
		printf("no sums match %d\n", atoi(argv[1]));
		exit(1);
	}

	printf("Found a subset with a sum of %d:", atoi(argv[1]));
	for (i = 0; i < 64; i++) {
		v = (uint64_t) 1 << i;
		if (ret & v) {
			printf("%2u | %5d %s\n", i, em[i].weight, (char*)em[i].data);
		}
	}

	exit(0);
}
