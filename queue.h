#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

struct part {
    int src, cuchy;
    struct part *prev, *next;
};

struct Queue {
    int size;
    struct part *head;
};

// tworzy kolejkę
struct Queue* createQueue();
// sprawdza czy lista jest pusta
int isEmpty(struct Queue *queue);
// dodaje element na koniec kolejki
void push_back(struct Queue *queue, int src, int cuchy);
// zdejmuje element z przodu kolejki
void pop_front(struct Queue *queue);
// zwraca pierwszy element w kolejce
struct part* front(struct Queue *queue);
// zwraca ostatni element w kolejce
struct part* back(struct Queue *queue);
// zwraca wybrany element w kolejce
struct part* find(struct Queue *queue, int i);

#endif
