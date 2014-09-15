/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "funzioni.h"
#include "speciali.h"
#include "parser.h"

#include <assert.h>

#include <stdarg.h>
#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

lista_pazienti* pazienti = NULL;
lista_malattie* malattie = NULL;
uint32_t n_pazienti = 0;
uint32_t n_malattie = 0;

/*
typedef struct _lista_malattie{
	const char* 	nome;		// nome
	int t;				// tempo di incubazione
	lista_malattie* next;		// malattia successiva
} lista_malattie;

typedef lista_malattie malattia;

*/

lista_malattie* genera_malattia(const char* nome){
	lista_malattie* ptr = (lista_malattie*) malloc(sizeof(lista_malattie));
	memset(ptr, 0, sizeof(lista_malattie));
	size_t len = sizeof(char)*fixed_strlen(nome)+1;
	char* txt = (char*) malloc(len);
	memset(txt, 0, len);
	--len;
	ptr->nome = (const char*) txt;
	memcpy(txt, nome, len);
	to_lower(txt, len);
	rm_char (txt, len, ' ', '_');
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
		size_t len = fixed_strlen(nome_malattia);
		len = (len>=NOME_MALATTIA_MAX_STR) ? NOME_MALATTIA_MAX_STR-1 : len;
		memcpy(k,nome_malattia,len);
		to_lower(k, len);
		rm_char (k, len, ' ', '_');
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
	to_upper(ptr->codice_fiscale, 16);
//	DPRINTF("[Creo %s]\n", ptr->codice_fiscale);
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
			}else{
//				DPRINTF("[Trov %s]\n", ptr->codice_fiscale);
				return ptr;
			}
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

int find_and_add(lista_pazienti* p0, lista_pazienti *p1, contatti *c){
	int y, j;
	compare_t* cmp = NULL;
	if(p1->persona->n_contatti != 0){
		for(y = 0; y<p1->persona->n_contatti; y++){
			cmp = compare(p1->persona->catena[y]->persona->codice_fiscale, p0->codice_fiscale, 16);
			if(cmp){
				if(cmp->ret == 0) // should never get here
					break;
				if(cmp->ret < 0)
					break;
			}else
				break;
			free(cmp);
			cmp = NULL;
		}
		if(!cmp && y>=p1->persona->n_contatti){
			p1->persona->catena = (contatti**) realloc(p1->persona->catena, sizeof(contatti*)*(p1->persona->n_contatti+1));
			p1->persona->catena[p1->persona->n_contatti] = c;
			++p1->persona->n_contatti;
		}else if(cmp){
			if(cmp->ret < 0){
				p1->persona->catena = (contatti**) realloc(p1->persona->catena, sizeof(contatti*)*(p1->persona->n_contatti+1));
				for(j=p1->persona->n_contatti; j>y; j--)
					p1->persona->catena[j] = p1->persona->catena[j-1];
				p1->persona->catena[y] = c;
				++p1->persona->n_contatti;
				free(cmp);
				cmp = NULL;
			}else{
				// should never get here
				free(cmp);
				cmp = NULL;
				return 1;
			}
		}else return 1;
		if(cmp)
			free(cmp);
	}else{
		p1->persona->catena = (contatti**) malloc(sizeof(contatti*));
		p1->persona->catena[0] = c;
		p1->persona->n_contatti = 1;
	}
	return 0;
}

void contatto(const char* codice_fiscale0, const char* codice_fiscale1, int v, int crea){
	if(!codice_fiscale0 || !codice_fiscale1 || codice_fiscale0 == codice_fiscale1)
		return;

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
	if(find_and_add(p0, p1, d) == 1)
		free(c);
	if(find_and_add(p1, p0, c) == 1)
		free(d);
}

void area(const char* codice_fiscale){
	lista_pazienti* p0 = soggetto(codice_fiscale, 0);
	if(!p0)
		return; // p0 non l'ho trovato.
	int i;
	printf("Contatti di %s\n", codice_fiscale);
	for(i=0;i<p0->persona->n_contatti;++i)
		printf("|%16s|%d|\n",p0->persona->catena[i]->persona->codice_fiscale, p0->persona->catena[i]->v);
}

