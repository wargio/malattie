/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "comando.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "funzioni.h"
#include "speciali.h"
#include "parser.h"
#include "speciali.h"
#include "options.h"
#include "rw_file.h"

static const char* list_cmd[][2] = {
	/*cmd, description*/
	{"quit","Esce dal programma."},
	{"help","Mostra questa schermata."},
	{"pazienti","Stampa su schermo tutti i pazienti in archivio"},
	{"patologie","Stampa su schermo tutte le patologie in archivio"},
	{"salva","Salva in un formato CSV la lista delle patologie e/o la lista dei pazienti"},
	{"soggetto","Se il soggetto X (identificato dal codice fiscale) non e' gia' monitorato, viene messo sotto monitoraggio.."},
	{"malattia","Se la malattia M (identificata da una stringa univoca) non e' ancora monitorata, viene messa sotto monitoraggio."},
	{"contatto","Se i soggetti X e Y sono sotto monitoraggio, annota che X e Y sono in contatto, e che tale contatto ha valore caratteristico V"},
	{"area","Se il soggetto X e' sotto monitoraggio, stampa tutti i soggetti sotto monitoraggio che si trovano nell'area di contagio di X."},
	{"incubazione","Se il soggetto X e' sotto monitoraggio, stampa tutti i soggetti sotto monitoraggio che si trovano nell'area di contagio di X."},
	{"immune","Se la malattia M e il soggetto X sono sotto monitoraggio, annota che X e' immune alla malattia M."},
	{"valore","Se i soggetti X e Y sono sotto monitoraggio ed esiste una catena di contatti fra loro, calcola il valore di contagio tra X e Y."},
	{"untore","Se i soggetti X e Y sono sotto monitoraggio, stabilisce se esiste un untore tra X e Y e ne stampa il codice fiscale."},
#ifdef DEBUG
	{"integer","Testa get_integer_cmd(). [DEBUG]"},
#endif
	{NULL, NULL},
};

const char* next_cmd(const char* next){
	if(!next)
		return NULL;
	size_t i=0;
	unsigned int c = next[i];
	while(c != '-' && c != '\0' && c != '\n' && !isalnum(c))
		c = next[++i];
	if(c == '\0' || c == '\n')
		return NULL;
	return next + i;
}

size_t size_cmd(const char* cmd){
	if(!cmd)
		return 0;
	size_t i=0;
	unsigned int c = cmd[i++];
	while(isgraph(c))
		c = cmd[i++];
	return i;
}

long int get_integer_cmd(const char* next){
	long int X = 0;
	if(!next)
		return X;
	if(sscanf(next, " %ld", &X) != 1)
		return 0;
	return X;
}

