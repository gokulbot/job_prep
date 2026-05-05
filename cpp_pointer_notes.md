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

**Q: Why `void*` for memcpy params and return?**
- `void*` = generic pointer, any type's address fits
- Lets memcpy work on any data without explicit casts at call site
- Cannot be dereferenced — must cast inside the function (typically to `unsigned char*`)
- Return `dest` for chaining (`char *p = memcpy(malloc(n), src, n)`) and to match standard library convention

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

**Intuition:** copy in the direction where each write happens AFTER the corresponding read. If `dest < src`, forward copy reads byte 0 first (= `src[0]`), writes to `dest[0]`. Since `dest[0] < src[0]` in memory, the write is "behind" us — no risk to unread src bytes. Reverse logic for `dest > src`.

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

## 11. Tomorrow's drill suggestion

- Implement `strcmp`, `strlen`, custom `sizeof` macro
- "Find bugs in this C snippet" exercises (race conditions, use-after-free, off-by-one, UB)
- Image-processing-flavored MCQs: image buffer pointer arithmetic, fixed-point Q-format conversion, endianness for raw sensor data
