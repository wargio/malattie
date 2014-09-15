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
lista_pazienti* soggetto    (const char* codice_fiscale, int crea);
void            contatto    (const char* codice_fiscale0, const char* codice_fiscale1, int v, int crea);
void            area        (const char* codice_fiscale);
void            untore      (const char* codice_fiscale0, const char* codice_fiscale1);
void            valore      (const char* codice_fiscale0, const char* codice_fiscale1);
void            maggiore    (void);
lista_malattie* malattia    (const char* nome_malattia, int crea);
void            incubazione (const char* nome_malattia, int n, int crea);
void            tempo       (const char* codice_fiscale0, const char* codice_fiscale1, const char* nome_malattia);
void            immune      (const char* codice_fiscale, const char* malattia, int crea);
void            contagio    (const char* codice_fiscale0, const char* codice_fiscale1, const char* nome_malattia);
void            bacino      (const char* nome_malattia, const char* codice_fiscale0);
void            vaccinazione(unsigned int n_paesi, unsigned int m_centri, int* posizione);

void print_pazienti(void);
void print_malattie(void);

void free_pazienti(void);
void free_malattie(void);

#endif
