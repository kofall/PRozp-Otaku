#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>

struct part {
    int ts, src, cuchy;
    struct part *prev, *next;
};

struct Queue {
    int size;
    struct part *head;
};

// tworzy kolejkę
struct Queue* create_queue();
// sprawdza czy lista jest pusta
int is_empty(struct Queue *queue);
// dodaje element według etykiety czasowej lub jeżeli są równe to według id procesu
void push_by_time(struct Queue *queue, int ts, int src, int cuchy);
// zdejmuje element według id procesu
void pop_by_src(struct Queue *queue, int src);
// zwraca pierwszy element w kolejce
struct part* front(struct Queue *queue);
// zwraca wybrany element w kolejce według pozycji
struct part* get_by_id(struct Queue *queue, int i);
// zwraca wybrany element w kolejce według id procesu
struct part* get_by_src(struct Queue *queue, int src);
// zwraca indeks elemntu w kolejce według id procesu
int find_by_src(struct Queue *queue, int src);
// wypisuje kolejkę
void print_queue(struct Queue *queue);

#endif
