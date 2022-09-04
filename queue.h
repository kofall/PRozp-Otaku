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
// zwraca wartosć spod danego indeksu z kolejki
struct part* get_value(struct Queue *queue, int idx);
// usuwa pozycje o danym indeksie
void part* delete_idx(struct Queue *queue, int idx);

#endif
