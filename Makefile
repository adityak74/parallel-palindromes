# Makefile for compiling forkos programs
# Aditya Karnam
# September, 2017
# Added extra function to clen .log and .out files for easy testing

CC	= gcc
TARGETS	= master palin 
OBJS	= master.o palin.o
SRCDIR  = src
HEADER = shm_header.h

all: $(TARGETS)

$(TARGETS): % : %.o
		$(CC) -o $@ $<

$(OBJS) : %.o : $(SRCDIR)/%.c
		$(CC) -c $<

clean:
		/bin/rm -f *.o $(TARGETS) *.log *.out

cleanobj: 
		/bin/rm -f *.o