/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "funzioni.h"
#include "speciali.h"
#include "parser.h"

#include <assert.h>

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static lista_pazienti* pazienti = NULL; // dovrebbe essere già a 0, ma per sicurezza mettiamo a NULL
static lista_malattie* malattie = NULL; // dovrebbe essere già a 0, ma per sicurezza mettiamo a NULL

static uint32_t n_pazienti = 0;
static uint32_t n_malattie = 0;

/*
typedef struct _lista_malattie{
	const char* 	nome;			// nome
	int t;							// tempo di incubazione
	lista_malattie* next;			// malattia successiva
} lista_malattie;

typedef lista_malattie malattia;

*/

lista_malattie* genera_malattia(const char* nome){
	lista_malattie* ptr = (lista_malattie*) malloc(sizeof(lista_malattie));
	memset(ptr, 0, sizeof(lista_malattie));
	size_t len = sizeof(char)*strlen(nome)+1;
	char* txt = (char*) malloc(len);
	memset(txt, 0, len);
	--len;
	ptr->nome = (const char*) txt;
	memcpy(txt, nome, len);
	to_lower(txt, len);
	++n_malattie;
	return ptr;
}

lista_malattie* malattia(const char* nome_malattia, int crea){
	lista_malattie* ptr = NULL;
	if(malattie == NULL && crea){
		ptr = malattie = genera_malattia(nome_malattia);
		ptr->t = NO_T;
	}else if(malattie){
		char k[NOME_MALATTIA_MAX_STR];
		memset(k,0,NOME_MALATTIA_MAX_STR);
		size_t len = strlen(nome_malattia);
		len = (len>=NOME_MALATTIA_MAX_STR) ? NOME_MALATTIA_MAX_STR-1 : len;
		memcpy(k,nome_malattia,len);
		to_lower(k, len);
		lista_malattie *old;
		compare_t* cmp = NULL;
		ptr = malattie;
		old = NULL;
		while(ptr != NULL){
			cmp = compare(ptr->nome, k, len);
			if(cmp){
				if(cmp->ret < 0)
					break;
				free(cmp);
				cmp = NULL;
			}else
				return ptr;
			old = ptr;
			ptr = ptr->next;
		}
		if(crea){
			if(old == NULL){
				old = malattie;
				malattie = genera_malattia(nome_malattia);
				malattie->next = old;
				ptr = malattie;
			}else{
				old->next = genera_malattia(nome_malattia);
				old->next->next = ptr;
				ptr = old->next;
			}
			ptr->t = NO_T;
		}else
			ptr = NULL;
		if(cmp)
			free(cmp);
	}
	return ptr;
}


lista_pazienti* genera_paziente(const char* codice_fiscale){
	paziente* 		new = (paziente*) malloc(sizeof(paziente));
	lista_pazienti* ptr = (lista_pazienti*) malloc(sizeof(lista_pazienti));
	memset(ptr, 0, sizeof(lista_pazienti));
	memset(new, 0, sizeof(paziente));
	memcpy(ptr->codice_fiscale, codice_fiscale, 16); 
	ptr->persona = new;
	++n_pazienti;
	return ptr;
}

lista_pazienti* soggetto(const char* codice_fiscale, int crea){
	lista_pazienti* ptr = NULL;
	if(pazienti == NULL){
		if(crea)
			pazienti = genera_paziente(codice_fiscale);
		ptr = pazienti;
	}else{
		lista_pazienti* old = NULL;
		ptr = pazienti;
		compare_t* cmp = NULL;
		while(ptr != NULL){
			cmp = compare(ptr->codice_fiscale, codice_fiscale, 16);
			if(cmp){
				if(cmp->ret < 0)
					break;
				free(cmp);
				cmp = NULL;
			}else
				return ptr;
			old = ptr;
			ptr = ptr->next;
		}
		if(crea){
			if(old == NULL){
				old = pazienti;
				pazienti = genera_paziente(codice_fiscale);
				pazienti->next = old;
				ptr = pazienti;
			}else{
				old->next = genera_paziente(codice_fiscale);
				old->next->next = ptr;
				ptr = old->next;
			}
		}else
			ptr = NULL;
		if(cmp)
			free(cmp);
	}
	return ptr;
}

