// SPDX-FileCopyrightText:  2019-2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-License-Identifier: Apache-2.0
#ifndef _macro_map_H
#define _macro_map_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>

#include "the-macro-library/macro_cmp.h"

typedef struct macro_map_s {
    size_t parent_color;
    struct macro_map_s *left;
    struct macro_map_s *right;
} macro_map_t __attribute__((aligned(2)));
/* The alignment is needed because the color uses the lowest bit */

/* iteration */
static inline macro_map_t *macro_map_first(macro_map_t *n);
static inline macro_map_t *macro_map_last(macro_map_t *n);
static inline macro_map_t *macro_map_next(macro_map_t *n);
static inline macro_map_t *macro_map_previous(macro_map_t *n);
static inline macro_map_t *macro_map_postorder_first(macro_map_t *n);
static inline macro_map_t *macro_map_postorder_next(macro_map_t *n);

/*
  macro_map_copy_node_cb is a callback meant to be used with macro_map_copy to
  copy one map to another given a root node.  The arg will typically be an
  allocator such as the pool.
*/
typedef macro_map_t *(*macro_map_copy_node_cb)(macro_map_t *n, void *arg);

static macro_map_t *macro_map_copy(macro_map_t *root, macro_map_copy_node_cb copy, void *arg);

static bool macro_map_erase(macro_map_t **root, macro_map_t *node);

static void _macro_map_fix_insert(macro_map_t *node, macro_map_t *parent, macro_map_t **root);

#define macro_map_color(n) ((n)->parent_color & 1)
#define _macro_map_is_red(n) (((n)->parent_color & 1) == 0)
#define _macro_map_is_black(n) (((n)->parent_color & 1) == 1)
#define _macro_map_parent(n) (macro_map_t *)((n)->parent_color - ((n)->parent_color & 1))

#define _macro_map_set_black(n) (n)->parent_color |= 1
#define _macro_map_set_red(n) (n)->parent_color -= ((n)->parent_color & 1)
#define _macro_map_set_parent(n, parent)    \
    (n)->parent_color = ((n)->parent_color & 1) + (size_t)(parent)

#define _macro_map_clear_black(n) (n)->parent_color = 1

/* iteration */
static inline macro_map_t *macro_map_first(macro_map_t *n) {
    if (!n)
        return NULL;
    while (n->left)
        n = n->left;
    return n;
}

static inline macro_map_t *macro_map_last(macro_map_t *n) {
    if (!n)
        return NULL;
    while (n->right)
        n = n->right;
    return n;
}

static inline macro_map_t *macro_map_next(macro_map_t *n) {
    if (n->right) {
        n = n->right;
        while (n->left)
            n = n->left;
        return n;
    }
    macro_map_t *parent = _macro_map_parent(n);
    while (parent && parent->right == n) {
        n = parent;
        parent = _macro_map_parent(n);
    }
    return parent;
}

static inline macro_map_t *macro_map_previous(macro_map_t *n) {
    if (n->left) {
        n = n->left;
        while (n->right)
            n = n->right;
        return n;
    }
    macro_map_t *parent = _macro_map_parent(n);
    while (parent && parent->left == n) {
        n = parent;
        parent = _macro_map_parent(n);
    }
    return parent;
}

static inline macro_map_t *_macro_map_left_deepest_node(macro_map_t *n) {
    while (true) {
        if (n->left)
            n = n->left;
        else if (n->right)
            n = n->right;
        else
            return n;
    }
}

static inline macro_map_t *macro_map_postorder_first(macro_map_t *n) {
    if (!n)
        return NULL;
    return _macro_map_left_deepest_node(n);
}

static inline macro_map_t *macro_map_postorder_next(macro_map_t *n) {
    macro_map_t *parent = _macro_map_parent(n);
    if (parent && n == parent->left && parent->right)
        return _macro_map_left_deepest_node(parent->right);
    else
        return parent;
}

