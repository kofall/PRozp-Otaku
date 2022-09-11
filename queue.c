#include "queue.h"
#include "main.h"

// tworzy kolejkę
struct Queue* create_queue() {
    struct Queue *queue = (struct Queue*)malloc(sizeof(struct Queue));
    queue->size = 0;
}

// sprawdza czy lista jest pusta
int is_empty(struct Queue* queue) {
    if(queue->size == 0) {
        return 1;
    }
    return 0;
}

// tworzy nowy element kolejki
struct part* create_part(int ts, int src, int cuchy) {
    struct part *new = (struct part*)malloc(sizeof(struct part));
    new->ts = ts;
    new->src = src;
    new->cuchy = cuchy;
    new->prev = NULL;
    new->next = NULL;
    return new;
}

// dodaje element według etykiety czasowej lub jeżeli są równe to według id procesu
void push_by_time(struct Queue *queue, int ts, int src, int cuchy) {
    struct part *new = create_part(ts, src, cuchy);
    struct part *p = queue->head;
    // kolejka jest pusta
    if(is_empty(queue)) {
        queue->head = new;
        queue->size++;
        return;
    }
    // znajdź pierwsze żądanie z większą etykietą czasową lub jeżeli są równe to z większym id procesu
    while(p->next && (p->ts < ts || (p->ts == ts && p->src < src))) {
        p = p->next;
    }
    
    // nie znaleziono żądania o większej etykiecie czasowej lub jeżeli są równe to z większym id procesu, więc wstawiamy żądanie na koniec kolejki
    if(p->ts < ts || (p->ts == ts && p->src < src)) {
        p->next = new;
        new->prev = p;
    }
    // znaleziono żądanie o większej etykiecie czasowej 
    else {
        new->next = p;
        new->prev = p->prev;
        // żądanie jest na początku kolejki
        if(!p->prev) {
            queue->head = new;
        }
        // żądanie nie jest na początku kolejki
        else {
            p->prev->next = new;
        }
        p->prev = new;
    }
    queue->size++;
}

// zdejmuje element według id procesu
void pop_by_src(struct Queue *queue, int src) {
    if(is_empty(queue)) {
        debug("!!! Próba usunięcia żądania %d z pustej kolejki !!!", src);
        return;
    }
    struct part* p = get_by_src(queue, src);
    if(p) {
        if(!p->prev) {
            queue->head = p->next;
        } else {
            p->prev->next = p->next;
        }
        if(p->next) {
            p->next->prev = p->prev;
        }
        queue->size--;
        // free(p);
    }
}

// zwraca pierwszy element w kolejce
struct part* front(struct Queue *queue) {
    return queue->head;
}

// zwraca wybrany element w kolejce według pozycji
struct part* get_by_id(struct Queue *queue, int i) {
    struct part *p = queue->head;
    if(is_empty(queue)) {
        debug("!!! Próba pobrania żądanie według pozycji %d na pustej kolejce !!!", i);
        return NULL;
    }
    while(p && --i >= 0) {
        p = p->next;
    }
    return p;
}

// zwraca wybrany element w kolejce według id procesu
struct part* get_by_src(struct Queue *queue, int src) {
    struct part *p = queue->head;
    if(is_empty(queue)) {
        debug("!!! Próba pobrania żądanie według id procesu %d na pustej kolejce !!!", src);
        return NULL;
    }
    while(p && p->src != src) {
        p = p->next;
    }
    return p;
}


// zwraca indeks elemntu w kolejce według id procesu
int find_by_src(struct Queue *queue, int src) {
    struct part *p = queue->head;
    int i = 0;
    if(is_empty(queue)) {
        debug("!!! Próba pobrania pozycji żądania według id procesu %d na pustej kolejce !!!", src);
        return -1;
    }
    while(p->next && p->src != src) {
        p = p->next;
        i++;
    }
    return i;
}

// wypisuje kolejkę
void print_queue(struct Queue *queue) {
    struct part *p = queue->head;
    debug("-------KOLEJKA-------");
    if(is_empty(queue)) {
        return;
    }
    while(p) {
        debug(" id:%d | ts:%-5d | c:%d ", p->src, p->ts, p->cuchy);
        p = p->next;
    }
}