#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/times.h>

#define BLOCK_SIZE	64		/* Tamanho maximo do salto entre bytes */
#define GRANDE		800000		/* Tamanho do array a ler da
					 * memoria. Tem de ser maior que a
					 * cache, para o programa funcionar */
/*
 * Funcao que devolve o numero de tiques de relogio
 *
 * OUT: Numero de tiques de relogio do sistema
 */
double
get_tick()
{
	struct tms rusage;

	times(&rusage);

	return (double) (rusage.tms_utime);
}

/*
 * Programa principal
 * Le um array grande da memoria, aumentando o salto entre bytes de modo a
 * detectar o tamanho dos blocos da cache. O numero de leituras na memoria e
 * sempre o mesmo.
 */
int
main()
{
	double tick0;
	double tick1;		/* Utilizado para guardar o numero de tiques de relogio */
	int i;
        int n;
        int z;			/* Utilizado nos ciclos for */
	char x[GRANDE];		/* Array utilizado para ler a memoria por blocos */
	char dummy;		/* Apenas utilizado na leitura de x[] */

	/* Para saltos entre bytes de memoria de 1 a BLOCK_SIZE bytes */
	for (i = 1; i < BLOCK_SIZE; i++) {
		/* Numero de tiques antes da operacao de leitura */
		tick0 = get_tick();
		/* Multiplica o numero de leituras de x[] por i, de modo a
		 * serem sempre lidos o mesmo numero de bytes */
		for (z = 0; z < i; z++) {
			/* Ler o array todo */
			for (n = 0; n < GRANDE; n += i)
				dummy = x[n];

		}
		/* Numero de tiques depois da operacao de leitura */
		tick1 = get_tick();

		/* Imprime no ecran o tempo que demorou (tiques de relogio) */
		printf("i:%d - ticks:%f ", i, tick1 - tick0);
	}

	exit(0);
	/* NOTREACHED */
}
