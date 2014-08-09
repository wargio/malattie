/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "speciali.h"

#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

compare_t* compare(const char* s0, const char* s1, size_t n){
	assert(n>0 && s0 && s1);
	int x = 0;
	while(*s0 != '\0' && *s1 != '\0' && *s0 == *s1 && x < n){
		x++;
		s0++;
		s1++;
	}
	if(x == n)
		return (compare_t*) 0;
	compare_t* cmp = (compare_t*)malloc(sizeof(compare_t));
	cmp->ret = (*s0) - (*s1);
	cmp->pos = x;
	return cmp;
}

void to_lower(char* s, size_t len){
	size_t i=0;
	while(i < len){
		s[i] = (char) tolower(s[i]);
		i++;
	}
}

void to_upper(char* s, size_t len){
	size_t i=0;
	while(i < len){
		s[i] = (char) toupper(s[i]);
		i++;
	}
}
