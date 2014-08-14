#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cf.h"

int main(int argc, char** argv) {
	char *codfisc, alfa, *cogn, *nome;
	int gg,mm,aaaa;
	if(argc != 9)
		return 1;
	if(carica(argv[8])==0) {
		printf("ERRORE: File 'comuni.txt' non trovato o danneggiato!\n");
		return EXIT_FAILURE;
	}
	comune luogoNascita;
	cogn = argv[1];
	nome = argv[2];
	gg = atoi(argv[3]);
	mm = atoi(argv[4]);
	aaaa = atoi(argv[5]);
	alfa = argv[6][0];
//	luogoNascita=ricerca();
	codfisc=cfCod(cogn,nome,gg,mm,aaaa,alfa, argv[7]);
	printf("%s\n",codfisc);
	return 0;
}
