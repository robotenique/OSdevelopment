/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Header for some process structs and global variables.
 */
#ifndef _PROCESS_H_
#define _PROCESS_H_
#include <pthread.h>

typedef enum { false, true } bool;

typedef struct {
    char *name;
    double t0;
    double dt;
    double deadline;
    pthread_t pid;
    unsigned int nLine;
} Process;


struct pstruct{
    Process *v;
    int i; // Last position of the last process
    int size;
    int nextP; // Pointer to next Process in the array
};

typedef struct pstruct *ProcArray;

typedef struct node_t {
    Process *p;
    pthread_mutex_t mtx;
    pthread_t t;
    int CPU;
    struct node_t *next, *prev;
} Node;

typedef struct post_node_t {
    Node *n;
    bool ready;
} PNode;

#endif
