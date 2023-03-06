#ifndef BITONIC_SORT_H
#define BITONIC_SORT_H

#include <stdbool.h>

// define maximum size of values array
#define MAX_VALUES 10

// define struct for data
struct data {
    int flag;
    int values[MAX_VALUES];
};

// function prototypes
void bitonic_sort(struct data array[], int left, int right, bool ascending);
void bitonic_merge(struct data array[], int left, int right, bool ascending);
void swap_data(struct data* a, struct data* b);

#endif  // BITONIC_SORT_H
