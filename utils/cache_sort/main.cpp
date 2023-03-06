#include <stdio.h>
#include <stdlib.h>
#include "bitonic_sort.h"

#define M 20    // number of data structures in array
#define N 5     // size of each data structure's value array

int main() {
    // create array of data structures
    struct data array[M] = {
    {1, {1, 2, 3, 4, 5}},
    {0, {6, 7, 8, 9, 10}},
    {1, {11, 12, 13, 14, 15}},
    {0, {16, 17, 18, 19, 20}},
    {1, {21, 22, 23, 24, 25}},
    {0, {26, 27, 28, 29, 30}},
    {1, {31, 32, 33, 34, 35}},
    {0, {36, 37, 38, 39, 40}},
    {1, {41, 42, 43, 44, 45}},
    {0, {46, 47, 48, 49, 50}},
    {1, {51, 52, 53, 54, 55}},
    {1, {56, 57, 58, 59, 60}},
    {0, {61, 62, 63, 64, 65}},
    {0, {66, 67, 68, 69, 70}},
    {1, {71, 72, 73, 74, 75}},
    {0, {76, 77, 78, 79, 80}},
    {1, {81, 82, 83, 84, 85}},
    {0, {86, 87, 88, 89, 90}},
    {1, {91, 92, 93, 94, 95}},
    {0, {96, 97, 98, 99, 100}}
};

    // print original array
    printf("Original array:\n");
    for (int i = 0; i < M; i++) {
        printf("Data %d: flag=%d, values=[%d,%d,%d,%d,%d]\n", i, array[i].flag, array[i].values[0], array[i].values[1], array[i].values[2], array[i].values[3], array[i].values[4]);
    }

    // sort array based on flag domain using bitonic sort
    bitonic_sort(array, 0, M, false);

    // print sorted array
    printf("Sorted array:\n");
    for (int i = 0; i < M; i++) {
        printf("Data %d: flag=%d, values=[%d,%d,%d,%d,%d]\n", i, array[i].flag, array[i].values[0], array[i].values[1], array[i].values[2], array[i].values[3], array[i].values[4]);
    }

    return 0;
}
