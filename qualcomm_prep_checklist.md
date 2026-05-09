# Qualcomm Camera Systems Modeling — Prep Plan (Restructured)

**Screening**: Mon 2026-05-11 · 1hr virtual · HM technical screen
**Role**: Senior Engineer, Camera Systems Modeling, Qualcomm Bengaluru
**Daily budget**: ~7-9 hours focused work

---

## Framing Principles

- **ISP and image processing are ONE merged block** — ISP is a subset of image processing; treat theory + implementation as paired, not separate.
- **Bug finding, output prediction, whiteboard coding are PRACTICE MODES, not topics** — folded into each block as the format of practice (e.g., write C/C++ on paper, predict output of image processing code).
- **Fixed-point arithmetic gets explicit billing** — it's a JD minimum qual, not a sub-bullet.
- **DSA is CAPPED** — Tue/Wed already strong, don't let it grow back. ~20 problems P1+P2 max for the rest of the week.
- **NPTEL ISP course (Prof. Rajagopalan, IIT Madras, 108106168)** is the theory companion to Block 1 implementations. This is the single biggest differentiator on the resume.

---

## 5 Technical Blocks (Restructured)

### Block 1 — Image Processing / ISP (HIGHEST PRIORITY, ~12 hrs)

**1a — Theory: NPTEL course refresh (~3 hrs)**
- [ ] NPTEL Lectures Tier 1: demosaicing + white balance — Thu, 1.5 hr
- [ ] NPTEL Lectures Tier 1: CCM + gamma + color spaces — Fri, 1.5 hr
- Goal: refresh formal ISP grounding so theory pairs cleanly with code

**1b — Implementation: Q-format C++ impls (~8 hrs)**

*Tier 1 — almost certainly fair game:*
- [ ] RGB → Grayscale (Q15, BT.601 weights) — 1.5 hr
- [ ] Gamma correction with LUT — 1 hr
- [ ] 3×3 Box filter (2D pointer arithmetic, edges) — 1.5 hr
- [ ] Bilinear interpolation (Q8/Q16 sub-pixel) — 1 hr
- [ ] Histogram equalization (CDF + LUT) — 1.5 hr
- [ ] RGB → YUV422 fixed-point (matrix multiply) — 1 hr
- [ ] Bayer demosaic (basic bilinear) — 1.5 hr

*Tier 2 — bonus if time on Sat:*
- [ ] CCM 3×3 + Sobel/gradient — 2.5 hr

**1c — Pipeline whole-board write-out (~45 min)**
- [ ] Sat: write the full ISP pipeline (sensor → Bayer → black level → LSC → demosaic → WB → CCM → gamma → tone map → denoise → sharpen → RGB→YUV) on paper from memory

### Block 2 — Fixed-Point Arithmetic (~1 hr standalone)

Mostly folded into Block 1b implementations. Standalone:
- [ ] Q-format cheat sheet: Q15, conversions, saturation, rounding modes, overflow
- [ ] Worked examples: Q15 × Q15 → Q30, shift back to Q15, saturate to int16

### Block 3 — C/C++ Implementation (~5 hrs)

**3a — Refresh notes (~2 hrs)**
- [ ] Sat: re-skim `cpp_pointer_notes.md` end-to-end — 1 hr
- [ ] Sun: re-skim again before mock — 1 hr

**3b — Practice modes (single round, no rounds 2/3) — already done Wed**
- [x] Find-bugs-in-C × 5 — done Wed
- [x] Predict-output × 5 — done Wed
- [x] Multi-threading code (Counter, BoundedBuffer) — done Wed

**3c — Whiteboard warmups (~1 hr Sat)**
- [ ] Re-implement `memcpy` (overlap-safe) UNDER TIME — 20 min
- [ ] Re-implement `strcmp`, `strlen`, `MY_SIZEOF` UNDER TIME — 20 min
- [ ] Swap without temp + endianness detect on paper — 20 min

**3d — Multi-threading concepts theory (~1 hr)**
- [ ] Thu: mutex, semaphore, deadlock, atomic, condition var — concise concepts pass

### Block 4 — DSA (CAPPED, ~5 hrs total this week)

See **LeetCode Priority Reference** section below. Total target after Tue/Wed: ~20 problems P1+P2 spread Thu/Fri/Sat.

### Block 5 — Behavioral + Mock (Sunday only, ~3.5 hrs)

