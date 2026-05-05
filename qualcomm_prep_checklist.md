# Qualcomm Camera Systems Modeling — Comprehensive Prep Plan

**Screening**: Mon 2026-05-11 · 1hr virtual · HM technical screen
**Role**: Senior Engineer, Camera Systems Modeling, Qualcomm Bengaluru
**Daily budget**: 9 hours focused work

---

## 5 Technical Blocks

### Block 1 — C/C++ Deep Knowledge (heavy lift, ~11 hrs)

**1a — Language depth (knowing it)**
- [x] MCQ diagnostic + drill (pointers, types, const, declarations, integer sizes)
- [ ] Multi-threading concepts (mutex, semaphore, deadlock, atomic, condition var)

**1b — Whiteboard coding (doing it under pressure)**
- [ ] Implement-from-scratch on paper / plain editor (no IDE):
    - `memcpy` (overlap-safe)
    - `strcmp`, `strlen`, custom `sizeof` macro
    - Swap without temp
    - Endianness detect at runtime
    - Thread-safe counter, producer-consumer with mutex
- [ ] Find-bugs-in-C snippets (race, UAF, off-by-one, UB, signed/unsigned traps)
- [ ] Predict-output exercises (trace by hand)
- [ ] Articulate reasoning while writing

**Reference**: `cpp_pointer_notes.md`

### Block 2 — Full ISP via C Implementation (~8 hrs)
Build ISP intuition by writing the algorithms. Covers image processing + color science + fixed-point all at once.

**Currently slotted (Thu/Fri):**
- [ ] RGB → Grayscale (Q15 fixed-point, BT.601 weights) — 1.5 hr
- [ ] Gamma correction with LUT — 1 hr
- [ ] 3×3 Box filter (2D pointer arithmetic, edges) — 1.5 hr
- [ ] Bilinear interpolation (Q8/Q16 sub-pixel) — 1.5 hr
- [ ] Histogram equalization (CDF + LUT) — 1.5 hr
- [ ] ISP pipeline conceptual review (block diagram, color spaces) — 1 hr

#### Role-specific algorithm tiers (camera systems modeling)

**Tier 1 — almost certainly fair game**
- 2D Convolution / Linear filters *(JD-confirmed in 2017 Camera SE interview)*
- RGB ↔ YUV conversion (fixed-point matrix multiply)
- Gamma correction (LUT-based)
- Histogram + CDF + equalization
- Bilinear interpolation
- Fixed-point Q-format multiply/add
- Saturation arithmetic (clipping uint8/int16 to range)

**Tier 2 — moderately likely**
- Bayer demosaicing (basic bilinear)
- White balance (gray-world / WB gain)
- Color matrix correction (CCM) — 3×3 matrix per pixel
- Box filter / separable Gaussian
- Bilateral filter (edge-preserving)
- Sobel / gradient operators
- Mean / variance / std on image regions (AE/AWB stats)

**Tier 3 — possible but lower priority**
- Median filter 3×3
- Black level subtraction
- Lens shading correction
- Tone mapping curve (Reinhard / global)
- Image rotation 90° (pointer/index gymnastics)
- PSNR / SSIM computation

**Tier 4 — niche / skip**
- DCT (8×8 block) — JPEG/encoder
- 2D FFT — frequency analysis
- Hough transform — more CV than ISP

**If extra time after Thu/Fri impls, prioritize in this order:**
1. Bayer demosaic (basic bilinear) — most ISP-flavored
2. RGB → YUV422 fixed-point — direct Q-format exercise
3. Color matrix correction (CCM) — 3×3 multiply
4. Sobel / gradient — easy add

### Block 3 — LeetCode DSA, Qualcomm Frequency (~5.5 hrs)

**Tier 1 (must-do, ~3.5 hrs)**
- [ ] LC 443 String Compression
- [ ] LC 206 Reverse Linked List
- [ ] LC 191 Number of 1 Bits
- [ ] LC 160 Intersection of Two Linked Lists
- [ ] LC 19 Remove Nth Node From End
- [ ] LC 876 Middle of Linked List
- [ ] LC 190 Reverse Bits
- [ ] LC 141 Linked List Cycle

