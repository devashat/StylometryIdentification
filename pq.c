#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "pq.h"

typedef struct Author {
    char *name;
    double dist;
} Author;

struct PriorityQueue {
    uint32_t capacity;
    uint32_t count;
    Author **nodes;
};

Author *author_create(char *word, double dist) {
    Author *node = (Author *) malloc(sizeof(struct Author));
    node->name = strdup(word);
    node->dist = dist;

    return node;
}

void author_delete(Author **n) {
    free((*n));
    n = NULL;
}

void author_print(Author *n) {
    if (n) {
        printf("(%s:%f)\n", n->name, n->dist);
    }
}

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *queue = (PriorityQueue *) malloc(sizeof(PriorityQueue));

    if (queue) {
        queue->capacity = capacity;
        queue->count = 0;
        queue->nodes = (Author **) calloc(capacity, sizeof(Author));
    }

    return queue;
}

void pq_delete(PriorityQueue **q) {
    free((*q)->nodes);
    free(*q);
    *q = NULL;
}

bool pq_empty(PriorityQueue *q) {
    bool is_empty = true;

    if (q->count > 0)
        is_empty = false;

    return is_empty;
}

bool pq_full(PriorityQueue *q) {
    bool is_full = true;

    if (q->count < q->capacity)
        is_full = false;

    return is_full;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->count;
}

void swap(PriorityQueue *q, uint32_t src, uint32_t dst) {
    Author *tmp = q->nodes[src];
    q->nodes[src] = q->nodes[dst];
    q->nodes[dst] = tmp;
}

int max_child(PriorityQueue *q, int first, int last) {
    int left = 2 * first;
    int right = left + 1;

    if (right <= last && q->nodes[right - 1]->dist > q->nodes[left - 1]->dist) {
        return right;
    }
    return left;
}

int min_child(PriorityQueue *q, int first, int last) {
    int left = 2 * first;
    int right = left + 1;

    if (right <= last && q->nodes[right - 1]->dist < q->nodes[left - 1]->dist) {
        return right;
    }
    return left;
}

void fix_heap(PriorityQueue *q, int first, int last, bool max_sort) {
    bool found = false;
    int mother = first;
    int extreme = (max_sort) ? max_child(q, mother, last) : min_child(q, mother, last);

    while (mother <= floor(last / 2) && !found) {

        if ((q->nodes[mother - 1]->dist < q->nodes[extreme - 1]->dist && max_sort == true)
            || (q->nodes[mother - 1]->dist > q->nodes[extreme - 1]->dist && max_sort == false)) {
            swap(q, mother - 1, extreme - 1);
            mother = extreme;
            extreme = (max_sort) ? max_child(q, mother, last) : min_child(q, mother, last);
        } else {
            found = true;
        }
    }
}

void build_heap(PriorityQueue *q, int first, int last, bool max_sort) {

    for (int father = floor(last / 2); father > first - 1; father--) {
        fix_heap(q, father, last, max_sort);
    }
}

void heap_sort(PriorityQueue *q, bool max_sort) {

    uint32_t first = 1;
    uint32_t last = q->count;

    build_heap(q, first, last, max_sort);

    for (uint32_t leaf = last; leaf > first; leaf--) {
        swap(q, first - 1, leaf - 1);
        fix_heap(q, first, leaf - 1, max_sort);
    }
}

bool enqueue(PriorityQueue *q, char *author, double dist) {

    Author *n = author_create(author, dist);

    bool processed = false;

    if (pq_full(q) != true) {
        q->nodes[q->count] = n;
        q->count += 1;
        heap_sort(q, true);
        processed = true;
    }
    return processed;
}

bool dequeue(PriorityQueue *q, char **author, double *dist) {
    Author *n = NULL;
    bool processed = false;

    if (pq_empty(q) == false) {
        heap_sort(q, false);
        n = (Author *) q->nodes[q->count - 1];
        *author = n->name;
        *dist = n->dist;
        q->count -= 1;
        heap_sort(q, false);
        processed = true;
    }

    return processed;
}
