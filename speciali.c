/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "speciali.h"

#ifdef DEBUG
	#include <stdio.h>
#endif

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
		s[i] = (char) tolower((unsigned char)s[i]);
		i++;
	}
}

void to_upper(char* s, size_t len){
	size_t i=0;
	while(i < len){
		s[i] = (char) toupper((unsigned char)s[i]);
		i++;
	}
}

void rm_char(char* s, size_t len, unsigned char find, unsigned char change){
	size_t i=0;
	while(i < len){
		if((unsigned char)s[i] == find)
			s[i] = change;
		i++;
	}
}

size_t fixed_strlen(const char* s){
	size_t i = 0;
	if(!s)
		return 0;
	while(s[i] != '\0' && s[i] != '\n' && s[i] != ';')
		++i;
	return i;
}

size_t max_positive(int* v, size_t n){
	if(!v || !n)
		return 0;
	size_t i, m;
	m = 0;
	for(i=0;i<n;++i)
		m = (m < v[i]) ? v[i] : m;
	return m;
}

size_t min_positive(int* v, size_t n){
	if(!v || !n)
		return 0;
	size_t i, m;
	m = 0;
	for(i=0;i<n;++i)
		m = (m > v[i]) ? v[i] : m;
	return m;
}

void quicksort(int* v, int s, int n){
	if(!v || !n)
		return;
    int tmp, swc;
    int  l,r,p;
    while (s<n){
        l = s;
		p = (s+n)/2;
		r = n;
        tmp = v[p];
        while (1){
            while((l<=r) && (v[l]<=tmp))
				++l;
            while((l<=r) && (v[r]>tmp))
				--r;
            if (l>r) break;
            swc=v[l];
			v[l]=v[r];
			v[r]=swc;
            if (p==r--)
				p=l;
            ++l;
        }
        v[p]=v[r];
		v[r--]=tmp;
        if((r-s)<(n-l)){
            quicksort(v, s, r);
            s=l;
        }else{
            quicksort(v, l, n);
            n=r;
        }
    }   
}

