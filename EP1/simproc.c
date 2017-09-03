/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Process scheduler simulator!
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "process.h"
#include "minPQ.h"
#include "error.h"
#include "roundrobin.h"
#include "sjf.h"

int comparator(Process, Process);
int cmp_ProcArray(const void *, const void *);
ProcArray create_ProcArray(char *);
void resizeProcArray(ProcArray, int);
void insertProcArray(ProcArray, char *, int);
void destroy_ProcArray(ProcArray self);

int main(int argc, char const *argv[]) {
    set_prog_name("simproc");
    DEBUG_MODE = false;
    if(argc < 4)
        die("Wrong number of arguments! \n Usage ./simproc <schedulerID> <traceFile> <outputFile> <d(optional)>");
    int schedType = atoi(argv[1]);
    char *infile = estrdup(argv[2]);
    char *outfile = estrdup(argv[3]);
    if(argc >= 5 && !strcmp(argv[4], "d"))
        DEBUG_MODE = true;
    ProcArray readyJobs = create_ProcArray(infile);
    readyJobs->nextP = 0;
    switch (schedType) {
        case 1:
            schedulerSJF(readyJobs, outfile);
            break;
        case 2:
            schedulerRoundRobin(readyJobs, outfile);
            break;
        case 3:
            //schedulerPriority(readyJobs, outfile);
            break;
        default:
            die("The scheduler algorithm id specified is invalid!");
    }

    //debug(readyJobs);
    /*
    // create a minPQ
    MinPQ pq = new_MinPQ(&comparator);
    for(int i = 0; i <= 1220; i++){
        // testing the minPQ
        Process ptest;
        char temp[20] = "";
        sprintf(temp, "Processo t%02i", i);
        ptest.nLine = i;
        ptest.t0 = 5*i;
        ptest.dt = -0.0045*(i*i) + 9*i; // parabola >>SUAVE<<
        ptest.deadline = 45;
        ptest.name = estrdup(temp);
        // insert into pq
        pq->insert(pq, ptest);
    }
    for(int i = 0; i <= 1200; i++)
        pq->delMin(pq);

    destroy_MinPQ(pq);
    */
    destroy_ProcArray(readyJobs);

    return 0;
}
/*
 * Function: comparator
 * --------------------------------------------------------
 * Comparator for use in the MinPQ. This is what defines
 * the priority of the MinPQ, after all!
 *
 * @args a : the first process
 *       b : the second process
 *
 * @return 1 if a > b, -1 if b < a, 0 otherwise
 */
int comparator(Process a, Process b){
    if (a.dt < b.dt)
        return -1;
    else if (a.dt > b.dt)
        return 1;
    return 0;
}
/*
 * Function: cmp_ProcArray
 * --------------------------------------------------------
 * Function to use as comparator in the qsort of the ProcArray
 * while being created. Sorts first by t0, then by dt.
 *
 * @args a : the first process
 *       b : the second process
 *
 * @return 1 if a > b, -1 if b < a, 0 otherwise
 */
int cmp_ProcArray(const void *a, const void *b){
    Process p1 = *(Process*)a;
    Process p2 = *(Process*)b;
    if(p1.t0 < p2.t0)
        return -1;
    if(p1.t0 > p2.t0)
        return 1;
    if(p1.dt < p2.dt)
        return -1;
    if(p1.dt > p2.dt)
        return 1;
    return 0;
}

/* Entry reading realated functions */
/*
 * Function: create_ProcArray
 * --------------------------------------------------------
 * Create a ProcArray, reading the processes from the passed
 * file. Doesn't check for errors, assuming the entry was
 * passed correct!
 * The list of processes is sorted by the t0, and if the t0
 * is equal, is sorted by dt to optimize the insert operation
 * into the process queue later.
 *
 * @args filename : The name of the trace file
 *
 * @return  The ProcArray with the processes to simulate
 */
ProcArray create_ProcArray(char *filename) {
    ProcArray temp = emalloc(sizeof(struct pstruct));
    temp->i = 0;
    temp->size = 1;
    temp->v = emalloc(sizeof(Process));
    FILE *fp;
    char buff[255];
    fp = efopen(filename,"r");

    int lNumber = 0;
    while(fgets(buff, 255, fp) != NULL)
        insertProcArray(temp, buff, lNumber++);
    qsort(temp->v, temp->i, sizeof(Process), cmp_ProcArray);
    fclose(fp);
    return temp;
}

/*
 * Function: insertProcArray
 * --------------------------------------------------------
 * Inserts a process in the ProcArray, by providing the line
 * with the format specified in the assignment description.
 * No errors checks are made, so the format in the string must
 * be met for the insertion to work correctly.
 *
 * @args  self : the ProcArray
 *        line : the line of the trace file
 *        lNumber : the number of the line in the trace file
 *
 * @return  returned value
 */
void insertProcArray(ProcArray self, char *line, int lNumber){
    Process p;
    if(self->i >= self->size)
        resizeProcArray(self, self->size*2);
    const char sep[2] = " ";
    p.nLine = lNumber;
    p.t0 = strtod(strtok(line, sep), NULL);
    p.dt = strtod(strtok(NULL, sep), NULL);
    p.deadline = strtod(strtok(NULL, sep), NULL);
    char *name = strtok(NULL, sep);
    name[strlen(name) - 1] = 0;
    p.name = estrdup(name);
    self->v[self->i] = p;
    self->i += 1;
}
/*
 * Function: resizeProcArray
 * --------------------------------------------------------
 * Resize the ProcArray to the new capacity.
 *
 * @args self : the ProcArray
 *       capacity  : the new capacity of the ProcArray
 *
 * @return
 */
void resizeProcArray(ProcArray self, int capacity){
    Process *t = emalloc(sizeof(Process)*capacity);
    for (int i = 0; i < self->i; t[i] = self->v[i], i++);
    free(self->v);
    self->v = t;
    self->size = capacity;
}
/*
 * Function: destroy_ProcArray
 * --------------------------------------------------------
 * Destroys the ProcArray provided, freeing its memory
 *
 * @args self : the ProcArray
 *
 * @return
 */
void destroy_ProcArray(ProcArray self) {
    free(self->v);
    free(self);
}
