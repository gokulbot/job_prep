# ISP Notes — Fixed-Point + 2D Image Processing

Reference for Qualcomm Camera Systems Modeling screening. Covers Q-format math, hardware DSP fusion, and 2D kernel filter patterns.

---

## 1. Fixed-Point Math (Q-format) — Hardware-Level

### 1.1 Why Q-format on DSPs

Float on hardware without an FPU (or with a slow one) is **50-300× slower** than integer:

| Op | Latency (cycles) |
|---|---|
| Integer ADD/SUB | 1 |
| Integer MUL | 2-3 |
| Integer shift | 1 |
| Float ADD/MUL (with FPU) | 3-5 |
| Float DIV | 10-25 |
| Float ops without FPU (software emulated) | 50-300 |

For real-time camera ISP at 30 fps (~33 ms/frame budget) on a 12 MP sensor:
- 12M × 100 cycles / 1 GHz = **1.2 s/frame** (misses frame rate)
- 12M × 3 cycles / 1 GHz = **36 ms/frame** (feasible after vectorization)

### 1.2 Q-format basics

**Qm.n** = signed integer where `m` integer bits + `n` fractional bits. Scale factor = 2ⁿ.

```c
real_value  = stored_int / 2^n
stored_int  = real_value * 2^n
```

**Common formats:**
- **Q15** = 1 sign + 15 fractional bits (16-bit signed). Range: [-1, 1). Scale = 32768.
- **Q16.16** = 16 integer + 16 fractional (32-bit signed). Scale = 65536.

**Reading a Q16.16:**
```c
int integer_part   = q >> 16;          // top 16 bits
int fractional_int = q & 0xFFFF;       // bottom 16 bits, in [0, 65536)
double as_float    = q / 65536.0;      // ONLY for debugging!
```

**Constants in Q-space:**
- "1.0" in Q16 = `1 << 16` = `65536`
- "0.5" in Q16 = `1 << 15` = `32768`
- "1.0" in Q15 = `1 << 15` = `32768`
- BT.601 luma weights as Q15: WR=9798, WG=19235, WB=3736

### 1.3 The four killer traps in C fixed-point

**Trap 1 — using literal `1` instead of `(1<<n)` in Q-space:**
```c
// WRONG: mixes scales
weight = (1 - x_frac) * pixel;        // 1 - 32768 = -32767 (nonsense)
// RIGHT: stay in Q-space
weight = ((1 << 16) - x_frac) * pixel;
```

**Trap 2 — operand promotion overflow:**
```c
// WRONG: RHS computes as int → overflows BEFORE assignment to int64_t
int64_t result = (65536 - x_frac) * (65536 - y_frac) * pixel;
//   2^32 max  *  2^32 max  → overflows int

// RIGHT: cast at least one operand to int64_t to force 64-bit math
int64_t result = (int64_t)(65536 - x_frac) * (65536 - y_frac) * pixel;
```
**Rule: when multiplying two Q-numbers, ALWAYS cast one operand to int64_t first.**

**Trap 3 — shift by ≥ width is UB:**
```c
int32_t x;
x >> 32;   // UB (max legal shift = 31)
// Use int64_t if you need to shift by 32:
int64_t x;
x >> 32;   // OK
```

**Trap 4 — signed left-shift into the sign bit is UB:**
```c
int rounding = 1 << 31;     // UB on 32-bit int
int rounding = 1LL << 31;   // OK (long long, 64-bit)
unsigned r   = 1U << 31;    // OK (unsigned never UB on shift)
```

### 1.4 Round-to-nearest pattern

`>> n` alone is **truncation** (floor for positives → consistent downward bias).

For round-to-nearest (half-up): **add half the divisor, then shift**.

```c
// General: shift back from Qn with round-to-nearest
result = (acc + (1LL << (n-1))) >> n;

// Q15 → Q0:    result = (acc + (1 << 14))  >> 15;
// Q32 → Q0:    result = (acc + (1LL << 31)) >> 32;
```

**Why it works:** floor(x + 0.5) = round(x). In Q-space, "0.5" = 2^(n-1). Add it, then truncate.

