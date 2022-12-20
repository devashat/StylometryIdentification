#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "metric.h"
#include "text.h"
#include "pq.h"

int main(int argc, char **argv) {
    int opt = 0;
    char *database_fn = "lib.db", *noise_fn = "noise.txt", author_name[256], author_fn[256];
    FILE *database_f = NULL, *noise_f = NULL, *author_f = NULL;
    uint32_t matches = 5, author_count = 0;
    Metric metric = EUCLIDEAN;
    bool default_metric = true;

    while ((opt = getopt(argc, argv, "d:n:k:l:emch")) != -1) {
        switch (opt) {
        case 'd': database_fn = optarg; break;
        case 'n': noise_fn = optarg; break;
        case 'k': matches = atol(optarg); break;
        case 'l': noiselimit = atol(optarg); break;
        case 'e':
            if (default_metric == false) {
                fprintf(stderr, "More than one metric provided\n");
                return 1;
            } else {
                metric = EUCLIDEAN;
                default_metric = false;
            }
            break;
        case 'm':
            if (default_metric == false) {
                fprintf(stderr, "More than one metric provided\n");
                return 1;
            } else {
                metric = MANHATTAN;
                default_metric = false;
            }
            break;
        case 'c':
            if (default_metric == false) {
                fprintf(stderr, "More than one metric provided\n");
                return 1;
            } else {
                metric = COSINE;
                default_metric = false;
            }
            break;
        case 'h':
            printf("SYNOPSIS:\n");
            printf("This program looks at test and finds the most likely authors.\n");
            printf("USAGE\n");
            printf("To run: ./encrypt -d -n -k -l -e -m -c -h\n");
            printf("OPTIONS\n");
            printf("-d: Specify the path to database of authors."
                   "-n: Specify the path to noise words that need to be filtered out."
                   "-k: Specify the number of matches."
                   "-l: Specify how many noise words need to be filtered out."
                   "-e: Sets distance to Euclidean."
                   "-m: Sets distance to Manhattan."
                   "-c: Sets distance to cosine."
                   "-h: Displays the program's purpose and how to run it.");
            break;
        default: fprintf(stderr, "Error.\n Type in -h for help."); return 1;
        }
    }

    noise_f = fopen(noise_fn, "r");
    Text *noise = text_create(noise_f, NULL);
    fclose(noise_f);

    Text *anonymous = text_create(stdin, noise);

    database_f = fopen(database_fn, "r");
    fscanf(database_f, "%d", &author_count);

    PriorityQueue *pq = pq_create(author_count);
    fgets(author_name, 256, database_f);

    for (uint32_t i = 0; i < author_count; i++) {
        Text *author = NULL;

        fgets(author_name, 256, database_f);
        fgets(author_fn, 256, database_f);
        author_name[strcspn(author_name, "\r\n")] = 0;
        author_fn[strcspn(author_fn, "\r\n")] = 0;

        author_f = fopen(author_fn, "r");

        if (author_f == NULL) {
            printf("****** File [%s] of [%s] cannot be opened\n", author_fn, author_name);
        } else {
            Text *author = text_create(author_f, noise);
            double distance = text_dist(anonymous, author, metric);
            enqueue(pq, author_name, distance);
        }

        if (author)
            text_delete(&author);
    }

    printf("Top %d, Metric: %s noise limit: %d\n", matches, metric_names[metric], noiselimit);
    for (uint32_t i = 0; i < matches; i++) {
        char *name;
        double value;

        dequeue(pq, &name, &value);

        printf("%d) %s [%.15f]\n", i + 1, name, value);
    }

    return 0;
}
