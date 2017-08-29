/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Header file of a Min Priority Queue
 */

#ifndef __MIN_PQ_H__
#define __MIN_PQ_H__

/*-------- Types definitions -------- */
typedef struct {
    char **name;
    double t0;
    double dt;
    double deadline;
    unsigned int id;
} Process;

typedef struct pqimpl_s *MinPQ;

typedef enum { false, true } bool;

/*-------- Functions -------- */
MinPQ create_MinPQ(int initCapacity, int (*compare)(Process a, Process b));
bool isEmpty_MinPQ(MinPQ pq);
int size_MinPQ(MinPQ pq);
const Process *const min_MinPQ(MinPQ pq);
void insert_MinPQ(Process p);
Process delMin_MinPQ(MinPQ pq);


#endif
