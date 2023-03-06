#include "bitonic_sort.h"
#include <stdio.h>
#include <stdlib.h>

// bitonic sort function for array of data structures based on flag domain
void bitonic_sort(struct data array[], int left, int right, bool ascending) {
    if (right > 1) {
        int m = right / 2;
        bitonic_sort(array, left, m, true);
        bitonic_sort(array, left + m, m, false);
        bitonic_merge(array, left, right, ascending);
    }
}

// bitonic merge function for array of data structures based on flag domain
void bitonic_merge(struct data array[], int left, int right, bool ascending) {
    int i, j;
    for (int k = right / 2; k > 0; k /= 2) {
        for (i = left, j = left + k; j < left + right; i++, j++) {
            if ((array[i].flag > array[j].flag) == ascending) {
                swap_data(&array[i], &array[j]);
            }
        }
    }
}

// function to swap two data structures
void swap_data(struct data* a, struct data* b) {
    struct data temp = *a;
    *a = *b;
    *b = temp;
}