void contatto(const char* codice_fiscale0, const char* codice_fiscale1, int v, int crea){
	if(!codice_fiscale0 || !codice_fiscale1 || codice_fiscale0 == codice_fiscale1)
		return;

	compare_t* cmp = NULL;
	lista_pazienti* p0, *p1;
	p0 = p1 = NULL;

	p0 = soggetto(codice_fiscale0, crea);
	if(!p0)
		return; // p0 non l'ho trovato.

	p1 = soggetto(codice_fiscale1, crea);
	if(!p1)
		return; // p1 non l'ho trovato.
	
	contatti *c = NULL, *d = NULL;
	c = (contatti*) malloc(sizeof(contatti));
	d = (contatti*) malloc(sizeof(contatti));
	assert(c && d);
	c->v = v;
	c->persona = p1;
	d->v = v;
	d->persona = p0;
	if(!p0->persona->catena){
		p0->persona->catena = (contatti**) malloc(sizeof(contatti*));
		++p0->persona->n_contatti;
		*p0->persona->catena = c;
		if(!p1->persona->catena){
			p1->persona->catena = (contatti**) malloc(sizeof(contatti*));
			++p1->persona->n_contatti;
			*p1->persona->catena = d;
		}else{
			int k;
			size_t size;
			size = sizeof(contatti*) * (p1->persona->n_contatti+1);
			p1->persona->catena  = (contatti**) realloc (p1->persona->catena, size);
			size = p1->persona->n_contatti;
			for(k=0; k<size; k++){
				cmp = compare(p1->persona->catena[k]->persona->codice_fiscale, codice_fiscale0, 16);
				if(cmp){
				if(cmp->ret < 0)
							break;
				}else
					break;
				free(cmp);
			}
			if(cmp){
				int y;
				for(y=k+1; y<size; y++)
					p1->persona->catena[y] = p1->persona->catena[y-1];
				p1->persona->catena[k] = d;
				free(cmp);
			}else{
				free(d);
				p1->persona->catena[k]->v = v;
			}
			++p1->persona->n_contatti;
		}
	}else{
		int i;
		size_t size;
		size = sizeof(contatti*) * (p0->persona->n_contatti+1);
		p0->persona->catena  = (contatti**) realloc (p0->persona->catena, size);
		size = p0->persona->n_contatti;
		for(i=0; i<size; i++){
			cmp = compare(p0->persona->catena[i]->persona->codice_fiscale, codice_fiscale1, 16);
			if(cmp){
				if(cmp->ret < 0)
					break;
			}else
				break;
			free(cmp);
		}
		if(cmp){
			free(cmp);
			int j;
			for(j=i+1; j<size; j++)
				p0->persona->catena[j] = p0->persona->catena[j-1];
			p0->persona->catena[i] = c;
			if(!p1->persona->catena){
				p1->persona->catena = (contatti**) malloc(sizeof(contatti*));
				++p1->persona->n_contatti;
				*p1->persona->catena = d;
			}else{
				int k;
				size_t size;
				size = sizeof(contatti*) * (p1->persona->n_contatti+1);
				p1->persona->catena  = (contatti**) realloc (p1->persona->catena, size);
				size = p1->persona->n_contatti;
				for(k=0; k<size; k++){
					cmp = compare(p1->persona->catena[k]->persona->codice_fiscale, codice_fiscale0, 16);
					if(cmp){
						if(cmp->ret < 0)
							break;
					}else
						break;
					free(cmp);
				}
				if(cmp){
					int y;
					for(y=k+1; y<size; y++)
						p1->persona->catena[y] = p1->persona->catena[y-1];
					p1->persona->catena[k] = d;

					free(cmp);
				}else{
					free(d);
					p1->persona->catena[k]->v = v;
				}
				++p1->persona->n_contatti;
			}
		}else{
			free(c);
			p0->persona->catena[i]->v = v;
		}
		++p0->persona->n_contatti;
	}
}