size_t trova_untore(lista_ricerca* ricerca, lista_ricerca* untori, const char* codice_fiscale, size_t call){
	if(!ricerca || !untori || !codice_fiscale || call >= n_pazienti)
		return 0;
	if(!ricerca[call].paziente->persona->n_contatti)
		return 0;
	
	lista_pazienti* p0 = NULL;
	contatti** cat = NULL;
	size_t i, j, ret, trovato, ncont;
	compare_t* cmp = NULL;
	
	cat = ricerca[call].paziente->persona->catena;
	ncont = ricerca[call].paziente->persona->n_contatti;

	for(i=0; i<ncont; ++i){
		cmp = compare(cat[i]->persona->codice_fiscale, codice_fiscale, 16);
		if(cmp && cmp->ret < 0)
			break;
		else if(!cmp)
			return 1;
		free(cmp);
		cmp = NULL;
	}
	if(cmp)
		free(cmp);
	unsigned char* tas = (unsigned char*) malloc(sizeof(unsigned char)*ncont); // test & set.
	memset(tas, 0, ncont);
	for(j=0; j<=call; ++j){
		p0 = ricerca[j].paziente;
		for(i=0; i<ncont; ++i){
			cmp = compare(cat[i]->persona->codice_fiscale, p0->codice_fiscale, 16);
			if(!cmp)
				tas[i] = 1; // l'ho trovato e quindi devo saltarlo
			else
				free(cmp);

		}
	}
	ret = 0;
	trovato = 0;
	ricerca[call].v = 0;
	for(i=0; i<ncont; ++i){
		if(tas[i]) // se 1 lo salto
			continue;
		if(call+1 < n_pazienti){
			ricerca[call+1].paziente = cat[i]->persona;
			if(trova_untore(ricerca, untori, codice_fiscale, call+1))
				tas[i] = 2;
		}
	}
	trovato = 0;
	for(i=0; i<ncont; ++i){
		if(tas[i]==2 && !trovato){
			untori[call].paziente = cat[i]->persona;
			trovato = ret = 1;
		}else if(tas[i]==2 && trovato){
			untori[call].paziente = NULL;
			trovato = ret = 0;
		}
	}
	free(tas);
	return ret;
}

void untore(const char* codice_fiscale0, const char* codice_fiscale1){
	if(!codice_fiscale0 || !codice_fiscale1 || codice_fiscale0 == codice_fiscale1)
		return;

	size_t i, ret;
	lista_pazienti* p0, *p1;
	lista_ricerca*  ricerca = NULL;
	lista_ricerca*  untori  = NULL;

	p0 = p1 = NULL;

	p0 = soggetto(codice_fiscale0, 0);
	if(!p0)
		return; // p0 non l'ho trovato.

	p1 = soggetto(codice_fiscale1, 0);
	if(!p1)
		return; // p1 non l'ho trovato.

	
	ricerca = (lista_ricerca*) malloc(sizeof(lista_ricerca)*n_pazienti);
	untori = (lista_ricerca*) malloc(sizeof(lista_ricerca)*n_pazienti);
	memset(ricerca, 0, sizeof(lista_ricerca)*n_pazienti);
	memset(untori, 0, sizeof(lista_ricerca)*n_pazienti);
	ricerca[0].paziente = p0;
	ricerca[0].v = 0;

	ret = trova_untore(ricerca, untori, p1->codice_fiscale, 0);
	if(ret){
		printf("Untori tra %s e %s:\n", p0->codice_fiscale, p1->codice_fiscale);
		for(i=0; untori[i].paziente!=NULL; ++i){
			printf("|%16s|\n", untori[i].paziente->codice_fiscale);
		}
	}
	else
		printf("Nessun untore tra %s e %s\n", p0->codice_fiscale, p1->codice_fiscale);
	free(ricerca);
	free(untori);
}

/*
 Potrei cercare tramite puntatore (ptr0 == ptr1), ma non sarebbero necessariamente ordinati
 O posso usare i codici fiscali, dato che sono ordinati
 */