**Why `1LL` not `1`:** `1 << 31` on a 32-bit int is signed-overflow UB. `1LL << 31` is well-defined long long.

#### Match the "0.5" to the Q-space you're in

This trips people up. After `Q16 × Q16 = Q32`, your accumulator is in **Q32** space, NOT Q16:

| Shifting back by | Add this for round-to-nearest | Why (it's "half of the divisor") |
|---|---|---|
| `>> 15` (Q15 → Q0) | `1 << 14` (= 16384) | half of 2¹⁵ |
| `>> 16` (Q16 → Q0) | `1 << 15` (= 32768 = 65536/2) | half of 2¹⁶ |
| `>> 32` (Q32 → Q0) | `1LL << 31` (= 2³¹ ≈ 2.15B) | half of 2³² |

#### Trace — why "add 0.5 then truncate" works

The +0.5 is a **probe** that pushes you over the next integer boundary only if the fractional part was already ≥ 0.5:

| Original | + 0.5 | floor | Result | Round-to-nearest correct? |
|---|---|---|---|---|
| 1.4 | 1.9 | 1 | **1** | ✓ (1.4 closer to 1) |
| 1.6 | 2.1 | 2 | **2** | ✓ (1.6 closer to 2) |
| 1.5 | 2.0 | 2 | **2** | ✓ (round-half-up) |
| 0.4 | 0.9 | 0 | **0** | ✓ |
| 0.6 | 1.1 | 1 | **1** | ✓ |

Truncation alone (no +0.5) always rounds **down** → systematic downward bias across many pixels (visible as gradual darkening in ISP). Adding 0.5 first gives unbiased round-to-nearest.

**Mental model:** `>> n` is "divide by 2ⁿ and strip fractional." The +0.5 pushes the value past the next integer threshold only when the fractional was already past the halfway mark.

### 1.5 Hardware-fused mul-shift instructions

The C pattern `(int64_t)a * b >> 32` looks like 2 ops, but on real hardware it's **one fused instruction**.

**ARM (Cortex-A, Snapdragon application CPU):**
- **`SMMUL Rd, Rn, Rm`** — Signed Most-significant word Multiply: `Rd = (Rn × Rm) >> 32`. One instruction, no 64-bit register pair, no shift.
- **`SMULWB Rd, Rn, Rm`** — `Rd = (Rn × Rm[15:0]) >> 16`. For Q16 × int32 patterns.
- **`SMMLA`** — multiply-accumulate variant: `acc += (a × b) >> 32`. Critical for filter taps.
- A good optimizing compiler **emits SMMUL automatically** for `(int64_t)a * b >> 32` — verify on godbolt.

**NEON (ARM SIMD, 128-bit):**
- **`vqrdmulh_s16`** — Q15 saturating rounding doubling multiply high, vectorized.
  ```c
  int16x8_t r = vqrdmulh_s16(a, b);
  // 8 parallel: (a*b*2 + 0x8000) >> 16, saturated to int16
  // = Q15 multiply with rounding + saturation, free
  ```
  Replaces `mul + shift + round + saturate` per pixel × 8 pixels with **one instruction**.

**Hexagon HVX (Qualcomm DSP, 1024-bit) — most relevant for the role:**
- **`Vd.h = vmpyh(Vu.h, Vv.h):<<1:rnd:sat`** — vector Q15 multiply with shift + round + saturate.
  - 64 parallel Q15 multiplies per instruction (8× wider than NEON)
  - Same operation: `(a*b*2 + 0x8000) >> 16`, saturated, but on 64 lanes
- **`Vdd.w = vmpa(Vuu.h, Rt.b)`** — vector multiply-accumulate, used for filter convolutions. Two vector registers worth of products accumulated.
- **`Vd.w = vasr(Vu.w, Vv.w):rnd`** — arithmetic shift right with rounding, vectorized.

### 1.6 Saturation in hardware

ARM has **`QADD`**, **`QSUB`** for saturating add/subtract — clip to int32 range automatically. NEON has `vqaddq_*`, `vqsubq_*`. Hexagon has `:sat` modifier on most arithmetic ops.

In C, the standard saturation idiom:
```c
if (x > 255) x = 255;
else if (x < 0) x = 0;
// or: x = (x > 255) ? 255 : (x < 0) ? 0 : x;
// or: x = x < 0 ? 0 : (x > 255 ? 255 : x);
```
A good compiler turns this into branchless conditional-move (`CSEL` on ARM). On NEON/HVX, saturation is built into the instruction (`:sat`).

### 1.7 Interview answer template

> Q: "Why is your Q15 implementation faster than float on Hexagon?"
>
> A:
> 1. **No FPU** on Hexagon scalar units → float is software-emulated (50-300 cycles per op).
> 2. **Fused mul-shift-saturate** — Hexagon HVX has dedicated Q15 vector instructions (`vmpyh ... :<<1:rnd:sat`) that do multiply + round + shift + saturate in **one cycle on 64 lanes**.
> 3. **Predictable latency** — no denormals, NaN, Inf edge cases. Integer ops are 1-cycle deterministic.
> 4. **Memory bandwidth** — int16 is half the size of float32, so 2× the pixels per cache line.

### 1.8 Mental rule — the fixed-point checklist

1. ☐ All "1.0" constants written as `(1 << n)` not literal `1`
2. ☐ Multiplications cast at least one operand to int64_t
3. ☐ Accumulator type is wider than worst-case product
4. ☐ Shift amount is < bit-width of the type being shifted
5. ☐ Round-to-nearest: `(x + (1LL << (n-1))) >> n` — note the `LL`
6. ☐ Saturate to output range at the end (e.g., clip to [0, 255])
7. ☐ Compiler will emit `SMMUL` / `SMLAL` / NEON / HVX equivalent — verify on godbolt for hot paths

---

## 2. 2D Image Processing Patterns (kernel filters)

### 2.1 Row-major indexing — the always-rule

For a 2D image stored in row-major layout (the default in C/C++ and almost every ISP):

> **Pixel at row `y`, column `x` is at index `y * width + x`.**

This is **always** the formula. Mistakes happen when:

- You write `(width - 1) * y` instead of `y * width` — that gives row stride times y minus y, not row offset
- You write `y * height + x` — wrong dimension
- You mix `dst[y * width + x]` with `src[(width-1) * y]` on the same line — silent bug

**Mental check:** if you see `width * y` (or `y * width`) it's a row offset. If you see `width - 1` it should be **added** as a column offset, not multiplied.

### 2.2 3×3 kernel access pattern

Standard nested-loop form for kernel-based filters (box, Sobel, Gaussian, median, etc.):

```c
// Output (y, x) depends on src in [y-1..y+1] × [x-1..x+1]
for (int y = 1; y < height - 1; y++) {        // skip border
    for (int x = 1; x < width - 1; x++) {     // skip border
        int32_t acc = 0;
        for (int m = 0; m < 3; m++) {          // kernel row
            for (int n = 0; n < 3; n++) {      // kernel col
                int row = y + m - 1;           // shift to [-1, +1]
                int col = x + n - 1;
                acc += kernel[m * 3 + n] * src[row * width + col];
            }
        }
        // For Q15 kernel: round and shift
        acc = (acc + (1 << 14)) >> 15;
        // Saturate
        if (acc > 255) acc = 255;
        if (acc < 0)   acc = 0;
        dst[y * width + x] = (uint8_t) acc;
    }
}
```

**Key offsets:** `m=0,1,2` maps to `row = y-1, y, y+1`. The `-1` is the kernel center offset.

### 2.3 Border handling — three options

| Strategy | Code | Use case |
|---|---|---|
| **Replicate** | `dst[border] = src[border]` | Simplest, common for small kernels |
| **Reflect** | mirror values across the edge | Better for derivatives (Sobel) |
| **Zero-pad** | `dst[border] = 0` | Simplest but introduces dark borders |
| **Skip** | leave dst[border] uninitialized | NEVER. Always undefined garbage |

**Replicate pattern (the boilerplate):**

```c
// Top + bottom rows
for (int x = 0; x < width; x++) {
    dst[0 * width + x]            = src[0 * width + x];           // top
    dst[(height-1) * width + x]   = src[(height-1) * width + x];  // bottom
}
// Left + right cols
for (int y = 0; y < height; y++) {
    dst[y * width + 0]            = src[y * width + 0];           // left
    dst[y * width + (width - 1)]  = src[y * width + (width - 1)]; // right
}
// Corners get written twice — harmless since src == src
```

**Critical:** the border replication pass is NOT optional. Without it, `dst[0..width-1]` and `dst[(height-1)*width..]` are uninitialized — undefined behavior the moment anyone reads them. Interviewers will absolutely catch this.

### 2.4 Operator precedence killers in 2D code

**Shift vs `+` — the recurring trap:**
```c
// WRONG — `+` binds tighter than `<<`
sum = (sum + 1LL << 14) >> 15;       // parses as ((sum + 1LL) << 14) >> 15

// RIGHT — explicit parens around the shift
sum = (sum + (1LL << 14)) >> 15;
```

**Indexing vs cast precedence:**
```c
(uint8_t)src[i]            // OK — [i] binds tighter than (uint8_t)
*(uint8_t*)&value          // unary cast then deref
```

**Rule of thumb when mixing `<<`, `>>` with `+`, `-`, `*`, `/`:**
> **Always parenthesize the shift expression.** It costs nothing and saves debugging hours.

### 2.5 Accumulator sizing

For a kernel filter:

| Operation | Worst-case product | Sum of N terms | Min accumulator |
|---|---|---|---|
| 3×3 box, Q15 weight (3641) × uint8 (255) | ~928K | ~8.4M (9 terms) | int32 (max 2.1B) |
| 5×5 Gaussian, Q15 × uint8 | ~928K | ~23M (25 terms) | int32 |
| Q16 × Q16 (bilinear weights, both fractional) | ~4.3B | × pixel = ~10¹² | **int64** |
| Q31 × Q15 | ~6.9 × 10¹³ | larger | **int64** |

**Rule:** when one operand of the multiply is small (uint8 or int8), int32 accumulator is usually fine. When both operands are wide Q-numbers, you need int64.

### 2.6 Box filter optimization (since all weights equal)

When kernel weights are uniform (box / mean filter), skip the per-tap multiply:

```c
// Naive: 9 mults + 9 adds per output pixel
for (m, n in kernel) acc += weight * pixel;

// Optimized: 9 adds + 1 mult per output pixel
int32_t pixel_sum = 0;
for (m, n in kernel) pixel_sum += pixel;
acc = pixel_sum * weight;
```

**Even better — separable filter:** a 3×3 box filter is **separable** into a 1×3 horizontal pass followed by a 3×1 vertical pass. Reduces work from 9 ops/pixel to 6 ops/pixel. Same idea applies to Gaussian (separable into two 1D Gaussians) and Sobel (separable into smoothing + difference). Worth mentioning in interviews.

### 2.7 Common bugs from the box-filter session (lessons learned)

1. **`heigth` typo** — mistype of `height`. Compiler will catch but easy to glance past.
2. **`srcdst` typo** — mashing two variable names. Read variable names character-by-character before pasting.
3. **`=` vs `+=` in accumulation** — `=` overwrites, only the last iteration counts. Always use `+=`.
4. **Storing accumulator in `dst[]` directly** — uint8 truncates partial sums. Use a local int32 variable, write to dst only at the end.
5. **Operator precedence in rounding** — `acc + 1LL<<14` parses as `(acc+1LL)<<14`. Always paren the shift.
6. **Border replication missing** — skipped entirely on first pass. ALWAYS add a border pass for kernel filters.
7. **Inconsistent indexing on src vs dst** — e.g., `dst[y*width + x] = src[(width-1)*y + ...]`. Both sides must use the SAME row-major formula.
8. **Forgetting `weight_[]` declaration** — referencing an array that doesn't exist. Always declare before use, or restructure to not need it.
