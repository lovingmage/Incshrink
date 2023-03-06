#ifndef OBLISORT_H__
#define OBLISORT_H__

#include "data.h"
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

/*The parameter dir indicates the sorting direction, ASCENDING
   or DESCENDING; if (arr[i] > arr[j]) agrees with the direction,
   then arr[i] and arr[j] (sorting array) are interchanged. 
   The cache will also be swaped accordingly*/
void swap_data(struct data *a, struct data *b, Bit to_swap);

/*This function call is same as the arr_bitonic_merge, but it will
  passes *cache and block size rsz to the cache_cmp_swap function .*/
void bitonic_merge(struct data array[], int left, int right, bool ascending);

/*This function call is same as the arr_bitonic_merge, but it will
  passes *cache and block size rsz to the cache_bitonic_merge function .*/
void bitonic_sort(struct data array[], int left, int right, bool ascending);

#include "oblisort.hpp"

#endif
