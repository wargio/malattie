/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */
#include <stdio.h>
#include "comando.h"
#include "copyright.h"
#include "funzioni.h"
#include "options.h"
#include "read_file.h"

void do_init(){
	fload_malattie(fpath_malattie);
	fload_pazienti(fpath_pazienti);
}

void do_end(){
	if(fpath_wmalattie)
		fwrite_malattie(fpath_wmalattie);
	if(fpath_wpazienti)
		fwrite_pazienti(fpath_wpazienti);
	free_malattie();
	free_pazienti();
}

int main(int argc, char** argv){
	pargs(argc, argv);
	printf(COPYWRIGHT_TXT);
	if(!avvia_console || real_args < 1)
		return 1;

	do_init();

	if(avvia_console)
		console();

	do_end();
	return 0;
}
