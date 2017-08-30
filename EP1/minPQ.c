/*
* @author: João Gabriel
* @author: Juliano Garcia
*
* MAC0422
* 11/09/17
*
* Implementation of a MinPQ of processes, and Functions
* to peek the minimum, delete the minimum and insert
* elements
*/

#include "minPQ.h"
#include "error.h"
// TODO: REMOVE THE STDIO IMPORT AND DEBUGPQ FUNCTION
#include <stdio.h>

bool isEmpty(MinPQ);
int size(MinPQ);
const Process min(MinPQ);
void insert(MinPQ, Process);
Process delMin(MinPQ);
void resize(MinPQ , int);
void swap(MinPQ, int, int);
bool greater(MinPQ, int, int);
void swim(MinPQ , int);
void sink(MinPQ, int);

/* DEBUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUG */
bool isMinHeap(MinPQ self, int k){
    if(k > self->n) return true;
    int left = 2*k;
    int right = 2*k + 1;
    if(left <= self->n && greater(self, k, left)) return false;
    if(right <= self->n && greater(self, k, right)) return false;
    return (isMinHeap(self, left) && isMinHeap(self, right));
}

void debugPQ(MinPQ self) {
    printf("  ID  dt     NOME\n");
    printf("--------------------------------------------------\n");
    for (int i = 1; i <= self->n; i++) {
        Process p = self->pq[i];
        printf("  %02d  %.2lf  %s\n", p.id, p.dt, p.name);
    }
    int k = 1;
    bool isHeap = isMinHeap(self, k);
    if(isHeap)
        printf("\n-------> PARABÉNS! É UM MIN-HEAP :DDD\n.\n.\n.\n.\n.\n.\n");
    else
        printf("\n-------> BUGOUUUUUUUUU\n");
}



void pqclass_Init(pqclass* self){
    self->isEmpty = &isEmpty;
    self->size = &size;
    self->min = &min;
    self->delMin = &delMin;
    self->insert = &insert;
}

MinPQ new_MinPQ(int (*comp)(Process a, Process b)) {
    pqclass *self = emalloc(sizeof(pqclass));
    pqclass_Init(self);
    self->compare = comp;
    self->pq = emalloc(2*sizeof(Process));
    self->n = 0;
    self->length = 2;
    return self;
}

void destroy_MinPQ(MinPQ self){
    self->isEmpty = NULL;
    self->size = NULL;
    self->min = NULL;
    self->delMin = NULL;
    self->insert = NULL;
    free(self->pq);
}


/* -- min HEAP METHODS public --- */

bool isEmpty(MinPQ self){
    return self->n == 0;
}

int size(MinPQ self){
    return self->n;
}

const Process min(MinPQ self){
    if(self->isEmpty(self))
        die("Priority queue underflow!");
    return self->pq[1];
}

void insert(MinPQ self, Process p){
    if(self->n == self->length - 1)
        resize(self, 2*self->length);
    self->n += 1;
    self->pq[self->n] = p;
    swim(self, self->n);
    if(!isMinHeap(self, 1)) die("BUGO... INSERT");
}

Process delMin(MinPQ self){
    if(self->isEmpty(self))
        die("Priority queue underflow!");
    Process min = self->pq[1];
    swap(self, 1, self->n);
    self->n -= 1;
    sink(self, 1);
    if((self->n > 0) && (self->n == (self->length - 1)/4))
        resize(self, self->length/2);
    if(!isMinHeap(self, 1)) die("BUGO... delMIN");
    return min;
}


/*----------- PRIVATE CLASS METHODS -----------*/
void resize(MinPQ self, int capacity){
    Process *temp = emalloc(capacity*sizeof(Process));
    for (int i = 1; i <= self->n; temp[i] = self->pq[i], i++);
    self->pq = temp;
    self->length = capacity;
}

void swim(MinPQ self, int k){
    while(k > 1 && greater(self, k/2, k)){
        swap(self, k, k/2);
        k = k/2;
    }
}

bool greater(MinPQ self, int i, int j){
    int compRes = self->compare(self->pq[i], self->pq[j]);
    return compRes > 0;
}

void swap(MinPQ self, int i, int j){
    Process tmp = self->pq[i];
    self->pq[i] = self->pq[j];
    self->pq[j] = tmp;
}

void sink(MinPQ self, int k){
    while(2*k <= self->n){
        int j = 2*k;
        if(j < self->n && greater(self, j, j+1))
            j++;
        if(!greater(self, k, j))
            break;
        swap(self, k ,j);
        k = j;
    }
}



















bool isEmpty_MinPQ(MinPQ pq);
int size_MinPQ(MinPQ pq);
const Process *const min_MinPQ(MinPQ pq);
void insert_MinPQ(Process p);
Process delMin_MinPQ(MinPQ pq);