/* copy */
static void _macro_map_tree_copy(macro_map_t *n, macro_map_t **res, macro_map_t *parent,
                                        macro_map_copy_node_cb copy, void *arg) {
    macro_map_t *c = copy(n, arg);
    c->parent_color = n->parent_color;
    _macro_map_set_parent(c, parent);
    if (n->left)
        _macro_map_tree_copy(n->left, &c->left, c, copy, arg);
    else
        c->left = NULL;
    if (n->right)
        _macro_map_tree_copy(n->right, &c->right, c, copy, arg);
    else
        c->right = NULL;
}

static inline macro_map_t *macro_map_copy(macro_map_t *root, macro_map_copy_node_cb copy, void *arg) {
    macro_map_t *res = NULL;
    if (root)
        _macro_map_tree_copy(root, &res, NULL, copy, arg);
    return res;
}

static inline void _macro_map_rotate_left(macro_map_t *A, macro_map_t **root) {
    macro_map_t *new_root = A->right;

    size_t tmp_pc = A->parent_color;
    A->parent_color = new_root->parent_color;
    new_root->parent_color = tmp_pc;
    macro_map_t *parent = _macro_map_parent(new_root);
    if (parent) {
        if (parent->left == A)
            parent->left = new_root;
        else
            parent->right = new_root;
    } else
        *root = new_root;

    macro_map_t *tmp = new_root->left;
    new_root->left = A;
    _macro_map_set_parent(A, new_root);

    A->right = tmp;
    if (tmp)
        _macro_map_set_parent(tmp, A);
}

static void _macro_map_rotate_right(macro_map_t *A, macro_map_t **root) {
    macro_map_t *new_root = A->left;
    size_t tmp_pc = A->parent_color;
    A->parent_color = new_root->parent_color;
    new_root->parent_color = tmp_pc;
    macro_map_t *parent = _macro_map_parent(new_root);
    if (parent) {
        if (parent->left == A)
            parent->left = new_root;
        else
            parent->right = new_root;
    } else
        *root = new_root;

    macro_map_t *tmp = new_root->right;
    new_root->right = A;
    _macro_map_set_parent(A, new_root);

    A->left = tmp;
    if (tmp)
        _macro_map_set_parent(tmp, A);
}

static void _macro_map_fix_insert(macro_map_t *node, macro_map_t *parent, macro_map_t **root) {
    _macro_map_set_red(node);
    _macro_map_set_parent(node, parent);
    node->left = node->right = NULL;

    macro_map_t *grandparent, *uncle;

    while (true) {
        parent = _macro_map_parent(node);
        if (!parent) {
            _macro_map_clear_black(node);
            break;
        }

        if (_macro_map_is_black(parent))
            break;

        grandparent = _macro_map_parent(parent);
        if (grandparent->left == parent) {
            uncle = grandparent->right;
            if (uncle && _macro_map_is_red(uncle)) {
                _macro_map_set_red(grandparent);
                _macro_map_set_black(parent);
                _macro_map_set_black(uncle);
                node = grandparent;
                continue;
            }
            if (parent->right == node)
                _macro_map_rotate_left(parent, NULL);
            _macro_map_rotate_right(grandparent, root);
            break;
        } else {
            uncle = grandparent->left;
            if (uncle && _macro_map_is_red(uncle)) {
                _macro_map_set_red(grandparent);
                _macro_map_set_black(parent);
                _macro_map_set_black(uncle);
                node = grandparent;
                continue;
            }
            if (parent->left == node)
                _macro_map_rotate_right(parent, NULL);
            _macro_map_rotate_left(grandparent, root);
            break;
        }
    }
}

