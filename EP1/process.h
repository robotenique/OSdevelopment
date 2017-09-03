#ifndef _PROCESS_H_
#define _PROCESS_H_

typedef enum { false, true } bool;

typedef struct {
    char *name;
    double t0;
    double dt;
    double deadline;
    unsigned int nLine;
} Process;


struct pstruct{
    Process *v;
    int i; // Last position of array
    int size;
    int nextP;
};

typedef struct pstruct *ProcArray;

#endif
