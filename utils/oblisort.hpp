
void arr_cmp_swap(Integer *arr, uint32_t i, uint32_t j, Bit dir)
{
    Bit to_swap = ((arr[i] > arr[j]) == dir);
    swap(to_swap, arr[i], arr[j]);
}


void arr_bitonic_merge(Integer *arr, uint32_t lo, uint32_t n, Bit dir)
{
	if (n > 1) {
        uint32_t m = greatestPowerOfTwoLessThan(n);
        for (uint32_t i = lo; i < lo + n - m; i++)
            arr_cmp_swap(arr, i, i + m, dir);
        arr_bitonic_merge(arr, lo, m, dir);
        arr_bitonic_merge(arr, lo + m, n - m, dir);
    }
}

void arr_bitonic_sort(Integer *arr, uint32_t lo, uint32_t n, Bit dir) {
    if (n > 1) {
        int m = n / 2;
        arr_bitonic_sort(arr, lo, m, !dir);
        arr_bitonic_sort(arr, lo + m, n - m, dir);
        arr_bitonic_merge(arr, lo, n, dir);
    }
}



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
            Bit to_swap = ((array[i].flag > array[j].flag) == ascending);
            swap_data(&array[i], &array[j], to_swap);
        }
    }
}

// function to swap two data structures
void swap_data(struct data *a, struct data *b, Bit to_swap) {
    struct data temp = *a;

    //swap data a 
    a->flag = If(to_swap, b->flag, a->flag);
    for (uint32_t i=0; i < MAX_VALUES; i++)
        a->values[i] = If(to_swap, b->values[i], a->values[i]);


    //swap data b
    b->flag = If(to_swap, temp.flag, b->flag);
    for (uint32_t i=0; i < MAX_VALUES; i++)
        b->values[i] = If(to_swap, temp.values[i], b->values[i]);
}
