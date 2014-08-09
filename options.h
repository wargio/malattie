/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef __OPTIONS_H__
#define __OPTIONS_H__

extern int real_args;
extern int avvia_console;
extern const char* fpath_pazienti;
extern const char* fpath_malattie;
extern const char* fpath_wpazienti;
extern const char* fpath_wmalattie;

void usage(void);
void pargs(int argc, char **argv);

#endif
