#
# @author: João Gabriel Basi Nº USP: 9793801
# @author: Juliano Garcia de Oliveira Nº USP: 9277086
# Makefile para EP1 de S.O.
#
# TODO: REMOVE -g FLAG
CC=gcc
CFLAGS = -Wall -pedantic -g -std=gnu11
LDFLAGS = -lrt -lm -pthread

.PHONY: clean


all: bikeSim

graph: test.o error.o randomizer.o bikeStructures.o biker.o debugger.o graph.o
		$(CC) -o $@ $^ $(LDFLAGS)


bikeSim: bikeSim.o error.o randomizer.o bikeStructures.o biker.o debugger.o graph.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS)


clean:
	rm -f *.o *~ *.out bikeSim graph
