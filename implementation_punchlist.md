# Qualcomm Screening — Implementation Punch List

**Goal:** complete top-to-bottom by Sat 2026-05-09 evening. Sun = behavioral + mock + light review. Mon = screening.

**Theoretical ceiling at 100% completion:** ~92-94%.

---

## ISP / Image Processing (12)

### NPTEL-aligned (theory + impl paired)

- [x] **#1 Bilinear Interpolation** (Q16.16 sub-pixel) — Lec 8 ✅
- [x] **#2 Box filter 3×3** (Q15, edge handling) — Lec 22-23 ✅
- [x] **#3 Histogram Equalization** (CDF + LUT) — Lec 63-64 ✅
- [x] **#4 Sobel / gradient** (Gx, Gy with |Gx|+|Gy|) — Lec 22-23 application ✅
- [x] **#5 Median filter 3×3** (impulse noise) — Lec 68-69 ✅
- [ ] **#6 Gaussian filter** (separable 1D × 1D, Q15 weights) — Lec 65 — https://www.youtube.com/watch?v=dmZi3eGEC0k

### Non-NPTEL (blog/spec refs)

- [ ] **#7 RGB → Grayscale** (Q15, BT.601: WR=9798, WG=19235, WB=3736)
- [ ] **#8 Gamma correction with LUT**
- [ ] **#9 RGB → YUV422** (Q15 matrix multiply)
- [ ] **#10 Bayer demosaic** (basic bilinear, RGGB)
- [ ] **#11 AWB grayworld** (per-channel mean → gain → apply)
- [ ] **#12 CCM 3×3** (Q15 matrix, saturation) — Tier 2

---

## C/C++ Implementation (5 pending)

✅ Done: Counter (mutex), Counter (atomic), BoundedBuffer, swap (arithmetic + XOR), endianness detection

- [ ] **#13 `memcpy`** (alignment, overlap question)
- [ ] **#14 `strcmp`** (under time pressure)
- [ ] **#15 Multi-threading concepts theory pass** (mutex, semaphore, deadlock, atomic, condvar — concise)
- [ ] **#16 Q-format cheat sheet consolidation** (Q15 conversions, saturation, rounding modes, overflow)
- [ ] **#17 Re-skim `cpp_pointer_notes.md` + `isp_notes.md`** — Sun (final review before screening)

---

## DSA / LeetCode (14 P1 pending)

✅ Done: 1, 20, 53, 141, 160, 19, 206, 21, 23, 234, 443, 876

### Trees — Thu (45 min)
- [x] **#18 LC 100** Same Tree ✅
- [x] **#19 LC 104** Max Depth Binary Tree ✅
- [x] **#20 LC 226** Invert Binary Tree ✅

### Fri (LC mediums)
- [x] **#21 LC 48** ⭐ Rotate Image (image-flavored, top Qualcomm freq) ✅
- [ ] **#22 LC 56** Merge Intervals
- [ ] **#23 LC 3** Longest Substring Without Repeating
- [ ] **#24 LC 215** Kth Largest Element
- [ ] **#25 LC 8** String to Integer (atoi)
- [ ] **#26 LC 836** Rectangle Overlap

### Sat
- [ ] **#27 LC 102** Level Order Traversal
- [ ] **#28 LC 235** LCA of BST
- [ ] **#29 LC 236** LCA of Binary Tree
- [ ] **#30 LC 146** LRU Cache
- [ ] **#31 LC 200** Number of Islands

---

## Sun — Behavioral + Mock (separate from impl list)

- [ ] Why-pivot narrative — 60-90 sec, practice 3× out loud
- [ ] 5 project pitches with camera/ISP framing (depth map compression, IMU pre-integration, TensorRT, EKF Numba, GRU embedded)
- [ ] HM questions list — 5-7 sharp ones for camera modeling team
- [ ] Full 1-hr mock interview + self-review
- [ ] Logistics check — link, time, ID, environment
- [ ] Sleep by 10pm

---

## Status

**Total pending impls:** 12 ISP + 5 C/C++ + 14 LC = **31 items**
**Time budget remaining (Thu evening + Fri + Sat):** ~17 hrs of focus time
**Per item:** ~30 min average — tight but achievable for revision-mode

Strike items as we go. Hard stop on LC at 8pm each day.
