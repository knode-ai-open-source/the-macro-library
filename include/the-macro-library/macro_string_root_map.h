// SPDX-FileCopyrightText:  2019-2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024-2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
#ifndef _macro_string_root_H
#define _macro_string_root_H

#include "the-macro-library/macro_map.h"

#include <string.h>

typedef struct {
    macro_map_t node;
    macro_map_t *root;
} macro_string_root_t;

static inline
int compare_macro_string_root_for_insert(const macro_string_root_t *p1, const macro_string_root_t *p2) {
    const char *a = (const char *)(p1+1);
    const char *b = (const char *)(p2+1);
    return strcasecmp(a, b);
}

static inline
int compare_macro_string_root_for_find(const char *a, const macro_string_root_t *p2) {
    const char *b = (const char *)(p2+1);
    return strcasecmp(a, b);
}

static inline
macro_map_insert(macro_string_root_insert, macro_string_root_t, compare_macro_string_root_for_insert);
// void macro_string_root_insert(macro_map_t **root, macro_string_id_map_t *item);
static inline
macro_map_find_kv(macro_string_root_find, char, macro_string_root_t, compare_macro_string_root_for_find);
// macro_string_root_t* macro_string_root_find(const macro_map_t *root, const char *str);

#endif