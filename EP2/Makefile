#
# Makefile para EP1 de S.O.
#
CC=gcc
CFLAGS = -Wall -pedantic -O2 -std=gnu11
LDFLAGS = -lrt -lm -pthread

.PHONY: clean


all: bikeSim


bikeSim: bikeSim.o error.o randomizer.o bikeStructures.o
	$(CC) -o $@ $^ -lrt

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@


%.o: %.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@


clean:
	rm -f *.o *~ *.out bikeSim
