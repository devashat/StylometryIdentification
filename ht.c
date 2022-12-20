#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"
#include "ht.h"
#include "salts.h"
#include "speck.h"

struct HashTable {
    uint64_t salt[2];
    uint32_t size;
    Node **slots;
};

struct HashTableIterator {
    HashTable *table;
    uint32_t slot;
} r;

HashTable *ht_create(uint32_t size) {
    HashTable *ht = (HashTable *) malloc(sizeof(struct HashTable));
    ht->salt[0] = SALT_HASHTABLE_LO;
    ht->salt[1] = SALT_HASHTABLE_HI;
    ht->size = size;

    ht->slots = (Node **) malloc(size * sizeof(Node));

    memset(ht->slots, 0, size * sizeof(Node));

    return ht;
}

void ht_delete(HashTable **ht) {
    for (uint32_t i = 0; i < (*ht)->size; i++) {
        node_delete(&(*ht)->slots[i]);
    }
    free((*ht));
    ht = NULL;
}

uint32_t ht_size(HashTable *ht) {
    return ht->size;
}

Node *ht_lookup(HashTable *ht, char *word) {
    uint32_t index = hash(ht->salt, word);
    index %= ht->size;

    Node *n = ht->slots[index];

    while (n != NULL) {
        if (strcmp(n->word, word) == 0) {
            break;
        }
        index++;
        index %= ht->size;
        n = ht->slots[index];
    }

    return n;
}

Node *ht_insert(HashTable *ht, char *word) {
    if (ht == NULL) {
        return NULL;
    }

    uint32_t index = hash(ht->salt, word);
    index %= ht->size;
    Node *n = ht->slots[index];

    while (n != NULL) {
        if (strcmp(n->word, word) == 0) {
            n->count++;

            break;
        }
        index++;
        index %= ht->size;
        n = ht->slots[index];
    }

    if (n == NULL) {
        n = node_create(word);
        n->count++;
        ht->slots[index] = n;
    }

    return n;
}

void ht_print(HashTable *ht) {
    printf("HashTable  -> {\n");
    for (uint32_t i = 0; i < ht->size; i++) {
        Node *n = ht->slots[i];
        if (n != NULL) {
            printf("{[%d] -> ", i);
            node_print(ht->slots[i]);
            printf("}");
        }
    }
    printf("}\n");
}

HashTableIterator *hti_create(HashTable *ht) {
    HashTableIterator *hti = (HashTableIterator *) malloc(sizeof(struct HashTableIterator));
    hti->table = ht;
    hti->slot = 0;

    return hti;
}

void hti_delete(HashTableIterator **hti) {
    free((*hti));
    hti = NULL;
}

Node *ht_iter(HashTableIterator *hti) {
    Node *node = hti->table->slots[hti->slot];
    hti->slot++;
    return node;
}
