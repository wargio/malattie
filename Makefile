
## Copyright 2014, Grazioli Giovanni Dante <wargio@libero.it>
## Licensed under the terms of the GNU GPL, version 2
## http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt

V		=	@
FILES	=	funzioni speciali parser rw_file options
FILES	+=	comando
COMMON	=	Makefile strutture.h copyright.h
CC		=	gcc
CFLAGS	=	-Wall -ggdb -DDEBUG
LDLIBS  =	-lm
OBJS	=	$(addsuffix .o, $(FILES)) main.o
DEPS	=	$(addsuffix .h, $(FILES))
EXEC	=	malattie

all: $(OBJS)
	@echo [LD] $(EXEC)
	$(V)$(CC) $(CFLAGS) -o $(EXEC) $(OBJS) $(LDLIBS)
	@echo [OK]

$(OBJS): %.o: %.c $(DEPS) $(COMMON)
	@echo [CC] $@
	$(V)$(CC) $(CFLAGS) -c -o $@ $< 
	
clean:
	@echo [RM] $(OBJS) $(EXEC) 
	$(V)rm -f $(OBJS) $(EXEC) *~ *.stackdump
	$(V)make --no-print-directory -C cf clean
	
make-cf:
	@echo [MK] cf
	$(V)make --no-print-directory -C cf

.PHONY: all clean