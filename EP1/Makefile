#
# Makefile para EP1 de S.O.
# TODO: Remove the '-g' flag when the work is done!
#
CC=gcc
CFLAGS = -Wall -pedantic -O2 -std=gnu11
LDFLAGS = -lrt -lm -pthread

.PHONY: clean


all: ep1sh ep1


ep1sh: ep1sh.o error.o buffer.o
	$(CC) -o $@ $^ -lrt -lreadline

ep1: ep1.o error.o utilities.o minPQ.o roundrobin.o sjf.o priorityScheduler.o stack.o deque.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@


%.o: %.c
	$(CC) $(CFLAGS) -c $< $(LDFLAGS) -o $@


clean:
	rm -f *.o *~ *.out ep1sh ep1
