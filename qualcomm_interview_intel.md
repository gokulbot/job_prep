# Qualcomm India Interview Intel

Aggregated from real interview reports + JD signals. Use to calibrate prep.

---

## Search Queries — for continued intel gathering

Run these in Google directly in your browser. Sort by date / use Past Year filter to skip stale posts.

### Glassdoor (free account gives full access)
```
site:glassdoor.com Qualcomm "Camera Systems Modeling" interview
site:glassdoor.co.in Qualcomm camera systems interview Bengaluru
site:glassdoor.com Qualcomm camera ISP interview
site:glassdoor.com Qualcomm "senior engineer" camera interview
site:glassdoor.com Qualcomm multimedia interview Bengaluru
site:glassdoor.com Qualcomm "image signal processor" interview
site:glassdoor.com Qualcomm Bengaluru hiring manager screening
site:glassdoor.com Qualcomm fixed-point interview
```

### Blind (teamblind.com — needs verified work email)
```
site:teamblind.com Qualcomm camera interview
site:teamblind.com Qualcomm "camera systems modeling"
site:teamblind.com Qualcomm Bengaluru interview
site:teamblind.com Qualcomm Bangalore camera
site:teamblind.com Qualcomm ISP interview
site:teamblind.com Qualcomm multimedia interview
site:teamblind.com Qualcomm senior engineer offer Bengaluru
site:teamblind.com Qualcomm camera ISP modeling
```

### LeetCode Discuss
```
site:leetcode.com/discuss Qualcomm camera
site:leetcode.com/discuss Qualcomm multimedia
site:leetcode.com/discuss Qualcomm Bengaluru
site:leetcode.com/discuss Qualcomm Bangalore
site:leetcode.com/discuss Qualcomm ISP
site:leetcode.com/discuss Qualcomm "senior engineer" India interview
site:leetcode.com/discuss Qualcomm modeling
```

### Reddit (Google's the way in — direct scrape blocked)
```
site:reddit.com Qualcomm camera interview
site:reddit.com Qualcomm camera systems modeling
site:reddit.com Qualcomm Bangalore interview
site:reddit.com r/qualcomm interview
site:reddit.com r/cscareerquestionsindia Qualcomm
site:reddit.com r/developersIndia Qualcomm
site:reddit.com Qualcomm "fixed point" interview
```

### InterviewBit / GeeksforGeeks
```
site:interviewbit.com Qualcomm camera interview
site:geeksforgeeks.org Qualcomm camera systems interview
site:geeksforgeeks.org Qualcomm multimedia interview
```

### Broad (no site filter — catches Medium, personal blogs, etc.)
```
"Qualcomm" "Camera Systems Modeling" interview experience
"Qualcomm" Bengaluru camera senior engineer offer 2024 OR 2025 OR 2026
"Qualcomm" multimedia camera interview "hiring manager"
"Qualcomm" "fixed-point" image processing interview
Qualcomm Bangalore "camera systems" interview hiring manager round
```

### Tips for sifting

- Add **Google Tools → Past year** filter to skip stale 2018-2021 reports (interview style has shifted)
- Glassdoor: free account gives full access to interview pages
- Blind: verified work email needed, but Google snippets often show key info even without login
- Sort by **Date** when results have a Date column
- If a snippet shows "free, paid offer..." the page often has the full interview write-up
- Prefer 2023-2026 posts for current style, but old reports (2017+) still useful for topical patterns

### Subreddits worth searching natively (logged in to Reddit)

- r/qualcomm
- r/cscareerquestionsindia
- r/developersIndia
- r/EmbeddedSystems
- r/computerscience
- r/cscareerquestions

---

## Source: LeetCode — Qualcomm Senior Engineer, Display Team, Hyderabad (April 2024 [Offer])

**Candidate background**: 3.2 yrs experience, prior SDE-2 at product MNC.

### Round 1 — Coding (1hr 10min)
- **10 min**: Intro + project discussion
- **45 min C/C++ deep dive**:
  - Processes, threads
  - Dangling pointers
  - **Polymorphism** (types, examples, code samples)
  - **Virtual** and **friend** keywords
  - **MCQ-style questions on code output** (compile vs runtime errors)
- **Coding**: Merge K Sorted Lists on shared whiteboard + complexity analysis + walkthrough

