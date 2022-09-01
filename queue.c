#include "queue.h"

// tworzy kolejkę
struct Queue* createQueue() {
    struct Queue *queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->size = 0;
}

// sprawdza czy lista jest pusta
int isEmpty(struct Queue* queue) {
    if(queue->size == 0)
        return 1;
    return 0;
}

// dodaje element na koniec kolejki i zwraca pierwszy elment listy
void push_back(struct Queue* queue, int src, int cuchy) {
    struct part *last = back(queue);
    struct part *new = (struct part*)malloc(sizeof(struct part));
    new->next = NULL;
    if(isEmpty(queue)) {
        queue->head = new;
        new->prev = NULL;
    } else {
        new->prev = last;
        last->next = new;
    }
    new->src = src;
    new->cuchy = cuchy;
    queue->size += 1;
}

// zdejmuje element z przodu kolejki
void pop_front(struct Queue *queue) {
    if(isEmpty(queue))
        return;
    struct part *curr = queue->head;
    queue->head = curr->next;
    if(queue->head != NULL)
        queue->head->prev = NULL;
    queue->size -= 1;
    free(curr);
}

// zwraca pierwszy element w kolejce
struct part* front(struct Queue *queue) {
    return queue->head;
}

// zwraca ostatni element w kolejce
struct part* back(struct Queue *queue) {
    struct part *p = queue->head;
    if(isEmpty(queue))
        return NULL;
    while(p->next != NULL) {
        p = p->next;
    }
    return p;
}