// SPDX-FileCopyrightText:  2019-2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-License-Identifier: Apache-2.0
#ifndef _macro_cmp_H
#define _macro_cmp_H

#include <stdbool.h>

/*
    macro_less, macro_equal makes it easier to define different styles of comparisons.

    cmp_no_arg   => int compare(const type *a, const type *b);
    cmp_arg      => int compare(const type *a, const type *b, void *arg);
    arg_cmp      => int compare(void *arg, const type *a, const type *b);
    less_no_arg  => bool less(const type *a, const type *b);
    less_arg     => bool less(const type *a, const type *b, void *arg);
    arg_less     => bool less(void *arg, const type *a, const type *b);
    less         => no comparison, but expects *(a) < *(b) to function properly
    cmp          => no comparison, but expects *(a) < *(b) and *(a) == *(b) to function properly
*/
#define macro_less_cmp_no_arg(type, cmp, a, b) (cmp((const type *)(a), (const type *)(b)) < 0)
#define macro_less_cmp_arg(type, cmp, a, b) (cmp((const type *)(a), (const type *)(b), (arg)) < 0)
#define macro_less_arg_cmp(type, cmp, a, b) (cmp((arg), (const type *)(a), (const type *)(b)) < 0)
#define macro_less_less_no_arg(type, cmp, a, b) cmp((const type *)(a), (const type *)(b))
#define macro_less_less_arg(type, cmp, a, b) cmp((const type *)(a), (const type *)(b), (arg))
#define macro_less_arg_less(type, cmp, a, b) cmp((arg), (const type *)(a), (const type *)(b))
#define macro_less_less(type, cmp, a, b) (*(a) < *(b))
#define macro_less_cmp(type, cmp, a, b) (*(a) < *(b))

#define macro_less(style, type, cmp, a, b) macro_less_ ## style(type, cmp, a, b)


#define macro_equal_cmp_no_arg(type, cmp, a, b) (cmp((const type *)(a), (const type *)(b)) == 0)
#define macro_equal_cmp_arg(type, cmp, a, b) (cmp((const type *)(a), (const type *)(b), (arg)) == 0)
#define macro_equal_arg_cmp(type, cmp, a, b) (cmp((arg), (const type *)(a), (const type *)(b)) == 0)
#define macro_equal_less_no_arg(type, cmp, a, b) (!macro_less_less_no_arg(type, cmp, a, b) && !macro_less_less_no_arg(type, cmp, b, a))
#define macro_equal_less_arg(type, cmp, a, b) (!macro_less_less_arg(type, cmp, a, b) && !macro_less_less_arg(type, cmp, b, a))
#define macro_equal_arg_less(type, cmp, a, b) (!macro_less_arg_less(type, cmp, a, b) && !macro_less_arg_less(type, cmp, b, a))
#define macro_equal_less(type, cmp, a, b) (!macro_less_less(type, cmp, a, b) && !macro_less_less(type, cmp, b, a))
#define macro_equal_cmp(type, cmp, a, b) (*(a) == *(b))

#define macro_equal(style, type, cmp, a, b) macro_equal_ ## style(type, cmp, a, b)

#define macro_le(style, type, cmp, a, b) (macro_cmp_ ## style(type, cmp, (a), (b))<=0)
#define macro_ge(style, type, cmp, a, b) (macro_cmp_ ## style(type, cmp, (a), (b))>=0)
#define macro_greater(style, type, cmp, a, b) macro_less(style, type, cmp, b, a)

#define macro_cmp_cmp_no_arg(type, cmp, a, b) cmp((const type *)(a), (const type *)(b))
#define macro_cmp_cmp_arg(type, cmp, a, b) cmp((const type *)(a), (const type *)(b), (arg))
#define macro_cmp_arg_cmp(type, cmp, a, b) cmp((arg), (const type *)(a), (const type *)(b))
#define macro_cmp_less_no_arg(type, cmp, a, b) (macro_less_less_no_arg(type, cmp, a, b) ? -1 : macro_less_less_no_arg(type, cmp, b, a) ? 1 : 0)
#define macro_cmp_less_arg(type, cmp, a, b) (macro_less_less_arg(type, cmp, a, b) ? -1 : macro_less_less_arg(type, cmp, b, a) ? 1 : 0)
#define macro_cmp_arg_less(type, cmp, a, b) (macro_less_arg_less(type, cmp, a, b) ? -1 : macro_less_arg_less(type, cmp, b, a) ? 1 : 0)
#define macro_cmp_less(type, cmp, a, b) (macro_less_less(type, cmp, a, b) ? -1 : macro_less_less(type, cmp, b, a) ? 1 : 0)
#define macro_cmp_cmp(type, cmp, a, b) (macro_less_less(type, cmp, a, b) ? -1 : macro_less_less(type, cmp, b, a) ? 1 : 0)

