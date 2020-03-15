CC = gcc
CFLAGS = -I. -g -Wall
.SUFFIXES: .c .o

all: master bin_adder

master: master.o
	$(CC) $(CLFAGS) -o $@ master.o

bin_adder: bin_adder.o
	$(CC) $(CFLAGS) -o $@ bin_adder.o

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o master bin_adder
