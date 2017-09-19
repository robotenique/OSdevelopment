#ifndef _STACK_H_
#define _STACK_H_

#include "process.h"
#include "error.h"

typedef struct stack_t {
    Node *v;
    int i, size;
} Stack;

/*
 * Function: new_stack
 * --------------------------------------------------------
 * Creates a new Node Stack
 *
 * @args size : The size of the Stack
 *
 * @return A pointer to the Stack
 */
Stack *new_stack(int);

/*
 * Function: stack_remove
 * --------------------------------------------------------
 * Pops the top Node of the Stack
 *
 * @args s : Stack*
 *
 * @return A pointer to the top Node
 */
Node *stack_remove(Stack*);

/*
 * Function: stack_top
 * --------------------------------------------------------
 * Returns the top Node of the Stack
 *
 * @args s : Stack*
 *
 * @return A pointer to the top Node
 */
Node *stack_top(Stack*);

#endif