#define macro_cmp(style, type, cmp, a, b) macro_cmp_ ## style(type, cmp, a, b)

/* The macro_..._kv are used to compare a key_type and a value_type.  Because a and b can't be swapped, only cmp
   is supported for this.
*/
#define macro_cmp_kv_cmp_no_arg(key_type, value_type, cmp, a, b) cmp((const key_type *)(a), (const value_type *)(b))
#define macro_cmp_kv_cmp_arg(key_type, value_type, cmp, a, b) cmp((const key_type *)(a), (const value_type *)(b), (arg))
#define macro_cmp_kv_arg_cmp(key_type, value_type, cmp, a, b) cmp((arg), (const key_type *)(a), (const value_type *)(b))

#define macro_cmp_kv(style, key_type, value_type, cmp, a, b) macro_cmp_kv_ ## style(key_type, value_type, cmp, a, b)

#define macro_equal_kv(style, key_type, value_type, cmp, a, b) (macro_cmp_kv_ ## style(key_type, value_type, cmp, (a), (b))==0)
#define macro_less_kv(style, key_type, value_type, cmp, a, b) (macro_cmp_kv_ ## style(key_type, value_type, cmp, (a), (b))<0)
#define macro_le_kv(style, key_type, value_type, cmp, a, b) (macro_cmp_kv_ ## style(key_type, value_type, cmp, (a), (b))<=0)
#define macro_ge_kv(style, key_type, value_type, cmp, a, b) (macro_cmp_kv_ ## style(key_type, value_type, cmp, (a), (b))>=0)
#define macro_greater_kv(style, key_type, value_type, cmp, a, b) (macro_cmp_kv_ ## style(key_type, value_type, cmp, (a), (b))>0)

#define macro_cmp_kv_signature_cmp_no_arg(param, key_type, value_type) param
#define macro_cmp_kv_signature_arg_cmp(param, key_type, value_type) param, void *arg
#define macro_cmp_kv_signature_cmp_arg(param, key_type, value_type) param, void *arg
#define macro_cmp_kv_signature_compare_cmp_no_arg(param, key_type, value_type)    \
    param, int (*cmp)(const key_type *, const value_type *)

#define macro_cmp_kv_signature_compare_arg_cmp(param, key_type, value_type)    \
    param, int (*cmp)(void *arg, const key_type *, const value_type *), void *arg

#define macro_cmp_kv_signature_compare_cmp_arg(param, key_type, value_type)    \
    param, int (*cmp)(const key_type *, const value_type *, void *), void *arg

#define macro_cmp_kv_signature(param, style, key_type, value_type) macro_cmp_kv_signature_ ## style(param, key_type, value_type)



#define macro_cmp_signature_cmp_no_arg(param, type) param
#define macro_cmp_signature_cmp(param, type) param
#define macro_cmp_signature_arg_cmp(param, type) param, void *arg
#define macro_cmp_signature_cmp_arg(param, type) param, void *arg
#define macro_cmp_signature_compare_cmp_no_arg(param, type)    \
    param, int (*cmp)(const type *, const type *)

#define macro_cmp_signature_compare_arg_cmp(param, type)    \
    param, int (*cmp)(void *arg, const type *, const type *), void *arg

#define macro_cmp_signature_compare_cmp_arg(param, type)    \
    param, int (*cmp)(const type *, const type *, void *), void *arg

#define macro_cmp_signature_less_no_arg(param, type) param
#define macro_cmp_signature_less(param, type) param
#define macro_cmp_signature_arg_less(param, type) param, void *arg
#define macro_cmp_signature_less_arg(param, type) param, void *arg
#define macro_cmp_signature_compare_less_no_arg(param, type)    \
    param, bool (*cmp)(const type *, const type *)

#define macro_cmp_signature_compare_arg_less(param, type)    \
    param, bool (*cmp)(void *arg, const type *, const type *), void *arg

#define macro_cmp_signature_compare_less_arg(param, type)    \
    param, bool (*cmp)(const type *, const type *, void *), void *arg

#define macro_cmp_signature(param, style, type) macro_cmp_signature_ ## style(param, type)

#endif /* _macro_cmp_H */