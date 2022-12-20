#include "bv.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

struct BitVector {
    uint8_t *vector;
    uint32_t length;
};

BitVector *bv_create(uint32_t length) {
    BitVector *bv = (BitVector *) malloc(sizeof(BitVector));
    if (bv) {
        uint32_t bytes = length / 8 + (length % 8 ? 1 : 0);
        bv->vector = (uint8_t *) calloc(bytes, sizeof(uint8_t));
        bv->length = length;

        for (uint32_t i = 0; i < length; i++) {
            bv_clr_bit(bv, i);
        }

        return bv;

    } else {
        return NULL;
    }
}

void bv_delete(BitVector **bv) {
    if (bv && (*bv)->vector) {
        free((*bv)->vector);
    }
    if (bv) {
        free(bv);
    }
    bv = NULL;
}

uint32_t bv_length(BitVector *bv) {
    return bv ? bv->length : 0;
}

bool bv_set_bit(BitVector *bv, uint32_t i) {
    bool return_value = false;

    if (bv && bv->vector) {
        if (i < bv->length) {
            bv->vector[i / 8] |= (0x1 << i % 8);
            return_value = true;
        }
    }

    return return_value;
}

bool bv_clr_bit(BitVector *bv, uint32_t i) {
    bool return_value = false;

    if (bv && bv->vector) {
        if (i < bv->length) {
            bv->vector[i / 8] &= ~(0x1 << i % 8);
            return_value = true;
        }
    }

    return return_value;
}

bool bv_get_bit(BitVector *bv, uint32_t i) {
    bool return_value = false;

    if (bv && bv->vector) {
        if (i < bv->length) {
            return_value = (bv->vector[i / 8] >> i % 8) & 0x1;
        }
    }

    return return_value;
}

void bv_print(BitVector *bv) {
    for (uint32_t i = 0; i < bv->length; i++) {
        printf("%d", bv_get_bit(bv, i));
    }
}