int call(const char* cmd){
	static char arg[CMD_LENGTH];
	size_t len;
	size_t larg;
	const char* argX;
	const char *arg1 = NULL, *arg2 = NULL, *arg3 = NULL; //, *arg4 = NULL;
	int v;

	memset(arg, 0, CMD_LENGTH);
	len = size_cmd(cmd);
	argX = next_cmd(cmd + len);

	if(!strncmp(cmd, "quit", 4))
		return CMD_EXIT;
#ifdef DEBUG
	else if(len == 2 && !strncmp(cmd, "q", 1))
		return CMD_EXIT;
#endif
	else if(!strncmp(cmd, "help", 4)){
		int i=0;
		printf("# Lista dei comandi #\n");
		while(list_cmd[i][0]){
			printf("%16s | %s\n", list_cmd[i][0], list_cmd[i][1]);
			++i;
		}
	}

	else if(!strncmp(cmd, "salva", 5)){
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(arg1 + larg);
		larg = size_cmd(arg2);

		if(!arg1 || !arg2)
			printf("[Errore] uno o piu' argomenti non sono validi\n[Utilizzo] salva <pazienti/malattie> <nome_file.csv>\n");
		else{
			larg = size_cmd(arg1);
			memcpy(arg, arg1, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); // larg + space char
			if(!strncmp(arg, "pazienti", 5)){
				larg = size_cmd(arg2);
				memcpy(arg, arg2, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); // larg + space char
				printf("[salva] pazienti %s\n", arg);
				fwrite_pazienti(arg);
			}else if(!strncmp(arg, "malattie", 5)){
				larg = size_cmd(arg2);
				memcpy(arg, arg2, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); // larg + space char
				printf("[salva] malattie %s\n", arg);
				fwrite_malattie(arg);
			}else
				printf("[Errore] salva accetta solo 'pazienti' o 'malattie'\n[Utilizzo] salva <pazienti/malattie> <nome_file.csv>\n");
		}
	}

	else if(!strncmp(cmd, "soggetto", 8)){
		larg = size_cmd(argX);
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		else if(larg != 17)
			printf("[Errore] Il formato del codice fiscale e' strano (len %ld)\n", larg-1);
		else{
			memcpy(arg, argX, 16); // 16 + space char
			printf("[soggetto] ADD %s\n", arg);
			soggetto(argX, 1);
		}
	}
	
	else if(!strncmp(cmd, "malattia", 8)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		else{
			larg = size_cmd(argX);
			memcpy(arg, argX, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); // larg + space char
			printf("[malattia] ADD %s\n", arg);
			malattia(argX, 1);
		}
	}
	
	else if(!strncmp(cmd, "contatto", 7)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(arg1 + larg);
		larg = size_cmd(arg2);
		arg3 = next_cmd(arg2 + larg);
		if(!arg1 || !arg2 || !arg3)
			printf("[Errore] uno o piu' argomenti non sono validi (%p %p %p)\n", arg1, arg2, arg3);
		else if(size_cmd(arg1) != 17 || size_cmd(arg2) != 17)
			printf("[Errore] uno o entrambi i formati del codice fiscale sono strani (len %ld %ld)\n", size_cmd(arg1)-1, size_cmd(arg2)-1);
		else{
			memcpy(arg, arg1, 16); // 16 + space char
			printf("[contatto] %s", arg);
			memcpy(arg, arg2, 16); // 16 + space char
			printf(" <-> %s", arg);
			v = get_integer_cmd(arg3);
			printf(" = %d\n", v);
			contatto(arg1, arg2, v, 0);
		}
	}

	else if(!strncmp(cmd, "pazienti", 8))
		print_pazienti();

	else if(!strncmp(cmd, "patologie", 9))
		print_malattie();

	else if(!strncmp(cmd, "area", 4)){
		larg = size_cmd(argX);
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		else if(larg != 17)
			printf("[Errore] Il formato del codice fiscale e' strano (len %ld)\n", larg-1);
		else{
			larg = size_cmd(argX);
			memcpy(arg, argX, 16); // 16 + space char
			printf("[area] %s\n", arg);
			area(arg);
		}
	}

	else if(!strncmp(cmd, "incubazione", 11)){
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(arg1 + larg);
		if(!arg1 || !arg2)
			printf("[Errore] uno o piu' argomenti non sono validi (%p %p)\n", arg1, arg2);
		else{
			v = get_integer_cmd(arg2);
			memcpy(arg, arg1, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); // larg + space char
			printf("[incubazione] %s %d\n", arg, v);
			incubazione(arg, v, 0);
		}
	}

	else if(!strncmp(cmd, "immune", 6)){
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(arg1 + larg);
		if(!arg1 || !arg2)
			printf("[Errore] uno o piu' argomenti non sono validi (%p %p)\n", arg1, arg2);
		else if(larg != 17)
			printf("[Errore] Il formato del codice fiscale e' strano (len %ld)\n", larg-1);
		else{
			memcpy(arg, arg1, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); // larg + space char
			printf("[immune] %s ", arg);
			memset(arg, 0, larg);
			larg = size_cmd(arg2);
			memcpy(arg, arg2, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); // larg + space char
			printf("%s\n", arg);
			immune(arg1, arg, 0);
		}
	}

	else if(!strncmp(cmd, "valore", 6)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(arg1 + larg);

		if(!arg1 || !arg2)
			printf("[Errore] uno o piu' argomenti non sono validi (%p %p %p)\n", arg1, arg2, arg3);
		else if(size_cmd(arg1) != 17 || size_cmd(arg2) != 17)
			printf("[Errore] uno o entrambi i formati del codice fiscale sono strani (len %ld %ld)\n", size_cmd(arg1)-1, size_cmd(arg2)-1);
		else{
			memcpy(arg, arg1, 16); // 16 + space char
			printf("[valore] %s", arg);
			memcpy(arg, arg2, 16); // 16 + space char
			printf(" <=> %s\n", arg);
			valore(arg1, arg2);
		}
	}

	else if(!strncmp(cmd, "untore", 6)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(arg1 + larg);

		if(!arg1 || !arg2)
			printf("[Errore] uno o piu' argomenti non sono validi (%p %p %p)\n", arg1, arg2, arg3);
		else if(size_cmd(arg1) != 17 || size_cmd(arg2) != 17)
			printf("[Errore] uno o entrambi i formati del codice fiscale sono strani (len %ld %ld)\n", size_cmd(arg1)-1, size_cmd(arg2)-1);
		else{
			memcpy(arg, arg1, 16); // 16 + space char
			printf("[untore] '%s'", arg);
			memcpy(arg, arg2, 16); // 16 + space char
			printf(" '%s'\n", arg);
			untore(arg1, arg2);
		}
	}

	else if(!strncmp(cmd, "maggiore", 8))
		maggiore();

	else if(!strncmp(cmd, "tempo", 5)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(arg1 + larg);
		larg = size_cmd(arg2);
		arg3 = next_cmd(arg2 + larg);
		if(!arg1 || !arg2 || !arg3)
			printf("[Errore] uno o piu' argomenti non sono validi (%p %p %p)\n", arg1, arg2, arg3);
		else if(size_cmd(arg1) != 17 || size_cmd(arg2) != 17)
			printf("[Errore] uno o entrambi i formati del codice fiscale sono strani (len %ld %ld)\n", size_cmd(arg1)-1, size_cmd(arg2)-1);
		else{
			memcpy(arg, arg1, 16); // 16 + space char
			printf("[tempo] %s", arg);
			memcpy(arg, arg2, 16); // 16 + space char
			printf(" %s", arg);
			larg = size_cmd(arg3);
			memset(arg, 0, CMD_LENGTH_CPY);
			memcpy(arg, arg3, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); 
			printf(" %s\n", arg);
			tempo(arg1, arg2, arg);
		}
	}
	
	else if(!strncmp(cmd, "vaccinazione", 12)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(arg1 + larg);
		larg = size_cmd(arg2);
		arg3 = next_cmd(arg2 + larg);
		if(!arg1 || !arg2 || !arg3)
			printf("[Errore] uno o piu' argomenti non sono validi (%p %p %p)\n", arg1, arg2, arg3);
		else{
			int n = get_integer_cmd(arg1);
			if(n>0){
				int j;
				int* pos = (int*) malloc(sizeof(int)*n);
				for(j=0; j<n && arg3; ++j){
					pos[j] = get_integer_cmd(arg3);
					DPRINTF("[vaccinazione] %d\n", pos[j]);
					larg = size_cmd(arg3);
					arg3 = next_cmd(arg3 + larg);
				}
				vaccinazione(j, get_integer_cmd(arg2), pos);
				free(pos);
			}
		}
	}

#ifdef DEBUG
	else if(!strncmp(cmd, "integer", 7))
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		else
			printf("[integer] %ld\n", get_integer_cmd(argX));
#endif
	else{
		printf("[Errore] Comando non trovato.\n[Errore] Scrivi 'help' per la lista dei comandi accettati\n");
		return CMD_NOT_FOUND;
	}
	return 0;
}

void prompt(void){

	call("vaccinazione 10 5 1 2 3 6 50 9 11 22 44 7\n");
	return;
	const char* next = NULL;
	char* buf = NULL;
	size_t buf_len;

	printf(PROMPT);
	while(getline (&buf, &buf_len, stdin)){
		if(buf[0] == '\n' || buf[0] == '\0'){
			printf(PROMPT);
			continue;
		}
		next = next_cmd(buf);
		if(call(next) == CMD_EXIT)
			break;
		printf(PROMPT);
	}
	if(buf)
		free(buf);
}