size_t trova_contatti(lista_ricerca*  ricerca, const char* codice_fiscale, size_t call){
	if(!ricerca || !codice_fiscale || call >= n_pazienti)
		return 0;
	if(!ricerca[call].paziente->persona->n_contatti)
		return 0;

	lista_pazienti* p0 = NULL;
	contatti** cat = NULL;
	size_t i, j, ret, ncont;
	compare_t* cmp = NULL;
	
	cat = ricerca[call].paziente->persona->catena;
	ncont = ricerca[call].paziente->persona->n_contatti;

	for(i=0; i<ncont; ++i){
		cmp = compare(cat[i]->persona->codice_fiscale, codice_fiscale, 16);
		if(cmp && cmp->ret < 0)
			break;
		else if(!cmp){
			ricerca[call].v = cat[i]->v;
			return (call) ? call : 1;
		}
		free(cmp);
		cmp = NULL;
	}
	if(cmp)
		free(cmp);
	unsigned char* tas = (unsigned char*) malloc(sizeof(unsigned char)*ncont); // test & set.
	memset(tas, 0, ncont);
	for(j=0; j<=call; ++j){
		p0 = ricerca[j].paziente;
		for(i=0; i<ncont; ++i){
			cmp = compare(cat[i]->persona->codice_fiscale, p0->codice_fiscale, 16);
			if(!cmp)
				tas[i] = 1; // l'ho trovato e quindi devo saltarlo
			else
				free(cmp);
		}
	}
	for(i=0; i<ncont; ++i){
		if(tas[i]) // se 1 lo salto
			continue;
		if(call+1 < n_pazienti){
			ricerca[call+1].paziente = cat[i]->persona;
			ricerca[call].v = cat[i]->v;
			DPRINTF("\r[trova_contatti] %s %ld ", cat[i]->persona->codice_fiscale, call);
			if((ret = trova_contatti(ricerca, codice_fiscale, call+1))){
				free(tas);
				return ret;
			}
			ricerca[call+1].paziente = NULL;
		}
	}
	free(tas);
	return 0;
}

void valore(const char* codice_fiscale0, const char* codice_fiscale1){
	// c = v(t1, t2) + v(t2, t3) + . . . v(tn−1, tn)
	if(!codice_fiscale0 || !codice_fiscale1 || codice_fiscale0 == codice_fiscale1)
		return;

	size_t i, ret;
	int v = NO_CONTATTO;
	lista_pazienti* p0, *p1;
	lista_ricerca*  ricerca  = NULL;
	p0 = p1 = NULL;

	p0 = soggetto(codice_fiscale0, 0);
	if(!p0)
		return; // p0 non l'ho trovato.

	p1 = soggetto(codice_fiscale1, 0);
	if(!p1)
		return; // p1 non l'ho trovato.
	
	ricerca = (lista_ricerca*) malloc(sizeof(lista_ricerca)*n_pazienti);
	memset(ricerca, 0, sizeof(lista_ricerca)*n_pazienti);
	ricerca[0].paziente = p0;
	ricerca[0].v = NO_CONTATTO;

	ret = trova_contatti(ricerca, p1->codice_fiscale, 0);
	if(ret){
		for(v=0, i=0; ricerca[i].paziente != NULL; ++i){
			DPRINTF("[valore][trovato] %s %d\n", ricerca[i].paziente->codice_fiscale, ricerca[i].v);
			v+=ricerca[i].v;
		}
		printf("Valore: %s <=> %s = %d\n", p0->codice_fiscale, p1->codice_fiscale, v);
	}
	else
		printf("Valore: Nessun contatto tra %s e %s\n", p0->codice_fiscale, p1->codice_fiscale);
	free(ricerca);
}

