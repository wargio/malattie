/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#include "parser.h"
#include "funzioni.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

static const char* parser_types_str[] = {
	"PARSER_NULL",
	"PARSER_STRING",
	"PARSER_STRING_EOF",
	"PARSER_BAD_STRING",
	"PARSER_LEN_ZERO",
	"PARSER_STR_NULL",
	"PARSER_COMPOSE",
	"PARSER_COMPOSE_NO_SPACE",
	NULL
};

static FILE* csv_fp = NULL;

size_t csv_string_len(parser_t* pp){
	if(pp && pp->n)
		return pp->n - pp->p - 1;
	return 0;
}

const char* csv_full_string(parser_t* pp){
	if(pp && pp->c)
		return (const char*) pp->c;
	return NULL;
}

size_t csv_string_pos(parser_t* pp){
	if(pp)
		return pp->p;
	return 0;
}

int csv_type(parser_t* pp){
	if(pp)
		return pp->r;
	return PARSER_NULL;
}

const char* csv_type_name(parser_t* pp){
	if(pp)
		return parser_types_str[pp->r];
	return parser_types_str[PARSER_NULL];
}

parser_t* csv_init(char* str, size_t len){
	if(len == 0)
		return NULL;
	parser_t* pp = (parser_t*) malloc(sizeof(parser_t));
	assert(pp);
	memset(pp,0,sizeof(parser_t));
	pp->c = str;
	pp->l = len;
	return pp;
}

int csv_destroy(parser_t* pp){
	if(pp){
		free(pp);
		return PARSER_OK;
	}
	return PARSER_BAD;
}

const char* csv_next_string(parser_t* pp){
	if(!pp)
		return NULL;
	if(pp->r == PARSER_STR_NULL || pp->r == PARSER_STRING_EOF || pp->r >= PARSER_COMPOSE)
		return NULL;
	size_t i   = pp->n;
	size_t len = pp->l;
	const char* str = pp->c;
	if(i >= len){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return NULL;
	}
	
	unsigned char c = str[i];
	if(c == '#'){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return NULL;
	}else if(c == ';'){
		pp->r = PARSER_LEN_ZERO;
		pp->p = pp->n;
		pp->n++;
		return NULL;
	}
	while(i < len && c != ';' && c != '#' && !isalnum(c))
		c = str[++i];
	if(c == '#'){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return NULL;
	}else if(c == ';'){
		pp->r = PARSER_LEN_ZERO;
		pp->p = pp->n;
		pp->n++;
		return NULL;
	}
	pp->p = i;
	while(i < len && c != ';' && c != '#' && c != '\n' && c != '\0')
		c = str[i++];
	pp->n = i;
	if(c == '\n' || c == '\0')
		pp->r = PARSER_STRING_EOF;
	else
		pp->r = PARSER_STRING;
	return pp->c + pp->p;
}

long int csv_next_integer(parser_t* pp){
	if(!pp)
		return 0;
	if(pp->r == PARSER_STR_NULL || pp->r == PARSER_STRING_EOF || pp->r >= PARSER_COMPOSE)
		return 0;
	size_t i   = pp->n;
	size_t len = pp->l;
	const char* str = pp->c;
	if(i >= len){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return 0;
	}
	long int X = 0;
	unsigned char c = str[i];
	if(c == '#'){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return 0;
	}else if(c == ';'){
		pp->r = PARSER_LEN_ZERO;
		pp->p = pp->n;
		pp->n++;
		return 0;
	}
	while(i < len && c != ';' && c != '#' && c != '-' && !isalnum(c))
		c = str[++i];
	if(c == '#'){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return 0;
	}else if(c == ';'){
		pp->r = PARSER_LEN_ZERO;
		pp->p = pp->n;
		pp->n++;
		return 0;
	}
	pp->p = i;
	while(i < len && c != ';' && c != '#' && c != '\n' && c != '\0')
		c = str[i++];
	pp->n = i;
	if(c == '\n' || c == '\0')
		pp->r = PARSER_STRING_EOF;
	else
		pp->r = PARSER_STRING;
	
	sscanf(pp->c + pp->p, "%ld", &X);
	return X;
}