- [ ] "Why pivot" narrative — 60-90 sec answer + practice out loud 3× — 45 min
- [ ] 5 project pitches with camera/ISP framing (depth map compression, IMU pre-integration math, TensorRT optimization, EKF Numba, GRU on embedded) — 1.5 hr
- [ ] HM questions list — 5-7 sharp ones for camera modeling team — 45 min
- [ ] Full 1-hr mock interview + self-review — 1.5 hr

---

## LeetCode Priority Reference

**Priority key:**
- **P0** — Already done (Tue/Wed)
- **P1** — Must do (high ROI, in slim plan)
- **P2** — Should do (stretch high — only if P1 done)
- **P3** — Optional (stretch low — only if everything else done)
- **P4** — Skip (low ROI for Monday)

**Time-boxed rules:**
1. P2 only opens after P1 is complete each day
2. 15 min per P2 problem max — revision mode, not learning mode
3. 8pm hard stop on LC, switch to ISP/C++
4. No Hards on Sun

### P1 — Must Do (~3 hrs total, spread Thu/Fri/Sat)

| # | Problem | Difficulty | Slot | Notes |
|---|---|---|---|---|
| 100 | Same Tree | Easy | Thu | Tree warmup |
| 104 | Max Depth BT | Easy | Thu | Tree warmup |
| 226 | Invert BT | Easy | Thu | Tree warmup |
| 48 | **Rotate Image** | Medium | Fri | ⭐ Image-flavored, 87.5% Qualcomm freq |
| 56 | Merge Intervals | Medium | Fri | Common Qualcomm |
| 3 | Longest Substring No Repeat | Medium | Fri | Sliding window classic |
| 215 | Kth Largest | Medium | Fri | Heap pattern |
| 8 | String to Integer (atoi) | Medium | Fri | Edge-case parsing |
| 836 | Rectangle Overlap | Easy | Fri | Geometric, image-relevant |
| 102 | Level Order | Medium | Sat | BFS |
| 235 | LCA BST | Medium | Sat | Tree |
| 236 | LCA BT | Medium | Sat | Tree |
| 146 | LRU Cache | Medium | Sat | Design |
| 200 | Number of Islands | Medium | Sat | DFS/BFS |

### P2 — Should Do (only if P1 done)

| # | Problem | Difficulty | Slot |
|---|---|---|---|
| 392 | Is Subsequence | Easy | Thu |
| 26 | Remove Duplicates | Easy | Thu |
| 11 | Container Most Water | Medium | Thu |
| 283 | Move Zeroes | Easy | Thu |
| 88 | Merge Sorted Array | Easy | Fri |
| 70 | Climbing Stairs | Easy | Fri |
| 344 | Reverse String | Easy | Fri |
| 169 | Majority Element | Easy | Fri |
| 121 | Best Time Stock | Easy | Fri |
| 217 | Contains Duplicate | Easy | Fri |
| 155 | Min Stack | Easy | Fri |
| 24 | Swap Nodes in Pairs | Medium | Sat |
| 50 | Pow(x, n) | Medium | Sat |
| 7 | Reverse Integer | Medium | Sat |
| 9 | Palindrome Number | Easy | Sat |
| 232 | Queue using Stacks | Easy | Sat |
| 110 | Balanced BT | Easy | Sat |
| 198 | House Robber | Medium | Sat |
| 322 | Coin Change | Medium | Sat |
| 138 | Copy LL Random | Medium | Sat |

### P3 — Optional (Sat afternoon or Sun if confidence boost wanted)

| # | Problem | Difficulty | Slot |
|---|---|---|---|
| 387 | First Unique Char | Easy | Sun |
| 342 | Power of Four | Easy | Sun |
| 58 | Length of Last Word | Easy | Sun |
| 5 | Longest Palindrome Substr | Medium | Sun |
| 49 | Group Anagrams | Medium | Sun |
| 202 | Happy Number | Easy | Sun |
| 69 | Sqrt | Easy | Sun |
| 46 | Permutations | Medium | Sat |
| 622 | Design Circular Queue | Medium | Sat |
| 210 | Course Schedule II | Medium | Sat |
| 560 | Subarray Sum K | Medium | Sat |
| 25 | Reverse Nodes K-Group | Hard | Sun (max 1 Hard) |
| 42 | Trapping Rain Water | Hard | Sun (max 1 Hard) |

### P4 — SKIP (don't touch)

LC 72 Edit Distance, LC 4 Median 2 Sorted, LC 297 Serialize BT, LC 407 Trapping II, LC 1183 Max Ones, LC 2183 Pairs Div K, LC 2502 Memory Allocator, LC 3163 String Compress III, LC 2563 Fair Pairs.

### P0 — Already Done (Tue/Wed)

