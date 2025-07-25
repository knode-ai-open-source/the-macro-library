// SPDX-FileCopyrightText:  2019-2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-License-Identifier: Apache-2.0
#ifndef _macro_isort_H
#define _macro_isort_H

#include "the-macro-library/macro_cmp.h"

/* the micro_check_sorted can easily be enabled / disabled by switching from the
    used case to the unused case.  descending is a bad case for insertion sort so
    this is entirely optional */

#define macro_micro_check_reverse_off(style, type, cmp, base, n, xp, yp)

#define macro_micro_check_reverse_on(style, type, cmp, base, n, xp, yp)              \
    if(n > 7) { \
        xp = base;                                                              \
        yp = xp+(n-1);                                                        \
        if(macro_less(style, type, cmp, yp, xp )) {                           \
            if(macro_less(style, type, cmp, xp, xp+2)) goto after_micro_check;         \
            xp += 2;                                                  \
            if(macro_less(style, type, cmp, xp, xp+2)) goto after_micro_check;         \
            xp += 2;                                                  \
            if(macro_less(style, type, cmp, xp, xp+2)) goto after_micro_check;         \
            xp += 2;                                                  \
            if(macro_less(style, type, cmp, xp, yp)) goto after_micro_check;         \
            xp = yp;    \
            while(xp > base) {                                                    \
                if(macro_less(style, type, cmp, xp-1, xp)) goto after_micro_check;        \
                xp--;                                                             \
            }                                                                     \
            while(xp < yp) {                                                  \
                macro_swap(xp, yp);                                           \
                xp++;                                                           \
                yp--;                                                           \
            }                                                                     \
            return; \
        } \
    }   \
    after_micro_check:;


/*
    This macro defines an insertion sort algorithm for an array of elements

    * style, type, cmp are used by the macro_less for comparing elements
    * base, n is the array to be sorted and the number of elements
    * ep, curp, p are used internally and are pointers to type
    * tmp is used internally and is an instance of type
*/

#define macro_isort(style, type, cmp, base, n,                   \
                    ep, curp, p, tmp)                            \
    ep = base + n;                                               \
    curp = base + 1;                                             \
    while (curp < ep) {                                          \
        tmp = *curp;                                             \
        p = curp;                                                \
        while (p > base &&                                       \
               (macro_less(style, type, cmp, &tmp, p - 1))) {    \
            *p = *(p - 1);                                       \
            --p;                                                 \
        }                                                        \
        *p = tmp;                                                \
        ++curp;                                                  \
    }

#endif /* _macro_isort_H */
