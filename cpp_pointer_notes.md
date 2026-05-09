# C/C++ Pointer Notes — Qualcomm Prep

Notes from MCQ diagnostic session. Focus on gaps to drill.

---

## 1. `sizeof`: Array vs Pointer (gotcha #1)

```c
int arr[10];          // sizeof(arr) = 40   (10 * sizeof(int))
int *p = arr;         // sizeof(p)   = 8    (pointer size on 64-bit)
```

**Arrays decay to pointers in MOST contexts BUT NOT in:**
- `sizeof(arr)` — gives total array bytes
- `&arr` — gives `int(*)[10]`, NOT `int**`
- `typeof(arr)` — gives `int[10]`

**Crucial trap — function parameters:**
```c
void foo(int x[10]) {
    sizeof(x);   // = 8 (DECAYED to int*)
}
// x[10] in parameter is just sugar for int*
```

#### The three equivalent function signatures

These three are **identical to the compiler** — array parameters always decay to pointers:

```c
void foo(int arr[10]) { ... }     // looks like array of 10
void foo(int arr[])   { ... }     // looks like "some array"
void foo(int *arr)    { ... }     // explicit pointer
```

All three: `arr` has type `int *` inside the function. `sizeof(arr)` gives `8` (pointer size on 64-bit). The `[10]` in the first form is **decoration only** — the compiler ignores the size.

> **Memorize**: in a function parameter, `T arr[N]` means `T *arr`. Period. The size is a lie.

#### Real-world impact

```c
void zero_buf(uint8_t buf[1024]) {
    memset(buf, 0, sizeof(buf));     // BUG: zeroes 8 bytes (sizeof pointer), not 1024
}
```

**Fix**: pass the size explicitly:

```c
void zero_buf(uint8_t *buf, size_t n) {
    memset(buf, 0, n);
}
```

This is **the most common silent C bug** in HW/embedded code. Senior reviewers catch it on sight.

### sizeof on string literals

```c
strlen("hello")  // = 5 — chars BEFORE null terminator
sizeof("hello")  // = 6 — chars INCLUDING null terminator (literal type is char[6])

char s[] = "hello";
sizeof(s)        // = 6 — array of 6 chars

char *p = "hello";
sizeof(p)        // = 8 — pointer size on 64-bit (NOT the literal!)
```

**Key distinction**:
- `s` declared as `char[]` → array, `sizeof` gives total bytes
- `p` declared as `char *` → pointer, `sizeof` gives pointer size

### 1.1 `char *s = "hello"` vs `char str[] = "hello"` — full comparison

These look identical at the source level but have **completely different semantics in memory, mutability, and operations**.

#### Memory layout

```c
char *s   = "hello";    // s is a POINTER (8 bytes on stack)
char str[] = "hello";   // str is an ARRAY (6 bytes on stack)
```

```
s (pointer, 8 bytes on stack)        str (array, 6 bytes on stack)
┌─────────────────┐                  ┌───┬───┬───┬───┬───┬────┐
│  0x400500       │ ─────┐           │ h │ e │ l │ l │ o │ \0 │
└─────────────────┘      │           └───┴───┴───┴───┴───┴────┘
                         ▼
                  ┌───┬───┬───┬───┬───┬────┐
                  │ h │ e │ l │ l │ o │ \0 │   ← string literal in .rodata
                  └───┴───┴───┴───┴───┴────┘     (read-only, shared)
```

- `s` itself is **8 bytes** (a pointer) on stack, *pointing to* a 6-byte string in **read-only memory**
- `str` itself **IS** 6 bytes of chars on stack — the literal got **copied in**

#### Comparison table

| Property | `char *s = "hello"` | `char str[] = "hello"` |
|---|---|---|
| Type | `char *` | `char [6]` |
| Storage location | Pointer on stack, string in `.rodata` | Full 6 bytes on stack |
| `sizeof` | **8** (pointer) | **6** (array) |
| Mutable? | **NO** — `s[0] = 'H'` is **UB** (typically segfault) | **YES** — `str[0] = 'H'` is fine |
| `&` (address-of) | `&s` is `char**` (addr of pointer var) | `&str` is `char(*)[6]` (ptr to array of 6) |
| Re-assignment | `s = "world";` ✓ (rebind pointer) | `str = "world";` ✗ (compile error) |
| String shared? | Multiple `char *s` to same literal can share | Each `str[]` gets its own copy |

#### Key gotchas

```c
char *s = "hello";
s[0] = 'H';        // UB! Modifying string literal — segfault on most systems

char str[] = "hello";
str[0] = 'H';      // OK — modifies stack array

s = "world";       // OK — rebinds pointer
str = "world";     // COMPILE ERROR — arrays aren't assignable
```

#### Mental model

| | What it IS | `sizeof` gives |
|---|---|---|
| `char *s` | A *pointer* (a thing holding an address) | size of pointer (8) |
| `char str[]` | An *array* (a chunk of bytes) | size of those bytes (6) |

> **Pointer = "I know where it lives."**
> **Array = "I AM it."**

This is the foundation of Q2 (`sizeof(p)` where p is `char *`) and Q9 (`sizeof(arr)/sizeof(arr[0])` length idiom) from the MCQ warmup.

### 1.2 C-vs-C++ trivia (interview traps)

Subtle differences that show up on MCQs. The standalone facts you can't derive — just memorize.

#### Character literal type

```c
sizeof('A')
```

| Language | Result | Why |
|---|---|---|
| **C** | **4** | Character constants have type `int` |
| **C++** | **1** | Character constants have type `char` |

This is **the** classic C-vs-C++ trick question. Q1 from the warmup. Memory hook: in C, `'A'` is "small int", not "small char."

#### `bool` type

| Language | `sizeof(bool)` | Header needed |
|---|---|---|
| C | usually 1, but originally not a keyword (used `int` flags) | `<stdbool.h>` for `bool` macro (C99+) |
| C++ | 1 (built-in keyword since day 1) | none |

#### `void f()` — empty parameter list

| Language | Meaning |
|---|---|
| **C** | Function takes **unspecified** parameters (any number, any type) — UB if mismatched |
| **C++** | Function takes **zero** parameters — strictly type-checked |

In C, write `void f(void)` to mean "no parameters." In C++, both forms are equivalent.

#### Struct vs class

| Language | Struct |
|---|---|
| C | Just data aggregation. No methods, no inheritance. Tag namespace separate (`struct Foo` vs `Foo`). |
| C++ | Full class with methods/inheritance. **Default access is `public`** (vs `private` in `class`). No tag namespace separation. |

#### `const` linkage

| Language | `const int x = 5;` at file scope |
|---|---|
| C | **External** linkage by default (visible across TUs) |
| C++ | **Internal** linkage by default (file-local; need `extern` to share) |

#### NULL pointer constant

| Language | Idiomatic null pointer |
|---|---|
| C | `NULL` macro (typically `((void*)0)`) |
| C++ | **`nullptr`** (typed, since C++11) — `NULL` works but can break overload resolution |

#### String literal mutability

| Language | `char *s = "hello"; s[0] = 'H';` |
|---|---|
| C | UB (literal is in read-only memory in practice) — implementations *may* allow it |
| C++ | **Compile error** — string literals are `const char[]`, won't bind to `char *` (deprecated then removed) |

For C++ source: `const char *s = "hello"` is the correct form.

### The single most common C bug: `sizeof(buf)` in a function

```c
void zero_buffer(uint8_t buf[1024]) {
    memset(buf, 0, sizeof(buf));     // BUG! sizeof = 8 (pointer), zeroes only 8 bytes
}
```

The `[1024]` in the parameter is a **lie** — it decays to `uint8_t *`. `sizeof(buf)` gives pointer size (8 bytes on 64-bit), not 1024.

**Fix — always pass size as a separate parameter:**
```c
void zero_buffer(uint8_t *buf, size_t n) {
    memset(buf, 0, n);
}
```

