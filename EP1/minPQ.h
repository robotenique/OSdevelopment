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
    char *name;
    double t0;
    double dt;
    double deadline;
    unsigned int id;
} Process;

typedef enum { false, true } bool;

typedef struct pqclass {
    unsigned int n; // Last index in the PQ
    unsigned int length; // Absolute size of the array
    Process *pq; // the process queue itself

    bool(*isEmpty)(struct pqclass*);
    int(*size)(struct pqclass*);
    const Process (*min)(struct pqclass*);
    void(*insert)(struct pqclass*, Process p);
    Process(*delMin)(struct pqclass*);
    int (*compare)(Process a, Process b);

} pqclass;

typedef pqclass* MinPQ;

/*-------- Fucntion definitions (public) -------- */
MinPQ new_MinPQ(int (*comp)(Process, Process));
// TODO: remove this debug in the end
void debugPQ(MinPQ);


#endif
