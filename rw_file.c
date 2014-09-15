/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "rw_file.h"

#include "funzioni.h"
#include "parser.h"
#include "speciali.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern lista_pazienti* pazienti;
extern lista_malattie* malattie;
extern uint32_t n_pazienti;
extern uint32_t n_malattie;

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
		/* potrei usare contatto(...., crea=1) ma eseguirlo n volte sarebbe pesante.*/
		v = csv_next_integer(p);
		type = csv_type(p);
		if(t && type != PARSER_LEN_ZERO && type != PARSER_STR_NULL)
			incubazione(t, v, 1);
		else
			malattia(t, 1);
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
// non funziona ancora correttamente
/*
dal file non funzia
area VLNBSL52A14E391H
[area] VLNBSL52A14E391H
Contatti di VLNBSL52A14E391H
|PRSMRG10M05I057Q|1|
manca BLLNTN75M09F965F
----------------------------------------
da quello generato dal programma si.
> area VLNBSL52A14E391H
[area] VLNBSL52A14E391H
Contatti di VLNBSL52A14E391H
|PRSMRG10M05I057Q|1|
|BLLNTN75M09F965F|8|

*/
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
				//memset(paz->persona->catena[i], 0, sizeof(contatti));
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

			contatti *c = (contatti*) malloc(sizeof(contatti));
			paz->persona->catena[i]->v = v;
			c->v = v;
			c->persona = paz;
			
			if(cnt->persona->n_contatti != 0){
				int y;
				compare_t* cmp = NULL;
				for(y = 0; y<cnt->persona->n_contatti; y++){
					cmp = compare(cnt->persona->catena[y]->persona->codice_fiscale, paz->codice_fiscale, 16);
					if(cmp){
						if(cmp->ret == 0)
							break;
						if(cmp->ret < 0)
							break;
					}else
						break;
					
					free(cmp);
					cmp = NULL;
				}
				
				if(!cmp && y>=cnt->persona->n_contatti){
					cnt->persona->catena = (contatti**) realloc(cnt->persona->catena, sizeof(contatti*)*(cnt->persona->n_contatti+1));
					cnt->persona->catena[cnt->persona->n_contatti] = c;
					++cnt->persona->n_contatti;
				}else if(cmp){
					if(cmp->ret < 0){
						cnt->persona->catena = (contatti**) realloc(cnt->persona->catena, sizeof(contatti*)*(cnt->persona->n_contatti+1));
						int j;
						for(j=cnt->persona->n_contatti; j>y; j--)
							cnt->persona->catena[j] = cnt->persona->catena[j-1];
						cnt->persona->catena[y] = c;
						++cnt->persona->n_contatti;
					}else free(c);
				}
				
				if(cmp)
					free(cmp);
			}else{
				cnt->persona->catena = (contatti**) malloc(sizeof(contatti*));
				cnt->persona->catena[0] = c;
				cnt->persona->n_contatti = 1;
			}
			t = csv_next_string(p);
			type = csv_type(p);
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

void fwrite_malattie(const char* fpath){
	if(!fpath || !malattie){
		printf("ERRORE: Non sono riuscito a scrivere %s(%s)\n", (fpath) ? fpath : "", (fpath) ? "lista malattie vuota" : "percorso non valido");
		return;
	}
	lista_malattie *ptr = malattie;
	char* my_space = (char*) malloc(1024);
	memset(my_space, 0, 1024);
	parser_t* ctx = csv_init(my_space, 1024);
	csv_open(fpath, ctx);
	DPRINTF("[fwrite_malattie] [%s]\n", fpath);
	while(ptr != NULL){
		csv_compose_string(ctx, ptr->nome);
		csv_compose_integer(ctx, ptr->t);
		csv_write(ctx);
		DPRINTF("[fwrite_malattie] < %s\n", my_space);
		csv_reset_compose(ctx);
		ptr = ptr->next;
	}
	csv_close(ctx);
	csv_destroy(ctx);
	free(my_space);
	printf("%s scritto\n", fpath);
}

void fwrite_pazienti(const char* fpath){
	if(!fpath || !pazienti){
		printf("ERRORE: Non sono riuscito a scrivere %s(%s)\n", (fpath) ? fpath : "", (fpath) ? "lista pazienti vuota" : "percorso non valido");
		return;
	}
	lista_pazienti *ptr = pazienti;
	char* my_space = (char*) malloc(1024);
	memset(my_space, 0, 1024);
	parser_t* ctx = csv_init(my_space, 1024);
	csv_open(fpath, ctx);
	DPRINTF("[fwrite_pazienti] [%s]\n", fpath);
	while(ptr != NULL){
		csv_compose_string(ctx, ptr->codice_fiscale);
		if(ptr->persona->contagiato)
			csv_compose_string(ctx, ptr->persona->contagiato->nome);
		else
			csv_compose_string(ctx, NULL);
		if(ptr->persona->immune)
			csv_compose_string(ctx, ptr->persona->immune->nome);
		else
			csv_compose_string(ctx, NULL);
		if(ptr->persona->n_contatti){
			csv_compose_integer(ctx, ptr->persona->n_contatti);
			int i, r;
			for(i=0, r=PARSER_OK; r == PARSER_OK && i<ptr->persona->n_contatti; ++i){
				r = csv_compose_string(ctx, ptr->persona->catena[i]->persona->codice_fiscale);
				if(r != PARSER_OK)
					break;
				r = csv_compose_integer(ctx, ptr->persona->catena[i]->v);
			}
		}
		csv_write(ctx);
		DPRINTF("[fwrite_pazienti] < %s\n", my_space);
		csv_reset_compose(ctx);
		ptr = ptr->next;
	}
	csv_close(ctx);
	csv_destroy(ctx);
	free(my_space);
	printf("%s scritto\n", fpath);
}