LC 191, 190, 136, 231 (Tue bit ops); LC 206, 160, 19, 876, 141, 21, 23, 234, 443, 1, 53, 20 (Wed LL+arrays).

---

## Day-by-Day Plan

### Tue 2026-05-05 ✅ DONE
**Theme: C/C++ depth + DSA bit ops**
- [x] C/C++ MCQ diagnostic (3 batches) + notes — pointers, types, const, declarations, integer sizes
- [x] `memcpy` (overlap-safe) on paper — final canonical with `dest < src` rule
- [x] `strcmp`, `strlen`, `sizeof` macro on paper — elegant `*p1 - *p2` strcmp, MY_SIZEOF macro
- [x] DSA: bit manipulation × 4 (LC 191 Kernighan, 190 reverseBits, 136 XOR, 231 power-of-2)
- [x] 30-MCQ end-of-day recap → 30/30 ✓

### Wed 2026-05-06 ✅ DONE
**Theme: DSA grind + whiteboard skills + multi-threading**
- [x] Morning warm-up: 30-MCQ recap on Tuesday's topics → **24/30**
- [x] Notes additions: §1.1 char* vs char[], §1.2 C-vs-C++ trivia, §9.1 shift cheatsheet, §11.8 trailing padding, §13 HW numbers cheatsheet
- [x] Created `dsa_patterns_notes.md` (7 LC patterns)
- [x] Created `concurrency_notes.md` (5 sections: lock_guard, atomic vs mutex, unique_lock, CV, producer-consumer)
- [x] Notes additions: §7.1 *p++ trap, §14 Common C Bug Patterns (7 sub-sections), §6.5 expanded conversion rules, §2.1-2.4 byte arithmetic + endianness
- [x] **DSA: linked list × 8** (LC 206, 160, 19, 876, 141, 21, 23, 234) — all passing
- [x] **DSA: strings/arrays × 4** (LC 443 String Compress, 1 Two Sum, 53 Max Subarray, 20 Valid Parens) — all passing
- [x] **Block 3b: Multi-threading code** — Counter (mutex + atomic) + BoundedBuffer (mutex + 2 CVs)
- [x] **Block 3b: Find-bugs-in-C × 5** — UAF, off-by-one, malloc null check, *p++ trap, race condition
- [x] **Block 3b: Predict-output × 5** — pointer arith, sizeof decay, signed/unsigned, op precedence, sizeof('A')
- [x] **Block 3b: swap without temp + endianness detect on paper** — arithmetic + XOR + canonical endianness

### Thu 2026-05-07 (today, ~5 hrs left)
**Theme: ISP impls Tier 1 start + NPTEL theory + LC trees**

P1:
- [ ] **Block 1a: NPTEL — demosaicing + white balance lectures — 1.5 hr** ← start here
- [ ] **Block 1b: RGB → Grayscale (Q15) — 1.5 hr**
- [ ] **Block 1b: Gamma correction with LUT — 1 hr**
- [ ] **Block 4 P1: LC trees × 3** (LC 100, 104, 226) — 45 min
- [ ] **Block 3d: Multi-threading concepts theory** (mutex, semaphore, deadlock, atomic) — 1 hr (compressed)

P2 (if time):
- [ ] Block 4 P2: LC 392, 26, 11, 283 (15 min each, 30 min cap) — only if P1 done

### Fri 2026-05-08 (~7 hrs)
**Theme: ISP impls Tier 1 finish + NPTEL theory finish + LC high-leverage**

P1:
- [ ] **Block 1b: 3×3 Box filter — 1.5 hr**
- [ ] **Block 1b: Bilinear interpolation — 1 hr**
- [ ] **Block 1b: Histogram equalization — 1.5 hr**
- [ ] **Block 1b: Bayer demosaic basic bilinear — 1.5 hr**
- [ ] **Block 1a: NPTEL — CCM + gamma + color spaces lectures — 1.5 hr**
- [ ] **Block 4 P1: LC × 6** (LC 48 ⭐, 56, 3, 215, 8, 836) — 75 min

P2 (if time):
- [ ] Block 4 P2: LC 88, 70, 344, 169, 121, 217, 155 (15 min cap each) — only if P1 done

### Sat 2026-05-09 (~7 hrs)
**Theme: ISP Tier 2 + pipeline write-out + C/C++ refresh + LC harder**

