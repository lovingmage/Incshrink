#ifndef OBLISORT_H__
#define OBLISORT_H__

using namespace emp;
using namespace std;

/*The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (arr[i] > arr[j]) agrees with the direction,
   then arr[i] and arr[j] are interchanged.*/
void arr_cmp_swap(Integer *arr, uint32_t i, uint32_t j, Bit dir);

/*It recursively sorts a bitonic sequence in ascending order,
  if dir = 1, and in descending order otherwise (means dir=0).
  The sequence to be sorted starts at index position low,
  the parameter cnt is the number of elements to be sorted.*/
void arr_bitonic_merge(Integer *arr, uint32_t lo, uint32_t n, Bit dir);

/* This function first produces a bitonic sequence by recursively
    sorting its two halves in opposite sorting orders, and then
    calls bitonic merge to make them in the same order */
void arr_bitonic_sort(Integer *arr, uint32_t lo, uint32_t n, Bit dir);

#include "oblisort.hpp"

#endif