**Or use the C99 stack-allocated form (sizeof works inside the function only if it's a true VLA, not just decoration):**
```c
void zero_buffer_vla(size_t n, uint8_t buf[n]) {
    memset(buf, 0, sizeof(buf));   // STILL DECAYED — sizeof(buf) is still pointer size
                                    // VLAs don't help here
}
```

VLAs don't fix the issue. **Always pass size explicitly.** This pattern is the #1 silent bug at HW-adjacent companies (Qualcomm, embedded shops).

### Array vs Pointer types — four distinct things

```
TYPE                  MEANING                           SIZE (64-bit)
────────────────────────────────────────────────────────────────────
int                  one int                            4
int *                pointer to int                     8
int[10]              array of 10 ints (the array)       40
int (*)[10]          pointer to array of 10 ints        8
int *[10]            array of 10 int pointers           80 (10 × 8)
```

**Read right-to-left** (using the spiral rule):
- `int[10]` — array of 10 ints
- `int *[10]` — `[10]` binds tighter → array of 10, of (`int *`) → array of 10 pointers
- `int (*)[10]` — parens force `*` first → pointer, to (`int[10]`) → pointer to array of 10 ints

### Array-to-pointer decay: when it happens, when it doesn't

**Decay happens (array becomes pointer):**
```c
int arr[10];
int *p = arr;       // ← decays to int*
foo(arr);           // ← decays when passed
arr + 1;            // ← decays in expression
```

**Decay BLOCKED (array stays array):**
```c
sizeof(arr)         // ← returns 40, NOT 8
&arr                // ← gives int(*)[10], NOT int**
typeof(arr)         // ← gives int[10]
char buf[10];
"buf"               // string literal "buf" stays as char[4] until used
```

**Why this matters**: same variable `arr`, different `sizeof` results depending on context. The trap behind the function-parameter bug.

---

## 11.5 Custom `sizeof` Macro (interview classic)

**Problem**: write a macro that gives `sizeof(T)` without using the `sizeof` keyword.

```c
#define MY_SIZEOF(T)  ((char *)((T *)0 + 1) - (char *)((T *)0))
```

### Step-by-step trace for `MY_SIZEOF(int)`

| Expression | Meaning | Result |
|---|---|---|
| `(int *)0` | cast 0 to int* (a "null int-pointer") | address 0 |
| `(int *)0 + 1` | pointer arithmetic: scale by `sizeof(int)` | address 4 |
| `(char *)((int *)0 + 1)` | cast to char* (byte-granular pointer) | address 4 |
| `(char *)((int *)0)` | cast to char* | address 0 |
| Subtract | char* subtraction = byte difference | **4** |

### Why each part is needed

1. **`(T *)0`** — manufactures a typed pointer at address 0. Doesn't access memory; only used as a typed reference for arithmetic.
2. **`+ 1`** — pointer arithmetic auto-scales by `sizeof(T)`. So `(T*)0 + 1` lives at byte offset `sizeof(T)`.
3. **`(char *)` cast** — without this, subtracting two `T *` would give `1` (one element). Casting to `char *` gives byte-granular subtraction → `sizeof(T)` bytes.
4. **No dereferencing** — `*((T *)0)` would crash. We only do address arithmetic, never reads.

### Works for all type categories

```c
MY_SIZEOF(char)         // 1
MY_SIZEOF(int)          // 4
MY_SIZEOF(double)       // 8
MY_SIZEOF(int[10])      // 40 (array type)
MY_SIZEOF(struct Foo)   // sizeof of the struct
MY_SIZEOF(int *)        // 8 (pointer type)
```

For `MY_SIZEOF(int[10])`: `T = int[10]`, so `T *` is `int[10] *` (pointer to array of 10 ints). `+1` scales by `sizeof(int[10])` = 40 bytes. Cast to `char *` and subtract → 40.

### Interview gold (4-sentence explanation)

> "I cast 0 to a `T *` to manufacture a typed pointer at address 0. Pointer arithmetic on `T *` scales by `sizeof(T)`, so `+1` advances exactly that many bytes. I cast both to `char *` because `char *` is byte-granular — subtracting `char *` gives the byte difference, while subtracting `T *` would give 1 (element count). I never dereference, so no memory is actually accessed."

---

## 2. Pointer Arithmetic Scales by `sizeof(pointee)` (gotcha #2)

**Pointer subtraction returns ELEMENTS, not bytes.**

```c
int arr[3] = {1, 2, 3};
int *p = arr;
int *q = &arr[2];
q - p;                // = 2 (elements)
(char*)q - (char*)p;  // = 8 (bytes)
```

**Same for addition:**
```c
int *p;
p + 1;        // advances 4 bytes (1 int)
char *c;
c + 1;        // advances 1 byte
int16_t *s;
s + 1;        // advances 2 bytes
```

**Why it matters for Qualcomm**: ISP/DSP buffer walking — same memory, different stride based on cast. Core to fixed-point image processing.

### 2.1 Pointer arithmetic moves by BYTES, not bits

C has **no concept of "pointer to a bit."** The smallest addressable unit is a **byte** (`char`).

> **`pointer + N` moves by `N * sizeof(pointee)` bytes.**

| Pointer type | `p + 1` advances by |
|---|---|
| `char *` | 1 byte |
| `unsigned char *` | 1 byte |
| `int16_t *` | 2 bytes |
| `int *` (32-bit) | 4 bytes |
| `double *` | 8 bytes |
| `void *` | undefined (no size) — illegal in standard C |

`sizeof(char) == 1` is **guaranteed** by the C standard. Memory addresses are byte addresses. There's no way to point to "bit 3 of byte 0" directly.

#### Bit-level access is via bitwise operators on a loaded byte

```c
unsigned char byte = s[0];            // load whole byte
int bit5 = (byte >> 5) & 1;           // extract bit 5
int set5 = byte | (1U << 5);          // set bit 5
int clr5 = byte & ~(1U << 5);         // clear bit 5
```

For HW register modeling, **bitfields** let the compiler do the bit-packing for you:

```c
struct Reg {
    unsigned int field_a : 3;     // 3-bit field
    unsigned int field_b : 5;     // 5-bit field
};
// Compiler still loads/stores whole bytes/words; you access via .field_a
```

### 2.2 Out-of-bounds pointer arithmetic = UB

The C standard (§6.5.6) only defines pointer arithmetic when the result points to:
- An element of the **same array** (or aggregate object), OR
- **One past the end** (without dereferencing it)

Anything else is **undefined behavior** — even just *computing* the pointer, before any dereference.

```c
int32_t t = 1;                                // t is 4 bytes
unsigned char *s = (unsigned char *) &t;

s, s+1, s+2, s+3   // OK — all point within t
s+4                // OK as a pointer (one past end), UB to deref
s+5                // UB — computing the pointer is already UB
s+31               // UB — way out of bounds
*(s+31)            // UB cubed — computation + deref + reading random memory
```

**Why this is dangerous**: even when no crash happens, the optimizer is free to assume UB never happens. It might:
- Delete the entire if-statement that depends on the UB read
- Merge unrelated code paths
- Produce results that change between -O0 and -O2 builds

> **Stay within the object you're pointing into. One past the end is OK as a sentinel; never dereference it.**

### 2.3 `s[i]`, `*s`, and `*(s+i)` are identical

```c
unsigned char *s;

s[0]       // sugar for *(s + 0)  →  same as *s
s[1]       // sugar for *(s + 1)
s[i]       // sugar for *(s + i)
```

The `[]` operator **is itself a dereference**. So:

```c
*s         // OK — first byte
s[0]       // OK — same as *s
*(s+0)     // OK — same as *s
*(s[0])    // BUG — double dereference
//   ↑
//   s[0] is already a byte VALUE; *(byte_value) treats the byte as a pointer
//   tries to read from address (byte_value) → garbage / segfault
```

**Mental rule**: when accessing array-style memory, use `s[i]` OR `*(s+i)` — never combine `*` with `[]`.

### 2.4 Canonical endianness detection

```c
int is_little_endian() {
    int32_t t = 1;
    unsigned char *s = (unsigned char *) &t;
    return s[0] == 1;        // or: return *s == 1;
}
```

How it works (with `t = 1`):

```
              byte 0   byte 1   byte 2   byte 3
LE memory:    [0x01]   [0x00]   [0x00]   [0x00]
BE memory:    [0x00]   [0x00]   [0x00]   [0x01]
                ↑                          ↑
               s[0]                       s[3]
```

- LE: byte 0 holds the LSB → `s[0] == 1` → returns 1
- BE: byte 0 holds the MSB → `s[0] == 0` → returns 0

#### Why `unsigned char *` (and not `char *` or `int *`)

1. **`unsigned char *` is exempt from strict aliasing** — it can legally view the bytes of any object (covered in §11.10). Casting `int *` to `float *` is UB; casting to `unsigned char *` is always safe.
2. **No high-bit surprises** — `unsigned char(0xFF)` stays 255 in arithmetic; `signed char(0xFF)` becomes -1 when promoted to `int`, which can mess up comparisons.
3. **Byte-granular** — `+1` advances exactly 1 byte, so `s[i]` reads the i-th byte cleanly.

### Common bugs from this exercise

- **Casting the value, not the address**: `(unsigned char *) t` instead of `(unsigned char *) &t` — gives a pointer to address `t` (the integer 1), not a pointer to where `t` lives. Dereferences fail.
- **Out-of-bounds index**: `*(s+31)` for a 4-byte object — UB, reads random memory.
- **Double dereference**: `*(s[0])` — `s[0]` is already a byte, dereferencing a byte-as-pointer crashes.
- **Forgetting the cast**: `unsigned char *s = &t;` without `(unsigned char *)` — type mismatch, compile error.

### Memorization aid

> **"Address-of, not value-of. Inspect byte 0 of an int = 1. LE writes the LSB first."**

---

## 3. Complex Declaration Reading — Right-Left Rule (gotcha #3)

**The trap pair:**
```c
int (*p)[10];   // Pointer to array of 10 ints      (parens make * win)
int *p[10];     // Array of 10 int pointers         (brackets bind tighter)
```

**Memory diagram:**
```
int (*p)[10];
   p ──→ [_,_,_,_,_,_,_,_,_,_]   (one chunk: 40 bytes)

int *p[10];
   p[0] ──→ ?
   p[1] ──→ ?
   ...
   p[9] ──→ ?                    (10 pointers, 80 bytes)
```

**Right-left rule:**
1. Start at the variable name
2. Look right first (for `[]` or `()`), respecting parens
3. Look left (for `*` or qualifiers)
4. Repeat outward

**Mnemonic:**
- **Parens around `*` → pointer first** → `int (*p)[10]` = pointer to array
- **No parens, just `[]` → array first** → `int *p[10]` = array of pointers

**Practice declarations:**
- `int *(*p)[10]` → pointer to array of 10 int pointers
- `int (*p[10])(int)` → array of 10 function pointers (each takes int, returns int)
- `int (*(*foo)(int))[3]` → foo is pointer to function (taking int) returning pointer to array of 3 ints
- `char *(*fp)(void)` → pointer to function (no args) returning char*

---

## 4. `const` Placement Rules (gotcha #4)

**The rule**: `const` applies to whatever is **immediately to its left** (or right, if `const` is leftmost).

```c
const int *p;        // pointer to const int     → can't *p = 5     | can p = &other
int const *p;        // SAME as above
int * const q;       // const pointer to int     → can *q = 5       | can't q = &other
const int * const r; // const pointer to const int → can't change anything
```

**Read right-to-left:**
- `const int *p` → "p is pointer to int const"
- `int * const q` → "q is const pointer to int"

**Pointer-to-pointer cases:**
```c
const int **pp;        // pointer to (pointer to const int)
                       // *pp can change, **pp cannot
int * const * pp;      // pointer to (const pointer to int)
                       // *pp cannot change, **pp can
const int * const * const *ppp;   // all const except deepest int
```

---

## 5. UB vs Runtime Error vs Compile Error

| Type | Meaning |
|---|---|
| **Compile error** | Compiler refuses to build (syntax, type mismatch) |
| **UB** (undefined behavior) | Code builds, runs, but standard says "anything can happen" |
| **Runtime error** | Code builds, runs, fails predictably (div/0, segfault on null deref) |

**UB is NOT a runtime error.** UB might:
- Print 42 successfully (memory not yet overwritten)
- Print garbage
- Crash
- Format your hard drive (in theory)

**Common UB sources Qualcomm asks about:**
- Returning pointer to stack variable (use-after-scope)
- Use-after-free
- Reading uninitialized memory
- Out-of-bounds array access
- Signed integer overflow
- Modifying string literals (`char *s = "hello"; s[0] = 'H';` is UB)
- Strict aliasing violations

---

## 6. Function Pointers

**Declaration:**
```c
void (*fp)(int);    // pointer to function taking int, returning void
```

**Calling — both work in C/C++:**
```c
fp(5);          // implicit deref, modern style
(*fp)(5);       // explicit deref, K&R style
```

**Assignment:**
```c
void greet(int x) { /* ... */ }
fp = greet;     // implicit address-of (equivalent below)
fp = &greet;    // explicit
```

**Array of function pointers:**
```c
void (*ops[4])(int);    // array of 4 fn pointers
ops[0] = &add;
ops[1] = &sub;
ops[0](5);              // calls add(5)
```

**Typedef trick (much cleaner):**
```c
typedef void (*Callback)(int);
Callback fp = greet;
Callback ops[4];
```

---

## 6.5 Integer Type Sizes (critical for Qualcomm — fixed-point + HW models)

### Standard guarantees (minimums)

| Type | Min bits | Linux 64-bit (LP64) | Windows 64-bit (LLP64) |
|---|---|---|---|
| `char` | 8 | 1 byte | 1 byte |
| `short` | 16 | 2 bytes | 2 bytes |
| `int` | 16 | 4 bytes | 4 bytes |
| `long` | 32 | **8 bytes** | **4 bytes** ← gotcha |
| `long long` | 64 | 8 bytes | 8 bytes |
| pointer | — | 8 bytes | 8 bytes |

**Rule of thumb**: never assume `long` = 8 across platforms. Use `int64_t` for portable 64-bit code.

### Fixed-width types `<stdint.h>` — production answer

```c
int8_t,  int16_t,  int32_t,  int64_t   // exact width signed
uint8_t, uint16_t, uint32_t, uint64_t  // exact width unsigned
intptr_t, uintptr_t                     // big enough to hold pointer
size_t                                  // unsigned, for sizes/indices
ptrdiff_t                               // signed, pointer subtraction result
```

**Always use these for image buffers, fixed-point math, register modeling.** This is what Qualcomm production C/C++ models use.

**Critical interview point**: `int32_t` is the **only type guaranteed to be exactly 32 bits** across all platforms. `int` is at least 16 bits (typically 32 but not guaranteed — some embedded MCUs use 16-bit int). `long` is at least 32 bits but varies (4 bytes Windows, 8 bytes Linux). Never assume specific widths for plain `int`/`long`.

### Signed vs unsigned ranges

| Type | Range |
|---|---|
| `int8_t` | −128 to 127 |
| `uint8_t` | 0 to 255 *(pixel values)* |
| `int16_t` | −32 768 to 32 767 |
| `uint16_t` | 0 to 65 535 *(10/12-bit raw sensor)* |
| `int32_t` | ≈ ±2.1 × 10⁹ |
| `uint32_t` | 0 to ≈ 4.3 × 10⁹ |

### Overflow behavior

- **Signed overflow** → **UB** (compiler can assume it never happens — affects optimization)
- **Unsigned overflow** → **wraps modulo 2^N** (well-defined)

```c
uint8_t x = 250;
x += 10;        // x = 4 (defined: wraps mod 256)

int8_t y = 120;
y += 10;        // UB! Standard makes no guarantee.
```

### Integer promotion rules (gotcha)

In any expression, types smaller than `int` are **promoted to `int`** before the operation:

```c
uint8_t a = 200;
uint8_t b = 200;
uint16_t c = a + b;      // c = 400 (not 144!) because a,b promoted to int
                         //   then implicit conversion to uint16_t
uint8_t d = a + b;       // d = 144 (truncated to 8 bits)

int8_t e = -1;
unsigned int f = 1;
if (e < f) { /* false! */ }  // e promoted to unsigned, becomes huge positive
```

**Critical for fixed-point**: when multiplying two `int16_t` values, promote to `int32_t` *first* to avoid overflow.

```c
int16_t a = 30000, b = 30000;
int16_t prod_bad      = a * b;           // UB if computed at int16_t width
int32_t prod_modern   = a * b;           // OK on 32-bit int platforms — promotion saves you
                                          // FAILS on 16-bit int embedded platforms
int32_t prod_portable = (int32_t)a * b;  // ALWAYS safe — explicit cast forces int32_t arithmetic
```

**Idiom in Qualcomm-style HW model code**: always do the explicit cast — `(int32_t)a * b`. Don't rely on implicit promotion when the result might exceed 16 bits. Bulletproof against any platform's int width.

### Unsigned wrap math (compute by hand)

```c
uint8_t  x = 250; x += 10;   // x = 4   (260 mod 256)
uint8_t  x = 0;   x -= 1;    // x = 255 (mod 256)
uint16_t x = 1;   x -= 2;    // x = 65535 (mod 65536)
```

The wrap is **mod 2^N** where N is the type's bit width. **Compute it explicitly** — don't guess.

### Implementation-defined vs UB (subtle)

| Operation | Standard says |
|---|---|
| Signed *arithmetic* overflow (e.g. `INT_MAX + 1` at int width) | **UB** |
| Signed narrowing assignment (e.g. assigning 130 to int8_t) | **Implementation-defined** (typically wraps to -126 on x86) |
| Unsigned overflow (any) | **Defined: wraps mod 2^N** |
| Unsigned-to-signed conversion of out-of-range value | **Implementation-defined** |
| Float-to-int conversion of out-of-range value | **UB** |

In interviews, calling narrowing "UB" is common but technically inaccurate — it's implementation-defined. Either is usually accepted.

### Signed-unsigned mixing trap (silent and devastating)

```c
unsigned int u = 1;
int s = -1;
if (s < u) printf("yes"); else printf("no");
// Compiles fine. Prints "no". No warning by default.
```

**Mechanics — same bits, different interpretation:**

```
-1 as int:           0xFFFFFFFF  (treated as signed)   → -1
-1 as unsigned int:  0xFFFFFFFF  (treated as unsigned) → 4,294,967,295
```

Two's complement means both have the **same bit pattern**. The implicit conversion is essentially free at the hardware level — just a type change. Which is why it's so easy to miss.

**The "usual arithmetic conversions"** rule: when comparing signed vs unsigned of the same rank, signed gets converted to unsigned. No warning, no UB, just wrong logic.

#### Full conversion rules

| Case | What happens |
|---|---|
| Both same type | No conversion |
| Both signed (or both unsigned), different sizes | Smaller promotes to larger |
| **One signed, one unsigned, same rank** | **Signed → unsigned** ⚠ |
| One signed, one unsigned, different ranks | If signed type can represent all unsigned values → unsigned promotes to signed. Else → both go to unsigned version of the signed type. |

The trap is the third row: `int` vs `unsigned int` (same rank, 32 bits each on most platforms) → signed converts to unsigned silently.

#### Why C converts signed → unsigned (not the other way)

Three reasons, in order of importance:

1. **Range preservation** (the practical one)
   - **Unsigned → signed conversion** of `value > INT_MAX` would overflow signed → **implementation-defined** (or UB in some contexts)
   - **Signed → unsigned conversion** is always **defined** as modular reduction (`value mod 2^N`)
   - The standard takes the safer direction → goes to unsigned

2. **Bit-pattern preservation** (low-level reason)
   - `(unsigned)-1 = 0xFFFFFFFF` — same bit pattern, just reinterpreted
   - The hardware's compare instruction works on bits; the "conversion" is a type-system fiction
   - For HW/embedded code, the bits in memory are what matter

3. **Historical / K&R legacy**
   - Pre-ANSI C had this rule; C89 codified it; backward compatibility froze it
   - Decades of code rely on this direction; can't change without breaking the world

#### Defensive idioms

```c
// BAD — implicit signed/unsigned mixing
int x = ...;
unsigned int y = ...;
if (x > y) ...                 // silent surprise

// GOOD #1 — match types from the start
int x = ..., y = ...;          // both signed
if (x > y) ...                 // works as expected

// GOOD #2 — explicit cast at the comparison (makes intent visible)
if ((unsigned)x > y) ...       // I AM intentionally treating x as unsigned

// GOOD #3 — pre-validate before mixing
if (x < 0) { /* handle negative case separately */ }
else if ((unsigned)x > y) ...
```

Compilers warn (`-Wsign-compare`) — **treat the warning as an error**.

#### TL;DR

> **C converts signed to unsigned because unsigned conversion is always defined (modular), while unsigned-to-signed of large values is undefined or implementation-defined. The bit pattern is preserved; the semantic value is silently reinterpreted.**

This is foundational for any HW-adjacent C work — Qualcomm, embedded, kernel, firmware all hit this constantly.

**Common bug pattern at Qualcomm-style code:**
```c
for (int i = width - 1; i >= 0; i--) { ... }      // safe
// Refactor: change i to size_t
for (size_t i = width - 1; i >= 0; i--) { ... }   // INFINITE LOOP
                                                    // size_t is always >= 0
```

**Protective habits:**
- Enable `-Wsign-compare` and treat as error
- Use signed types for indices that might go negative (loop counters)
- Use `size_t` only when it monotonically can't be negative (sizes, lengths)
- Cast explicitly when crossing the boundary: `(int)u`, `(unsigned)s`

### Adding negative to unsigned: NOT UB (defined wrap)

```c
int8_t arr[5] = {-1, -2, -3, -4, -5};
unsigned sum = 0;
for (int i = 0; i < 5; i++) sum += arr[i];
printf("%u", sum);   // 4294967281 (= 2^32 - 15)
```

**This is fully defined**, not UB. Mechanics per iteration:

1. `arr[i]` (int8_t) promoted to `int`
2. `sum + arr[i]`: signed `int` converted to unsigned (same bit pattern reinterpreted)
3. `(int)-1` reinterpreted as unsigned = `0xFFFFFFFF` = `4294967295`
4. Unsigned addition wraps mod 2^32 — defined

Total signed sum is -15. As unsigned 32-bit: `2^32 - 15 = 4294967281`.

### UB vs defined matrix (memorize this)

| Operation | Status |
|---|---|
| Signed arithmetic overflow (`INT_MAX + 1`) | **UB** |
| Unsigned arithmetic overflow / wrap | **Defined: mod 2^N** |
| Signed→unsigned conversion of negative value | **Defined: `value + 2^N`** |
| Unsigned→signed conversion of out-of-range value | Implementation-defined |
| Mixing signed/unsigned in arithmetic | **Defined: usual arithmetic conversions** |
| Shift by negative or ≥ width | **UB** |
| Float→int conversion of out-of-range value | **UB** |

**Rule of thumb**: only signed integer arithmetic *at the type's width* triggers UB. Unsigned wrap, conversion, and mixing are all well-defined — just produce non-obvious values.

### Pattern: silent unsigned bugs in image processing

```c
// Pixel subtraction without saturation
uint8_t diff = pixel_a - pixel_b;   // if pixel_b > pixel_a, diff = (a - b + 256) — wraps

// Reverse loop with size_t
for (size_t i = n - 1; i >= 0; i--) ...   // INFINITE LOOP — size_t never < 0

// Histogram bin diff
uint16_t delta = current[bin] - baseline[bin];   // wraps when current < baseline

// Image stride underflow
size_t step = end - start;   // if end < start, step is huge positive
```

**Never confuse "weird result" with UB.** Unsigned wrap is predictable — just compute it by hand.

### Type punning rules

- **Use `memcpy` to reinterpret bits** (safe, defined)
- Don't use union except for `int32_t` ↔ `float` style — strict aliasing rules complicate this
- `char*` and `unsigned char*` can alias anything (UB-free)

```c
float f = 3.14;
uint32_t bits;
memcpy(&bits, &f, sizeof(bits));   // Safe bit reinterpret
```

---

## 7. `char*` Pointer/Value Operator Precedence

```c
char *s = "hello";
*(s+1)   // 'e' — increment pointer, then deref
*s+1     // 'h' + 1 = 'i' — deref pointer, then increment value
```

**`*` (deref) has higher precedence than `+` (addition).** So `*s+1` is `(*s) + 1`.

### 7.1 The `*p++` trap (postfix > unary precedence)

```c
*p++       // parses as *(p++)   ← postfix ++ is HIGHER than unary *
(*p)++     // increment what p points to
*++p       // pre-increment p, then deref
```

**Postfix `++` has higher precedence than unary `*`.** This bites people constantly.

```c
void increment(int *x) {
    (*x)++;    // increment the int — what you usually want
    *x++;      // ← TRAP: read *x (discard), advance local pointer x. Useless.
}
```

`*x++` is a **dead operation** when `x` is a function parameter — you discard the read value and move a local pointer copy that goes out of scope.

### Common idioms using this precedence

```c
*p++ = value;        // standard write-and-advance: writes *p, then p++. Common in copy loops.
*p++ + 1;            // useless — reads *p, advances p, adds 1, discards
char c = *p++;       // standard read-and-advance: reads *p into c, then advances p
```

Note: `*p++ = value` works as expected because the assignment uses the deref result (`*p`) as the target, while the post-increment of `p` happens after.

### Defensive habit

When ambiguous, **add parens**:

```c
(*x)++;    // unambiguous — increment the pointee
*(x++);    // unambiguous — read pointee, advance pointer
```

The compiler doesn't care about parens; the human reader does.

### Full precedence cheatsheet for pointers

| Precedence | Operators | Example |
|---|---|---|
| Highest | `[]`  `()`  `->`  `.`  postfix `++/--` | `arr[i]`, `f(x)`, `s->x`, `p++` |
| | unary `*`  unary `&`  prefix `++/--`  `!`  `~` | `*p`, `&x`, `++p`, `!cond` |
| | `* / %` | `a * b` |
| | `+ -` | `a + b` |
| | `<< >>` | shifts |
| | `< <= > >=` | comparisons |
| | `== !=` | equality |
| | `& ^ |` | bitwise (in this order) |
| | `&& ||` | logical |
| | `?:` | ternary |
| Lowest | `=` and compound assignments | `a = b`, `a += b` |

**Trap**: `==` and `!=` are HIGHER than `&` `^` `|`. So `if (x & 0xF == 0)` parses as `if (x & (0xF == 0))` — covered in MCQ Q12. Always parenthesize bitwise expressions.

---

## 8. Quick Self-Test Checklist (cold readiness)

Can you answer these in 30 sec each? If not, drill more.

- [ ] What does `int (*p)[10]` declare?
- [ ] What does `int *p[10]` declare?
- [ ] Can you modify `*p` if `const int *p`?
- [ ] Can you modify `p` itself if `int * const p`?
- [ ] If `int *p; p++` advances how many bytes?
- [ ] What does `q - p` return when both are `int*`?
- [ ] If `int arr[10]`, what is `sizeof(arr)`?
- [ ] If function takes `int x[10]`, what is `sizeof(x)` inside?
- [ ] Read `int (*(*foo)(int))[3]`
- [ ] Difference between UB and runtime error?

---

## 9. Bit Shifts — UB Rules (Qualcomm-relevant for fixed-point)

**The rule (C §6.5.7p3)**: *Shift by negative amount, or by ≥ width of promoted left operand, is **UB**.*

```c
uint8_t x = 200;
x >> -8;     // UB — negative shift
x >> 8;      // OK — pixel gets promoted to int (32 bits) before shift
x >> 32;     // UB on 32-bit int promotion (shift = width)
x << 33;     // UB — shift > width
```

**Don't conflate:**
- **Unsigned wrap** (defined, mod 2^N): only for `+`, `-`, `*`. Shift is NOT covered by this rule.
- **Signed overflow / shift OOB**: UB. Standard makes no guarantee.

**Defensive pattern for fixed-point code:**
```c
int amount = compute_shift();
if (amount < 0 || amount >= 32) { /* error path */ }
result = value >> amount;
```

**DSP/ISP idiom — bound at type level:**
```c
uint8_t amount = ...;   // can't go negative
if (amount < 32) result = value >> amount;
```

### 9.1 Full shift UB cheatsheet (Qualcomm interview-grade)

The "negative or ≥ width" rule is the headline, but there are **three more signed-specific traps** that show up in interview questions:

| Case | Behavior |
|---|---|
| `x << -3` (negative shift count) | **UB** |
| `x >> -3` | **UB** |
| `x << 32` on 32-bit type (≥ width) | **UB** |
| `(-1) << 1` (left-shift of **negative signed**) | **UB** (pre-C++20) |
| `1 << 31` for `int32_t` (signed result not representable) | **UB** (pre-C++20) |
| `(-8) >> 1` (right-shift of **negative signed**) | **Implementation-defined** (usually arithmetic, sign-extends) |
| `1U << 31` (unsigned, fits in width) | **Defined** = `0x80000000` |
| `1U << 32` (unsigned, ≥ width) | **UB** (still!) |
| `0xFFFFFFFFU << 1` (unsigned, "overflow") | **Defined** = `0xFFFFFFFE` (modular wrap) |

### 9.2 The five mental rules

1. **Shift count must be `[0, width-1]`** — always. Negative or ≥ width is UB regardless of signed/unsigned.
2. **Unsigned left-shift** is modular (wraps), defined as long as count is in range.
3. **Signed left-shift** is UB if the bit pattern doesn't fit as a non-negative value of that type. *(Loosened in C++20 for two's-complement, but assume UB in interviews.)*
4. **Signed right-shift of negative** = implementation-defined (almost always arithmetic shift on real compilers, but spec doesn't promise — fills with sign bit).
5. **Type of shift expression = type of LEFT operand** (after promotion). So `1 << 31` is **signed-int** land, not unsigned. This is the trap behind Q10.

### 9.3 The `1U` idiom (fix in real code)

```c
// BAD — UB on signed int
uint32_t mask = (1 << 31);

// GOOD — unsigned literal, defined behavior
uint32_t mask = (1U << 31);          // = 0x80000000
uint32_t bit_n = (1U << n);          // safe iff n in [0, 31]

// Bit-set / bit-clear / bit-toggle / bit-test idioms — always 1U
flags |=  (1U << pos);               // set
flags &= ~(1U << pos);               // clear
flags ^=  (1U << pos);               // toggle
bool on = (flags >> pos) & 1U;       // test
```

**Why it matters for camera/HW code**: embedded code constantly does register manipulation with `(1 << bit)`. If `bit` could be 31 on a 32-bit signed type → UB lurking. Always `1U` (or `1UL`, `1ULL` for wider types).

---

## 10. memcpy / memmove — Implementation Notes

### Quick Q&A reference (the 4 questions every C newcomer asks)

**Q: Why `unsigned char*` for byte work (vs `char*` or `int*`)?**
1. **Byte granularity** — pointer arithmetic advances by exactly 1 byte
2. **Strict aliasing safety** — `char*` / `unsigned char*` / `signed char*` are the ONLY types that can safely alias any other type without UB
3. **No high-bit surprises** — `unsigned char(255)` stays 255 in arithmetic; `signed char(0xFF)` becomes -1 when promoted

**Q: Is plain `char` signed or unsigned?**
Implementation-defined. Three distinct types:
- `char` — might be signed or unsigned (x86 GCC: signed; ARM: usually unsigned)
- `signed char` — always signed, -128 to 127
- `unsigned char` — always unsigned, 0 to 255

When signedness matters, write `signed char` or `unsigned char` explicitly.

**Q: Is `size_t` the same as `uint32_t`?**
No — `size_t` is platform-dependent:
- 64-bit systems: 8 bytes (≈ uint64_t)
- 32-bit systems: 4 bytes (≈ uint32_t)
- Embedded: might be 2 bytes

It's "unsigned, big enough to hold any object size on this platform." Use for sizes/lengths/indices.

**Q: Why `void*` for memcpy params?**
- `void*` = generic pointer, any type's address fits
- Lets memcpy work on any data without explicit casts at call site
- Cannot be dereferenced — must cast inside the function (typically to `unsigned char*`)

**Q: Why does memcpy return `void*` (specifically `dest`)?**

Three reasons:

1. **Convention** — standard library `memcpy`, `memmove`, `memset`, `strcpy` all return `dest`. Matching saves a (minor) red flag. Skipping `return dest` signals you don't know the standard.

2. **Chaining / inline initialization** — lets you write:
   ```c
   char *copy = my_memcpy(malloc(n), src, n);     // alloc + copy in one line
   my_memcpy(my_memcpy(buf, src1, n1) + n1, src2, n2);   // append two buffers
   ```

3. **Use in expressions** — return value flows into the next call without an intermediate variable.

**Honest reality**: 95% of callers throw the return away because they already have `dest`. It's a historical leftover from C's "make functions composable cheaply" philosophy. But always include it.

**Q: Why `const void*` for `src`?**
- Promises we won't write through `src`
- Catches bugs at compile time
- Lets caller pass const arrays without casts

---

### Function signature anatomy

```c
void *my_memcpy(void *dest, const void *src, size_t n);
```

**Why `void*`:**
- Generic pointer — can hold address of ANY type
- Lets memcpy work on any data (int[], struct[], float[], raw bytes)
- Caller passes any pointer type without explicit cast (implicit `T*` → `void*`)
- **Cannot be dereferenced directly** — must cast inside the function

**Why `const void*` for src:**
- Promises we won't write through `src`
- Catches bugs at compile time
- Lets caller pass const arrays

**Why return `void*`:**
- Returns `dest` for chaining/inline initialization (`char *p = memcpy(malloc(n), src, n)`)
- Convention: standard `memcpy`/`memmove`/`memset`/`strcpy` all do this
- Mostly historical — most callers ignore the return, but skipping it is a (minor) red flag

**Why `size_t` for n:**
- Unsigned, big enough for any object size on the platform
- 8 bytes on 64-bit, 4 bytes on 32-bit
- `size_t` is NOT specifically `uint32_t` — it's whatever can hold any object size

### Why `unsigned char*` for byte work

Three reasons:

1. **Plain `char` signedness is implementation-defined** — x86 GCC: signed; ARM: unsigned. Always specify explicitly.

2. **`unsigned char*` (and `char*`) can ALIAS ANY type** without violating strict aliasing. This is unique to character types. You can't safely byte-walk an `int` through a `short*` (UB), but you can through `unsigned char*` (defined). That's why memcpy works on arbitrary data.

3. **Byte granularity is what we want** — pointer arithmetic increments by 1 byte, which is exactly the unit we're copying.

### Standard library: memcpy vs memmove

- **`memcpy`**: undefined behavior if regions overlap. Caller's responsibility to avoid overlap.
- **`memmove`**: handles overlap correctly. Slightly slower because of overlap detection.

**At Qualcomm**: HW models often need overlap-safe behavior, but the function is *named* memcpy by convention. Common interview ask: "implement memcpy that's overlap-safe."

### Overlap detection — the two cases

```
Case A: dest > src  (forward overlap)
src ────────►
       dest ────────►
       └── overlap ──┘
Forward copy CORRUPTS. Must copy backward (start from end).

Case B: src > dest  (backward overlap)
       src ────────►
dest ────────►
       └── overlap ──┘
Backward copy CORRUPTS. Forward copy WORKS.

Case C: no overlap
Either direction works.
```

### Direction selection (the simple rule)

```c
if (dest < src) {
    // forward copy (handles Case B and C)
    for (i = 0; i < n; i++) d[i] = s[i];
} else {
    // backward copy (handles Case A and C)
    for (i = n; i > 0; i--) d[i-1] = s[i-1];
}
```

**Why this works:** picking direction based on `dest < src` vs `dest >= src` covers all overlap scenarios. You don't need to check `n` at all — direction alone is sufficient.

**Mental shortcut: "Copy AWAY from the overlap."**
- If dest is AHEAD of src (overlap to the right) → copy backward (away)
- If dest is BEHIND src (overlap to the left) → copy forward (away)

**Intuition:** copy in the direction where each write happens AFTER the corresponding read. If `dest < src`, forward copy reads byte 0 first (= `src[0]`), writes to `dest[0]`. Since `dest[0] < src[0]` in memory, the write is "behind" us — no risk to unread src bytes. Reverse logic for `dest > src`.

### Subtle: `void*` comparison is technically UB

```c
if (dest > src) ...
```

C standard only guarantees pointer comparison within the **same array** (or one-past-the-end). Comparing pointers to unrelated objects is technically UB.

**In practice**: every flat-memory system gives you what you expect, and standard library `memmove` itself relies on this. Universally accepted in interview code.

**For strict-standards correctness** (senior-level discussion):
```c
if ((uintptr_t)dest > (uintptr_t)src) ...
```

Cast to `uintptr_t` first, then compare as integers. Defined for any two pointers.

### n=0 edge case

```c
if (n == 0) return dest;   // first line of robust memcpy
```

Without this guard, the backward branch's `d += n - 1` triggers UB:
- `n` is `size_t` (unsigned) → `n - 1` wraps to `SIZE_MAX` when `n=0`
- `d += SIZE_MAX` is pointer arithmetic outside the object → UB

Standard `memcpy(dest, src, 0)` is **defined as a no-op** per C11 §7.24.1p2. Match the standard.

### Final canonical implementation

```c
void *my_memcpy(void *dest, const void *src, size_t n) {
    if (n == 0) return dest;
    
    unsigned char *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;
    
    if (dest > src) {
        d += n - 1;
        s += n - 1;
        for (size_t i = n; i > 0; i--) {
            *d-- = *s--;
        }
    } else {
        for (size_t i = 0; i < n; i++) {
            *d++ = *s++;
        }
    }
    return dest;
}
```

### Off-by-one trap in backward copy

If you set `d += n` and `s += n` first, your pointers are **one PAST the last valid byte**. The first iteration writes out-of-bounds.

Two correct patterns:

**Pattern 1 — start at last index:**
```c
d += n - 1;
s += n - 1;
for (size_t i = n; i > 0; i--) {
    *d-- = *s--;
}
```

**Pattern 2 — pre-decrement:**
```c
d += n;
s += n;
for (size_t i = 0; i < n; i++) {
    *--d = *--s;
}
```

Both produce the same result. Pick one and stay consistent.

### Test cases for any memcpy implementation

Always test these 4 cases:

```c
char buf[10] = "ABCDEFGHIJ";

// 1. dest > src overlap (Case A — forward fails)
my_memcpy(buf + 2, buf, 5);
// Expected: "ABABCDEHIJ"

// 2. dest < src overlap (Case B — backward fails)
my_memcpy(buf, buf + 2, 5);
// Expected: "CDEFGFGHIJ"

// 3. No overlap
char a[5] = "Hello";
char b[5];
my_memcpy(b, a, 5);
// Expected: b = "Hello"

// 4. n = 0 (must be a no-op, not crash)
my_memcpy(dest, src, 0);
// Expected: dest unchanged
```

---

## 10.5 Image Stride & Alignment (HW-aware optimization)

**Cache lines** (typically 64 bytes x86, 128 bytes some Apple/ARM):
- Accesses within same line ≈ free
- Cache miss = 100+ cycles
- Unaligned row starts → wasted prefetch + extra misses

**SIMD alignment** (NEON 16-byte vector):
- `vld1q_u8` faster on 16-byte-aligned addresses
- Misaligned loads cross cache lines → 2 transactions

**CUDA coalescing** (warp = 32 threads):
- Coalesced access: 1 memory transaction per warp
- Uncoalesced: up to 32 transactions → **32x bandwidth penalty**

**Standard idiom — pad stride to alignment:**
```c
size_t aligned_stride = (width + 31) & ~31;   // multiple of 32
size_t buf_size = height * aligned_stride;
```

**Always index as:** `img[row * stride + col]` — never `width`. Stride and width may differ when padding is applied.

**Interview gold**: when asked "how to optimize this filter?", lead with stride alignment + cache line awareness + coalescing/vector load alignment. Senior-engineer answer.

---

## 11. strcmp — Implementation Notes

### Spec

```c
int my_strcmp(const char *s1, const char *s2);
```

Returns:
- `0` if equal
- Negative if `s1 < s2` lexicographically
- Positive if `s1 > s2` lexicographically

Standard guarantees only **sign**, not magnitude. Returning the actual byte difference (`*s1 - *s2`) is the de-facto convention used by glibc/Windows/etc.

### Canonical implementation (5 lines)

```c
int my_strcmp(const char *s1, const char *s2) {
    const unsigned char *p1 = (const unsigned char *)s1;
    const unsigned char *p2 = (const unsigned char *)s2;
    
    while (*p1 != '\0' && *p1 == *p2) {
        p1++;
        p2++;
    }
    return *p1 - *p2;
}
```

### Why one subtraction handles all 5 endings

Loop exits when either:
- A mismatch (`*p1 != *p2`) — `*p1 - *p2` gives the byte difference (correct sign)
- s1 hits `'\0'` — implies one of {both ended, s1 ended first}

Single `return *p1 - *p2;` covers everything:

| State | `*p1` | `*p2` | result | meaning |
|---|---|---|---|---|
| Mismatch s1 < s2 | 'a' | 'b' | -1 | negative ✓ |
| Mismatch s1 > s2 | 'b' | 'a' | +1 | positive ✓ |
| s1 ended first | '\0' | 'd' | -100 | negative ✓ (s1 shorter) |
| s2 ended first | 'd' | '\0' | +100 | positive ✓ (s1 longer) |
| Both ended | '\0' | '\0' | 0 | equal ✓ |

No post-loop branching needed. **Standard library `strcmp` source is literally this.**

### Why cast to `unsigned char`

Plain `char` signedness is implementation-defined. For chars with bit 7 set:

```c
char c = 0xE0;   // 'à' or similar
// On signed-char platform (x86 GCC default):
//   c == -32   when promoted to int
// On unsigned-char platform (ARM):
//   c == 224   when promoted to int
```

For string comparison, we want **byte values 0-255** (unsigned). Without the cast:

```c
"àbc" vs "abc"
// 'à' (0xE0) - 'a' (0x61):
//   signed:   -32 - 97  = -129  (wrong sign for the comparison we want!)
//   unsigned: 224 - 97  = +127  (correct: à > a)
```

Standard `strcmp` always casts to `unsigned char` for this reason.

### Common bugs (exercises caught)

- `'\0'` vs `'/0'` — backslash vs forward slash (multi-char constant)
- Missing dereference: `s1 != '\0'` (compares pointer) vs `*s1 != '\0'` (compares char)
- Lost const on cast: `unsigned char*` instead of `const unsigned char*`
- `elseif` (invalid in C) vs `else if`
- Sign inversion in length-difference branches
- Subtracting same pointer from itself: `*s1 - *s1` = 0 always (typo trap)

### When you need `unsigned char` cast vs when you don't

| Operation | Need unsigned cast? | Why |
|---|---|---|
| **Equality check** (`*p == '\0'`, `*p == 'A'`) | NO | Bit pattern compared, signedness irrelevant |
| **Ordering check** (`*p1 < *p2`, `*p1 > 'a'`) | YES | Signed `char` (e.g. 0xE0 = -32) gives wrong sign |
| **Arithmetic on bytes > 127** | YES | Sign extension corrupts the value |
| **Byte-level pointer subtraction** (strlen-style) | NO (or both same type) | Only counts elements, no value math |

### The portability angle (cross-platform pitfall)

Plain `char` signedness is **implementation-defined**:
- x86 GCC/Clang: `char` = `signed char`
- ARM compilers: `char` = `unsigned char`

So the same comparison code gives different results across platforms:

```c
char c = 0xE0;          // 'à' or similar high-bit byte

// On x86 GCC:    c == -32 → c > 'a' (97)? FALSE
// On ARM:        c == 224 → c > 'a' (97)? TRUE
```

That's a portability bug. Standard `strcmp` casts to `unsigned char` to lock unsigned behavior on every platform — guaranteed consistent ordering.

**Rule of thumb for cross-platform / HW-port code:**
- *Equality* checks (`x == 0`, `x == '\0'`): plain `char` is fine
- *Ordering* checks (`<`, `>`): always cast to `unsigned char` first
- *Arithmetic on bytes that might be ≥128*: cast to `unsigned char`

---

## 11.6 Bit Manipulation — Common Patterns

### Counting set bits (Hamming weight / popcount)

**Naive — check each bit:** O(width)
```c
int popcount(uint32_t n) {
    int count = 0;
    while (n) {
        if (n & 1) count++;
        n >>= 1;
    }
    return count;
}
```

**Brian Kernighan's algorithm — clear lowest set bit each iteration:** O(set-bits)
```c
int popcount(uint32_t n) {
    int count = 0;
    while (n) {
        n &= (n - 1);  // clears the lowest set bit
        count++;
    }
    return count;
}
```

**Why it works:**
```
n     = 0b1100
n - 1 = 0b1011    // flips all bits up to and including lowest set bit
n & (n-1) = 0b1000   // ANDing clears that lowest set bit
```

For sparse inputs (e.g., 4 set bits in a uint32_t), iterates 4 times vs 32. Big win.

**Compiler intrinsic** (production code):
```c
int popcount(uint32_t n) { return __builtin_popcount(n); }
```

CPUs with SSE4+ or ARMv8 have a single `popcnt` instruction. Don't lead with this in interviews — show the algorithm first.

### Useful bit tricks (memorize these)

```c
// Check if bit i is set
(n >> i) & 1

// Set bit i
n |= (1u << i)

// Clear bit i
n &= ~(1u << i)

// Toggle bit i
n ^= (1u << i)

// Check if power of 2 (n must be > 0)
(n & (n - 1)) == 0

// Lowest set bit (isolate it)
n & -n
n & (-n)        // same — relies on two's complement

// Clear lowest set bit
n &= (n - 1)

// Set all bits up to and including lowest set bit
n | (n - 1)

// Sign extend: replicate bit `b` to all higher bits
// Useful when widening signed values

// Swap two ints without temp (XOR trick)
a ^= b; b ^= a; a ^= b;

// Min/max without branches (signed)
int min(int a, int b) { return b ^ ((a ^ b) & -(a < b)); }
int max(int a, int b) { return a ^ ((a ^ b) & -(a < b)); }
```

### Reverse bits (LC 190 pattern)

**Naive — bit-by-bit:** O(width)
```c
uint32_t reverseBits(uint32_t n) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        result = (result << 1) | (n & 1);
        n >>= 1;
    }
    return result;
}
```

**Divide-and-conquer (constant time, no loop):**
```c
uint32_t reverseBits(uint32_t n) {
    n = (n >> 16) | (n << 16);
    n = ((n & 0xff00ff00) >> 8) | ((n & 0x00ff00ff) << 8);
    n = ((n & 0xf0f0f0f0) >> 4) | ((n & 0x0f0f0f0f) << 4);
    n = ((n & 0xcccccccc) >> 2) | ((n & 0x33333333) << 2);
    n = ((n & 0xaaaaaaaa) >> 1) | ((n & 0x55555555) << 1);
    return n;
}
```

Each line swaps groups of size 16, 8, 4, 2, 1. Total: 5 operations regardless of input. Used in DSP/ISP code where reversing bits is a hot path.

### Single number (LC 136 — XOR trick)

If every element appears twice except one, XOR all elements:
- `a ^ a = 0` (any element XOR'd with itself is 0)
- `a ^ 0 = a` (XOR with 0 is identity)
- Order doesn't matter (XOR is commutative + associative)

```c
int singleNumber(int* nums, int n) {
    int result = 0;
    for (int i = 0; i < n; i++) result ^= nums[i];
    return result;
}
```

O(n) time, O(1) space. Beautiful, common interview answer.

### Why this matters at Qualcomm

Bit manipulation is the *bread and butter* of:
- Fixed-point arithmetic (rounding bits, saturation, sign extension)
- ISP statistics (bin counting, histogram packing)
- HW register modeling (set/clear/test specific bits)
- Performance optimization (popcount → cache utilization metrics)

Confirmed in May 2026 Qualcomm Firmware interview: 2 LeetCode-style **bit manipulation** coding questions.

---

## 11.7 Sequence Points & Modify-Same-Variable UB

**The rule (C99/C11)**: between two sequence points, modifying the same object more than once OR reading + modifying it is **UB**.

```c
i = i++;          // UB
a[i] = i++;       // UB
i++ + i++;        // UB
x = x++ + ++x;    // UB — modifies x twice
arr[i++] = i;     // UB — depends on order of i++ vs read
```

### Why it's UB

C compilers don't guarantee left-to-right evaluation for most operators. The compiler can:
- Evaluate `++x` before `x++`
- Reorder side effects across the `+`
- Use any cached value of `x`

Different compilers / optimization levels can produce different output. Some make it crash. Standard explicitly says **don't rely on any behavior**.

### Sequence points (between which order IS defined)

| Operator / construct | Effect |
|---|---|
| `,` (comma operator) | Left side fully evaluated before right |
| `&&`, `\|\|` | Short-circuit — left fully evaluated first |
| `?:` (ternary condition) | Condition evaluated before either branch |
| Statement boundary `;` | All side effects of statement complete |
| Function call boundary | Arguments evaluated before call (but order *between* args is unspecified) |

### Function arguments — partial defined-ness

```c
foo(i++, i++);    // UB — order of i++ between args unspecified
foo(i, i++);      // UB — read AND modify of i, order unspecified
```

But:
```c
i++;
foo(i);           // OK — sequence point at `;` between
```

### How to spot it

If a single expression both **reads** and **modifies** the same variable, OR **modifies it twice**, suspect UB. Examples to memorize as red flags:

```c
i = i++;
a[i++] = b[i];
*p++ = *p;
arr[i] = arr[i+1] = 0;   // OK actually — assignment chain has rightward sequence points (subtle)
```

### Why it matters at Qualcomm

C compilers (especially GCC/Clang at `-O2`) aggressively optimize based on "UB never happens" assumptions. Code that "works" at `-O0` can silently break at `-O2`. This is a classic source of **release-mode-only bugs** — hardest to debug.

**Senior engineer signal**: spotting UB instantly when reading code. Common interview filter for C depth.

---

## 11.8 Struct Padding & Alignment (HW-relevant)

### Why padding exists

CPUs read memory in aligned chunks. Reading an `int` at an unaligned address requires multiple memory transactions (slow on x86, crashes on strict-alignment platforms like older ARM/MIPS).

The compiler inserts **padding bytes** between struct members to ensure each member starts at a properly aligned address.

### The rules

1. **Each member aligned to its natural alignment** (typically `sizeof(type)` for primitives, or the alignment of the strictest member for compound types)
2. **Struct total size must be multiple of the strictest member alignment** — so arrays of the struct keep all elements aligned

### Worked example

```c
struct A {
    char c;     // 1 byte
    int i;      // 4 bytes (4-byte alignment)
    char d;     // 1 byte
};
sizeof(struct A) == ?
```

Layout on 64-bit system:
```
offset 0:  char c    (1 byte)
offset 1:  ────────── (3 bytes padding — int needs 4-byte alignment)
offset 4:  int i     (4 bytes, ends at offset 8)
offset 8:  char d    (1 byte)
offset 9:  ────────── (3 bytes trailing — struct size multiple of 4)
total:     12 bytes
```

`sizeof(struct A) = 12`, NOT 6.

### Worked example — bigger trailing padding (the Q20 trap)

```c
struct B {
    char   a;     // 1 byte
    double b;     // 8 bytes (8-byte alignment!)
    char   c;     // 1 byte
};
sizeof(struct B) == ?
```

Layout on 64-bit:
```
offset  0:  char a       (1 byte)
offset  1:  ──────────── (7 bytes padding — double needs 8-byte alignment)
offset  8:  double b     (8 bytes, ends at offset 16)
offset 16:  char c       (1 byte)
offset 17:  ──────────── (7 bytes TRAILING — struct size multiple of 8)
total:      24 bytes
```

**Answer: 24, not 17, not 12.**

#### Why the trailing 7 bytes?

If you had `struct B arr[2]`, then:
- `arr[0]` starts at offset 0 → `arr[0].b` at offset 8 (aligned ✓)
- `arr[1]` must start at a multiple of 8 (otherwise `arr[1].b` would be misaligned)

So struct size = next multiple of strictest alignment (8) ≥ last-byte-used (17) = **24**.

#### The trailing-padding rule (memorize)

> **Struct total size = next multiple of `alignof(strictest member)` ≥ offset of last byte used.**

This is independent of internal padding. Even a struct with no internal gaps can have trailing padding:
```c
struct C { double d; char c; };  // d=8, c=1, then 7 trailing → 16 bytes
```

### Reorder to save space (senior engineer move)

```c
struct A_better {
    int i;      // 4 bytes at offset 0
    char c;     // 1 byte at offset 4
    char d;     // 1 byte at offset 5
    // 2 bytes trailing padding
};
sizeof = 8     // saved 4 bytes
```

**Rule of thumb**: order members **largest to smallest** for minimal padding. Critical for large arrays of structs (saves cache, memory bandwidth).

### `__attribute__((packed))` — when you need exact layout

GCC/Clang extension to force no padding:

```c
struct __attribute__((packed)) HwRegister {
    uint8_t  status;
    uint32_t address;
    uint8_t  command;
};
sizeof(struct HwRegister) == 6   // no padding
```

**Used for:**
- HW register layouts (must match exact silicon offsets)
- Network packet definitions (wire format)
- File format parsers (binary on-disk layout)
- Cross-platform serialization

**Trade-off**: packed structs may force unaligned access, which can be:
- Slow on x86 (extra cycles per load)
- UB on strict-alignment platforms (older ARM, MIPS, some embedded)

For HW models at Qualcomm: you'll see packed structs everywhere mapping silicon registers. The HW model has to byte-match silicon, padding-free.

### `alignas` and `alignof` (C11/C++11)

```c
alignas(16) uint8_t buffer[256];   // align buffer to 16-byte boundary
alignof(int)                       // typically 4 (alignment requirement of int)
```

Used for:
- SIMD vector buffers (NEON 16-byte, AVX2 32-byte)
- Cache line alignment (typically 64 bytes)
- DMA-friendly buffers

**Idiom — cache-line-aligned struct:**
```c
struct alignas(64) CacheFriendly {
    uint64_t data[8];   // exactly one cache line
};
```

### Why this matters at Qualcomm

- **HW register modeling**: must match silicon byte-by-byte → packed structs
- **DMA buffers**: aligned to specific boundaries → `alignas`
- **SIMD-vectorized code**: aligned loads (vld1q_u8 etc.) require 16-byte alignment
- **Memory-mapped I/O**: writing to specific register offsets requires layout precision

**Senior engineer signal**: knowing both that padding exists AND when to fight it (packed/alignas).

### Eigen alignment connection (transferable from your SLAM work)

You've seen `EIGEN_MAKE_ALIGNED_OPERATOR_NEW` in Eigen-using SLAM code. **It's the same alignment story applied to heap allocation.**

```cpp
class MyClass {
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    Eigen::Vector4d vec;       // needs 16-byte alignment for SSE
    int other_stuff;
};
```

**The problem**: default `new` returns 8-byte aligned memory on most platforms. If `vec` lives at offset 0 inside `MyClass`, it's only 8-byte aligned → `_mm_load_ps` (aligned SSE load) might segfault.

**The macro fix**: overrides `operator new` / `operator delete` to use a 16-byte aligned allocator. Now `new MyClass()` returns memory aligned to 16 bytes → `vec` is properly aligned → SIMD safe.

### Same family of mechanisms (different scopes)

| Mechanism | Scope | Purpose |
|---|---|---|
| Struct padding | Within struct | Compiler aligns members |
| `__attribute__((aligned(N)))` / `alignas(N)` | Variable / type | Force N-byte alignment |
| `__attribute__((packed))` | Within struct | Force NO padding (HW layouts) |
| `EIGEN_MAKE_ALIGNED_OPERATOR_NEW` | Heap-allocated class | Force allocator to return aligned memory |
| `posix_memalign` / `aligned_alloc` | Manual heap | Allocate aligned memory directly |
| C++17 over-aligned `new` | Heap | Standard handles over-aligned types |

All trying to satisfy CPU/SIMD/HW alignment requirements at different scopes.

### Bridge to Qualcomm

Your Eigen alignment experience transfers directly:
- **NEON intrinsics**: 16-byte aligned loads (same constraint as SSE)
- **Hexagon HVX**: 1024-bit / 128-byte vectors
- **ISP DMA buffers**: often 4096-byte (page) aligned
- **Cache line alignment**: 64-byte (x86), 128-byte (some Apple)

**Interview gold framing**: *"I've worked with Eigen alignment in SLAM where SSE-vectorized matrix ops needed 16-byte aligned heap allocations — the same thinking applies to NEON for ISP code, and Hexagon HVX for DSP-level work. Alignment isn't a Qualcomm-specific concept; it's a HW reality that shows up everywhere SIMD does."*

That's a senior-engineer-level connection from your SLAM experience to camera modeling.

---

## 11.9 Endianness (HW/network/file format gotcha)

### Definition

**Endianness = byte order convention for storing multi-byte values in memory.**

- **Little-endian (LE)**: LSB (least significant byte) at *lowest* address
- **Big-endian (BE)**: MSB (most significant byte) at *lowest* address

### Visual

For `uint32_t x = 0x12345678;`:

```
Little-endian (x86, ARM normal mode):
addr:  0    1    2    3
byte:  78   56   34   12     ← LSB first

Big-endian (network byte order, older PowerPC):
addr:  0    1    2    3
byte:  12   34   56   78     ← MSB first
```

### Mnemonic

- **Little-endian** = "Little end first" — the *small* end (LSB) lives at the *low* address
- **Big-endian** = "Big end first" — the *big* end (MSB) lives at the *low* address

### Reading bytes through pointers

```c
uint32_t x = 0x12345678;
uint8_t *p = (uint8_t *)&x;
p[0]    // 0x78 on LE,  0x12 on BE
p[3]    // 0x12 on LE,  0x78 on BE
```

### Why cast to `uint8_t *` (byte-granular access)

The cast lets you read **one byte at a time** from a multi-byte value. The pointer type tells the compiler *how much* to read when you dereference.

```c
uint32_t x = 0x12345678;

uint32_t *p32 = &x;
*p32           // reads 4 bytes → 0x12345678 (whole int)

uint8_t *p8 = (uint8_t *)&x;
*p8            // reads 1 byte → 0x78 (LE) or 0x12 (BE)
p8[0]          // same as *p8
p8[1]          // byte at &x + 1
```

Both `p32` and `p8` point to the *same address* — only the **type** differs, which determines:
1. Bytes read per dereference (sizeof(*p32)=4 vs sizeof(*p8)=1)
2. Pointer arithmetic step (p32+1 advances 4 bytes, p8+1 advances 1)

### Why it's safe (strict aliasing exception)

Casting any pointer to **`char *`, `unsigned char *`, or `uint8_t *`** is the unique strict-aliasing exception in C — these types can legally alias any other type without UB. That's exactly why:

- `memcpy` uses `unsigned char *` internally to walk bytes
- Endianness inspection casts `&x` to `uint8_t *`
- Generic byte-buffer code (image data, network packets) uses `uint8_t *`

```c
// SAFE (strict aliasing exception)
uint8_t *p = (uint8_t *)&x;
p[0];       // OK

// UB (strict aliasing violation)
uint16_t *p16 = (uint16_t *)&x;
*p16;       // UB on most compilers with -O2
```

### Use cases for byte-pointer casts

| Use case | Pattern |
|---|---|
| Endianness check | `*(uint8_t *)&x == 1` |
| Memcpy / memmove | Cast `void *` to `unsigned char *`, walk byte-by-byte |
| Hash function input | Treat any object as bytes via `unsigned char *` |
| Byte-level serialization | Dump struct as bytes for transport/storage |
| Hex dump / debugging | Print each byte of a value or buffer |
| Image processing | `uint8_t *` for pixel access |

**Universal pattern**: when you need byte granularity from any data type, cast to `uint8_t *` (or `unsigned char *`). It's defined behavior and what every byte-walking function does internally.

### Detect endianness at runtime

```c
int is_little_endian() {
    uint32_t x = 1;
    return *(uint8_t *)&x == 1;
}
```

`x = 1` has bytes `00 00 00 01` conceptually.
- LE: byte at `&x` is `0x01` → returns 1
- BE: byte at `&x` is `0x00` → returns 0

### Common platforms

| Platform | Endianness |
|---|---|
| x86 / x86-64 | Little-endian |
| ARM (typical mode, including Snapdragon) | Little-endian |
| RISC-V | Little-endian (mostly) |
| Older PowerPC, SPARC | Big-endian |
| Network byte order (TCP/IP) | Big-endian |
| JPEG, DNG file formats | varies (typically BE for legacy) |

### Standard byte-swap functions

```c
#include <arpa/inet.h>      // POSIX
htonl(x)    // host-to-network 32-bit (always to BE)
ntohl(x)    // network-to-host 32-bit (BE to host)
htons(x), ntohs(x)   // 16-bit versions

#include <byteswap.h>       // GCC
bswap_32(x), bswap_16(x), bswap_64(x)

#include <endian.h>         // Linux
htobe32(x), be32toh(x), htole32(x), le32toh(x)
```

These compile to a single CPU instruction (`BSWAP` on x86, `REV` on ARM).

### Manual byte-swap (interview-style implementation)

```c
uint32_t swap32(uint32_t x) {
    return ((x & 0x000000FF) << 24) |
           ((x & 0x0000FF00) << 8)  |
           ((x & 0x00FF0000) >> 8)  |
           ((x & 0xFF000000) >> 24);
}

uint16_t swap16(uint16_t x) {
    return (x << 8) | (x >> 8);
}
```

### Why this matters at Qualcomm

ISP / camera systems hit endianness everywhere:

1. **Raw sensor data**: often delivered MSB-first, processed on LE chips
2. **File formats** (DNG, JPEG, RAW): have explicit endianness markers in headers
3. **Network protocols** (RTP/RTSP for video): big-endian wire format
4. **HW register access**: bus may require byte-swap depending on architecture
5. **DSP ↔ CPU shared memory**: Hexagon DSP and ARM core may need swap glue
6. **Cross-platform serialization**: file written on BE machine, read on LE

**Idiom for portable code**: always serialize/deserialize through explicit byte-order functions, never assume host endianness.

### Snapdragon-specific

- ARM cores: little-endian in normal mode
- Hexagon DSP: little-endian
- DMA buffers shared between CPU and DSP: aligned + endian-consistent
- Sensor MIPI-CSI data: depends on sensor — some big-endian, requires swap on ingest

---

## 11.10 Strict Aliasing Rule (the silent killer)

### What it says

The C standard (§6.5p7): an object can only be accessed through a pointer whose type is one of:
1. **Same type** as the object (or qualified variant — `const`, `volatile`)
2. **Compatible signed/unsigned variant** (e.g., `int` ↔ `unsigned int`)
3. **`char *`, `unsigned char *`, or `signed char *`** ← the universal exception
4. A `struct`/`union` containing such a type

Accessing an object through any **other** pointer type is **UB**.

### Concrete violation

```c
uint32_t x = 0x12345678;
uint16_t *p = (uint16_t *)&x;
*p;                          // UB — uint16_t can't alias uint32_t
```

`uint16_t *` is NOT in the allowed list when accessing a `uint32_t`.

### What UB means in practice

The compiler is **allowed to assume** that pointers of incompatible types don't point to the same memory. So:

```c
uint32_t *a = ...;
uint16_t *b = ...;
*a = 5;
*b = 10;
return *a;          // compiler may assume *a is still 5
                    // even if a and b actually overlap!
```

Optimization-level-dependent bugs:
- `-O0`: probably "works" as naively expected
- `-O2`: may reorder reads/writes, cache reads, eliminate updates → silent bugs

Classic "works in debug, breaks in release" symptom.

### Why `char *` / `unsigned char *` are exempt

Byte-level access is universal — needed for `memcpy`, hashing, serialization, IO. The exemption ensures these patterns stay defined.

```c
// SAFE (strict aliasing exception)
uint8_t *p = (uint8_t *)&x;
p[0];               // OK

// UB (strict aliasing violation)
uint16_t *p16 = (uint16_t *)&x;
*p16;               // UB on most compilers
```

### Safe ways to type-pun

**1. `memcpy` — always safe** (canonical):
```c
uint32_t x = 0x12345678;
uint16_t low_half;
memcpy(&low_half, &x, sizeof(low_half));   // safe: gets low 2 bytes (LE)
```

`memcpy` doesn't actually copy in optimized builds — the compiler recognizes the pattern and emits a load. But the strict aliasing rule treats it as defined.

**2. `union` — defined in C, UB in C++ strict mode but works on GCC/Clang:**
```c
union {
    float f;
    uint32_t bits;
} u;
u.f = 3.14f;
uint32_t b = u.bits;   // C: defined. C++: technically UB but accepted.
```

**3. `unsigned char *` cast** — defined for byte-level access:
```c
uint32_t x = 0x12345678;
unsigned char *p = (unsigned char *)&x;   // safe
p[0];   // OK
```

**4. Compiler attributes (GCC/Clang)**:
```c
typedef int __attribute__((may_alias)) int_alias_t;
int_alias_t *p = (int_alias_t *)buf;   // disables strict aliasing for this pointer
```

### Common violations to avoid

```c
// Float ↔ int bit reinterpretation
float f = 3.14f;
uint32_t bits = *(uint32_t *)&f;          // UB
uint32_t bits2; memcpy(&bits2, &f, 4);    // safe

// Reading struct as bytes via wrong pointer
struct A { int x; int y; } a;
short *p = (short *)&a;                    // UB to dereference

// Network packet parsing as struct
char buf[100] = ...;
struct Packet *p = (struct Packet *)buf;   // UB to dereference *p
                                            // unless buf is unsigned char[]
                                            // and you use memcpy
```

### The escape hatch — compiler flags

- `-fno-strict-aliasing` (GCC/Clang) disables the optimization based on this rule. Linux kernel uses this. Slower but safer.
- Most application code keeps strict aliasing on for the optimizations.

### Why this matters at Qualcomm

Heavy in HW model code:
- **Sensor data parsing**: raw bytes → typed struct → strict aliasing trap if done naively
- **Network buffer interpretation**: same problem
- **Bit-pattern reinterpretation** (float ↔ int for sign extraction, etc.)

The protective habits at HW shops:
1. Use `memcpy` for type punning (clean, defined, optimized away)
2. Use `unsigned char *` for byte-level work
3. Don't cast `void *` to anything other than the original type or `unsigned char *`

**Senior engineer signal**: knowing strict aliasing exists, what's exempt, and the `memcpy` pun pattern.

---

## 12. Tomorrow's drill suggestion

- Implement `strcmp`, `strlen`, custom `sizeof` macro
- "Find bugs in this C snippet" exercises (race conditions, use-after-free, off-by-one, UB)
- Image-processing-flavored MCQs: image buffer pointer arithmetic, fixed-point Q-format conversion, endianness for raw sensor data

---

## 13. Hardware Numbers Cheatsheet (memorize cold)

These are pure facts you can't derive — flashcard them. Showed up in MCQ Q25, Q26.

### CPU / Memory

| Item | Value | Notes |
|---|---|---|
| **Cache line size (x86 / ARM Cortex-A)** | **64 bytes** | False sharing boundary, SIMD load alignment |
| Cache line size (Apple M-series) | 128 bytes | Outlier; not relevant for Snapdragon |
| Cache line size (some embedded) | 32 bytes | Older / simpler ARM cores |
| Page size (default) | 4 KB | DMA buffers often page-aligned |
| Huge page (x86) | 2 MB / 1 GB | TLB pressure relief |
| Pointer size (64-bit ARM/x86) | 8 bytes | `sizeof(void*)` |
| Pointer size (32-bit ARM/x86) | 4 bytes | Older embedded |

### SIMD vector widths

| Extension | Register width | Platform |
|---|---|---|
| **NEON (AArch64)** | **128 bits** | Snapdragon CPU side — 32 registers |
| NEON (ARMv7) | 64 + 128 bits | Older 32-bit ARM |
| SVE / SVE2 | 128–2048 bits (scalable) | Newer ARM server / mobile |
| **Hexagon HVX** | **1024 bits** | Qualcomm DSP — way bigger than NEON |
| SSE / SSE2 | 128 bits | x86 baseline |
| AVX / AVX2 | 256 bits | x86 desktop / server |
| AVX-512 | 512 bits | High-end x86 |

**Mental anchor**: NEON = 128 (small), HVX = 1024 (big). The 8× ratio is why HVX is preferred for camera/CV throughput on Snapdragon.

### Endianness

| Platform | Endianness |
|---|---|
| **Snapdragon (ARM)** | **Little-endian** (default; ARM is bi-endian capable) |
| x86 / x86-64 | Little-endian |
| Network byte order | **Big-endian** (TCP/IP, file formats like JPEG markers) |
| ARMv8+ | Bi-endian, but Linux/Android always run LE |

### Alignment requirements (typical)

| Type | Alignment |
|---|---|
| `char` | 1 |
| `int16_t` | 2 |
| `int32_t` / `float` | 4 |
| `int64_t` / `double` / `void*` (64-bit) | 8 |
| NEON `vld1q_*` (16-byte SIMD load) | 16 |
| AVX2 aligned load | 32 |
| Cache-line (false-sharing boundary) | 64 |
| Page (DMA buffers) | 4096 |

### Snapdragon platform breakdown (Qualcomm-specific)

| Component | Role |
|---|---|
| **Kryo CPU** | ARM Cortex-A derivatives — runs Android, app code (NEON SIMD) |
| **Adreno GPU** | Graphics + GPGPU compute (OpenCL, Vulkan compute) |
| **Hexagon DSP** | Low-power scalar + HVX vector — audio, sensor fusion, parts of ISP |
| **Spectra ISP** | Dedicated camera image signal processor (fixed-function + programmable) |
| **NPU / AI Engine** | Hexagon Tensor accelerator for ML inference |

For a **Camera Systems Modeling** role, the C/C++ model you write would emulate Spectra ISP behavior bit-exactly, then engineers map blocks to Hexagon HVX or hardened logic.

### Qualcomm interview "numbers" you should be able to recite cold

```
Cache line:   64 bytes
NEON:         128 bits (16 bytes)
HVX:          1024 bits (128 bytes)
Page:         4 KB
Pointer:      8 bytes (64-bit)
double:       8 bytes
ARM:          Little-endian
Network:      Big-endian
```

If you see any of these as MCQ options, you should answer in <2 seconds.

---

## 14. Common C Bug Patterns (find-bugs ammunition)

Bugs that consistently appear in interview "find the bug" questions. Memorize the shape — they're often subtle one-liners.

### 14.1 Returning pointer to stack-allocated local (dangling pointer / UAF)

```c
char* get_message(int code) {
    char buf[64];                                // stack-allocated
    snprintf(buf, sizeof(buf), "Code: %d", code);
    return buf;                                  // ← UB: stack frame destroyed on return
}

char *msg = get_message(42);
printf("%s\n", msg);                             // reads freed memory — UB
```

The local `buf` lives on the stack and is destroyed when the function returns. Returning its address gives the caller a pointer to **dead memory**. The crash is non-deterministic: the data may "look right" briefly until the stack is reused.

#### Fix patterns

| Pattern | Code |
|---|---|
| **Caller-provides buffer (C idiom)** | `void get_msg(int code, char *out, size_t n) { snprintf(out, n, ...); }` |
| **Heap allocation** | `char *buf = malloc(64); ...; return buf;` (caller `free`s) |
| **Static buffer (DANGEROUS)** | `static char buf[64];` — not thread-safe, not reentrant |

For HW/embedded code: **caller-provides is the convention**. Zero allocation, caller controls lifetime.

### 14.2 Skipping `malloc` NULL check (the recurring HW gotcha)

```c
Point *arr = malloc(n * sizeof(Point));
arr[0].x = 1;                                    // ← segfaults if malloc returned NULL
```

`malloc` returns `NULL` on allocation failure (out of memory, fragmentation, oversized request). On embedded/HW systems with limited RAM, **this is not rare** — every malloc must be checked.

#### Fix

```c
Point *arr = malloc(n * sizeof(Point));
if (arr == NULL) return NULL;                    // propagate failure to caller
arr[0].x = 1;
```

#### Why it matters at Qualcomm

- Limited RAM on embedded targets (compared to PC/server)
- Memory fragmentation in long-running processes (camera daemon, ISP pipeline)
- Huge image / sensor frame allocations (failure more likely)
- Integer overflow in `n * sizeof(...)` → small alloc → out-of-bounds writes later (the **calloc** pattern is safer for size calc: `calloc(n, sizeof(Point))` does the multiply with overflow check)

#### Senior signal

**Always check malloc/calloc/realloc returns.** Junior code skips it; senior code propagates failures. In code review, missing null checks after allocation are a fast-fail signal.

### 14.3 Off-by-one in loop bounds (`<=` vs `<`)

```c
for (int i = 0; i <= len; i++) {                 // ← writes one past end
    buf[i] = ...;
}
```

Valid indices for an array of length `len` are `0..len-1`. `<=` writes to `buf[len]` — out-of-bounds.

**Rule**: when iterating an array of length `N`, use `i < N`, not `i <= N`. Strict less-than.

The classic exception: when iterating *between* boundaries (e.g., bilinear interpolation needs both pixel and pixel+1), bounds may legitimately go to `len-1` only.

### 14.4 Saturation arithmetic (image-data context)

```c
buf[i] = buf[i] * 2;                             // uint8_t * 2: 200 * 2 = 144 (wraps mod 256)
```

For image data (intensity values), wrap-around is **not** what you want. A bright pixel becomes dim. Use saturation:

```c
int doubled = buf[i] * 2;
buf[i] = doubled > 255 ? 255 : (uint8_t)doubled; // clamp to max
```

#### Saturation idioms

```c
// Saturate add of two uint8_t to [0, 255]
uint8_t saturating_add(uint8_t a, uint8_t b) {
    uint16_t sum = (uint16_t)a + b;
    return sum > 255 ? 255 : (uint8_t)sum;
}

// Saturate sub
uint8_t saturating_sub(uint8_t a, uint8_t b) {
    return a > b ? a - b : 0;
}
```

ARM NEON has hardware saturation instructions (`vqadd_u8`, `vqsub_u8`) — generates one instruction per 8 lanes. The C version above is what Qualcomm camera-modeling code emulates bit-exactly before the SIMD lowering.

### 14.5 Use-after-free with `realloc`

```c
char *p = malloc(64);
char *q = p;
p = realloc(p, 1024);    // p might MOVE if realloc relocates
*q = 'a';                // ← q is now a dangling pointer if realloc moved
```

`realloc` may return a **different pointer** if it had to relocate the buffer. Any aliases of the old pointer are invalidated.

**Fix**: always re-derive aliases after `realloc`. Don't keep references to the buffer mid-flight.

### 14.6 Integer overflow in size calculation

```c
// On 32-bit size_t, n * sizeof(Point) overflows for huge n
Point *arr = malloc(n * sizeof(Point));
```

If `n * sizeof(Point)` overflows, `malloc` is called with a small size → succeeds → subsequent indexed writes go out of bounds → exploitable.

**Defense:**
- Validate `n` against a known upper bound
- Use `calloc(n, sizeof(Point))` — performs an internal overflow-checked multiplication (returns NULL on overflow)
- C++17: `std::vector` does this for you internally

### 14.7 Find-bugs checklist (apply to any snippet)

When given a "what's wrong with this C code" question, run through:

| Category | Watch for |
|---|---|
| **Memory** | malloc not checked for NULL · dangling pointer (return stack ptr) · use-after-free · double-free · leak (no free) · realloc invalidates aliases |
| **Bounds** | `<=` instead of `<` in loop · array decay (`sizeof(buf)` in function param) · off-by-one on read vs write |
| **Types** | signed/unsigned comparison · narrowing on assignment · `'A'` is `int` in C · `sizeof('A')` |
| **Operators** | `*p++` is `*(p++)` · `==` higher than `&`/`|` · `<<` UB on signed if not representable |
| **Sequence points** | `i = i++ + i++` UB · `arr[i] = i++` UB |
| **Strict aliasing** | casting `int*` to `float*` is UB; use `memcpy` or `char*` |
| **Concurrency** | shared `int` increment is not atomic — needs mutex or `_Atomic`/`std::atomic` |
| **Initialization** | uninitialized stack vars contain garbage (auto storage), heap NOT zeroed by malloc (use calloc) |

Run through this list mentally on every snippet. The bug is almost always in one of these categories.

### Memorization aid

> **"In any C bug question: check NULL, check bounds, check types, check operators. If those pass, check sequence points and aliasing."**

---

## 15. `volatile` — what it means and what it doesn't

### 15.1 What `volatile` actually does

`volatile T x;` tells the compiler: **"this variable's value can change asynchronously — don't optimize away reads or writes."**

Specifically:
1. Every read of `x` must reload from memory (no caching in register)
2. Every write of `x` must actually emit a store (no elision)
3. Reads/writes to `x` cannot be reordered with respect to each other

### 15.2 What `volatile` does NOT do (common misconceptions)

| Misconception | Reality |
|---|---|
| "volatile is atomic" | ❌ NO. `volatile int x; x++;` is still 3 ops (read/modify/write) — race condition. |
| "volatile is thread-safe" | ❌ NO. No memory ordering guarantees across cores. |
| "volatile prevents reordering with non-volatile loads" | ❌ NO. Only orders volatile-to-volatile. |
| "volatile prevents the variable from being optimized out" | ❌ Imprecise. It prevents *load/store elision*, not the variable's existence. |

**For thread safety, use `std::atomic<T>` (C++) or `_Atomic T` (C11), NOT `volatile`.**

### 15.3 Canonical use cases for `volatile`

1. **Memory-mapped I/O registers** — hardware can change the value out-of-band:
   ```c
   volatile uint32_t* status_reg = (volatile uint32_t*)0xDEADBEEF;
   while (*status_reg & READY_BIT) { ... }   // must reload every iteration
   ```
   Without `volatile`, the compiler would hoist the read out of the loop → infinite loop or stale value.

2. **Signal handler communication** in single-threaded programs (e.g., `sig_atomic_t`):
   ```c
   volatile sig_atomic_t flag = 0;
   void handler(int sig) { flag = 1; }
   while (!flag) { ... }   // must reload flag
   ```

3. **`setjmp`/`longjmp`** — variables modified between setjmp and longjmp must be `volatile` to survive the jump.

### 15.4 `volatile` vs `std::atomic` vs mutex — when to use what

| Scenario | Use |
|---|---|
| Memory-mapped I/O, signal handlers | `volatile` |
| Multi-threaded shared counter | `std::atomic<int>` |
| Multi-threaded shared complex state | `std::mutex` + plain types |
| Single-threaded with compiler optimization concerns | usually nothing — compiler is right |

**The Linus Torvalds line:** *"`volatile` is almost always wrong in kernel code."* Reason: `volatile` doesn't provide ordering between threads, only "this single variable can change." For multi-threaded code, you need fences/atomics, not `volatile`.

### 15.5 Senior engineer signal

If asked "what's the difference between `volatile` and `atomic`":
> `volatile` is for **single-threaded async access** (hardware, signal handlers) — it tells the compiler not to elide loads/stores. `std::atomic` is for **multi-threaded access** — it provides hardware-level atomic ops AND memory ordering guarantees across cores. They solve different problems and are not interchangeable.

---

## 16. Standard library memory ops (memcpy / memmove / memset)

### 16.1 Signatures and return values

```c
void* memcpy (void* dst, const void* src, size_t n);   // returns dst
void* memmove(void* dst, const void* src, size_t n);   // returns dst
void* memset (void* dst, int byte_value, size_t n);    // returns dst
char* strcpy (char* dst, const char* src);             // returns dst
char* strcat (char* dst, const char* src);             // returns dst
```

**Convention: every `mem*`/`str*` mutator returns the destination pointer.** This enables chaining:
```c
puts(memcpy(buf, "hello", 6));   // copy + print in one line
```

**Note `const` discipline:** dst is *not* const (being modified), src *is* const (read-only). Good API design pattern to mirror in your own code.

### 16.2 `memcpy` vs `memmove` — overlap handling

| Function | Overlap allowed? | Speed |
|---|---|---|
| `memcpy` | ❌ NO — undefined behavior if regions overlap | Fast (no overlap check, vectorizable) |
| `memmove` | ✓ YES — handles overlap safely | Slower (must detect direction) |

**Why memcpy fails on overlap:**
```c
char buf[] = "hello";
memcpy(buf+1, buf, 5);   // UB — naive forward copy corrupts source
// Step 1: buf[1] = buf[0] = 'h' → "hhllo"
// Step 2: buf[2] = buf[1] = 'h' → "hhhlo"  ← already wrong
// Expected "hhell" but you get garbage
```

**memmove handles it by direction-detection:**
```c
void* memmove(void* dst, const void* src, size_t n) {
    if (dst < src) {
        // Copy forward (low → high)
        for (size_t i = 0; i < n; i++) ((char*)dst)[i] = ((char*)src)[i];
    } else {
        // Copy backward (high → low) to avoid stomping on src
        for (size_t i = n; i > 0; i--) ((char*)dst)[i-1] = ((char*)src)[i-1];
    }
    return dst;
}
```

### 16.3 Why both functions exist

- **memcpy** is faster: no overlap check, autovectorizable, often 2-3× the throughput of memmove
- **memmove** is safer: handles overlap, but pays the direction-detect cost

**Rule:** use `memcpy` when you *know* regions don't overlap (e.g., copying between disjoint buffers). Use `memmove` when you can't guarantee non-overlap (e.g., shifting elements within an array).

### 16.4 `memset` — byte-level fill

```c
memset(buf, 0, sizeof(buf));        // zero-fill (common)
memset(buf, 0xFF, sizeof(buf));     // fill with 0xFF bytes
```

**Trap:** `memset` writes the value as **bytes**, not as the target type. So:
```c
int arr[10];
memset(arr, 1, sizeof(arr));   // ❌ NOT all 1s — fills with 0x01010101 per int
                                //    each int = 16843009 (0x01010101)
```
For non-zero fills of typed arrays, use a loop or `std::fill`.

### 16.5 Implementation tips for `memcpy` (interview ask)

When asked to implement `memcpy`:
1. **Naive byte loop** works but is slow:
   ```c
   void* memcpy(void* dst, const void* src, size_t n) {
       char* d = dst; const char* s = src;
       while (n--) *d++ = *s++;
       return dst;
   }
   ```
2. **Word-aligned bulk copy** (production-quality):
   - Copy bytes until both pointers are word-aligned
   - Copy `size_t`-sized chunks in the middle (8 bytes per iteration on 64-bit)
   - Copy remaining bytes one-at-a-time
3. **Senior signal:** mention that `glibc` memcpy uses SIMD (SSE/AVX/NEON) for large copies, and that cache-line-aware copies (64B chunks) avoid false sharing.

If interviewer asks "what about overlap?" → segue to memmove. That's the cue.

### 16.6 Common bugs

| Bug | Example | Fix |
|---|---|---|
| Wrong size with `sizeof` | `memcpy(dst, src, sizeof(src))` where `src` is a pointer (gets 8, not array size) | Pass length explicitly, or use sizeof(*src) * count |
| Off-by-one on strings | `memcpy(dst, "hello", 5)` — missing `\0` | Use 6, or `strcpy`, or `sizeof("hello")` |
| `memset` for non-zero typed fill | `memset(int_arr, 1, n)` ≠ "fill with 1s" | Use loop |
| Overlap with memcpy | shifting elements in same array | Use memmove |

---

## 17. Other C/C++ Gotchas (MCQ-derived)

### 17.1 Bad-pointer terminology

Interviewers use specific terms — know the difference:

| Term | Meaning | Example |
|---|---|---|
| **Dangling pointer** | Points to memory that's been freed/destroyed | Returning pointer to stack local; using ptr after `free()` |
| **Use-after-free (UAF)** | Specifically dereferencing freed *heap* memory | `free(p); *p = 5;` |
| **Wild pointer** | Uninitialized pointer (random address) | `int *p; *p = 5;` (no init) |
| **Null pointer dereference** | Dereferencing `NULL` | `int *p = NULL; *p = 5;` (segfault) |
| **Stack-use-after-return** | Subset of dangling — returning ptr to stack local | `char* foo() { char buf[10]; return buf; }` |
| **Double-free** | Calling `free()` twice on same pointer | `free(p); free(p);` (heap corruption) |

**Senior signal:** when describing a bug, use the precise term ("this is a stack-use-after-return") rather than "this pointer is bad."

### 17.2 Float-to-int conversion: truncates toward ZERO, not floor

**C99 §6.3.1.4:** when converting float to int, the fractional part is **discarded** — the value is truncated **toward zero**.

| Value | `(int)x` (truncate) | `floor(x)` | `ceil(x)` | `round(x)` |
|---|---|---|---|---|
| 0.7 | 0 | 0 | 1 | 1 |
| -0.7 | **0** | -1 | 0 | -1 |
| 1.5 | 1 | 1 | 2 | 2 |
| -1.5 | **-1** | -2 | -1 | -2 |

**Trap:** `(int)-0.7` is **0**, not -1. People assume float-to-int rounds to floor — it doesn't.

**Why it matters in ISP:** when narrowing a Q-format computation with `(uint8_t)result`, you're truncating toward zero. For non-negative pixel data this matches floor. For signed gradient outputs (Sobel, etc.), the asymmetry around zero can cause subtle bias. Use explicit rounding (`+ 0.5`) or shift-with-rounding for unbiased behavior.

### 17.3 UB vs implementation-defined vs unspecified — the precise vocabulary

| Term | Meaning | Compiler obligation |
|---|---|---|
| **Undefined behavior (UB)** | Anything can happen — crash, wrong answer, "demons may fly out of your nose" | None. Compiler can assume the UB doesn't happen and optimize accordingly. |
| **Implementation-defined** | Behavior must be consistent for a given implementation, and the implementation must document the choice | Must pick a behavior and document it. Programs can rely on it within that implementation. |
| **Unspecified** | One of several valid behaviors allowed by the standard. Not necessarily documented. | Must pick one of the allowed options each time. |

**Examples:**

| Construct | Category (C99/C11) | Notes |
|---|---|---|
| `INT_MAX + 1` (signed overflow) | **UB** | Compiler may assume it never happens — common source of optimization-induced bugs |
| `UINT_MAX + 1` (unsigned overflow) | **Well-defined** | Wraps modulo 2ⁿ |
| `1 << 31` on signed 32-bit int | **UB** | Signed left-shift result not representable |
| `-1 >> 1` (right-shift of negative) | **Implementation-defined** (NOT UB) | In practice always arithmetic shift; C++20 made it well-defined |
| Order of arg evaluation in `f(a, b)` | **Unspecified** (in C; C++17 made some sequenced) | Compiler picks |
| `i = i++ + i++` | **UB** | Multiple unsequenced modifications |
| Sizeof char | **Well-defined as 1** | Not implementation-defined |
| Sizeof int | **Implementation-defined** | At least 16 bits, usually 32 |
| Endianness | **Implementation-defined** | Architecture choice |

**Why this matters for interviews:** saying "right-shift of negative is implementation-defined, not UB" is a senior-level distinction. Most candidates lump everything into "UB" — knowing the precise category shows you've read the standard.

**Operational rule:** treat UB and implementation-defined as "things to avoid relying on for portable code," but know the difference if pressed.

### 17.4 const correctness — pointer vs pointee

Read the declaration **right-to-left** (or "find the const, look at what's immediately to its left"):

| Declaration | What's const | Can change |
|---|---|---|
| `const char* p` | the **char** (data) | the **pointer** can be reassigned |
| `char* const p` | the **pointer** | the **char** (data) can be modified |
| `const char* const p` | both | nothing |

**Test:**
```c
const char* p1 = "hello";
p1[0] = 'H';       // ✗ ERROR — char is const
p1 = "world";      // ✓ OK    — pointer can change

char* const p2 = malloc(10);
p2[0] = 'H';       // ✓ OK
p2 = NULL;         // ✗ ERROR — pointer is const
```

**API design rule:**
- `const T*` (pointer to const) — "I won't modify what this points to" — typical for read-only inputs (`strlen(const char*)`)
- `T* const` — "this pointer won't be reassigned" — rarely seen in parameters; common for fixed buffers in embedded
- `void f(const T* const p)` — both — common in defensive code

### 17.5 Parsing complex C declarations — spiral / right-hand rule

The hardest C syntax to parse: declarations involving pointers to arrays, arrays of pointers, function pointers.

**The rule (right-hand rule):**

1. Start at the **variable name**
2. Go **right** as long as you can (`[]`, `()` keep going right)
3. When you hit `)`, go **left** until you hit `(`
4. Repeat — `(`-paren means flip back to going right

**Worked examples:**

| Declaration | Step-by-step | English |
|---|---|---|
| `int *p[10]` | name → right `[10]` → left `*` → left `int` | array of 10 pointers to int |
| `int (*p)[10]` | name → can't go right (paren) → left `*` → flip out of paren → right `[10]` → left `int` | pointer to array of 10 ints |
| `int *p()` | name → right `()` → left `*` → left `int` | function returning int* |
| `int (*p)()` | name → left `*` → right `()` → left `int` | pointer to function returning int |
| `int *(*p)()` | name → left `*` → right `()` → left `*` → left `int` | pointer to function returning int* |
| `int (*p[10])()` | name → right `[10]` → left `*` → right `()` → left `int` | array of 10 pointers to functions returning int |
| `void (*signal(int sig, void (*func)(int)))(int)` | (signal handler) | function `signal` taking int and pointer-to-function-of-int, returning pointer-to-function-of-int |

**Size differences (64-bit):**

| Declaration | Size |
|---|---|
| `int *p[10]` (10 pointers) | 80 bytes |
| `int (*p)[10]` (1 pointer) | 8 bytes |
| `int (*p)()` (1 function pointer) | 8 bytes |
| `int (*p[10])()` (10 function pointers) | 80 bytes |

**Practical tools:**

- **`cdecl`** command-line tool / website (https://cdecl.org/) — type a declaration, get English. Reverse direction also works.
- **`typedef`** every complex type to sanity-preserve readability:
  ```c
  typedef int (*int_func_ptr)(int);     // pointer to function int -> int
  int_func_ptr handlers[10];            // much clearer than int (*handlers[10])(int)
  ```

**ISP/embedded relevance:** function-pointer arrays are common for callback dispatch tables (e.g., array of ISP stage handlers, or ISR vectors). Always `typedef` them.

### 17.6 Pointer arithmetic and comparison rules — what's legal, what's UB

**Legal pointer operations:**

| Operation | Legal when |
|---|---|
| `p + n`, `p - n` | `p` and result point within `[arr_start, arr_end+1]` (one-past-the-end is OK to *form*) |
| `p1 - p2` (pointer subtraction) | both point within the **same array object** (or one-past-the-end of it) |
| `p1 < p2`, `>`, `<=`, `>=` | both point within the **same array object** |
| `p1 == p2`, `p1 != p2` | always defined for any two pointers (compares addresses) |
| `*p` (dereference) | `p` points to a valid object (not one-past-the-end, not NULL, not freed) |

**UB cases:**

```c
int a, b;                  // separate objects
int *p1 = &a, *p2 = &b;
p1 - p2;                   // UB — different objects (not in same array)
p1 < p2;                   // UB — same reason
*p1 == *p2;                // OK — comparing values, not pointers

int arr[10];
int *p = arr + 15;         // UB — out of bounds (allowed: arr+0 through arr+10)
int *q = arr + 10;         // OK — one-past-the-end is allowed to form
*q;                        // UB — but dereferencing one-past-the-end is not
```

**Pointer subtraction returns `ptrdiff_t`** (signed integer), not a pointer:

```c
int arr[10];
int *p1 = &arr[2];
int *p2 = &arr[7];
ptrdiff_t diff = p2 - p1;   // 5 (element count, not byte count)
```

**To get byte difference** (use case: stride calculation in image rows):

```c
ptrdiff_t bytes = (char*)p2 - (char*)p1;   // cast to char* for byte units
```

**Equality (`==`, `!=`) is always defined:**

This is how you check pointer identity (do these two pointers reference the same object?). Useful for:
- Self-assignment check: `if (this == &other) return *this;`
- Cycle detection (Floyd's tortoise-and-hare)
- Sentinel checks: `if (p == NULL)` or `if (p == END_MARKER)`

**Senior signal:** when seeing pointer arithmetic in a snippet, immediately verify both pointers are into the same array. If not, it's UB and you should flag it — even if the code "works" in testing.

