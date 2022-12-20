#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include "bv.h"
#include "bf.h"
#include "salts.h"
#include "speck.h"

struct BloomFilter {
    uint64_t primary[2];
    uint64_t secondary[2];
    uint64_t tertiary[2];
    BitVector *filter;
};

BloomFilter *bf_create(uint32_t size) {
    BloomFilter *bf = (BloomFilter *) malloc(sizeof(struct BloomFilter));

    bf->filter = (BitVector *) calloc(3, sizeof(BitVector *));

    bf->primary[0] = SALT_PRIMARY_LO;
    bf->primary[1] = SALT_PRIMARY_HI;
    bf->secondary[0] = SALT_SECONDARY_LO;
    bf->secondary[1] = SALT_SECONDARY_HI;
    bf->tertiary[0] = SALT_TERTIARY_LO;
    bf->tertiary[1] = SALT_TERTIARY_HI;

    bf->filter = bv_create(size);

    return bf;
}

void bf_delete(BloomFilter **bf) {
    bv_delete(&(*bf)->filter);
    free((*bf));
}

uint32_t bf_size(BloomFilter *bf) {
    return bv_length(bf->filter);
}

void bf_insert(BloomFilter *bf, char *word) {
    bv_set_bit(bf->filter, (hash(bf->primary, word) % bf_size(bf)));
    bv_set_bit(bf->filter, (hash(bf->secondary, word) % bf_size(bf)));
    bv_set_bit(bf->filter, (hash(bf->tertiary, word) % bf_size(bf)));
}

bool bf_probe(BloomFilter *bf, char *word) {
    bool word_exists = false;

    if (bv_get_bit(bf->filter, (hash(bf->primary, word) % bf_size(bf)))
        && bv_get_bit(bf->filter, (hash(bf->secondary, word) % bf_size(bf)))
        && bv_get_bit(bf->filter, (hash(bf->tertiary, word) % bf_size(bf)))) {
        word_exists = true;
    } else {
        word_exists = false;
    }
    return word_exists;
}

void bf_print(BloomFilter *bf) {
    printf("BloomFilter \n{");
    bv_print(bf->filter);
    printf("}\n");
}