static inline
void _fix_color_for_erase(macro_map_t *parent, macro_map_t *node,
                          macro_map_t **root) {
    macro_map_t *sibling;
    if (parent->right != node) {
        sibling = parent->right;
        if (_macro_map_is_red(sibling)) {
            _macro_map_rotate_left(parent, root);
            sibling = parent->right;
        }
        if (sibling->right && _macro_map_is_red(sibling->right)) {
            _macro_map_set_black(sibling->right);
            _macro_map_rotate_left(parent, root);
        } else if (sibling->left && _macro_map_is_red(sibling->left)) {
            _macro_map_rotate_right(sibling, root);
            _macro_map_rotate_left(parent, root);
            _macro_map_set_black(sibling);
        } else {
            _macro_map_set_red(sibling);
            if (_macro_map_parent(parent) && _macro_map_is_black(parent))
                _fix_color_for_erase(_macro_map_parent(parent), parent, root);
            else
                _macro_map_set_black(parent);
        }
    } else {
        sibling = parent->left;
        if (_macro_map_is_red(sibling)) {
            _macro_map_rotate_right(parent, root);
            sibling = parent->left;
        }
        if (sibling->left && _macro_map_is_red(sibling->left)) {
            _macro_map_set_black(sibling->left);
            _macro_map_rotate_right(parent, root);
        } else if (sibling->right && _macro_map_is_red(sibling->right)) {
            _macro_map_rotate_left(sibling, root);
            _macro_map_rotate_right(parent, root);
            _macro_map_set_black(sibling);
        } else {
            _macro_map_set_red(sibling);
            if (_macro_map_parent(parent) && _macro_map_is_black(parent))
                _fix_color_for_erase(_macro_map_parent(parent), parent, root);
            else
                _macro_map_set_black(parent);
        }
    }
}

static inline void _replace_node_with_child(macro_map_t *child, macro_map_t *node,
                                           macro_map_t **root) {
    macro_map_t *parent = _macro_map_parent(node);
    if (parent) {
        if (parent->left == node)
            parent->left = child;
        else
            parent->right = child;
    } else
        *root = child;

    child->parent_color = node->parent_color;
}

static bool macro_map_erase(macro_map_t **root, macro_map_t *node) {
    macro_map_t *parent = _macro_map_parent(node);
    if (!node->left) {
        if (node->right)
            _replace_node_with_child(node->right, node, root);
        else {
            if (parent) {
                if (parent->left == node)
                    parent->left = NULL;
                else
                    parent->right = NULL;
                if (_macro_map_is_black(node))
                    _fix_color_for_erase(parent, NULL, root);
            } else
                *root = NULL;
        }
    } else if (!node->right)
        _replace_node_with_child(node->left, node, root);
    else {
        macro_map_t *successor = node->right;
        if (!successor->left) {
            bool black = _macro_map_is_black(successor);
            _replace_node_with_child(successor, node, root);
            successor->left = node->left;
            _macro_map_set_parent(successor->left, successor);
            if (successor->right)
                _macro_map_set_black(successor->right);
            else {
                if (black)
                    _fix_color_for_erase(successor, NULL, root);
            }
        } else {
            while (successor->left)
                successor = successor->left;

            bool black = _macro_map_is_black(successor);
            macro_map_t *right = successor->right;
            macro_map_t *parent = _macro_map_parent(successor);
            parent->left = right;
            if (right) {
                _macro_map_clear_black(right);
                _macro_map_set_parent(right, parent);
                black = false;
            }
            _replace_node_with_child(successor, node, root);
            successor->left = node->left;
            _macro_map_set_parent(successor->left, successor);
            successor->right = node->right;
            _macro_map_set_parent(successor->right, successor);
            if (black)
                _fix_color_for_erase(parent, NULL, root);
        }
    }
    return true;
}

/*
  Given an address of a member of a structure, the base object type, and the
  field name, return the address of the base structure.
*/
#define macro_parent_object(addr, base_type, field)    \
  (base_type *)((char *)addr - offsetof(base_type, field))


#define _macro_map_find_code_head(value_type)

#define _macro_map_find_code_inner(value)    \
    if (n < 0)                               \
        root = root->left;                   \
    else if (n > 0)                          \
        root = root->right;                  \
    else                                     \
        return value;                        \

#define _macro_map_find_code_tail(key, style, type, cmp)
#define _macro_map_kv_find_code_tail(key, style, key_type, value_type, cmp)

/* first */

#define _macro_map_first_code_head(value_type)    \
    value_type *res = NULL;

#define _macro_map_first_code_inner(value)    \
      if (n <= 0) {                           \
        res = value;                          \
        root = root->left;                    \
      } else                                  \
        root = root->right;                   \

