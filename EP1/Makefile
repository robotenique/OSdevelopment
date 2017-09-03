#
# Makefile para EP1 de S.O.
# TODO: Remove the '-g' flag when the work is done!
#

CFLAGS = -Wall -std=c11 -O2 -g


.PHONY: clean


all: ep1sh simproc


ep1sh: ep1sh.o error.o buffer.o
	$(CC) -o $@ $^ -lreadline

simproc: simproc.o error.o minPQ.o roundrobin.o
	$(CC) -o $@ $^ -lm -pthread 

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f *.o *~ *.out ep1sh simproc
