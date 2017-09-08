/*
 * @author: João Gabriel
 * @author: Juliano Garcia
 *
 * MAC0422
 * 11/09/17
 *
 * Double linked queue implementation.
 */

#include "deque.h"

Queue new_queue() {
    Queue q = (Queue)emalloc(sizeof(Node));
    q->p = NULL;
    q->next = q;
    q->prev = q;
    return q;
}

Node *queue_first(Queue q) {
    if (q->prev == q)
        return NULL;
    return q->prev;
}

void queue_add(Queue q, Node *n) {
    n->prev = q;
    n->next = q->next;
    q->next->prev = n;
    q->next = n;
    if (!queue_first(q))
        q->prev = n;
}

int queue_remove(Queue q) {
    if (!queue_first(q))
        return 0;
    Node *tmp = q->prev;
    q->prev = tmp->prev;
    q->prev->next = q;
    return 1;
}

void queue_readd(Queue q) {
    Node *first = queue_first(q);
    queue_remove(q);
    queue_add(q, first);
}
