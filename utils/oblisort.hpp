
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
