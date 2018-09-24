CC=gcc
CFLAGS=-Wall -pedantic -g -O3
all: pipeit clean

pipeit: pipeit.o

pipeit.o: pipeit.c

clean:
	-rm *.o