void area(const char* codice_fiscale){
	lista_pazienti* p0 = soggetto(codice_fiscale, 0);
	if(!p0)
		return; // p0 non l'ho trovato.
	int i;
	printf("Contatti %s:\n", codice_fiscale);
	for(i=0;i<p0->persona->n_contatti;++i)
		printf("|%16s|%d|\n",p0->persona->catena[i]->persona->codice_fiscale, p0->persona->catena[i]->v);
}

void untore(const char* codice_fiscale0, const char* codice_fiscale1){

}

void valore(const char* codice_fiscale0, const char* codice_fiscale1){
	// c = v(t1, t2) + v(t2, t3) + . . . v(tn−1, tn)
}

void maggiore(){
// r(x) = SUM 1/v(x, y)
}

void incubazione(const char* nome_malattia, int n, int crea){
	patologia* p = malattia(nome_malattia, crea);
	p->t = n;
}

void immune(const char* codice_fiscale, const char* nome_malattia, int crea){
	lista_pazienti* pers  = soggetto(codice_fiscale, crea);
	pers->persona->immune = malattia(nome_malattia, crea);
}

void print_pazienti(void){
	#define NULL_STRING_	" "
	int i;
	lista_pazienti* ptr = pazienti;
	printf("\nLista Pazienti\n");
	printf("|%16s|%"NOME_MALATTIA_MAX_TXT"s|%"NOME_MALATTIA_MAX_TXT"s| Contatti\n", "Codice Fiscale", "Malattia", "Immunita'");
	while(ptr != NULL){
		printf("|%16s|%"NOME_MALATTIA_MAX_TXT"s|%"NOME_MALATTIA_MAX_TXT"s|", ptr->codice_fiscale, (ptr->persona->contagiato) ? ptr->persona->contagiato->nome : NULL_STRING_, (ptr->persona->immune) ? ptr->persona->immune->nome : NULL_STRING_);
		for(i=0;i<ptr->persona->n_contatti;++i){
			if(ptr->persona->catena[i])
				printf(" %16s(%d)",ptr->persona->catena[i]->persona->codice_fiscale, ptr->persona->catena[i]->v);
			else
				printf(" ptr->persona->catena[%d]", i);
		}
		printf("\n");
		ptr = ptr->next;
	}
	printf("\n");
	#undef NULL_STRING_
}

void print_malattie(void){
	lista_malattie* ptr = malattie;
	printf("\nLista Malattie\n");
	printf("|%"NOME_MALATTIA_MAX_TXT"s|%11s|\n", "Malattia", "Incubazione");
	while(ptr != NULL){
		printf("|%"NOME_MALATTIA_MAX_TXT"s", ptr->nome);
		if(ptr->t >= 0)
			printf("|%11d|\n", ptr->t);
		else if(ptr->t == INFINITE_T)
			printf("|%11s|\n", "INFINITE");
		else //if(ptr->t == NO_T)
			printf("|%11s|\n", " ");
		ptr = ptr->next;
	}
	printf("\n");
}

void free_pazienti(void){
	lista_pazienti *ptr, *tmp;
	ptr = pazienti;
	DPRINTF("[free_pazienti]\n");
	while(ptr != NULL){
		tmp = ptr->next;
		if(ptr->persona->catena){
			int i;
			for(i=0;i<ptr->persona->n_contatti; i++)
				free(ptr->persona->catena[i]);
			free(ptr->persona->catena);
		}
		free(ptr->persona);
		free(ptr);
		ptr = tmp;
	}
	pazienti = NULL;
}

void free_malattie(void){
	lista_malattie *ptr, *tmp;
	ptr = malattie;
	DPRINTF("[free_malattie]\n");
	while(ptr != NULL){
		tmp = ptr->next;
		free((char*)ptr->nome);
		free(ptr);
		ptr = tmp;
	}
	malattie = NULL;
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
