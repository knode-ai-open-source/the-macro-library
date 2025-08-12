# README.macro\_to.md — Tiny “to/from” & bit-twiddling helpers

`macro_to.h` bundles small, dependency-free helpers that cover three buckets:

1. **Time utilities** (wall-clock microseconds, UTC formatting)
2. **String → number** conversions with safe defaults
3. **Bit operations** (highest/lowest set bit, popcount)

It’s header-only for the public API, with the inline implementations in `src/macro_to.h`.

---

## TL;DR / Quick start

```c
#include "the-macro-library/macro_to.h"

void demo(void) {
    // 1) Wall-clock microseconds since Unix epoch (uses gettimeofday)
    long t_us = macro_to_time();

    // 2) Format UTC datetimes (caller allocates buffers)
    char dt[20];  // "YYYY-MM-DD hh:mm:ss" + '\0'
    char d[11];   // "YYYY-MM-DD" + '\0'
    time_t now = time(NULL);
    macro_to_date_time(dt, now);
    macro_to_date(d, now);

    // 3) String → number with defaults (strict parsing)
    int      port   = macro_to_int(getenv("PORT"), 8080);
    uint64_t limit  = macro_to_uint64("18446744073709551615", 0); // max u64
    double   ratio  = macro_to_double("0.75e+1", 1.0);             // 7.5
    bool     debug  = macro_to_bool(getenv("DEBUG"), false);       // "1","t","y" => true

    // 4) Bits
    int hi = macro_highest_bit_index(0x8100u);   // 15  (0-based from LSB)
    int lo = macro_lowest_bit_index(0x8100u);    // 8
    int pc = macro_bit_count(0xF0F0u);           // 8
}
```

---

## API surface

### Time

* `long macro_to_time(void);`
  Wall-clock time in **microseconds** since the Unix epoch (`gettimeofday`).
  *Note:* this is **not monotonic**; jumps if system time changes. Use `macro_time.h` for monotonic timing.

* `char *macro_to_date_time(char *dst, time_t ts);`
  Formats `ts` as **UTC** `"YYYY-MM-DD hh:mm:ss"`.
  **Buffer requirement:** `dst` must be at least **20** bytes.

* `char *macro_to_date(char *dst, time_t ts);`
  Formats `ts` as **UTC** `"YYYY-MM-DD"`.
  **Buffer requirement:** `dst` must be at least **11** bytes.

### String → number (strict parsers with default fallback)

If the input is `NULL` or contains any non-conforming character, **the default value is returned**.

* `const char *macro_to_string(const char *value, const char *default_value);`
  Returns `value` if non-NULL, else `default_value`.

* `int      macro_to_int     (const char *s, int      def);`

* `long     macro_to_long    (const char *s, long     def);`

* `int32_t  macro_to_int32   (const char *s, int32_t  def);`

* `uint32_t macro_to_uint32  (const char *s, uint32_t def);`

* `int64_t  macro_to_int64   (const char *s, int64_t  def);`

* `uint64_t macro_to_uint64  (const char *s, uint64_t def);`

Parsing rules for the integer family:

* Accepts **optional leading `-`** for signed variants; **unsigned do not accept signs**.
* Accepts **only decimal digits** after the sign (no whitespace, `+`, hex, underscores, etc.).
* **No range checks** are performed; out-of-range values may wrap during accumulation.
* Any invalid character → returns `def`.

Floating-point:

* `float  macro_to_float (const char *s, float  def);`
* `double macro_to_double(const char *s, double def);`

These use `strtof/strtod` and require **full consumption** of the string:

* Accept standard decimal and exponent notation (e.g., `"3.14"`, `"1e-6"`).
* If parsing fails or leaves trailing characters → returns `def`.
* Respects the **current C locale** for decimal separator.

Booleans:

* `bool macro_to_bool(const char *s, bool def);`
  Lightweight check of the **first character only**:

    * When `def == true`, strings starting with `f/F/0/n/N` → `false`, else `true`.
    * When `def == false`, strings starting with `t/T/1/y/Y` → `true`, else `false`.

### Bit operations

* `int macro_highest_bit_index(uint32_t x);`

