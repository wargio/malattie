/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef __STRUTTURE_H__
#define __STRUTTURE_H__

#define INFINITE_T			-2
#define NO_T				-1

#define NO_CONTATTO			-1

#define NOME_MALATTIA_MAX_STR			32
#define NOME_MALATTIA_MAX_TXT			"32"


typedef struct _lista_malattie lista_malattie;
typedef lista_malattie patologia;
typedef struct _contatti contatti;
typedef struct _paziente paziente;
typedef struct _lista_pazienti lista_pazienti;

struct _lista_malattie{
	const char* 	nome;		// nome
	int t;						// tempo di incubazione
	lista_malattie* next;		// malattia successiva
};

struct _contatti{
	int v;						// valore contatto
	lista_pazienti* persona;	// malattia successiva
};

struct _paziente{
	contatti** catena;			// catena di contatti
	int        n_contatti;		// numero di contatti
	// considero che il paziente possa ammalarsi di una sola malattia alla volta.
	patologia* contagiato;		// patologia paziente contagiato
	patologia* immune;			// immunità paziente verso una patologia
};

struct _lista_pazienti{
	char codice_fiscale[17];	// codice fiscale
	paziente*      persona;		// informazioni paziente
	lista_pazienti* next;		// paziente successivo
};


#endif
