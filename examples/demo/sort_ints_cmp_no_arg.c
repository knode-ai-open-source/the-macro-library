// SPDX-FileCopyrightText:  2019-2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-License-Identifier: Apache-2.0
#include "the-macro-library/macro_sort.h"

int compare_ints(const int *a, const int *b) {
    return *a - *b;
}

// the default macro_sort expects a less function, use _macro_sort to customize!
_macro_sort(sort_ints, cmp_no_arg, int, compare_ints);

int main() {
    int arr[] = { 5, 4, 3, 1, 2 };
    sort_ints(arr, sizeof(arr)/sizeof(arr[0]));
    for( int i=0; i<sizeof(arr)/sizeof(arr[0]); i++ )
        printf(" %d", arr[i] );
    printf( "\n");
    return 0;
}