### Round 2 — Design (2hr 5min)
- **20 min**: Intro + projects
- **1hr Low-Level Algorithmic Discussion** — CRT screen tearing problem:
  - Producer-consumer with shared memory buffer
  - 120 FPS, 1ms TAT
  - Design algorithm to prevent tearing
  - Reader-writer / sync flavor
  - Candidate gave 3 solutions
- **30 min Low-Level Design**: Text editor with undo/redo (10 ops). Follow-up: extend to allow editing/deleting earlier text
- **5 min**: Q&A

### Round 3 — Hiring Manager (25 min — much shorter than scheduled 45 min)
**KEY INSIGHT**: HM round was **PUZZLES + MATH**, not behavioral!
- **Door Flip Puzzle** (classic — answer = perfect squares)
- **Divisibility rule of 11** (math derivation)

Verbal positive feedback at end. Offer next business day.

---

## Source: GeeksforGeeks — Qualcomm Embedded Software (Set 2)

**Position**: Embedded Software Application Developer (C/RTOS/DS in telecom)

### Recurring topics across rounds
- C fundamentals: storage classes, memory mapping
- Linked list: deletion, reverse, loop detection
- Function pointers: usage, examples
- Implement strstr (optimal way)
- Big & Little endian definitions / representations
- **Implement memcpy** (with overlap handling)
- System vs library calls
- Priority inversion in RTOS
- UDP vs TCP
- Timer module design with callbacks
- Stack corruption / smashing
- Race conditions, semaphores, mutex
- Strcmp, pointer subtraction

---

## Source: GeeksforGeeks — Qualcomm Set 8 (Associate SE, 1 yr exp)

**Format**: 5 rounds (1 telephonic + 4 F2F + 1 manager/HR)

### Recurring patterns
- 10+ pointer output questions
- Reverse linked list, left view of binary tree, **implement memcpy**
- Multi-threading, mutex, locking
- Storage classes in C
- Reverse bits, count set bits
- Inline functions, custom sizeof
- Detect loop in linked list
- Static variables/functions

### Manager/HR round (20-30 min)
- Previous projects
- Role explanation
- Joining timeline + relocation
- Motivation for Qualcomm
- Salary expectations

---

## Source: Glassdoor — Qualcomm Camera Systems Engineer, Hyderabad (Feb 2017) ⭐ DIRECT ROLE MATCH

**Candidate**: Anonymous, accepted offer, positive experience, "average" difficulty.
**Application**: Through recruiter / online professional network.

### Process
- 2 telephonic rounds
- 4 F2F technical rounds (60-90 min each)
- 1 HR round (current profile, package, interest)
- 1 final telephonic round explaining the offered profile

### Question topics (3 categories the candidate listed)
1. **"Most of questions were on C language (Pointers, Arrays)"** — C language depth dominates
2. **"Image processing fundamentals like Linear Filters etc."** — FIRST DIRECT EVIDENCE that image processing concepts come up for camera roles
3. **"Linked lists, RTOS, multi-threaded programming, mutex and semaphores"** — embedded/OS flavor

### Implications
- Confirms heavy C/C++ pointer + array fluency expectation
- Confirms **linear filters / convolution** as fair game for image processing portion
- Confirms RTOS + multi-threading concepts (mutex, semaphores) — even for camera roles, not just embedded
- **Caveat**: 2017 report — 9 years old, some style may have shifted. But the topical focus (C + image processing + OS) is consistent with newer general Qualcomm reports.

---

## Source: Glassdoor — Firmware Development Student (4 May 2026) ⭐ FRESH

**Format**: Single round (student / intern position)

### Structure
- Resume + position-related questions
- Basic embedded questions
- **2 coding questions, LeetCode-style, about BITS** ← bit manipulation explicitly confirmed in May 2026

---

## Source: Glassdoor — Operating Systems Interview (26 Apr 2026) ⭐ FRESH

**Format**: 3 interviews — 2 technical + 1 experience/projects

### Round 1 (technical)
- 1 system design question (high level)
- 1 Fibonacci-style problem (standard DSA medium)

### Round 2 (technical) — KEY NEW PATTERN
- **Finding bugs in C code snippets** (read code, identify bugs) ← new format we haven't seen
- **Write basic concurrent code using mutex** (practical implementation, not theory)

### Round 3
- Experience + projects deep-dive

