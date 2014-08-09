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
#include "parser.h"
#include "speciali.h"
#include "options.h"

static const char* list_cmd[][2] = {
	/*cmd				description*/
	{"quit","Esce dal programma."},
	{"help","Mostra questa schermata."},
	{"pazienti","Stampa su schermo tutti i pazienti in archivio"},
	{"patologie","Stampa su schermo tutte le patologie in archivio"},
	{"soggetto","Se il soggetto X (identificato dal codice fiscale) non e' gia' monitorato, viene messo sotto monitoraggio.."},
	{"malattia","Se la malattia M (identificata da una stringa univoca) non e' ancora monitorata, viene messa sotto monitoraggio."},
	{"contatto","Se i soggetti X e Y sono sotto monitoraggio, annota che X e Y sono in contatto, e che tale contatto ha valore caratteristico V"},
#ifdef DEBUG
	{"integer","Testa get_integer_cmd()."},
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
	unsigned int c = 'a';
	while(isalnum(c))
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
	const char *arg1, *arg2, *arg3, *arg4;

	memset(arg, 0, CMD_LENGTH);
	len = size_cmd(cmd);
	argX = next_cmd(cmd + len);

	if(!strncmp(cmd, "quit", 4))
		return CMD_EXIT;
	else if(len == 2 && !strncmp(cmd, "q", 1))
		return CMD_EXIT;
	else if(!strncmp(cmd, "help", 4)){
		int i=0;
		printf("# Lista dei comandi #\n");
		while(list_cmd[i][0]){
			printf("%16s | %s\n", list_cmd[i][0], list_cmd[i][1]);
			++i;
		}
	}

	else if(!strncmp(cmd, "soggetto", 8)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		larg = size_cmd(argX);
		if(larg != 17)
			printf("[Errore] Il formato del codice fiscale e' strano (len %ld)\n", larg-1);
		else{
			memcpy(arg, argX, 16); // 16 + space char
			printf("[soggetto] ADD %s\n", arg);
			soggetto(arg, 1);
		}
	}
	
	else if(!strncmp(cmd, "malattia", 8)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		else{
			larg = size_cmd(argX);
			memcpy(arg, argX, (larg >= CMD_LENGTH) ? CMD_LENGTH_CPY : larg-1); // 16 + space char
			printf("[malattia] ADD %s\n", arg);
			malattia(argX, 1);
		}
	}
	
	else if(!strncmp(cmd, "contatto", 7)){
		if(!argX)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		arg1 = argX;
		larg = size_cmd(arg1);
		arg2 = next_cmd(cmd + larg);
		larg = size_cmd(arg2);
		arg3 = next_cmd(cmd + larg);
		if(!arg1 || ! arg2 || arg3)
			printf("[Errore] uno o piu' argomenti non sono validi\n");
		else if(size_cmd(arg1) != 17 || size_cmd(arg2) != 17)
			printf("[Errore] uno (o entrambi) formato/i del codice fiscale e'/sono strano/i (len %ld %ld)\n", size_cmd(arg1)-1, size_cmd(arg2)-1);
		else{
			int v;
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

static void prompt(void){
#ifdef DEBUG
	static char cmd[CMD_LENGTH];
	memset(cmd, 0, CMD_LENGTH);
	size_t len;
#endif

	const char* next = NULL;
	char* buf = NULL;
	size_t buf_len;

	printf(PROMPT);
	while(getline (&buf, &buf_len, stdin)){
#ifdef DEBUG
		next = next_cmd(buf);
		while(next != 0){
			len  = size_cmd(next);
			memcpy(cmd, next, (CMD_LENGTH > len) ? len : CMD_LENGTH_CPY);
			DPRINTF("[CMD IN] (%ld) %s\n", len, cmd);
			next = next_cmd(next+len);
			memset(cmd, 0, CMD_LENGTH);
		}
#endif
		next = next_cmd(buf);
		if(call(next) == CMD_EXIT)
			break;
		printf(PROMPT);
	}
	if(buf)
		free(buf);
}


void console(void){
	prompt();
}