P1:
- [ ] **Block 1b: RGB → YUV422 fixed-point — 1 hr**
- [ ] **Block 1b: CCM 3×3 + Sobel — 1.5 hr**
- [ ] **Block 1c: ISP pipeline whole-board write-out from memory — 45 min**
- [ ] **Block 4 P1: LC × 5** (LC 102, 235, 236, 146, 200) — 1.5 hr
- [ ] **Block 3c: Re-implement memcpy/strcmp UNDER TIME — 45 min**
- [ ] **Block 3a: Re-skim `cpp_pointer_notes.md` — 1 hr**
- [ ] **Block 2: Q-format cheat sheet — 30 min**

P2 / P3 (if time, time-boxed):
- [ ] Block 4 P2: LC 24, 50, 7, 9, 232, 110, 198, 322, 138 (15 min cap each)
- [ ] Block 4 P3: LC 46, 622, 210, 560 — only if confident

### Sun 2026-05-10 (~5-6 hrs, light)
**Theme: behavioral + mock + light technical refresh**

Behavioral (~3.5 hrs):
- [ ] "Why pivot" narrative — 60-90 sec answer + practice out loud 3× — 45 min
- [ ] **5 project pitches with ISP/camera framing** — depth map compression, IMU pre-integration math, TensorRT optimization, EKF Numba, GRU on embedded — 1.5 hr
- [ ] HM questions list — 5-7 sharp ones for camera modeling team — 45 min
- [ ] **Full 1-hr mock interview + self-review** — 1.5 hr

Technical refresh (~2 hrs):
- [ ] Mock review — patch top 2-3 gaps — 45 min
- [ ] Re-skim `cpp_pointer_notes.md` — 30 min
- [ ] Re-skim ISP implementations + Q-format cheat sheet — 30 min
- [ ] Light DSA P3 confidence-boost (2-3 easies, e.g., LC 387, 342, 58) — 30 min
- [ ] *(Optional, max 1)* One Hard for fun: LC 25 OR LC 42 — 30 min

Logistics (~30 min):
- [ ] Meeting link confirmed, camera/mic tested
- [ ] Environment ready (quiet room, water, notebook + pen, charged laptop)
- [ ] **Sleep by 10pm — no cramming after 9pm**

### Mon 2026-05-11 — Screening day
- [ ] Light review only — 30-min skim of notes
- [ ] Eat normal breakfast, hydrate
- [ ] 15 min before: re-read "why pivot" + project elevator pitches
- [ ] Screening
- [ ] Post-screen: dump everything you remember into a notes file while fresh

---

## What Got Cut (and Why)

These items were on earlier plan versions and have been **dropped** to avoid over-stuffing:

- ❌ **Block 5 LeetGPU** (18 problems) — irrelevant for camera systems modeling screen
- ❌ **SIMD/NEON intrinsics implementation** (vectorize Grayscale, Box filter) — awareness only is enough; not asked in 1hr HM screen
- ❌ **Math/logic puzzles** (daily 30-min + Sat 1-hr deep dive) — brain teasers out of fashion at modern Qualcomm
- ❌ **LC stretch list** (14 extra problems) — past diminishing returns
- ❌ **LC 72 Edit Distance** — DP-hard, low probability for this role
- ❌ **Find-bugs round 2 & 3, predict-output round 2** — single round done Wed is sufficient
- ❌ **CUDA vs SIMD vs Hexagon framing as a 30-min study block** — 5-min talking-point note is enough

**Time freed: ~10-12 hours**, redirected to NPTEL course refresh + project storytelling + breathing room.

---

## Cognitive load tips

- **Don't do 9 hrs of one thing.** Mix heavy (implementations, DSA) with medium (notes, project review) with light (talking out loud).
- **Pomodoro**: 50 min focused + 10 min break. 9 hrs of pure focus is impossible.
- **Sleep matters more than 1 extra hour.** Don't sacrifice sleep for prep.
- **"Why pivot" practiced out loud 3+ times** before Sunday's mock. Saying it cleanly under pressure is harder than thinking it cleanly.
- **NPTEL theory + Block 1b code = unified story.** When asked, lead with: *"I have formal ISP grounding from Prof. Rajagopalan's NPTEL course, plus hands-on Q-format implementations of demosaic, white balance, CCM, and gamma."*

---

## Reference docs

- `cpp_pointer_notes.md` — C/C++ deep knowledge notes
- `dsa_patterns_notes.md` — 7 LC patterns
- `concurrency_notes.md` — multi-threading reference
- `qualcomm_interview_intel.md` — interview reports + search queries
- `qualcomm_linkedin_outreach.md` — outreach (deferred — friend at Qualcomm covering)
- `resume_latest.pdf` — current resume
- `gpu_mode_lectures.md` — post-Qualcomm CUDA queue (deferred)