**Tier 2 (~2 hrs)**
- [ ] LC 1 Two Sum, LC 53 Maximum Subarray, LC 20 Valid Parentheses, LC 23 Merge K Sorted

**Tier 3 (skim-only)**: LC 72 Edit Distance approach (30 min)

### Block 4 — Resume (~30 min)
- [x] Done. Final pass: verify email, GitHub link polish.

### Block 5 — LeetGPU (~3.5 hrs)
- [ ] 3 problems/day × 6 days. Fun side track.

---

## Soft / Behavioral Prep (~10.5 hrs)

- [ ] **Project deep-dives** × 5 — algorithm-level, whiteboard-ready (1 hr each)
    - Visual Inertial SLAM (Addverb)
    - Inertial Odometry GRU-DNN (Monarch)
    - Visual Localization (Monarch)
    - Simulation Pipeline + RL (Addverb)
    - Indoor Mobile Robot + Localization Stack
- [ ] **"Why pivot" narrative** — SLAM → camera modeling story (1.5 hrs)
- [ ] **HM questions to ask** — 5-7 sharp ones (1 hr research + draft)
- [ ] **Math/logic puzzles** — light skim (1.5 hrs)
- [ ] **Mini-mock interview** — Saturday (30 min)
- [ ] **Full mock interview** — Sunday (1.5 hrs)
- [ ] **Practice projects out loud** — Saturday (2.5 hrs)

---

## Day-by-Day Plan (9 hrs/day, max technical + daily puzzles, Sun = light behavioral + tech refresh)

### Tue 2026-05-05 (today, ~5 hrs remaining)
**Theme: C/C++ depth + DSA bit ops**
- [x] C/C++ MCQ diagnostic (3 batches) + notes
- [ ] `memcpy` (overlap-safe) on paper — 1 hr
- [ ] `strcmp`, `strlen`, `sizeof` macro on paper — 1.5 hrs
- [ ] DSA: bit manipulation × 4 (LC 191, 190, 136, 231) — 1.5 hrs
- [ ] Math/logic puzzles — 2-3 classics (door flip, ball weighing) — 30 min
- [ ] LeetGPU × 3 — 45 min

### Wed 2026-05-06 (9 hrs)
**Theme: DSA grind + whiteboard skills + multi-threading + SIMD primer**
- [ ] DSA: linked list × 6 (LC 206, 160, 19, 876, 141, 23) — 2.5 hrs
- [ ] DSA: strings/arrays × 4 (LC 443, 1, 53, 20) — 1.5 hrs
- [ ] Block 1b: swap without temp + endianness detect on paper — 45 min
- [ ] Block 1b: Find-bugs-in-C × 5 snippets — 1 hr
- [ ] Block 1b: Predict-output × 5 snippets — 1 hr
- [ ] Block 1b: Multi-threading code (counter, producer-consumer) — 1.5 hrs
- [ ] **SIMD/NEON primer + Hexagon HVX awareness** (concepts only) — 45 min
- [ ] Math/logic puzzles — divisibility rules + 100 prisoners — 30 min
- [ ] LeetGPU × 3 — 45 min

### Thu 2026-05-07 (9 hrs)
**Theme: ISP impls Tier 1 + multi-threading concepts + first SIMD vectorization**
- [ ] Block 2: RGB → Grayscale (Q15) — 1.5 hrs
- [ ] Block 2: Gamma correction with LUT — 1 hr
- [ ] Block 2: 3×3 Box filter — 1.5 hrs
- [ ] Block 2: Bilinear interpolation — 1 hr
- [ ] **SIMD: vectorize Grayscale with NEON intrinsics** — 45 min
- [ ] Block 1a: Multi-threading concepts (mutex, semaphore, deadlock, atomic) — 1.5 hrs
- [ ] Block 3: LC 72 Edit Distance approach (skim) — 30 min
- [ ] Math/logic puzzles — measurement / probability — 30 min
- [ ] LeetGPU × 3 — 45 min