#define _macro_map_first_code_tail(key, style, type, cmp)    \
    if (res && macro_equal(style,type, cmp, key, res))       \
      return res;                                            \
    return NULL;

#define _macro_map_kv_first_code_tail(key, style, key_type, value_type, cmp)    \
    if (res && macro_equal_kv(style, key_type, value_type, cmp, key, res))      \
      return res;                                                               \
    return NULL;

/* last */
#define _macro_map_last_code_head(value_type) _macro_map_first_code_head(value_type)

#define _macro_map_last_code_inner(value)    \
    if (n < 0)                               \
        root = root->left;                   \
    else {                                   \
        res = value;                         \
        root = root->right;                  \
    }

#define _macro_map_last_code_tail(key, style, type, cmp) _macro_map_first_code_tail(key, style, type, cmp)

#define _macro_map_kv_last_code_tail(key, style, key_type, value_type, cmp)    \
    _macro_map_kv_first_code_tail(key, style, key_type, value_type, cmp)

/* lower_bound */
#define _macro_map_lower_bound_code_head(value_type) _macro_map_first_code_head(value_type)
#define _macro_map_lower_bound_code_inner(value) _macro_map_first_code_inner(value)
#define _macro_map_lower_bound_code_tail(key, style, type, cmp) return res;
#define _macro_map_kv_lower_bound_code_tail(key, style, key_type, value_type, cmp) return res;

/* upper_bound */
#define _macro_map_upper_bound_code_head(value_type) _macro_map_first_code_head(value_type)

#define _macro_map_upper_bound_code_inner(value)    \
    if (n < 0) {                                    \
        res = value;                                \
        root = root->left;                          \
    } else                                          \
        root = root->right;

#define _macro_map_upper_bound_code_tail(key, style, type, cmp)    \
    _macro_map_lower_bound_code_tail(key, style, type, cmp)
#define _macro_map_kv_upper_bound_code_tail(key, style, key_type, value_type, cmp)    \
    _macro_map_kv_lower_bound_code_tail(key, style, key_type, value_type, cmp)

/* floor */
#define _macro_map_floor_code_head(value_type)    \
    value_type *res = NULL, *equal = NULL;

#define _macro_map_floor_code_inner(value)    \
    if (n < 0) {                              \
        root = root->left;                    \
    } else if (n > 0) {                       \
        res = value;                          \
        root = root->right;                   \
    } else {                                  \
        equal = value;                        \
        root = root->right;                   \
    }

#define _macro_map_floor_code_tail(key, style, type, cmp) return equal ? equal : res;
#define _macro_map_kv_floor_code_tail(key, style, key_type, value_type, cmp) return equal ? equal : res;

/* ceiling */
#define _macro_map_ceiling_code_head(value_type)    \
    _macro_map_floor_code_head(value_type)

#define _macro_map_ceiling_code_inner(value)    \
    if (n > 0) {                                \
        root = root->right;                     \
    } else if (n < 0) {                         \
        res = value;                            \
        root = root->left;                      \
    } else {                                    \
        equal = value;                          \
        root = root->left;                      \
    }

#define _macro_map_ceiling_code_tail(key, style, type, cmp)    \
    _macro_map_floor_code_tail(key, style, type, cmp)

#define _macro_map_kv_ceiling_code_tail(key, style, key_type, value_type, cmp)    \
    _macro_map_kv_floor_code_tail(key, style, key_type, value_type, cmp)

/* end of custom find functions */

#define _macro_map_code_with_field(root, field, find_style, style, value_type, cmp, key )    \
    _macro_map_ ## find_style ## _code_head(value_type)                                      \
    while (root) {                                                                           \
        value_type *value = macro_parent_object(root, value_type, field );                   \
        int n = macro_cmp(style, value_type, cmp, key, value);                               \
        _macro_map_ ## find_style ## _code_inner(value)                                      \
    }                                                                                        \
    _macro_map_ ## find_style ## _code_tail(key, style, value_type, cmp)                     \
    return NULL;

