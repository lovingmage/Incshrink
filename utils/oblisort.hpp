
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



void cache_cmp_swap(Integer *arr, Integer *cache, uint32_t i, uint32_t j, uint32_t rsz, Bit dir)
{
    Bit to_swap = ((arr[i] > arr[j]) == dir);
    swap(to_swap, arr[i], arr[j]);
    for (uint32_t k=0; k < rsz; k++)
        swap(to_swap, cache[i*rsz + k], cache[j*rsz + k]);
}


void cache_bitonic_merge(Integer *arr, Integer *cache, uint32_t lo, uint32_t rsz, uint32_t n, Bit dir)
{
	if (n > 1) {
        uint32_t m = greatestPowerOfTwoLessThan(n);
        for (uint32_t i = lo; i < lo + n - m; i++)
            cache_cmp_swap(arr, cache, i, i + m, rsz, dir);
        cache_bitonic_merge(arr, cache, lo, rsz, m, dir);
        cache_bitonic_merge(arr, cache, lo + m, rsz, n - m, dir);
    }
}

void cache_bitonic_sort(Integer *arr, Integer *cache, uint32_t lo, uint32_t rsz, uint32_t n, Bit dir) {
    if (n > 1) {
        int m = n / 2;
        cache_bitonic_sort(arr, cache, lo, rsz, m, !dir);
        cache_bitonic_sort(arr, cache, lo + m, rsz, n - m, dir);
        cache_bitonic_merge(arr, cache, lo, rsz, n, dir);
    }
}


