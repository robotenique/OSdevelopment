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

/* -- Function signatures --- */
void pqclass_Init(MinPQ self);

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

/*
 * Function: new_MinPQ
 * --------------------------------------------------------
 * Creates a new MinPQ empty MinPQ and returns it. This
 * 'constructor' receives a pointer to a function to use
 * to compare the items in the PQ.
 *
 * @args  comp : pointer to a comparator function with the
 *               signature exactly as described. It should
 *               return > 0 if a > b, < 0 if a < b or 0 if
 *               a = b
 *
 * @return  a new empty MinPQ
 */
MinPQ new_MinPQ(int (*comp)(Process a, Process b)) {
    pqclass *self = emalloc(sizeof(pqclass));
    pqclass_Init(self);
    self->compare = comp;
    self->pq = emalloc(2*sizeof(Process));
    self->n = 0;
    self->length = 2;
    return self;
}
/*
 * Function: pqclass_Init
 * --------------------------------------------------------
 * Initialize the 'methods' of a MinPQ. It's an auxiliar
 * function just to set the correct pointers
 *
 * @args  self :  the MinPQ
 *
 * @return
 */
void pqclass_Init(MinPQ self){
    self->isEmpty = &isEmpty;
    self->size = &size;
    self->min = &min;
    self->delMin = &delMin;
    self->insert = &insert;
}
/*
 * Function: destroy_MinPQ
 * --------------------------------------------------------
 * Destroy a given MinPQ, removing the pointers, freeing the
 * internal heap and then freeing the MinPQ itself
 *
 * @args  self :  the MinPQ
 *
 * @return
 */
void destroy_MinPQ(MinPQ self){
    self->isEmpty = NULL;
    self->size = NULL;
    self->min = NULL;
    self->delMin = NULL;
    self->insert = NULL;
    free(self->pq);
    free(self);
}

/* -- public MinPQ methods --- */
/*
 * Function: isEmpty
 * --------------------------------------------------------
 * Returns true of the MinPQ is empty, false otherwise.
 *
 * @args self : the MinPQ
 *
 * @return true or false
 */
bool isEmpty(MinPQ self){
    return self->n == 0;
}
/*
 * Function: size
 * --------------------------------------------------------
 * Returns the size of the minPQ specified in the argument
 *
 * @args self : the MinPQ
 *
 * @return the size of the MinPQ
 */
int size(MinPQ self){
    return self->n;
}
/*
 * Function: min
 * --------------------------------------------------------
 * Returns the min element of the priority queue
 *
 * @args self : the MinPQ
 *
 * @return the minimum process of the priority queue
 */
const Process min(MinPQ self){
    if(self->isEmpty(self))
        die("Priority queue underflow!");
    return self->pq[1];
}
/*
 * Function: insert
 * --------------------------------------------------------
 * Insert a process into the priority queue
 *
 * @args self : the MinPQ
 *
 * @return
 */
void insert(MinPQ self, Process p){
    if(self->n == self->length - 1)
        resize(self, 2*self->length);
    self->n += 1;
    self->pq[self->n] = p;
    swim(self, self->n);
}
/*
 * Function: delMin
 * --------------------------------------------------------
 * Deletes and returns the process with the minimum Priority
 * in the MinPQ
 *
 * @args  self :  type
 *
 * @return the process with the minimum Priority
 */
Process delMin(MinPQ self){
    if(self->isEmpty(self))
        die("Priority queue underflow!");
    Process min = self->pq[1];
    swap(self, 1, self->n);
    self->n = self->n - 1;
    sink(self, 1);
    if((self->n > 0) && (self->n == (self->length - 1)/4))
        resize(self, self->length/2);
    return min;
}


/*----------- Private MinPQ methods -----------*/
/*
 * Function: resize
 * --------------------------------------------------------
 * Resize the internal heap of the MinPQ to a new capacity
 *
 * @args  self :  the MinPQ
 *        capacity:  the new capacity of the heap
 *
 * @return
 */
void resize(MinPQ self, int capacity){
    Process *temp = emalloc(capacity*sizeof(Process));
    for (int i = 1; i <= self->n; temp[i] = self->pq[i], i++);
    self->pq = temp;
    self->length = capacity;
}
/*
 * Function: greater
 * --------------------------------------------------------
 * Verify if the element at position i is greater than the
 * element at position j
 *
 * @args  self : the Minpq
 *        i    : the first position
 *        j    : the second position
 *
 * @return  true if pq[i] > pq[j] using the compare function.
 *          false otherwise.
 */
bool greater(MinPQ self, int i, int j){
    int compRes = self->compare(self->pq[i], self->pq[j]);
    return compRes > 0;
}
/*
 * Function: swap
 * --------------------------------------------------------
 * Swap two elements in the heap,
 *
 * @args  self :  the MinPQ
 *        i    : the first position
 *        j    : the second position
 *
 * @return
 */
void swap(MinPQ self, int i, int j){
    Process tmp = self->pq[i];
    self->pq[i] = self->pq[j];
    self->pq[j] = tmp;
}
/*
 * Function: swim
 * --------------------------------------------------------
 * Raises the element in the k-position to maintain the heap
 * in the right order
 *
 * @args  self :  the MinPQ
 *        i    : the position of the element
 *
 * @return
 */
void swim(MinPQ self, int k){
    while(k > 1 && greater(self, k/2, k)){
        swap(self, k, k/2);
        k = k/2;
    }
}
/*
 * Function: sink
 * --------------------------------------------------------
 * Decreases the level of the k-th element in the heap to
 * maintain the right order
 *
 * @args  self :  the MinPQ
 *        i    : the position of the element
 *
 * @return
 */
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
