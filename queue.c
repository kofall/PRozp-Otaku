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
// zwraca element o danym indeksie z kolejki
struct part* get_value(struct Queue *queue, int idx){
    if(isEmpty(queue)){
        return NULL;
    }
    struct part* current=queue->head;
    for(int i=1;i<=idx;i++){
        current=current->next;
    }
    return current;
}

//usuwa element o danym indeksie z kolejki
void delete_idx(struct Queue* queue, int idx){
    if(isEmpty(queue) || queue->size<idx){
        return;
    }
    struct part* current=queue->head;
    for(int i=1; i<=idx; i++){
        current=current->next;
    }
    if(idx==0){
        queue->head=current->next;
    }
    else{
        current->prev->next=current->next;
    }
    queue->size--;
    free(current);
}