---

## Source: GitHub krishnadey30/LeetCode-Questions-CompanyWise — Qualcomm all-time ⭐ ACTUAL DATA

Frequency-ranked list of LeetCode problems actually asked at Qualcomm.

| Rank | LC # | Title | Difficulty | Frequency |
|---|---|---|---|---|
| 1 | 160 | Intersection of Two Linked Lists | Easy | 0.061 |
| 2 | 191 | Number of 1 Bits | Easy | 0.055 |
| 3 | 206 | Reverse Linked List | Easy | 0.044 |
| 4 | 72 | Edit Distance | Hard | 0.038 |
| 5 | 237 | Delete Node in a Linked List | Easy | 0.030 |
| 6 | 2 | Add Two Numbers | Medium | 0.020 |
| 7 | 1 | Two Sum | Easy | 0.019 |
| 8 | 283 | Move Zeroes | Easy | 0.017 |
| 9 | 151 | Reverse Words in a String | Medium | 0.009 |
| 10 | 53 | Maximum Subarray | Easy | 0.008 |
| 11 | 1183 | Maximum Number of Ones | Hard | 0 |

### Key observations
- **4 of top 11 are linked list** (160, 206, 237, 2) — confirms heavy linked-list emphasis
- **Bit manip top 3** — LC 191 confirmed
- **Edit Distance (Hard DP) at #4** — surprising, but worth at least understanding the approach
- **Mostly Easy problems** — confirms Qualcomm's style is fundamentals over LeetCode hard
- **Two pointers / arrays**: LC 283, LC 53, LC 1
- **Strings**: LC 151

### Implication for prep
**Tier 1 (must-do, top 5)**: LC 160, 191, 206, 237, 2
**Tier 2 (high ROI)**: LC 1, LC 283, LC 53, LC 151, LC 72 (at least understand approach for Edit Distance)
**Skip**: LC 1183 (zero frequency)

---

## Source: GitHub liquidslr/interview-company-wise-problems — Qualcomm All ⭐ ACTUAL DATA (50 problems)

Top by frequency:

| Difficulty | Title | Freq |
|---|---|---|
| MEDIUM | String Compression III | 100.0 |
| HARD | Maximum Number of Ones | 97.6 |
| EASY | Reverse Linked List | 91.9 |
| MEDIUM | Remove Nth Node From End | 74.2 |
| MEDIUM | Rotate Image | 74.2 |
| MEDIUM | LRU Cache | 74.2 |
| MEDIUM | Number of Islands | 74.2 |
| EASY | Reverse Bits | 74.2 |
| EASY | Middle of the Linked List | 74.2 |
| MEDIUM | Design Memory Allocator | 66.9 |
| EASY | Majority Element | 66.9 |
| EASY | Merge Sorted Array | 66.9 |
| EASY | Two Sum | 66.9 |
| EASY | Valid Parentheses | 66.9 |
| EASY | Implement Queue using Stacks | 66.9 |
| EASY | Palindrome Number | 66.9 |
| EASY | Climbing Stairs | 66.9 |
| EASY | Number of 1 Bits | 66.9 |
| MEDIUM | Swap Nodes in Pairs | 66.9 |
| MEDIUM | Linked List Cycle | 56.6 |
| EASY | Single Number | 56.6 |
| EASY | Power of Two | 56.6 |
| EASY | Power of Four | 56.6 |
| MEDIUM | Maximum Subarray | 56.6 |
| (and 26 more at 56.6 freq) | | |

### Cross-source intersection (highest confidence)
Appears in BOTH source 1 (krishnadey30) AND source 2 (liquidslr):
- LC 206 Reverse Linked List
- LC 191 Number of 1 Bits
- LC 1 Two Sum
- LC 53 Maximum Subarray

