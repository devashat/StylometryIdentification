#include "node.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Node *node_create(char *word) {
    Node *node = (Node *) malloc(sizeof(struct Node));
    node->word = strdup(word);
    node->count = 0;

    return node;
}

void node_delete(Node **n) {
    free((*n));
    n = NULL;
}

void node_print(Node *n) {
    if (n != NULL) {
        printf("(%s:%d)", n->word, n->count);
    } else {
        printf("(null)");
    }
}