size_t trova_max(lista_ricerca*  ricerca, const char* codice_fiscale, size_t call, size_t max){
	if(!ricerca || !codice_fiscale || call >= n_pazienti)
		return 0;
	if(!ricerca[call].paziente->persona->n_contatti)
		return 0;

	lista_pazienti* p0 = NULL;
	contatti** cat = NULL;
	size_t i, j, ret, ncont;
	compare_t* cmp = NULL;
	
	cat = ricerca[call].paziente->persona->catena;
	ncont = ricerca[call].paziente->persona->n_contatti;

	for(i=0; i<ncont; ++i){
		cmp = compare(cat[i]->persona->codice_fiscale, codice_fiscale, 16);
		if(cmp && cmp->ret < 0)
			break;
		else if(!cmp){
			if(cat[i]->v > max)
				return 0;
			ricerca[call].v = cat[i]->v;
			return (call) ? call : 1;
		}
		free(cmp);
		cmp = NULL;
	}
	if(cmp)
		free(cmp);
	unsigned char* tas = (unsigned char*) malloc(sizeof(unsigned char)*ncont); // test & set.
	memset(tas, 0, ncont);
	for(j=0; j<=call; ++j){
		p0 = ricerca[j].paziente;
		for(i=0; i<ncont; ++i){
			cmp = compare(cat[i]->persona->codice_fiscale, p0->codice_fiscale, 16);
			if(!cmp)
				tas[i] = 1; // l'ho trovato e quindi devo saltarlo
			else
				free(cmp);
		}
	}
	for(i=0; i<ncont; ++i){
		if(tas[i]) // se 1 lo salto
			continue;
		if(call+1 < n_pazienti){
			ricerca[call+1].paziente = cat[i]->persona;
			ricerca[call].v = cat[i]->v;
			if(cat[i]->v<max){ 
				if((ret = trova_max(ricerca, codice_fiscale, call+1, max))){
					free(tas);
					return ret;
				}
			}
			ricerca[call+1].paziente = NULL;
		}
	}
	free(tas);
	return 0;
}

void maggiore(void){
// r(x) = SUM 1/v(x, y)
	if(!pazienti)
		return;
	size_t i, j, ret, count;
	int v = NO_CONTATTO, max = INT_MAX;
	lista_pazienti* p0, *p1;
	lista_ricerca*  ricerca  = NULL;
	lista_ricerca*  max_list = NULL;
	p0 = p1 = NULL;

	p0 = pazienti;
	p1 = p0->next;
	
	ricerca = (lista_ricerca*) malloc(sizeof(lista_ricerca)*n_pazienti);
	max_list = (lista_ricerca*) malloc(sizeof(lista_ricerca)*n_pazienti);
	memset(ricerca, 0, sizeof(lista_ricerca)*n_pazienti);
	memset(max_list, 0, sizeof(lista_ricerca)*n_pazienti);
	
	count=0;
	while(p0 != NULL){
		++count;
		while(p1 != NULL){
			ricerca[0].paziente = p0;
			ricerca[0].v = NO_CONTATTO;
			ret = trova_max(ricerca, p1->codice_fiscale, 0, max);
			if(ret){
				for(v=0, i=0; ricerca[i].paziente != NULL; ++i){
					v+=ricerca[i].v;
				}
				if(v < max){
					max = v;
					j = 0;
					memset(max_list, 0, sizeof(lista_ricerca)*n_pazienti);
					DPRINTF("Start again (%d)\n", max);
					max_list[j++].paziente = p0;
				}else if(v == max && j < n_pazienti){
					max_list[j++].paziente = p0;
				}
			}
			p1 = p1->next;
		}
		p0 = p0->next;
		if(p0)
			p1 = p0->next;
	}
	printf("Maggiore: %d\n", max);
	for(i=0; i<j; ++i)
		printf("|%s|\n", max_list[i].paziente->codice_fiscale);
	free(ricerca);
	free(max_list);
}

void incubazione(const char* nome_malattia, int n, int crea){
	patologia* p = malattia(nome_malattia, crea);
	if(p)
		p->t = n;
}

void immune(const char* codice_fiscale, const char* nome_malattia, int crea){
	lista_pazienti* pers  = soggetto(codice_fiscale, crea);
	pers->persona->immune = malattia(nome_malattia, crea);
}

