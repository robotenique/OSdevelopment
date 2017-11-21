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

#include "process.h"
#include "error.h"


typedef struct pqclass {
    unsigned int n; // Last index in the PQ
    unsigned int length; // Absolute size of the array
    Process **pq; // the process queue itself

    bool(*isEmpty)(struct pqclass*);
    int(*size)(struct pqclass*);
    const Process (*min)(struct pqclass*);
    void(*insert)(struct pqclass*, Process *);
    Process *(*delMin)(struct pqclass*);
    int (*compare)(Process a, Process b);

} pqclass;

typedef pqclass* MinPQ;

/*-------- Fucntion definitions (public) -------- */
MinPQ new_MinPQ(int (*comp)(Process, Process));
void destroy_MinPQ(MinPQ);

#endif
