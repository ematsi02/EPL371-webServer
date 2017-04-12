########################################################
# Makefile for compiling the program skeleton
# 'make' build executable file 'PROJ'
# 'make clean' removes all .o, executable and doxy log
# 'make debug' debugs the program
########################################################

PROJ = server # name of the project
CC = gcc # name of the compiler

# define any compile-time flags
CFLAGS = -g -W
LFLAGS = -lm -lpthread
DFLAGSON = -DDEBUG
DFLAGSOFF = -DNDEBUG

# to create the executable file we need the individual object files
server : server.o	
	$(CC) server.o -Wall $(LFLAGS) -o server

# to create each individual object file we need to compile these files
# using the following general purpose macro
server.o : server.c server.h
	$(CC) $(CFLAGS) $(DFLAGSOFF) -c server.c -o server.o

# to clean .o files: "make clean"
clean:
	rm -rf *.o
	rm server
	rm tests

# to debug: "make debug"
debug: 
	$(CC) $(CFLAGS) $(DFLAGSON) -c tests.c -o tests.o
	$(CC) tests.o -Wall $(LFLAGS) -o tests
