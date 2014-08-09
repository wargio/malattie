/*
 * Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
 * Licensed under the terms of the GNU GPL, version 2
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt
 */

#ifndef __COMANDO_H__
#define __COMANDO_H__

#define PROMPT							"> "
#define CMD_LENGTH						64
#define CMD_LENGTH_CPY					(CMD_LENGTH-1)

#define CMD_FOUND						 0
#define CMD_NOT_FOUND					 1
#define CMD_EXIT						-1

void console(void);

#endif
