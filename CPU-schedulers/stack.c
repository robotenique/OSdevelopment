/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Node stack implementation.
 */

#include "stack.h"

Stack *new_stack(int size) {
    Stack *s = emalloc(sizeof(Stack));
    s->v = emalloc(size*sizeof(Node));
    s->i = size;
    s->size = size;
    return s;
}

Node *stack_remove(Stack *s) {
    if (s->i == 0)
        return NULL;
    (s->i)--;
    return &(s->v[s->i]);
}

Node *stack_top(Stack *s) {
    if (s->i == 0)
        return NULL;
    return &(s->v[s->i - 1]);
}
