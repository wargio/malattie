/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef __READ_FILE_H__
#define __READ_FILE_H__

int fload_malattie(const char* fpath);
int fload_pazienti(const char* fpath);

void fwrite_malattie(const char* fpath);
void fwrite_pazienti(const char* fpath);

#endif