double csv_next_double(parser_t* pp){
	if(!pp)
		return 0;
	if(pp->r == PARSER_STR_NULL || pp->r == PARSER_STRING_EOF || pp->r >= PARSER_COMPOSE)
		return 0;
	size_t i   = pp->n;
	size_t len = pp->l;
	const char* str = pp->c;
	if(i >= len){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return 0;
	}
	double X = 0;
	unsigned char c = str[i];
	if(c == '#'){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return 0;
	}else if(c == ';'){
		pp->r = PARSER_LEN_ZERO;
		pp->p = pp->n;
		pp->n++;
		return 0;
	}
	while(i < len && c != ';' && c != '#' && c != '-' && !isalnum(c))
		c = str[++i];
	if(c == '#'){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return 0;
	}else if(c == ';'){
		pp->r = PARSER_LEN_ZERO;
		pp->p = pp->n;
		pp->n++;
		return 0;
	}
	pp->p = i;
	while(i < len && c != ';' && c != '#' && c != '\n' && c != '\0')
		c = str[i++];
	pp->n = i;
	if(c == '\n' || c == '\0')
		pp->r = PARSER_STRING_EOF;
	else
		pp->r = PARSER_STRING;
	
	sscanf(pp->c + pp->p, "%lf", &X);
	return X;
}

const char* csv_comment(parser_t* pp){
	if(!pp || pp->r == PARSER_STRING_EOF || pp->r >= PARSER_COMPOSE)
		return NULL;
	size_t i = 0;
	size_t len = pp->l;
	const char* str = pp->c;
	if(i >= len){
		pp->r = PARSER_STR_NULL;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return NULL;
	}
	char c = str[0];
	while(i < len && c != '#' && c != '\n' && c != '\0')
		c = str[i++];
	if(i >= len || c == '\n' || c == '\0'){
		pp->r = PARSER_STRING_EOF;
		pp->n = pp->l;
		pp->p = pp->l-1;
		return NULL;
	}
	pp->p = i;
	while(i < len && c != '\n' && c != '\0')
		c = str[i++];
	pp->n = i;
	pp->r = PARSER_STRING;
	return pp->c + pp->p;
}

int csv_reset(parser_t* pp, char* str, size_t len){
	if(!pp)
		return PARSER_BAD;
	memset(pp,0,sizeof(parser_t));
	pp->c = str;
	pp->l = len;
	return PARSER_OK;
}

int csv_reset_compose(parser_t* pp){
	if(!pp || pp->r != PARSER_COMPOSE || !pp->c)
		return PARSER_BAD;
	memset(pp->c, 0, pp->l);
	pp->n = 0;
	pp->p = 0;
	return PARSER_OK;
}

int csv_open(const char* fpath, parser_t* pp){
	if(!fpath || !pp)
		return PARSER_BAD;
	if(csv_fp){
		fclose(csv_fp);
		csv_fp = NULL;
	}
	csv_fp = fopen(fpath, "w");
	if(!csv_fp)
		return PARSER_BAD;
	pp->r = PARSER_COMPOSE;
	return PARSER_OK;
}

int csv_close(parser_t* pp){
	if(csv_fp && pp){
		fclose(csv_fp);
		csv_fp = NULL;
		pp->r = PARSER_NULL;
		return PARSER_OK;
	}
	return PARSER_BAD;
}

int csv_write(parser_t* p){
	if(!csv_fp || !p || p->r != PARSER_COMPOSE)
		return PARSER_BAD;
	size_t len = strlen(p->c)-1;
	fwrite(p->c, len, 1, csv_fp);
	char c = '\n';
	fwrite(&c, 1, 1, csv_fp);
	return PARSER_OK;
}

int csv_compose_integer(parser_t* p, long int i){
	if(!csv_fp || !p || p->r != PARSER_COMPOSE)
		return PARSER_BAD;
	char space[21];
	memset(space,0,21);
	snprintf(space, 21, "%ld", i);
	size_t len = strlen(space);
	if(p->p + len + 1 >= p->l) // strlen() + ';'
		return PARSER_BAD;

	memcpy(p->c+p->p, space, len);
	p->p += len;
	*(p->c+p->p) = ';';
	p->p++;
	return PARSER_OK;
}

int csv_compose_double(parser_t* p, double i){
	if(!csv_fp || !p || p->r != PARSER_COMPOSE)
		return PARSER_BAD;
	char space[512];
	memset(space,0,512);
	snprintf(space, 512, "%f", i);
	size_t len = strlen(space);
	if(p->p + len + 1 >= p->l) // strlen() + ';'
		return PARSER_BAD;

	memcpy(p->c+p->p, space, len);
	p->p += len;
	*(p->c+p->p) = ';';
	p->p++;
	return PARSER_OK;
}

int csv_compose_string(parser_t* p, const char* s){
	if(!csv_fp || !p || p->r != PARSER_COMPOSE)
		return PARSER_BAD;
	if(s == NULL)
		goto delim;
	size_t len = strlen(s);
	if(p->p + len + 1 >= p->l) // strlen() + ';'
		return PARSER_BAD;

	memcpy(p->c+p->p, s, len);
	p->p += len;
delim:
	*(p->c+p->p) = ';';
	p->p++;
	return PARSER_OK;
}
