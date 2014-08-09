/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "read_file.h"

#include "funzioni.h"
#include "parser.h"
#include "speciali.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fload_malattie(const char* fpath){
	if(!fpath)
		return 1;
	size_t righe = 0;
	FILE* fp = NULL;
	ssize_t read;
	char* line = NULL;
	size_t len = 0;
	const char * t;
	int  v=0, type=0;
	char k[NOME_MALATTIA_MAX_STR];
	size_t l;
	fp = fopen(fpath, "r");
	if(!fp)
		return 1;
	printf("Leggo il file %s...", fpath);
	parser_t* p = NULL;
	while ((read = getline(&line, &len, fp)) != -1){
		if(p)
			csv_destroy(p);
		++righe;
		p = csv_init(line, strlen(line));
		t = csv_next_string(p); /*malattia*/
		type = csv_type(p);
		if(type == PARSER_STR_NULL)
			continue;
		memset(k,0,NOME_MALATTIA_MAX_STR);
		l = csv_string_len(p);
		memcpy(k,t,(l>=NOME_MALATTIA_MAX_STR) ? NOME_MALATTIA_MAX_STR-1 : l);
		/* potrei usare contatto(...., crea=1) ma eseguirlo n volte sarebbe pesante.*/
		v = csv_next_integer(p);
		type = csv_type(p);
		if(t && type != PARSER_LEN_ZERO && type != PARSER_STR_NULL)
			incubazione(k, v, 1);
		else
			malattia(k, 1);
		csv_destroy(p);
		p = NULL;
    }
	
	if(line)
		free(line);

	if(fp)
		fclose(fp);
	printf(" lette %ld righe\n", righe);
	return 0;
}

int fload_pazienti(const char* fpath){
	if(!fpath)
		return 1;
	size_t righe = 0;
	FILE* fp = NULL;
	ssize_t read;
	char * line = NULL;
	size_t len = 0;
	const char * t;
	int n=0, i=0, v=0, type=0;
	char k[NOME_MALATTIA_MAX_STR];
	size_t l;
	fp = fopen(fpath, "r");
	if(!fp)
		return 1;
	printf("Leggo il file %s...", fpath);
	parser_t* p = NULL;
	lista_pazienti* paz = NULL, *cnt = NULL;
	while ((read = getline(&line, &len, fp)) != -1){
		if(p)
			csv_destroy(p);
		++righe;
		p = csv_init(line, strlen(line));
		t = csv_next_string(p); /*codice_fiscale*/
		if(!t || csv_type(p) == PARSER_STR_NULL)
			continue;
		paz = soggetto(t,1);
		t = csv_next_string(p); /*malattia*/
		type = csv_type(p);
		if(type == PARSER_STR_NULL)
			continue;
		if(t && type != PARSER_LEN_ZERO && type != PARSER_STRING_EOF){	
			memset(k,0,NOME_MALATTIA_MAX_STR);
			l = csv_string_len(p);
			memcpy(k,t,(l>=NOME_MALATTIA_MAX_STR) ? NOME_MALATTIA_MAX_STR-1 : l);
			paz->persona->contagiato = malattia(k,1);
		}
		t = csv_next_string(p); /*immunita'*/
		type = csv_type(p);
		if(type == PARSER_STR_NULL)
			continue;
		if(t && type != PARSER_LEN_ZERO && type != PARSER_STRING_EOF){
			memset(k,0,NOME_MALATTIA_MAX_STR);
			l = csv_string_len(p);
			memcpy(k,t,(l>=NOME_MALATTIA_MAX_STR) ? NOME_MALATTIA_MAX_STR-1 : l);
			immune(paz->codice_fiscale, k, 1);
		}
		/* potrei usare contatto(...., crea=1) ma eseguirlo n volte sarebbe pesante.*/
		n = csv_next_integer(p);
		type = csv_type(p);
		if(type != PARSER_LEN_ZERO && type != PARSER_STR_NULL && type != PARSER_STRING_EOF){
			if(n<0) // se accate questo, e' terribile
				continue;
			paz->persona->n_contatti = n;
			paz->persona->catena = (contatti**) malloc(sizeof(contatti*)*n);
			for(i=0; i<n; ++i){
				paz->persona->catena[i] = (contatti*) malloc(sizeof(contatti));
				paz->persona->catena[i]->persona = NULL;
				paz->persona->catena[i]->v = NO_CONTATTO;
			}
		}else continue;
		i = 0;
		t = csv_next_string(p); /*contatti (codice_fiscale) manca un pezzo. collegamento valori + ordinamento*/
		type = csv_type(p);
		while( (type != PARSER_STR_NULL && type != PARSER_LEN_ZERO && type != PARSER_STRING_EOF ) && i<n){
			cnt = soggetto(t,1);
			if(cnt == paz){
				csv_next_string(p);
				type = csv_type(p);
				continue;
			}
			paz->persona->catena[i]->persona = cnt;
			v = csv_next_integer(p);
			type = csv_type(p);
			if(type == PARSER_STR_NULL && type == PARSER_LEN_ZERO)
				break;
			t = csv_next_string(p);
			type = csv_type(p);
			contatti *c = (contatti*) malloc(sizeof(contatti));
			paz->persona->catena[i]->v = v;
			c->v = v;
			c->persona = paz;
			if(!cnt->persona->catena){
				cnt->persona->n_contatti = 1;
				cnt->persona->catena = (contatti**) malloc(sizeof(contatti*));
				cnt->persona->catena[0] = c;
			}else{
				int j = 0;
				compare_t* cmp = NULL;
				for(j=0; j<cnt->persona->n_contatti; ++j){
					cmp = compare(cnt->persona->catena[j]->persona->codice_fiscale, paz->codice_fiscale, 16);
					if(cmp){
						if(cmp->ret < 0)
							break;
						free(cmp);
						cmp = NULL;
					}else
						break;
				}
				if(cmp){
					cnt->persona->catena = (contatti**) realloc(cnt->persona->catena, sizeof(contatti*)*(cnt->persona->n_contatti+1)); // Non funziona, why?
					int y;
					for(y=cnt->persona->n_contatti+1; y>j; --y){
						cnt->persona->catena[y] = cnt->persona->catena[y-1];
					}
					cnt->persona->catena[j] = c;
					++cnt->persona->n_contatti;
				}else if(!cnt->persona->catena[j] || cnt->persona->catena[j]->persona != paz){
					cnt->persona->catena[j] = c;
					++cnt->persona->n_contatti;
				}
				if(cmp)
					free(cmp);
			}
			++i;
		}
		if(i<n){
			paz->persona->n_contatti = i;
			for(; i<n; ++i)
				free(paz->persona->catena[i]);
			paz->persona->catena = (contatti**) realloc(paz->persona->catena, sizeof(contatti*)*i);
		}
		n = 0;
		csv_destroy(p);
		p = NULL;
    }

	if(line)
		free(line);

	if(fp)
		fclose(fp);
	printf(" lette %ld righe\n", righe);
	return 0;
}