#define _macro_map_code(root, find_style, style, value_type, cmp, key )     \
    _macro_map_ ## find_style ## _code_head(value_type)                     \
    while (root) {                                                          \
        int n = macro_cmp(style, value_type, cmp, key, root);               \
        _macro_map_ ## find_style ## _code_inner((value_type *)root)        \
    }                                                                       \
    _macro_map_ ## find_style ## _code_tail(key, style, value_type, cmp)    \
    return NULL;

#define _macro_map_kv_code_with_field(root, field, find_style, style, key_type, value_type, cmp, key )    \
    _macro_map_ ## find_style ## _code_head(value_type)                                                   \
    while (root) {                                                                                        \
        value_type *value = macro_parent_object(root, value_type, field );                                \
        int n = macro_cmp_kv(style, key_type, value_type, cmp, key, value);                               \
        _macro_map_ ## find_style ## _code_inner(value)                                                   \
    }                                                                                                     \
    _macro_map_kv_ ## find_style ## _code_tail(key, style, key_type, value_type, cmp)                     \
    return NULL;

#define _macro_map_kv_code(root, find_style, style, key_type, value_type, cmp, key )     \
    _macro_map_ ## find_style ## _code_head(value_type)                                  \
    while (root) {                                                                       \
        int n = macro_cmp_kv(style, key_type, value_type, cmp, key, root);               \
        _macro_map_ ## find_style ## _code_inner((value_type *)root)                     \
    }                                                                                    \
    _macro_map_kv_ ## find_style ## _code_tail(key, style, key_type, value_type, cmp)    \
    return NULL;

#define _macro_map_h(name, style, type)    \
    type *name(const macro_map_t *root, macro_cmp_signature(const type *key, style, type))

#define _macro_map(name, find_style, style, type, cmp)               \
    _macro_map_h(name, style, type) {                                \
        _macro_map_code(root, find_style, style, type, cmp, key);    \
    }

