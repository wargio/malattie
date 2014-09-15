/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef __SPECIALI_H__
#define __SPECIALI_H__

#include <stddef.h>
#ifdef DEBUG
	#define DPRINTF(args ...)			printf("[DEBUG]"args)
#else
	#define DPRINTF(args ...)
#endif

typedef struct _compare_t{
	int ret;
	int pos;
} compare_t;

/*
	restituisce NULL se sono uguali, altrimenti compare_t
	compare_t.ret    | e' il valore di ritorno che restituirebbe strncmp
	compare_t.pos    | e' la posizione del primo carattere diverso
*/

compare_t* compare(const char* s0, const char* s1, size_t n);

void to_lower(char* s, size_t len);
void to_upper(char* s, size_t len);
void rm_char (char* s, size_t len, unsigned char find, unsigned char change);

size_t fixed_strlen(const char* s);

size_t max_positive(int* v, size_t n);
size_t min_positive(int* v, size_t n);
void quicksort(int* v, int s, int n);

#endif