### Topical breakdown of full list
- **Linked list (heavy)**: Reverse, Remove Nth, Middle, Swap Pairs, Cycle, Reverse K-Group, Intersection
- **Bit manipulation**: Number of 1 Bits, Reverse Bits, Power of Two, Power of Four, Single Number
- **Basic DP**: Climbing Stairs, Maximum Subarray, Edit Distance
- **Stack/Queue**: Valid Parentheses, Implement Queue using Stacks
- **Math**: Palindrome Number, Reverse Integer, Pow(x,n), atoi
- **Two pointers / sorting**: Merge Sorted Array, Two Sum II, Remove Duplicates
- **Strings**: String Compression (#1!), Reverse Words, Atoi, First Unique Char, Is Subsequence
- **Trees**: Max Depth, Serialize/Deserialize BT
- **Graph**: Number of Islands, Course Schedule II
- **System design lite**: LRU Cache, Design Memory Allocator, Design Tic-Tac-Toe

### Curated TOP 12 for prep (combining both sources)

**Tier 1 — must-do (~3-4 hrs)**
1. LC 443 String Compression *(rank 1 in source 2 + asked in actual SDE-1 round)*
2. LC 206 Reverse Linked List *(both sources, top freq)*
3. LC 191 Number of 1 Bits *(both sources, top freq)*
4. LC 160 Intersection of Two Linked Lists *(rank 1 source 1)*
5. LC 19 Remove Nth Node From End *(74.2% source 2)*
6. LC 876 Middle of Linked List *(74.2% source 2, easy warmup)*
7. LC 190 Reverse Bits *(74.2% source 2)*
8. LC 141 Linked List Cycle *(both sources)*

**Tier 2 — should-do (~2 hrs)**
9. LC 1 Two Sum *(both sources, warmup)*
10. LC 53 Maximum Subarray *(both sources, Kadane's)*
11. LC 20 Valid Parentheses *(66.9% source 2)*
12. LC 23 Merge K Sorted Lists *(asked in actual Display SE round)*

**Tier 3 — if time (skim only)**
- LC 146 LRU Cache, LC 70 Climbing Stairs, LC 200 Number of Islands, LC 24 Swap Pairs, LC 72 Edit Distance (understand approach only)

**Skip (Hard, low ROI for 1hr screen)**: LC 25 Reverse K-Group, LC 297 Serialize BT, LC 42 Trapping Rain Water

---

## Source: Reddit r/qualcomm — Camera Engineer (2 years ago, advice from commenter)

**Note**: This is the **Camera Engineer** role (firmware/driver flavor), NOT Camera Systems Modeling. Different team, different topical focus. Use as *adjacent* context only.

**OP**: Grad student in software ML/CV, applying for Camera Engineer position with no camera/HW background.
**OP did not post follow-up** with actual interview questions.

### Top comment (advice from commenter "thenewpants")
For a Camera Engineer (firmware-flavored) role, suggested topics:
- **MIPI Bus** — sensor↔SoC interface protocol
- **Linux kernel device tree** — driver/HW description
- **Camera tuning** — ISP parameter tuning

### Why this likely DOESN'T apply directly to Camera Systems Modeling

| Camera Engineer (firmware) | Camera Systems Modeling (your role) |
|---|---|
| MIPI Bus protocol | C/C++ models of image processing modules |
| Linux kernel device tree | Fixed-point implementations |
| Driver development | HW behavioral test vectors |
| Camera tuning | Algorithm complexity / quality trade-offs |

That said — if your HM probes "do you know our HW interface stack" type questions, having heard of MIPI / device tree won't hurt. Quick 15-min skim is the most you'd want to invest.

---

## Source: Medium — Qualcomm SDE-1 (8 months exp via referral)

### Online round (1hr, 3 problems)
- LeetCode 136: Single Number
- LeetCode 2183: Count Array Pairs Divisible by K
- String compression: "AABCCC" → "2A1B3C", but 13 A's → "9A4A"

### In-office round (2hr)
- DP: toy purchase variant of coin change
- Dockerfile + K8s commands
- SQL vs NoSQL, SQL queries, primary vs foreign key
- C++ application lifecycle
- Behavioral rapid-fire scenarios

---

## Synthesized patterns (high-confidence)

| Pattern | Confidence | Source coverage |
|---|---|---|
| C/C++ language minutiae (pointers, polymorphism, virtual, friend, dangling pointers) | **Very high** | Display SE, Embedded, Set 8 |
| Bit manipulation (reverse bits, count set bits, LeetCode-style bits problems) | **VERY HIGH — confirmed May 2026** | Set 8, Embedded, Firmware Student May 2026 |
| Find bugs in C code snippets (debugging exercise) | **Medium-high — new May 2026 signal** | OS Interview Apr 2026 |
| Write concurrent code with mutex (practical, not theory) | **Medium-high — new May 2026 signal** | OS Interview Apr 2026 |
| Linked list implementations in C/C++ | **Very high** | Display SE (Merge K), Set 8, Embedded |
| "Implement memcpy / strstr / strcmp / sizeof" | **High** | Embedded, Set 8 |
| MCQ on code output / compile vs runtime errors | **Medium** | Display SE |
| OS fundamentals (mutex, deadlock, IPC) | **High** | Embedded, Set 8 |
| Project deep-dive | **Universal** | All sources |
| HM round = puzzles + math (NOT behavioral) | **Medium-high** | Display SE — this contradicts typical FAANG pattern! |
| HM round = behavioral / fit | **Medium** | Set 8 |
| String compression problem | **Medium** | SDE-1 |
| Low-level design (text editor, etc.) | **Medium** | Display SE |
| Image processing fundamentals (linear filters, convolution) | **High** (for camera roles) | Camera SE Hyderabad 2017 |
| RTOS / multi-threaded / mutex-semaphore | **High** | Camera SE Hyderabad 2017, Embedded, Set 8 |
| Fixed-point specifics | **Unknown** | No direct reports yet |
| ISP block-level specifics (demosaic, AWB, gamma) | **Unknown** | No direct reports yet |

---

## Critical implications for Camera Systems Modeling screen (Mon 2026-05-11)

### What to expect — top guesses

1. **HM round at Qualcomm India ≠ pure behavioral**. Display team HM did puzzles. Could be:
   - Math/logic puzzles (door flip, divisibility, perfect squares classics)
   - Quick C/C++ MCQs on output/error
   - Algorithm question on whiteboard
   - Project deep-dive

2. **C/C++ language minutiae is universal at Qualcomm** — even if your role is "modeling," expect pointer/polymorphism/virtual/friend/dangling/MCQ questions to come up. Don't underestimate.

3. **One coding question on whiteboard is plausible** — Merge K sorted lists / linked list / bit manipulation / string compression range.

4. **Project deep-dive is guaranteed**. Pick 1-2 SLAM/perception projects and have algorithm-level depth ready (not narrative-level).

5. **Image processing / fixed-point** — no direct evidence from reports, but JD strongly signals it. Expect at least conceptual questions if not coding.

### Prep priorities (revised)

| Topic | Priority | Time budget |
|---|---|---|
| C/C++ language deep dive (polymorphism, virtual, friend, vtable, dangling pointers, code-output MCQs) | **CRITICAL** | 4-5 hrs |
| Linked list / bit manipulation / string in C++ on LeetCode | **CRITICAL** | 6-8 hrs |
| "Implement X in C" (memcpy, strcmp, strlen, sizeof) | **HIGH** | 2-3 hrs |
| Math/logic puzzle warmup (Cracking the Coding Interview Ch. 6 brainteasers) | **HIGH** | 2-3 hrs |
| Project deep-dive at algorithm level | **HIGH** | 3-4 hrs |
| Image processing concepts (convolution, filters, color spaces, histogram eq) | **MEDIUM-HIGH** | 3-4 hrs |
| Fixed-point arithmetic (Q-format, scaling, overflow) | **MEDIUM** | 2 hrs |
| ISP pipeline awareness (block diagram, demosaic, AWB, gamma) | **MEDIUM** | 2 hrs |
| OS fundamentals refresh (mutex, deadlock, IPC) | **MEDIUM** | 2 hrs |
| Behavioral / "why this role" / questions for HM | **REQUIRED** | 1 hr |

---

## Update log
- 2026-05-05: Initial intel from LeetCode (Display SE Hyderabad), GeeksforGeeks (Embedded, Set 8), Medium (SDE-1)
- 2026-05-05: Added Glassdoor Camera Systems Engineer Hyderabad 2017 — direct role match, confirms linear filters / image processing fundamentals as interview topic
- 2026-05-05: Added Glassdoor Firmware Student (May 2026) and OS Interview (Apr 2026) — confirms bit manipulation still prevalent + new patterns: find-bugs-in-C-snippets, write-concurrent-code-with-mutex
- 2026-05-05: Added Reddit r/qualcomm Camera Engineer (firmware variant) — adjacent role, suggests MIPI Bus / kernel device tree / camera tuning as study areas. Caveat: different from Camera Systems Modeling target.
