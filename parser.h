/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef __PARSER_H__
#define __PARSER_H__

#include <stdlib.h>

#define PARSER_OK		0
#define PARSER_BAD		1

enum {
	PARSER_NULL	= 0,
	PARSER_STRING,
	PARSER_STRING_EOF,
	PARSER_BAD_STRING,
	PARSER_LEN_ZERO,
	PARSER_STR_NULL,
	PARSER_COMPOSE,
	PARSER_COMPOSE_NO_SPACE,
} parser_types;

typedef struct _parser_t{
	int    r;		// return
	char* c;		// string
	size_t l;		// string length
	size_t p;		// position of the first word
	size_t n;		// punta a ';'
} parser_t;


/*
 * returns: parser_t*
 */
parser_t*   csv_init(char* str, size_t len);
/*
 * returns: PARSER_OK  se tutto va a buon fine
 *          PARSER_BAD se quacosa va storto.
 */
int         csv_destroy(parser_t* pp);
/*
 * returns: const char*
 */
const char* csv_full_string(parser_t* pp);
/*
 * returns: size_t 
 */
size_t      csv_string_len(parser_t* pp);
/*
 * returns: size_t
 */
size_t      csv_string_pos(parser_t* pp);
/*
 * returns: int (parser_types)
 */
int         csv_type(parser_t* pp);
/*
 * returns: const char*
 */
const char* csv_type_name(parser_t* pp);
const char* csv_next_string(parser_t* pp);
const char* csv_comment(parser_t* pp);
/*
 * returns: long int
 */
long int csv_next_integer(parser_t* pp);
/*
 * returns: double
 */
double csv_next_double(parser_t* pp);
/*
 * returns: PARSER_OK  se tutto va a buon fine
 *          PARSER_BAD se quacosa va storto.
 */
int csv_reset(parser_t* pp, char* str, size_t len);
int csv_open(const char* fpath, parser_t* pp);
int csv_close(parser_t* pp);
int csv_write(parser_t* p);
int csv_reset_compose(parser_t* pp);
int csv_compose_integer(parser_t* p, long int i);
int csv_compose_double(parser_t* p, double i);
int csv_compose_string(parser_t* p, const char* s);



#endif
