#include "stack.h"

Stack *new_stack(int size) {
    Stack *s = emalloc(sizeof(Stack));
    s->v = emalloc(size*sizeof(Node));
    s->i = size;
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