size_t trova_tempo(lista_ricerca* ricerca, lista_malattie* patologia, const char* codice_fiscale, size_t call){
	if(!ricerca || !codice_fiscale || call >= n_pazienti)
		return 0;
	if(!ricerca[call].paziente->persona->n_contatti)
		return 0;

	lista_pazienti* p0 = NULL;
	contatti** cat = NULL;
	size_t i, j, ret, ncont;
	compare_t* cmp = NULL;
	
	cat = ricerca[call].paziente->persona->catena;
	ncont = ricerca[call].paziente->persona->n_contatti;

	for(i=0; i<ncont; ++i){
		cmp = compare(cat[i]->persona->codice_fiscale, codice_fiscale, 16);
		if(cmp && cmp->ret < 0)
			break;
		else if(!cmp){
			if(cat[i]->persona->persona->immune == patologia)
				return 0;
			return (call) ? call : 1;
		}
		free(cmp);
		cmp = NULL;
	}
	if(cmp)
		free(cmp);
	unsigned char* tas = (unsigned char*) malloc(sizeof(unsigned char)*ncont); // test & set.
	memset(tas, 0, ncont);
	for(j=0; j<=call; ++j){
		p0 = ricerca[j].paziente;
		for(i=0; i<ncont; ++i){
			cmp = compare(cat[i]->persona->codice_fiscale, p0->codice_fiscale, 16);
			if(!cmp)
				tas[i] = 1; // l'ho trovato e quindi devo saltarlo
			else
				free(cmp);
		}
	}
	for(i=0; i<ncont; ++i){
		if(tas[i]) // se 1 lo salto
			continue;
		if(call+1 < n_pazienti){
			if(cat[i]->persona->persona->immune == patologia)
				continue;
			ricerca[call+1].paziente = cat[i]->persona;
			DPRINTF("\r[trova_contatti] %s %ld ", cat[i]->persona->codice_fiscale, call);
			if((ret = trova_contatti(ricerca, codice_fiscale, call+1))){
				free(tas);
				return ret;
			}
			ricerca[call+1].paziente = NULL;
		}
	}
	free(tas);
	return 0;
}

void tempo(const char* codice_fiscale0, const char* codice_fiscale1, const char* nome_malattia){
	// c = v(t1, t2) + v(t2, t3) + . . . v(tn−1, tn)
	if(!codice_fiscale0 || !codice_fiscale1 || codice_fiscale0 == codice_fiscale1)
		return;

	size_t i, ret;
	int v = NO_CONTATTO;
	lista_pazienti* p0, *p1;
	lista_malattie* m0;
	lista_ricerca*  ricerca  = NULL;
	p0 = p1 = NULL;
	m0 = NULL;

	p0 = soggetto(codice_fiscale0, 0);
	if(!p0){
		printf("[Errore] Tempo: %s non esiste \n", p0->codice_fiscale);
		return; // p0 non l'ho trovato.
	}
	
	p1 = soggetto(codice_fiscale1, 0);
	if(!p1){
		printf("[Errore] Tempo: %s non esiste \n", p1->codice_fiscale);
		return; // p1 non l'ho trovato.
	}
		

	if(p0 == p1){
		printf("[Errore] Tempo: %s == %s \n", p0->codice_fiscale, p1->codice_fiscale);
		return;
	}
	
	m0 = malattia(nome_malattia, 0);
	if(!m0 || m0->t < 0){
		printf("[Errore] Tempo: %s non esiste o il tempo di incubazione e' INFINITO\n", nome_malattia);
		return; // m0 non l'ho trovato oppure m0->t e' minore di zero.
	}
		
		
	ricerca = (lista_ricerca*) malloc(sizeof(lista_ricerca)*n_pazienti);
	memset(ricerca, 0, sizeof(lista_ricerca)*n_pazienti);
	ricerca[0].paziente = p0;
	ricerca[0].v = NO_CONTATTO;
//	ret = trova_contatti(ricerca, p1->codice_fiscale, 0);
	ret = trova_tempo(ricerca, m0, p1->codice_fiscale, 0);
	if(ret){
		for(v=0, i=0; ricerca[i].paziente != NULL; ++i){
			DPRINTF("[tempo] %s %d\n", ricerca[i].paziente->codice_fiscale, ricerca[i].v);
			++v;
		}
		if(v>0){
			v *= m0->t;
			printf("Tempo: %s <=> %s = %d\n", p0->codice_fiscale, p1->codice_fiscale, v);
		}else
			printf("Tempo: %s <=> %s = ???????? (%d)\n", p0->codice_fiscale, p1->codice_fiscale, v);
	}
	else
		printf("Tempo: %s <=> %s = INFINITO\n", p0->codice_fiscale, p1->codice_fiscale);
	free(ricerca);

}

void contagio(const char* codice_fiscale0, const char* codice_fiscale1, const char* nome_malattia){
	
}

void bacino(const char* nome_malattia, const char* codice_fiscale0){
	
}

void vaccinazione(unsigned int n_paesi, unsigned int m_centri, int* posizione){

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
	n_pazienti = 0;
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
	n_malattie = 0;
}
