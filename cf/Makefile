V		=	@
FILES	=	cf
COMMON	=	Makefile 
CC		=	gcc
CFLAGS	=	-Wall -ggdb 
LDLIBS  =	-lm
OBJS	=	$(addsuffix .o, $(FILES)) main.o
DEPS	=	$(addsuffix .h, $(FILES))
EXEC	=	cf

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
	
.PHONY: all clean