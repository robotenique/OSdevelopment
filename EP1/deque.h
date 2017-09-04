#ifndef _DEQUE_H_
#define _DEQUE_H_

#include "process.h"
#include "error.h"

typedef Node* Queue;

/*
 * Function: new_queue
 * --------------------------------------------------------
 * Creates a new Queue
 *
 * @args None
 *
 * @return  The new queue
 */
Queue new_queue();

/*
 * Function: queue_first
 * --------------------------------------------------------
 * Returns the first Node of the Queue (the last of the list)
 *
 * @args  q :  Queue
 *
 * @return  A pointer to the first Node
 */
Node *queue_first(Queue);

/*
 * Function: queue_add
 * --------------------------------------------------------
 * Enqueue a new Node
 *
 * @args n : A pointer to the new Node
 *
 * @return
 */
void queue_add(Queue, Node*);

/*
 * Function: queue_remove
 * --------------------------------------------------------
 * Removes the first Node of the Queue (the last of the list)
 * and returns 0 if the Queue is empty and 1 otherwise
 *
 * @args q : Queue
 *
 * @return 0 if the Queue is empty and 1 otherwise
 */
int queue_remove(Queue);

/*
 * Function: queue_readd
 * --------------------------------------------------------
 * Readds the first Node of the Queue to it
 *
 * @args q : Queue
 *
 * @return
 */
void queue_readd(Queue);

#endif
