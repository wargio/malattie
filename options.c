/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "options.h"

#include <unistd.h>
#include <getopt.h>
#include <stdio.h>

const char* fpath_pazienti  = NULL;
const char* fpath_malattie  = NULL;
const char* fpath_wpazienti = NULL;
const char* fpath_wmalattie = NULL;
int         console = 0;
int         write_flag = 0;
int         read_flag = 0;

static struct option options[] = {
	{"help",      no_argument,       NULL, 'h'},
	{"nuovo",     no_argument,       NULL, 'n'},
	{"console",     no_argument,     NULL, 'c'},
	{"pazienti",  required_argument, NULL, 'p'},
	{"malattie",  required_argument, NULL, 'm'},
	{"wpazienti", required_argument, NULL, 'k'},
	{"wmalattie", required_argument, NULL, 'l'},
	{NULL, no_argument, NULL, 0}
};

void usage(void)
{
	printf("utilizzo: malattie [opzioni] parametri\n");
	printf("      Opzione          Parametro    Descrizione\n");
	printf("      -h | --help                   Visualizza questa descrizione\n");
	printf("      -n | --nuovo                  Genera una nuova sessione (non c'e' bisogno dell'opzione -c).\n");
	printf("      -c | --console                Avvia  una sessione se si caricano dei file.\n");
	printf("      -p | --pazienti  file.csv     Legge  un CSV con le informazioni dei pazienti\n");
	printf("      -m | --malattie  file.csv     Legge  un CSV con le informazioni sulle malattie\n");
	printf("      -k | --wpazienti file.csv     Scrive un CSV con le informazioni dei pazienti\n");
	printf("      -l | --wmalattie file.csv     Scrive un CSV con le informazioni sulle malattie\n");
}

void pargs(int argc, char **argv)
{
	char c;
	while((c = getopt_long(argc, argv, "hcnp:m:k:l:", options, NULL)) != -1){
		switch(c){
			case 'n':
				read_flag = 1;
				write_flag = 1;
				console = 1;
				fpath_pazienti = NULL;
				fpath_malattie = NULL;
				return;
			case 'h':
				usage();
				return;
			case 'p':
				fpath_pazienti = optarg;
				++read_flag;
				break;
			case 'm':
				fpath_malattie = optarg;
				++read_flag;
				break;
			case 'k':
				fpath_wpazienti = optarg;
				++write_flag;
				break;
			case 'l':
				fpath_wmalattie = optarg;
				++write_flag;
				break;
			case 'c':
				console = 1;
				break;
		}
	}
	if(!write_flag && !read_flag && !console)
		printf("Scrivere './malattie --help' per ulteriori informazioni.\n");
}