* `int macro_highest_bit_index64(uint64_t x);`
  Returns index (0 = LSB) of the **most significant** set bit, or **-1** if `x == 0`.
  Uses `__builtin_clz/ll` when available; falls back to a loop.

* `int macro_lowest_bit_index(uint32_t x);`

* `int macro_lowest_bit_index64(uint64_t x);`
  Returns index (0 = LSB) of the **least significant** set bit, or **-1** if `x == 0`.
  Uses `__builtin_ctz/ll` when available; falls back to a loop.

* `int macro_bit_count(uint32_t x);`

* `int macro_bit_count64(uint64_t x);`
  Population count (set bits). Uses `__builtin_popcount/ll` when available.

---

## Behavior details & caveats

### Time formatting

* Both `macro_to_date_time` and `macro_to_date` use **UTC** via `gmtime_r` (thread-safe).
* They write NUL-terminated strings into your buffers.
* If you prefer local time, convert via `localtime_r` in your own wrapper.

### `macro_to_time()`

* Returns a **wall-clock** timestamp (microseconds) using `gettimeofday`.
  If your application requires **monotonic** elapsed timing, prefer `macro_time.h`’s `macro_now()` family.
* Return type is `long`; on some 32-bit platforms this may overflow earlier. Most modern targets use 64-bit `long` on 64-bit OSes.

### Integer parsing

* These parsers are **strict and fast**. Any character outside the accepted set causes an immediate fallback to `def`.
* There is **no overflow detection**; extremely large values may wrap during accumulation.
* If you need fully robust error/overflow handling, use `strtol/strtoll/strtoul` with explicit bounds checking.

### Floating-point parsing

* Uses the C library’s `strtof/strtod`.
* Honors the current locale’s decimal separator.

### Booleans

* Minimal, fast check based on the **first character**, useful for flags like `"0"/"1"`, `"true"/"false"`, `"yes"/"no"`.

### Performance

* All functions are `static inline` and compile down to tiny call-site code.
* On GCC/Clang, bit routines usually become single instructions (`clz/ctz/popcnt`) when supported.

---

## Examples

### Environment defaults

```c
const char *host = macro_to_string(getenv("HOST"), "127.0.0.1");
int         port = macro_to_int(getenv("PORT"), 8080);
bool        tls  = macro_to_bool(getenv("TLS"), false);
```

### Strict numeric parsing with fallback

```c
int32_t workers = macro_to_int32("16", 4);      // 16
int32_t bad     = macro_to_int32("16x", 4);     // -> 4 (default)
uint64_t cap    = macro_to_uint64(NULL, 1000);  // -> 1000
```

### Date strings (UTC)

```c
char buf_dt[20], buf_d[11];
time_t ts = time(NULL);
printf("UTC now: %s\n", macro_to_date_time(buf_dt, ts));
printf("UTC day: %s\n", macro_to_date(buf_d, ts));
```

### Bit ops

```c
uint32_t x = 0b1001'0000; // 0x90
printf("hi=%d lo=%d pc=%d\n",
       macro_highest_bit_index(x),   // 7
       macro_lowest_bit_index(x),    // 4
       macro_bit_count(x));          // 2
```

---

## Building & linkage

* **Public header:** `the-macro-library/macro_to.h`
* **Inline implementation:** `the-macro-library/src/macro_to.h` (included by the public header)
* Header-only from the user’s perspective; no library linking required.

---

## Portability notes

* Requires a POSIX-like environment for `gettimeofday`, `gmtime_r`, and standard C library functions.
* On Windows/MSVC:

    * Replace `gettimeofday` with a shim (e.g., `QueryPerformanceCounter` or `_ftime_s`) if you need `macro_to_time` there.
    * Use `gmtime_s` instead of `gmtime_r` (you can wrap it with a macro if needed).

---

## Design philosophy

* **Predictable on failure**: All conversion routines return a **caller-supplied default** on malformed input.
* **Tiny & inlinable**: Favor fast, branch-light code; pull in builtins when available.
* **Pragmatic**: These are building blocks for configs, flags, and fast math — not full replacements for robust parsing libraries.