### Fri 2026-05-08 (9 hrs)
**Theme: ISP Tier 1 finish + Tier 2 + SIMD round 2 + drill round 2**
- [ ] Block 2: Histogram equalization — 1.5 hrs
- [ ] Block 2: Bayer demosaic (basic bilinear) — 1.5 hrs
- [ ] Block 2: RGB → YUV422 fixed-point — 1.5 hrs
- [ ] Block 2: ISP pipeline conceptual review — 45 min
- [ ] **SIMD: vectorize Box filter with NEON intrinsics** — 45 min
- [ ] Block 1b: Find-bugs round 2 × 5 — 45 min
- [ ] Block 1b: Predict-output round 2 × 5 — 45 min
- [ ] Math/logic puzzles — bit-manip, lateral thinking — 30 min
- [ ] LeetGPU × 3 — 45 min

### Sat 2026-05-09 (9 hrs)
**Theme: ISP bonus + CUDA-SIMD-Hexagon framing + speed drills + puzzles + consolidation**
- [ ] Block 2 (bonus): CCM 3×3 + Sobel/gradient — 2.5 hrs
- [ ] **CUDA vs SIMD vs Hexagon DSP framing** — 30 min
- [ ] Block 1b: Re-implement `memcpy`/`strcmp` UNDER TIME — 1 hr
- [ ] Block 3: Re-solve 4-5 hardest LC problems — 1.5 hrs
- [ ] Block 1b: Find-bugs/predict-output round 3 — 1 hr
- [ ] **Math/logic puzzles deep-dive** — CtCI Ch. 6 full skim — 1 hr
- [ ] Notes consolidation: re-read `cpp_pointer_notes.md` — 45 min
- [ ] LeetGPU × 3 — 45 min

### Sun 2026-05-10 (9 hrs) — Light behavioral + technical refresh + mock
**Theme: minimum behavioral (you're interview-warm) + technical patching + mock**

Behavioral (~3.5 hrs):
- [ ] "Why pivot" narrative — draft 60-90 sec answer + practice out loud 3× — 45 min
- [ ] Project deep-dive quick pass × 5 (camera-angle framing) — 20 min each, 1.5 hrs total
- [ ] HM questions list — 5-7 sharp questions specific to camera modeling team — 45 min
- [ ] Full 1hr mock interview — 1.5 hrs *(includes setup + delivery + self-review)*

Technical refresh (~4.5 hrs):
- [ ] Mock review — identify gaps, patch top 2-3 — 1 hr
- [ ] Re-skim `cpp_pointer_notes.md` end-to-end — 1 hr
- [ ] Re-skim ISP implementations + key concepts — 1 hr
- [ ] Light DSA refresh — 2-3 easy problems for confidence — 1 hr
- [ ] LeetGPU × 3 (light) — 30 min

Logistics (~30 min):
- [ ] Meeting link confirmed, camera/mic tested
- [ ] Environment ready (quiet room, water, notebook + pen, charged laptop)
- [ ] Sleep early — no cramming after 9pm

### Mon 2026-05-11 — Screening day
- [ ] Light review only — 30-min skim of notes
- [ ] Eat normal breakfast, hydrate
- [ ] 15 min before: re-read your "why pivot" answer + project elevator pitches
- [ ] Screening
- [ ] Post-screen: dump everything you remember into a notes file while fresh

---

## Cognitive load tips

- **Don't do 9 hrs of one thing.** Mix heavy (implementations, DSA) with medium (notes, project review) with light (LeetGPU, talking out loud).
- **Pomodoro**: 50 min focused + 10 min break. 9 hrs of pure focus is impossible.
- **Sleep matters more than 1 extra hour.** Don't sacrifice sleep for prep.
- **"Why pivot" practiced out loud 3+ times** before Sunday's mock. Saying it cleanly under pressure is harder than thinking it cleanly.

---

## Reference docs

- `cpp_pointer_notes.md` — C/C++ deep knowledge notes
- `qualcomm_interview_intel.md` — interview reports + search queries
- `qualcomm_linkedin_outreach.md` — outreach (deferred — friend at Qualcomm covering)
- `resume_latest.pdf` — current resume
- `gpu_mode_lectures.md` — post-Qualcomm CUDA queue (deferred)
