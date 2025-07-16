// SPDX-FileCopyrightText:  2019-2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-License-Identifier: Apache-2.0
#ifndef _macro_swap_H
#define _macro_swap_H

/*
    macro_swap requires tmp to be declared and to be of the type that
    a and b are pointed to.
*/
#define macro_swap(a, b)    \
    tmp = *(a);             \
    *(a) = *(b);            \
    *(b) = tmp

#endif /* _macro_swap_H */