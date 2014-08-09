/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef __FUNZIONI_H__
#define __FUNZIONI_H__

#include "strutture.h"
/*
  crea == 0 se non lo trova, non fa nulla
  crea == 1 se non lo trova, lo aggiunge
 */
lista_pazienti* soggetto(const char* codice_fiscale, int crea);
lista_malattie* malattia(const char* nome_malattia, int crea);

void contatto(const char* codice_fiscale0, const char* codice_fiscale1, int v, int crea);
void immune(const char* codice_fiscale, const char* malattia, int crea);
void incubazione(const char* nome_malattia, int n, int crea);
void area(const char* codice_fiscale);

void print_pazienti(void);
void print_malattie(void);

void free_pazienti(void);
void free_malattie(void);

void fwrite_malattie(const char* fpath);
void fwrite_pazienti(const char* fpath);

#endif