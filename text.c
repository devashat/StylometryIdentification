#include <regex.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "bf.h"
#include "ht.h"
#include "bv.h"
#include "metric.h"
#include "text.h"
#include "parser.h"
#define WORD_EXP "[A-Za-z]+(['_-]?([A-Za-z])+)+"

struct Text {
    HashTable *ht;
    BloomFilter *bf;
    uint32_t word_count;
};

uint32_t noiselimit = 100;

char *strtoLower(char *str) {
    size_t len = strlen(str);

    char *str_l = calloc(len + 1, sizeof(char));

    for (size_t i = 0; i < len; ++i) {
        str_l[i] = tolower((unsigned char) str[i]);
    }
    return str_l;
}

Text *text_create(FILE *infile, Text *noise) {
    uint32_t ht_size = pow((double) 2, (double) 19);
    uint32_t bf_size = pow((double) 2, (double) 21);

    regex_t re;
    if (regcomp(&re, WORD_EXP, REG_EXTENDED)) {
        fprintf(stderr, " Failed to compile regex .\n ");
    }

    char *word = NULL;

    Text *text = (Text *) malloc(sizeof(struct Text));
    text->ht = ht_create(ht_size);
    text->bf = bf_create(bf_size);
    text->word_count = 0;

    if (text == NULL)
        return NULL;

    while ((word = next_word(infile, &re)) != NULL) {
        char *word_l = strtoLower(word);

        bool add_word = true;

        if (noise != NULL) {
            if (text_contains(noise, word_l) == true) {
                add_word = false;
            }
        }

        if (add_word == true) {
            if (text_contains(text, word_l) == false) {
                text->word_count++;
            }

            ht_insert(text->ht, word_l);
            bf_insert(text->bf, word_l);
        }

        if (text->word_count >= noiselimit && noise == NULL) {
            break;
        }
    }

    return text;
}

void text_delete(Text **text) {
    ht_delete(&((*text)->ht));
    bf_delete((&(*text)->bf));
    free(text);
    text = NULL;
}

uint32_t total_count(Text *text) {

    uint32_t total_count = 0;

    HashTableIterator *hti = hti_create(text->ht);

    for (uint32_t i = 0; i < ht_size(text->ht); i++) {
        Node *n = ht_iter(hti);
        if (n != NULL) {
            total_count += n->count;
        }
    }

    return total_count;
}

double text_dist_m(Text *text1, Text *text2) {
    double distance = 0;

    uint32_t text1_count = total_count(text1);
    uint32_t text2_count = total_count(text2);

    uint32_t total = text1->word_count + text2->word_count;

    HashTable *processed = ht_create(total);

    HashTableIterator *hti1 = hti_create(text1->ht);

    for (uint32_t i = 0; i < ht_size(text1->ht); i++) {
        Node *n = ht_iter(hti1);
        if (n != NULL) {
            char *word = n->word;
            uint32_t frequency1 = n->count;
            uint32_t frequency2 = text_frequency(text2, word);
            distance
                += fabs(((double) frequency1 / text1_count) - ((double) frequency2 / text2_count));

            ht_insert(processed, word);
        }
    }

    HashTableIterator *hti2 = hti_create(text2->ht);

    for (uint32_t i = 0; i < ht_size(text2->ht); i++) {
        Node *n = ht_iter(hti2);

        if (n != NULL) {
            char *word = n->word;

            Node *tmp_n = ht_lookup(processed, word);
            bool w_exists = true;
            if (tmp_n == NULL)
                w_exists = false;

            if (w_exists == false) {
                uint32_t frequency2 = n->count;
                uint32_t frequency1 = text_frequency(text1, word);
                distance += fabs(
                    ((double) frequency1 / text1_count) - ((double) frequency2 / text2_count));
            }
        }
    }

    return distance;
}

double text_dist_e(Text *text1, Text *text2) {
    double distance = 0;
    double sum_sqrd = 0;

    uint32_t text1_count = total_count(text1);
    uint32_t text2_count = total_count(text2);

    uint32_t total = text1->word_count + text2->word_count;
    HashTable *processed = ht_create(total);

    HashTableIterator *hti1 = hti_create(text1->ht);

    for (uint32_t i = 0; i < ht_size(text1->ht); i++) {
        Node *n = ht_iter(hti1);
        if (n != NULL) {
            char *word = n->word;
            uint32_t frequency1 = n->count;
            uint32_t frequency2 = text_frequency(text2, word);
            sum_sqrd += pow(
                ((double) frequency1 / text1_count) - ((double) frequency2 / text2_count), 2);

            ht_insert(processed, word);
        }
    }
    HashTableIterator *hti2 = hti_create(text2->ht);

    for (uint32_t i = 0; i < ht_size(text2->ht); i++) {
        Node *n = ht_iter(hti2);
        if (n != NULL) {
            char *word = n->word;

            Node *tmp_n = ht_lookup(processed, word);
            bool w_exists = true;
            if (tmp_n == NULL)
                w_exists = false;

            if (w_exists == false) {
                uint32_t frequency2 = n->count;
                uint32_t frequency1 = text_frequency(text1, word);
                sum_sqrd += pow(
                    ((double) frequency1 / text1_count) - ((double) frequency2 / text2_count), 2);
            }
        }
    }

    distance = sqrt(sum_sqrd);

    return distance;
}

double text_dist_c(Text *text1, Text *text2) {
    double distance = 0;
    double cos_sim = 0;

    uint32_t text1_count = total_count(text1);
    uint32_t text2_count = total_count(text2);

    uint32_t total = text1->word_count + text2->word_count;
    HashTable *processed = ht_create(total);

    HashTableIterator *hti1 = hti_create(text1->ht);

    for (uint32_t i = 0; i < ht_size(text1->ht); i++) {
        Node *n = ht_iter(hti1);
        if (n != NULL) {
            char *word = n->word;
            uint32_t frequency1 = n->count;
            uint32_t frequency2 = text_frequency(text2, word);
            cos_sim += ((double) frequency1 / text1_count) * ((double) frequency2 / text2_count);

            ht_insert(processed, word);
        }
    }

    HashTableIterator *hti2 = hti_create(text2->ht);

    for (uint32_t i = 0; i < ht_size(text2->ht); i++) {
        Node *n = ht_iter(hti2);
        if (n != NULL) {
            char *word = n->word;

            Node *tmp_n = ht_lookup(processed, word);
            bool w_exists = true;
            if (tmp_n == NULL)
                w_exists = false;

            if (w_exists == false) {
                uint32_t frequency2 = n->count;
                uint32_t frequency1 = text_frequency(text1, word);
                cos_sim
                    += ((double) frequency1 / text1_count) * ((double) frequency2 / text2_count);
            }
        }
    }

    distance = 1 - cos_sim;

    return distance;
}

double text_dist(Text *text1, Text *text2, Metric metric) {

    double distance = 0.0;

    if (metric == EUCLIDEAN) {
        distance = text_dist_e(text1, text2);
    } else if (metric == MANHATTAN) {
        distance = text_dist_m(text1, text2);
    } else if (metric == COSINE) {
        distance = text_dist_c(text1, text2);
    }

    return distance;
}

double text_frequency(Text *text, char *word) {
    uint32_t frequency = 0;

    if (text_contains(text, word) == 1) {
        Node *slot = ht_lookup(text->ht, word);
        if (slot != NULL)
            frequency = slot->count;
    }

    return frequency;
}

bool text_contains(Text *text, char *word) {
    return bf_probe(text->bf, word);
}

void text_print(Text *text) {
    printf("Text \n{");
    ht_print(text->ht);
    printf("}\n");
}
