// SPDX-FileCopyrightText:  2019-2025 Andy Curtis <contactandyc@gmail.com>
// SPDX-FileCopyrightText: 2024-2025 Knode.ai
// SPDX-License-Identifier: Apache-2.0
#ifndef _macro_to_H
#define _macro_to_H

#include <stdio.h>
#include <stdint.h>
#include <sys/time.h>

static inline int macro_highest_bit_index(uint32_t x) {
	if (x == 0)
		return -1;  // Or handle the zero-case as needed

#ifdef __GNUC__
    // __builtin_clz returns the number of leading zeros; for a 32-bit number,
    // the highest set bit index is 31 - __builtin_clz(x)
    return 31 - __builtin_clz(x);
#else
    // Fallback loop-based method
    int index = 0;
    while (x >>= 1) {
        index++;
    }
    return index;
#endif
}

static inline int macro_highest_bit_index64(uint64_t x) {
	if (x == 0)
		return -1;  // Or handle the zero-case as needed

#ifdef __GNUC__
	// __builtin_clz returns the number of leading zeros; for a 64-bit number,
	// the highest set bit index is 63 - __builtin_clzll(x)
	return 63 - __builtin_clzll(x);
#else
	// Fallback loop-based method
	int index = 0;
	while (x >>= 1) {
		index++;
	}
	return index;
#endif
}

static inline int macro_lowest_bit_index(uint32_t x) {
    if (x == 0)
        return -1;  // No bits set.
#ifdef __GNUC__
    // __builtin_ctz returns the count of trailing zeros.
    return __builtin_ctz(x);
#else
    int index = 0;
    while ((x & 1) == 0) {
        x >>= 1;
        index++;
    }
    return index;
#endif
}

static inline int macro_bit_count(uint32_t x) {
#ifdef __GNUC__
    return __builtin_popcount(x);
#else
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
#endif
}

#include <stdint.h>

static inline int macro_lowest_bit_index64(uint64_t x) {
    if (x == 0)
        return -1;  // No bits set.
#ifdef __GNUC__
    // __builtin_ctzll returns the count of trailing zeros for 64-bit integers.
    return __builtin_ctzll(x);
#else
    int index = 0;
    while ((x & 1) == 0) {
        x >>= 1;
        index++;
    }
    return index;
#endif
}

static inline int macro_bit_count64(uint64_t x) {
#ifdef __GNUC__
    return __builtin_popcountll(x);
#else
    int count = 0;
    while (x) {
        count += x & 1;
        x >>= 1;
    }
    return count;
#endif
}

// Get the current time in micro seconds
static inline long macro_to_time() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000000) + tv.tv_usec;
}

static char *macro_to_date_time(char *dest, time_t ts) {
  struct tm t;
  gmtime_r(&ts, &t);
  sprintf(dest, "%04d-%02d-%02d %02d:%02d:%02d", t.tm_year + 1900, t.tm_mon + 1,
          t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
  return dest;
}

static char *macro_to_date(char *dest, time_t ts) {
  struct tm t;
  gmtime_r(&ts, &t);
  sprintf(dest, "%04d-%02d-%02d", t.tm_year + 1900, t.tm_mon + 1,
          t.tm_mday);
  return dest;
}

// Returns default_value if value is NULL, otherwise value.
static inline const char *macro_to_string(const char *value, const char *default_value) {
    if(value) return value;
    return default_value;
}

static inline int macro_to_int(const char *str, int default_value) {
    if (!str) return default_value;

    int num = 0;
    int ch = *str;
    if(ch == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}

static inline long macro_to_long(const char *str, long default_value) {
    if (!str) return default_value;

    long num = 0;
    int ch = *str;
    if(ch == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}


static inline int32_t macro_to_int32(const char *str, int32_t default_value) {
    if (!str) return default_value;

    int32_t num = 0;

    int ch = *str;
    if(*str == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}

static inline uint32_t macro_to_uint32(const char *str, uint32_t default_value) {
    if (!str) return default_value;

    uint32_t num = 0;

    int ch = *str;
    while (ch) {
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            str++;
            ch = *str;
            continue;
        }
        // Not a valid digit
        return default_value;
    }

    return num;
}


static inline int64_t macro_to_int64(const char *str, int64_t default_value) {
    if (!str) return default_value;

    int64_t num = 0;
    int ch = *str;
    if(ch == '-') {
        str++;
        ch = *str;
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
        num = -num;
    }
    else {
        while (ch) {
            if (ch >= '0' && ch <= '9') {
                num = num * 10 + (ch - '0');
                str++;
                ch = *str;
                continue;
            }
            // Not a valid digit
            return default_value;
        }
    }
    return num;
}

static inline uint64_t macro_to_uint64(const char *str, uint64_t default_value) {
    if (!str) return default_value;

    uint64_t num = 0;

    int ch = *str;
    while (ch) {
        if (ch >= '0' && ch <= '9') {
            num = num * 10 + (ch - '0');
            str++;
            ch = *str;
            continue;
        }
        // Not a valid digit
        return default_value;
    }

    return num;
}

static inline float macro_to_float(const char *str, float default_value) {
    if (!str) return default_value;

    float r;
    if(sscanf(str, "%f", &r) == 1)
        return r;
    return default_value;
}

static inline double macro_to_double(const char *str, double default_value) {
    if (!str) return default_value;

    double r;
    if(sscanf(str, "%lf", &r) == 1)
        return r;
    return default_value;
}

static inline bool macro_to_bool(const char *str, bool default_value) {
    if (!str) return default_value;

    if(default_value == true) {
        if(str[0] == 'f' || str[0] == '0' || str[0] == 'n' || str[0] == 'F' || str[0] == 'N')
            return false;
        return true;
    }
    else {
        if(str[0] == 't' || str[0] == '1' || str[0] == 'y' || str[0] == 'T' || str[0] == 'Y')
            return true;
        return false;
    }
}

#endif