#define _macro_map_with_field(name, field, find_style, style, type, cmp)               \
    _macro_map_h(name, style, type) { {                                                \
        _macro_map_code_with_field(root, field, find_style, style, type, cmp, key);    \
    }

#define _macro_map_compare_h(name, style, type) _macro_map_h(name, compare_ ## style, type)

#define _macro_map_compare(name, find_style, style, type)            \
    _macro_map_compare_h(name, style, type) {                        \
        _macro_map_code(root, find_style, style, type, cmp, key);    \
    }

#define _macro_map_compare_with_field(name, field, find_style, style, type)            \
    _macro_map_compare_h(name, style, type) {                                          \
        _macro_map_code_with_field(root, field, find_style, style, type, cmp, key);    \
    }

#define _macro_map_kv_h(name, style, key_type, value_type)    \
    value_type *name(const macro_map_t *root, macro_cmp_kv_signature(const key_type *key, style, key_type, value_type))

#define _macro_map_kv(name, find_style, style, key_type, value_type, cmp)               \
    _macro_map_kv_h(name, style, key_type, value_type) {                                \
        _macro_map_kv_code(root, find_style, style, key_type, value_type, cmp, key);    \
    }

#define _macro_map_kv_with_field(name, field, find_style, style, key_type, value_type, cmp)               \
    _macro_map_kv_h(name, style, key_type, value_type) {                                                  \
        _macro_map_kv_code_with_field(root, field, find_style, style, key_type, value_type, cmp, key);    \
    }

#define _macro_map_kv_compare_h(name, style, key_type, value_type) _macro_map_kv_h(name, compare_ ## style, key_type, value_type)

#define _macro_map_kv_compare(name, find_style, style, key_type, value_type)             \
    _macro_map_kv_compare_h(name, style, key_type, value_type) {                         \
        _macro_map_kv_code(root, find_style, style, key_type, value_type, cmp, node);    \
    }

#define _macro_map_kv_compare_with_field(name, field, find_style, style, key_type, value_type)             \
    _macro_map_kv_compare_h(name, style, key_type, value_type) {                                           \
        _macro_map_kv_code_with_field(root, field, find_style, style, key_type, value_type, cmp, node);    \
    }

#define _macro_map_insert_code_with_field(root, field, style, value_type, cmp, node )    \
    macro_map_t **np = root, *parent = NULL;                                             \
    while (*np) {                                                                        \
        parent = *np;                                                                    \
        value_type *value = macro_parent_object(parent, value_type, field );             \
        int n = macro_cmp(style, value_type, cmp, node, value);                          \
        if(n < 0)                                                                        \
            np = &(parent->left);                                                        \
        else if (n > 0)                                                                  \
            np = &(parent->right);                                                       \
        else                                                                             \
            return false;                                                                \
    }                                                                                    \
    *np = &node->field;                                                                  \
    _macro_map_fix_insert(*np, parent, root);                                            \
    return true;

#define _macro_map_insert_code(root, style, value_type, cmp, node )    \
    macro_map_t **np = root, *parent = NULL;                           \
    while (*np) {                                                      \
        parent = *np;                                                  \
        int n = macro_cmp(style, value_type, cmp, node, parent);       \
        if(n < 0)                                                      \
            np = &(parent->left);                                      \
        else if (n > 0)                                                \
            np = &(parent->right);                                     \
        else                                                           \
            return false;                                              \
    }                                                                  \
    *np = (macro_map_t *)node;                                         \
    _macro_map_fix_insert(*np, parent, root);                          \
    return true;

#define _macro_multimap_insert_code_with_field(root, field, style, value_type, cmp, node )    \
    macro_map_t **np = root, *parent = NULL;                                                  \
    while (*np) {                                                                             \
        parent = *np;                                                                         \
        value_type *value = macro_parent_object(parent, value_type, field );                  \
        int n = macro_cmp(style, value_type, cmp, node, value);                               \
        if(n < 0)                                                                             \
            np = &(parent->left);                                                             \
        else if (n > 0)                                                                       \
            np = &(parent->right);                                                            \
        else {                                                                                \
            if (node < value)                                                                 \
                np = &(parent->left);                                                         \
            else if (node > value)                                                            \
                np = &(parent->right);                                                        \
            else                                                                              \
                return false;                                                                 \
        }                                                                                     \
    }                                                                                         \
    *np = &node->field;                                                                       \
    _macro_map_fix_insert(*np, parent, root);                                                 \
    return true;

#define _macro_multimap_insert_code(root, style, value_type, cmp, node )    \
    macro_map_t **np = root, *parent = NULL;                                \
    while (*np) {                                                           \
        parent = *np;                                                       \
        int n = macro_cmp(style, value_type, cmp, node, parent);            \
        if(n < 0)                                                           \
            np = &(parent->left);                                           \
        else if (n > 0)                                                     \
            np = &(parent->right);                                          \
        else {                                                              \
            if (node < (value_type *)parent)                                \
                np = &(parent->left);                                       \
            else if (node > (value_type *)parent)                           \
                np = &(parent->right);                                      \
            else                                                            \
                return false;                                               \
        }                                                                   \
    }                                                                       \
    *np = (macro_map_t *)node;                                              \
    _macro_map_fix_insert(*np, parent, root);                               \
    return true;

#define _macro_map_insert_h(name, style, type)    \
    bool name(macro_map_t **root, macro_cmp_signature(type *node, style, type))

#define _macro_map_insert_with_field(name, field, style, type, cmp)                \
    _macro_map_insert_h(name, style, type) {                                       \
        _macro_map_insert_code_with_field(root, field, style, type, cmp, node);    \
    }

#define _macro_map_insert(name, style, type, cmp)                \
    _macro_map_insert_h(name, style, type) {                     \
        _macro_map_insert_code(root, style, type, cmp, node);    \
    }

#define _macro_map_insert_compare_h(name, style, type) _macro_map_insert_h(name, compare_ ## style, type)

#define _macro_map_insert_compare_with_field(name, field, style, type)             \
    _macro_map_insert_compare_h(name, style, type) {                               \
        _macro_map_insert_code_with_field(root, field, style, type, cmp, node);    \
    }

#define _macro_map_insert_compare(name, style, type)             \
    _macro_map_insert_compare_h(name, style, type) {             \
        _macro_map_insert_code(root, style, type, cmp, node);    \
    }

/* multimap */
#define _macro_multimap_insert_with_field(name, field, style, type, cmp)                \
    _macro_map_insert_h(name, style, type) {                                            \
        _macro_multimap_insert_code_with_field(root, field, style, type, cmp, node);    \
    }

#define _macro_multimap_insert(name, style, type, cmp)                \
    _macro_map_insert_h(name, style, type) {                          \
        _macro_multimap_insert_code(root, style, type, cmp, node);    \
    }

#define _macro_multimap_insert_compare_with_field(name, field, style, type)             \
    _macro_map_insert_compare_h(name, style, type) {                                    \
        _macro_multimap_insert_code_with_field(root, field, style, type, cmp, node);    \
    }

#define _macro_multimap_insert_compare(name, style, type)             \
    _macro_map_insert_compare_h(name, style, type) {                  \
        _macro_multimap_insert_code(root, style, type, cmp, node);    \
    }


/* create a set of defaults - cmp_no_arg */
#define macro_map_default() cmp_no_arg

#define macro_map_find_with_field(name, field, type, cmp) _macro_map_with_field(name, field, find, macro_map_default(), type, cmp)
#define macro_map_find(name, type, cmp) _macro_map(name, find, macro_map_default(), type, cmp)

#define macro_map_find_compare_with_field(name, field, type) _macro_map_compare_with_field(name, field, find, macro_map_default(), type)
#define macro_map_find_compare(name, type) _macro_map_compare(name, find, macro_map_default(), type)

#define macro_map_find_kv_with_field(name, field, key_type, value_type, cmp)    \
    _macro_map_kv_with_field(name, field, find, macro_map_default(), key_type, value_type, cmp)
#define macro_map_find_kv(name, key_type, value_type, cmp)    \
    _macro_map_kv(name, find, macro_map_default(), key_type, value_type, cmp)

#define macro_map_find_kv_compare_with_field(name, field, key_type, value_type)    \
    _macro_map_kv_compare_with_field(name, field, find, macro_map_default(), key_type, value_type)
#define macro_map_find_kv_compare(name, key_type, value_type)    \
    _macro_map_kv_compare(name, find, macro_map_default(), key_type, value_type)

#define macro_map_insert_h(name, type) _macro_map_insert_h(name, macro_map_default(), type)
#define macro_map_insert_compare_h(name, type) _macro_map_insert_compare_h(name, macro_map_default(), type)

#define macro_map_insert_with_field(name, field, type, cmp)    \
    _macro_map_insert_with_field(name, field, macro_map_default(), type, cmp)

#define macro_map_insert(name, type, cmp) _macro_map_insert(name, macro_map_default(), type, cmp)

#define macro_map_insert_compare_with_field(name, field, type)    \
    _macro_map_insert_compare_with_field(name, field, macro_map_default(), type)

#define macro_map_insert_compare(name, cmp) _macro_map_insert_compare(name, macro_map_default(), cmp)

/* multimap */
#define _macro_multimap_insert_h(name, style, type) _macro_map_insert_h(name, style, type)
#define _macro_multimap_insert_compare_h(name, style, type) _macro_map_insert_compare_h(name, style, type)

#define macro_multimap_insert_h(name, type) _macro_multimap_insert_h(name, macro_map_default(), type)
#define macro_multimap_insert_compare_h(name, type) _macro_multimap_insert_compare_h(name, macro_map_default(), type)

#define macro_multimap_insert_with_field(name, field, type, cmp)    \
    _macro_multimap_insert_with_field(name, field, macro_map_default(), type, cmp)

#define macro_multimap_insert(name, type, cmp) _macro_multimap_insert(name, macro_map_default(), type, cmp)

#define macro_multimap_insert_compare_with_field(name, field, type)    \
    _macro_multimap_insert_compare_with_field(name, field, macro_map_default(), type)

#define macro_multimap_insert_compare(name, cmp) _macro_multimap_insert_compare(name, macro_map_default(), cmp)

#endif