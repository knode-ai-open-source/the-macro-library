// SPDX-FileCopyrightText:  2019-2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024-2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
#ifndef _MACRO_TO_H
#define _MACRO_TO_H

#include <stdbool.h>
#include <stdint.h>
#include <time.h>

// Get the current time in micro seconds
static inline long macro_to_time();

// Converts a time stamp to YYYY-MM-DD hh:mm:ss (requires a 20 byte dest)
static char *macro_to_date_time(char *dest, time_t ts);

// Converts a time stamp to YYYY-MM-DD (requires a 11 byte dest)
static char *macro_to_date(char *dest, time_t ts);

// Returns default_value if value is NULL, otherwise value.
static inline const char *macro_to_string(const char *value, const char *default_value);

// Converts a string to an int.
static inline int macro_to_int(const char *str, int default_value);

// Converts a string to an long.
static inline long macro_to_long(const char *str, long default_value);

// Converts a string to a 32-bit integer (int32_t).
static inline int32_t macro_to_int32(const char *str, int32_t default_value);

// Converts a string to an unsigned 32-bit integer (uint32_t).
static inline uint32_t macro_to_uint32(const char *str, uint32_t default_value);

// Converts a string to a 64-bit integer (int64_t).
static inline int64_t macro_to_int64(const char *str, int64_t default_value);

// Converts a string to an unsigned 64-bit integer (uint64_t).
static inline uint64_t macro_to_uint64(const char *str, uint64_t default_value);

// Converts a string to a float. Handles decimal and exponential notation.
static inline float macro_to_float(const char *str, float default_value);

// Converts a string to a double. Supports decimal and exponential notation.
static inline double macro_to_double(const char *str, double default_value);

// Converts a string to a bool. Interprets several representations of true/false.
static inline bool macro_to_bool(const char *str, bool default_value);

static inline int macro_highest_bit_index(uint32_t x);
static inline int macro_highest_bit_index64(uint64_t x);

static inline int macro_lowest_bit_index(uint32_t x);
static inline int macro_lowest_bit_index64(uint64_t x);

static inline int macro_bit_count(uint32_t x);
static inline int macro_bit_count64(uint64_t x);

#include "the-macro-library/src/macro_to.h"

#endif // _MACRO_TO_H
