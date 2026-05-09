# DSA Patterns — Qualcomm Prep

LC patterns + tricks captured during Wed DSA grind. Companion to `cpp_pointer_notes.md` (which covers C/C++ depth, not algorithms).

---

## 1. Two-pointer "Switch Heads" — LC 160 Intersection of Two Linked Lists

### The trick

Two pointers, each walks **its own list, then jumps to the other list's head when it hits null**. They meet at the intersection node, or both at nullptr if no intersection.

```cpp
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    ListNode* pA = headA;
    ListNode* pB = headB;
    while (pA != pB) {
        pA = (pA == nullptr) ? headB : pA->next;
        pB = (pB == nullptr) ? headA : pB->next;
    }
    return pA;   // intersection node, or nullptr
}
```

**O(m + n) time, O(1) space, single pass, no length computation.**

### Why it works — the math

Let:
- `a` = length of A's prefix before intersection
- `b` = length of B's prefix before intersection
- `c` = length of shared tail (intersection onwards)

Then `lenA = a + c`, `lenB = b + c`.

`pA`'s journey to intersection node (after switching once): `lenA + b = a + b + c`
`pB`'s journey to intersection node (after switching once): `lenB + a = a + b + c`

**Same total → they arrive at the same step.**

If no intersection: both arrive at `nullptr` after `lenA + lenB` steps simultaneously → loop exits with `pA == pB == nullptr` → returns nullptr.

### Mental model

> "Run my path then yours; you run yours then mine. We'll meet exactly when our journeys are the same length."

### When to use vs length-diff

| Approach | Passes | Code lines | Mental load | Interview signal |
|---|---|---|---|---|
| Length-diff | 3 | ~20 | Clear decomposition | Shows reasoning |
| Switch-heads | 1 | ~6 | "Aha" insight | Shows pattern library |

**Strategic interview play**: verbalize length-diff first, *then* offer "there's a slicker version using a lane-switch trick." Demonstrates both decomposition and pattern recognition.

### First-try probability

Cold without prior exposure: ~5-10% even for strong candidates. **Don't beat yourself up if you produce length-diff first.** Length-diff is fully acceptable.

### Length-diff alternative (also O(m + n), O(1))

```cpp
ListNode* getIntersectionNode(ListNode* headA, ListNode* headB) {
    size_t lenA = 0, lenB = 0;
    for (auto p = headA; p; p = p->next) lenA++;
    for (auto p = headB; p; p = p->next) lenB++;

    if (lenA > lenB) {
        for (size_t i = 0; i < lenA - lenB; i++) headA = headA->next;
    } else {
        for (size_t i = 0; i < lenB - lenA; i++) headB = headB->next;
    }

    while (headA && headB) {
        if (headA == headB) return headA;
        headA = headA->next;
        headB = headB->next;
    }
    return nullptr;
}
```

Three passes (compute lengths, advance, walk together). Same complexity, more lines, more intuitive.

### Common bugs (from my session)

- **Lockstep without sync**: walking both pointers from `headA` and `headB` simultaneously misses the intersection when prefix lengths differ.
- **Loop with `||`**: causes null-deref when one pointer runs out before the other.
- **Forgetting to decrement** in the advance loop → infinite loop / segfault.

---

## 2. Iterative Linked-List Reversal — LC 206

### The 3-pointer dance

```cpp
ListNode* reverseList(ListNode* head) {
    ListNode* prev = nullptr;
    ListNode* curr = head;
    while (curr != nullptr) {
        ListNode* next = curr->next;   // 1. SAVE next before clobbering
        curr->next = prev;             // 2. FLIP current node's pointer
        prev = curr;                   // 3. ADVANCE prev
        curr = next;                   // 4. ADVANCE curr
    }
    return prev;   // NOT curr — curr is nullptr at exit
}
```

**O(n) time, O(1) space.**

### Mental model

Think of three pointers walking a tightrope. At each step:
1. **Anchor before letting go** — save `next` before `curr->next = prev` clobbers it
2. **Flip the link** — point `curr` backward
3. **Slide both pointers forward** — `prev` becomes `curr`, `curr` becomes saved `next`

### The classic bug

> Returning `curr` instead of `prev`.

At loop exit, `curr` has walked off the end → `nullptr`. **`prev` holds the new head** (which was the original last node).

### Why this matters for hardware roles

Linked-list reversal isn't just a CS-101 question — it tests:
- **Pointer manipulation under invariants** (don't lose anchors)
- **Order-of-operations discipline** (you can't move both `prev` and `curr` until you've saved `next`)
- **Edge case handling** (empty list, single node)

Same skills needed for buffer management, ring buffers, free-list manipulation in HW model code.

---

## 3. Nth-from-End — LC 19 (two passes + dummy node)

### Two-pass approach (most intuitive)

```cpp
ListNode* removeNthFromEnd(ListNode* head, int n) {
    // Pass 1: compute length
    size_t len = 0;
    for (auto p = head; p; p = p->next) len++;

    // Walk to (len - n - 1)th node = the predecessor of the target
    int steps = len - n;
    ListNode* prev = nullptr;
    ListNode* curr = head;
    while (steps-- > 0) {
        prev = curr;
        curr = curr->next;
    }

    // Remove curr
    if (prev != nullptr) {
        prev->next = curr->next;
        return head;
    } else {
        return head->next;   // removing the head — no predecessor
    }
}
```

**O(L) time, O(1) space, two passes.**

### The dummy-node trick — eliminates the head-removal special case

When a linked-list operation might modify the head, prepend a **sentinel/dummy node**. Now `prev` always exists, no special case needed.

```cpp
ListNode* removeNthFromEnd(ListNode* head, int n) {
    size_t len = 0;
    for (auto p = head; p; p = p->next) len++;

    ListNode dummy(0, head);          // dummy → head → ...
    ListNode* prev = &dummy;
    int steps = len - n;
    while (steps-- > 0) {
        prev = prev->next;
    }
    prev->next = prev->next->next;    // skip the target

    return dummy.next;                // works for both head and non-head
}
```

**Pattern**: any LL problem where the head might be removed/swapped → use a dummy. Saves branching, fewer bugs.

### One-pass two-pointer (gap / fast-slow) — alternative

```cpp
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0, head);
    ListNode* fast = &dummy;
    ListNode* slow = &dummy;

    // Move fast n+1 steps ahead → creates an n-gap
    for (int i = 0; i <= n; i++) fast = fast->next;

    // Move both until fast hits null → slow is at predecessor
    while (fast != nullptr) {
        fast = fast->next;
        slow = slow->next;
    }

    slow->next = slow->next->next;    // remove
    return dummy.next;
}
```

**O(L) time, O(1) space, one pass.**

The trick: maintain a **fixed gap of n** between two pointers. When the leading one hits the end, the trailing one is exactly at the predecessor of the target.

### When to use each

| Approach | When |
|---|---|
| Two-pass | Most intuitive, easy to explain, fully accepted |
| Dummy node | Whenever head might be modified — make it your default for LL problems |
| One-pass gap | When interviewer asks "can you do it in one pass?" |

### Common bugs (from my session)

- **Typos in identifiers** (`cur` vs `curr`, `travesal` vs `traversal`) — write slowly on whiteboard
- **Head removal edge case**: forgetting that `prev` is null when removing first node → null deref
- **Off-by-one on traversal steps**: if you walk `len - n` steps you reach the target itself; you usually want `len - n - 1` steps to reach the **predecessor**, OR walk `len - n` steps but track `prev` along the way (this code does the latter)

---

## 4. Fast-Slow / Tortoise-Hare — LC 876 Middle of Linked List

### The pattern

Two pointers from `head`. Slow advances **1 step**, fast advances **2 steps**. When fast reaches the end, slow is at the middle.

```cpp
ListNode* middleNode(ListNode* head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    return slow;
}
```

**O(n) time, O(1) space, single pass.**

### The two-condition loop guard (critical)

```cpp
while (fast != nullptr && fast->next != nullptr)
```

Order matters — short-circuit evaluation skips `fast->next` when fast is null.

| Stopping case | Means |
|---|---|
| `fast == nullptr` | Even length — fast walked off the end |
| `fast->next == nullptr` | Odd length — fast on the last node |

Without **both** checks, the loop crashes on even-length lists (fast becomes null, then `fast->next` derefs).

### Where slow lands

| Length | Fast ends at | Slow ends at |
|---|---|---|
| Empty | n/a (loop skipped) | head (nullptr) |
| 1 node | last (fast->next is null) | head |
| 2 nodes (even) | nullptr | second node |
| Odd (e.g., 5) | last node | middle (3rd) |
| Even (e.g., 6) | nullptr | second middle (4th) |

For LC 876 the spec asks for the **second middle** on even — falls out naturally.

### To get the FIRST middle on even (e.g., for "split list" problems)

Track the predecessor of slow, or use a one-step-delayed fast:

```cpp
// First-middle variant: stop one step earlier
while (fast->next != nullptr && fast->next->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
}
// For [1,2,3,4]: returns slow=2 (first middle)
```

### Why this is the unlock for cycle detection (LC 141, 142)

The same two-pointer setup detects **cycles**:
- If there's no cycle, fast hits null
- If there's a cycle, fast eventually catches up to slow inside the loop

Once they meet, you can find cycle entry by resetting one to head and walking both at speed 1 until they meet again. Floyd's tortoise-hare. Same machinery as LC 876.

### Common bugs (from my session)

- **Missing null check on `fast`**: works for odd-length, crashes for even-length (`fast` becomes null, then `fast->next` derefs)
- **Wrong order of conditions**: `fast->next != nullptr && fast != nullptr` — checks `fast->next` BEFORE confirming `fast` is non-null → still derefs null

### Memorization aid

> **"Fast lands at the end, slow lands at the middle. Check both fast and fast->next, in that order."**

---

## 5. Cycle Detection — LC 141 (Floyd's Tortoise-Hare)

### The pattern

Same fast-slow setup as LC 876, but checking whether they ever **meet** (cycle exists) vs whether fast hits null (no cycle).

```cpp
bool hasCycle(ListNode *head) {
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;   // check AFTER advancing
    }
    return false;
}
```

**O(n) time, O(1) space.**

### The order-of-operations bug

`slow` and `fast` both start at `head` — they are **already equal** at iteration 0. If you check equality before advancing, you return true immediately for any non-empty list with ≥2 nodes:

```cpp
// WRONG — returns true for non-cycle lists
while (fast != nullptr && fast->next != nullptr) {
    if (slow == fast) return true;   // fires on iter 1 always
    slow = slow->next;
    fast = fast->next->next;
}
```

```cpp
// CORRECT — advance first, then check
while (fast != nullptr && fast->next != nullptr) {
    slow = slow->next;
    fast = fast->next->next;
    if (slow == fast) return true;
}
```

**Memorize this rule for any "two pointers from same start" problem**: advance before comparing.

### Why fast-slow detects cycles

If there's a cycle of length `C`:
- Once both pointers are inside the cycle, fast closes the gap by 1 step per iteration (relative to slow)
- Maximum iterations to meet inside cycle: `C` steps
- Fast can't "jump over" slow — at worst, fast is 1 step behind slow, and the next iteration fast advances 2 while slow advances 1, closing exactly to 0

If there's no cycle, fast eventually hits `nullptr` (or `fast->next == nullptr`) and the loop exits.

### Bridge to LC 142 (cycle entry point)

Once fast and slow meet inside the cycle:
1. Reset one pointer (say `slow`) back to `head`
2. Walk both at speed 1
3. Where they meet again is the **entry node** of the cycle

The math: distance from head to cycle entry equals distance from meeting point to cycle entry (both modulo cycle length). Floyd's complete cycle algorithm.

### Common bugs (from my session)

- **Equality check before advancing** → returns true for any list with ≥2 nodes
- **Return `1`/`0` instead of `true`/`false`** — works but reads as C, not C++
- Missing `fast != nullptr` guard (same bug as LC 876)

### Memorization aid

> **"Same loop as middle-of-list. Advance first, compare second. Meeting means cycle; null means no cycle."**

---

## 6. Iterative Pairwise Merge (Divide-and-Conquer Without Recursion) — LC 23

### The pattern

When you have `k` "things" that can be combined pairwise (sorted lists, intervals, etc.) and a 2-input merge primitive, you can combine all `k` in **O(N log k)** by repeatedly halving the count instead of merging sequentially (which would be O(k·N)).

```cpp
ListNode* mergeKLists(vector<ListNode*>& lists) {
    if (lists.empty()) return nullptr;

    while (lists.size() > 1) {
        vector<ListNode*> merged;
        for (size_t i = 0; i < lists.size(); i += 2) {
            if (i + 1 < lists.size()) {
                merged.push_back(mergeTwoLists(lists[i], lists[i+1]));
            } else {
                merged.push_back(lists[i]);   // orphan: carry forward
            }
        }
        lists = merged;
    }
    return lists[0];
}
```

**O(N log k) time, O(k) space** (for the auxiliary vector).

### Visual: shrinking by half each pass

```
Pass 1:  [L0, L1, L2, L3, L4]                  (5 lists, 1 orphan)
         merge(L0,L1)  merge(L2,L3)  L4
         → [M01, M23, L4]                      (3 lists)

Pass 2:  [M01, M23, L4]                        (3 lists, 1 orphan)
         merge(M01,M23)   L4
         → [M0123, L4]                         (2 lists)

Pass 3:  [M0123, L4]                           (2 lists)
         merge(M0123, L4)
         → [Mall]                              (1 list, exit)
```

### Why O(N log k), not O(k·N)

| Approach | Cost |
|---|---|
| **Sequential pairwise** (merge all into list[0] one at a time) | First merge: 2 nodes. Second: 3. ... Last: N nodes. Total: O(k·N) |
| **Iterative pairwise (this pattern)** | Each pass touches all N nodes once. log₂(k) passes. Total: O(N log k) |

For k=10⁴ and N=10⁴: sequential = 10⁸ ops, iterative = ~1.3×10⁵ ops. **~750× faster.**

### Why it's the merge step of merge-sort

Merge-sort splits an array into halves, sorts each half recursively, then merges. The "merge" pass at the bottom of merge-sort is exactly this pattern: `2k` sorted runs → merge to `k` runs → merge to `k/2` → ... → 1 run.

LC 23 just **starts at the merge phase** — the input is already `k` sorted lists, you just need the merging.

### Recursive alternative

Same complexity, often shorter to write:

```cpp
ListNode* mergeKLists(vector<ListNode*>& lists) {
    if (lists.empty()) return nullptr;
    return mergeRange(lists, 0, lists.size() - 1);
}

ListNode* mergeRange(vector<ListNode*>& lists, int lo, int hi) {
    if (lo == hi) return lists[lo];
    int mid = lo + (hi - lo) / 2;
    auto left = mergeRange(lists, lo, mid);
    auto right = mergeRange(lists, mid + 1, hi);
    return mergeTwoLists(left, right);
}
```

**Recursion depth = log k**, so stack-safe for k up to 10⁴.

### When to use iterative vs recursive

| Use iterative when | Use recursive when |
|---|---|
| You want to avoid stack-depth concerns | You want shorter, more declarative code |
| You need to inspect intermediate state | You're already in a recursive solution |
| You're worried about deep recursion (k > 10⁵) | Code clarity is paramount |

### Common bugs (from my session)

- **Wrong index for orphan**: `lists[i+1]` instead of `lists[i]` in the else branch — out-of-bounds when there's an unpaired list at the end
- **Missing return statement** after the while loop — function falls off the end
- **Empty input crash**: `return lists[0]` when `lists.empty()` — guard at the top
- **`lists.size()` re-evaluated each iter**: not a bug, but cache it for clarity (`for (size_t i = 0, n = lists.size(); i < n; i += 2)`)

### Where else this pattern shows up

- **LC 88 Merge Sorted Array** — k=2 case, in-place
- **LC 21 Merge Two Sorted Lists** — the inner primitive (k=2)
- **LC 23 Merge K Sorted Lists** — this problem
- **LC 218 Skyline Problem** — divide-and-conquer with interval merging
- **LC 315 Count Smaller After Self** — merge-sort with index tracking
- **General**: any "combine k things via pairwise op" — file merging, log compaction, interval intersection

### Memorization aid

> **"Halve each pass. Pair up, merge each pair into a new vector, replace. When one's left, return it."**

---

## 7. Palindrome LL — LC 234 (Pattern Composition)

### The pattern: combine three primitives

LC 234 is the classic example of **composing patterns you've already seen**. No new technique — just three sub-routines stitched together:

1. **Fast-slow** (LC 876) → find the middle
2. **In-place reverse** (LC 206) → reverse the second half
3. **Two-pointer compare** → walk first half forward, reversed second half forward

```cpp
bool isPalindrome(ListNode* head) {
    if (head == nullptr) return true;          // empty IS a palindrome
    
    // 1. Find middle
    ListNode* slow = head;
    ListNode* fast = head;
    while (fast != nullptr && fast->next != nullptr) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    // 2. Reverse from slow to end (the second half)
    ListNode* prev = nullptr;
    ListNode* curr = slow;
    while (curr != nullptr) {
        ListNode* next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    
    // 3. Compare first half (forward) with reversed second half (forward)
    while (prev != nullptr) {
        if (prev->val != head->val) return false;
        prev = prev->next;
        head = head->next;
    }
    return true;
}
```

**O(n) time, O(1) space.**

### The key insight: why "forward vs forward" works after reversal

Naive idea: walk both halves forward and compare values. **WRONG** — palindrome requires the second half to match the first half *reversed*.

```
[1, 2, 3, 2, 1]
First half:  1, 2
Second half: 2, 1

Forward vs forward:  1 == 2? NO.    (wrong check)
Forward vs reversed: 1 == 1? YES.    (correct check)
```

**The fix**: physically reverse the second half **before** comparing. Now both halves are walked forward, but you're comparing first-half-forward against second-half-reversed → which is the right semantic.

### Odd vs even length — does middle matter?

| Length | After fast-slow | Middle handling |
|---|---|---|
| Odd, e.g. `[1,2,3,2,1]` | `slow` at the middle (idx 2, val 3) | Include middle in second half. Middle compares to itself → always matches → no harm. |
| Even, e.g. `[1,2,2,1]` | `slow` at start of second half (idx 2) | No middle to worry about. Reverse from `slow` directly. |

Either way, **don't skip slow**. The cleanest version reverses from `slow` itself — works for both parities.

### Why slow-fast lands in the right place

For even length `[1,2,2,1]` (length 4):
- Iter 1: slow→idx 1, fast→idx 2
- Iter 2: slow→idx 2, fast→idx 4 (= nullptr)
- Loop exits. slow at idx 2 = first node of second half ✓

For odd length `[1,2,3,2,1]` (length 5):
- Iter 1: slow→idx 1, fast→idx 2
- Iter 2: slow→idx 2, fast→idx 4
- Iter 3: fast→null after 4->next->next, exit. slow at idx 2 = middle ✓

### Why this pattern matters

Pattern **composition** is the hallmark of senior-level LC problems. Once you see "find middle + reverse + compare" written as three primitives you already know, it stops being scary.

The same composition powers:
- **LC 143 Reorder List**: find middle + reverse second half + interleave
- **LC 86 Partition List**: dummy nodes for two sublists + concatenate
- **LC 92 Reverse LL II**: locate position + reverse range + stitch back
- **LC 25 Reverse K-Group**: identify groups + reverse each + chain

### Restoration (interview style point)

You **destructively modified** the input by reversing the second half. In production code, reverse it back after comparison to leave the list in original state:

```cpp
// After the compare loop:
// (optional) reverse `prev` again to restore the original list
```

Most interviewers don't require this, but mentioning it in passing scores style points.

### Common bugs (from my session)

- **Comparing forward vs forward without reversing**: returns true only for all-same-value lists
- **Order of operations in reversal**: `curr = next` BEFORE `prev = curr` clobbers the reference. Must be `prev = curr; curr = next;`
- **Typos under speed**: `Listnode` (lowercase n), `fasle` (transposed), missing semicolons. **Slow down on whiteboard.**

### Memorization aid

> **"Find middle. Reverse from middle. Walk both halves forward, comparing. The reversal is what makes forward-vs-forward semantically correct."**

---

## General LL Pattern Library

### Multi-pointer techniques (recurring across LC)

| Technique | Used in | Pattern |
|---|---|---|
| **Single iter + prev** | LC 19 (two-pass), LC 203 Remove Elements | Track previous node to splice |
| **3-pointer dance** | LC 206 Reverse, LC 92 Reverse II | prev, curr, next for in-place flip |
| **Fast-slow / tortoise-hare** | LC 141 Cycle, LC 142 Cycle II, LC 876 Middle | Two pointers, different speeds |
| **Fixed-gap two-pointer** | LC 19 (one-pass) | Two pointers, fixed distance apart |
| **Switch-heads** | LC 160 Intersection | Each pointer walks both lists |
| **Dummy/sentinel node** | LC 19, LC 21, LC 23, LC 86, LC 203 | Prepend dummy to handle head-modification |

**Default toolkit**: when an LL problem might modify the head, reach for a dummy node first. When you need to find a position relative to the end, reach for fast-slow or fixed-gap. When you need to reverse, the 3-pointer dance.

---

## 8. Matrix Rotation — LC 48 (Transpose + Reverse)

### The core trick

Square-matrix rotations decompose into **two simple passes**:
1. **Transpose** — swap across the main diagonal: `matrix[i][j] ↔ matrix[j][i]`
2. **Reverse** — reverse each row (or each column)

The combination + order determines which rotation:

| Rotation | Step 1 | Step 2 |
|---|---|---|
| **90° CW** | Transpose | Reverse each row |
| **90° CCW** | Transpose | Reverse each column |
| **90° CW (alt)** | Reverse each column | Transpose |
| **90° CCW (alt)** | Reverse each row | Transpose |
| **180°** | Reverse rows top-to-bottom | Reverse each row's contents |

### 90° clockwise (LC 48)

```cpp
void rotate(vector<vector<int>>& matrix) {
    int n = matrix.size();
    // Step 1: transpose (upper triangle, j > i, to avoid double-swap)
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            swap(matrix[i][j], matrix[j][i]);
    // Step 2: reverse each row
    for (int i = 0; i < n; i++)
        reverse(matrix[i].begin(), matrix[i].end());
}
```

### 90° counter-clockwise

```cpp
void rotateCCW(vector<vector<int>>& matrix) {
    int n = matrix.size();
    // Transpose
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            swap(matrix[i][j], matrix[j][i]);
    // Reverse each column
    for (int j = 0; j < n; j++)
        for (int i = 0; i < n / 2; i++)
            swap(matrix[i][j], matrix[n - 1 - i][j]);
}
```

### 180° rotation

```cpp
void rotate180(vector<vector<int>>& matrix) {
    int n = matrix.size();
    // Reverse rows top-to-bottom
    for (int i = 0; i < n / 2; i++)
        swap(matrix[i], matrix[n - 1 - i]);
    // Reverse each row's contents
    for (int i = 0; i < n; i++)
        reverse(matrix[i].begin(), matrix[i].end());
}
```

**Or** direct point-to-point swap (each pixel with its diametrically opposite):
```cpp
for (int i = 0; i < n; i++)
    for (int j = 0; j < n; j++) {
        if (i * n + j >= (n * n) / 2) break;  // stop at center
        swap(matrix[i][j], matrix[n - 1 - i][n - 1 - j]);
    }
```

### Mental model — the geometry

> **Transpose flips along the main diagonal** (top-left to bottom-right). After transpose, the image is mirrored along the diagonal.
>
> Then a **horizontal flip** (reverse each row) gives 90° CW.
> A **vertical flip** (reverse each column) gives 90° CCW.
> Both flips give 180°.

Each rotation is `transpose + 1 flip`; 180° is `2 flips` (or 0 transposes + 2 flips, etc.).

### Common bugs (from my session)

1. **Iterating wrong half of matrix in transpose** — must use `j > i` (upper triangle), else double-swap restores the original. Common: `j = 0` instead of `j = i + 1`.
2. **Mixing iterators across containers** — `reverse(matrix[i].begin(), matrix.end())` mixes row iterator with matrix iterator. Both must be from the same row: `matrix[i].end()`.
3. **Off-by-one in reverse-rows half-iterate** — for `n` rows, swap rows 0..(n/2-1) with their mirrors. Going `i < n` would double-swap and undo the work.

### Why rotations matter for ISP / Qualcomm

Camera sensors are sometimes mounted at non-standard orientations (90° / 180° / 270°) — the ISP must rotate the raw frame before further processing. Common in:
- Mobile camera modules (different sensors face different directions)
- Automotive ADAS (camera mounted upside-down on rear-view)
- Robot/drone cameras

**Production trick:** the naive transpose is **memory-bandwidth-bound** because consecutive iterations access non-contiguous memory (column traversal). Real ISP code uses **tile-based transpose** (e.g., transpose 8×8 tiles in L1, then transpose tile positions among themselves) to keep the working set cached. This is the "cache-oblivious transpose" pattern.

**Senior signal in interviews:**
> "For image rotation in production ISP, I'd use a tile-based transpose to maintain cache locality — the naive single-element transpose is memory-bound. The math is the same, but iterating in 8×8 or 16×16 tiles keeps each tile resident in L1."

### Memorization aid

> **Transpose + reverse one axis = 90° rotation. Two reverses = 180°. The axis you reverse picks CW vs CCW.**

---

## 9. Interval Merge — LC 56 (Sort + Sweep)

### The pattern

Two-step recipe that handles every interval-merge problem:

1. **Sort by start time** — `O(N log N)`. After sorting, any two overlapping intervals are adjacent.
2. **Sweep linearly** — for each new interval:
   - **Extend** the last result interval if it overlaps (`new.start <= last.end`)
   - **Append** as a new interval otherwise

```cpp
vector<vector<int>> merge(vector<vector<int>>& intervals) {
    sort(intervals.begin(), intervals.end());
    vector<vector<int>> result;
    for (auto& iv : intervals) {
        if (!result.empty() && iv[0] <= result.back()[1]) {
            result.back()[1] = std::max(iv[1], result.back()[1]);
        } else {
            result.push_back(iv);
        }
    }
    return result;
}
```

**O(N log N) time, O(N) space (output).**

### Why sorting unlocks the linear sweep

Without sort, every pair must be compared → `O(N²)`. After sorting by start:
- If `new.start > last.end` → no overlap. **And** no earlier interval can overlap either (they all started even earlier and were already merged into `last`).
- If `new.start <= last.end` → overlap.

Sorting by start is the key invariant that lets you forget everything except `result.back()`.

### The bug-magnet: fully-contained intervals

```
[[1,10], [2,3]]  →  must merge to [1,10], NOT [1,3]
```

If you naively write `result.back()[1] = iv[1]` you destroy the wider end. **Always `max(last.end, new.end)`** when merging — the new interval might be entirely swallowed.

### Touching vs strictly overlapping

LC 56 counts touching intervals (`[1,4]` and `[4,5]`) as overlap → use `<=` not `<`.

If the problem says "intervals are open at the right endpoint" or "strictly overlapping" → use `<`. **Read the spec carefully.**

### Mental model

> **"Sort by start. Walk the sorted list. For each new interval, either extend the current run or start a new one. Always take max of ends when extending."**

### Why this matters for hardware / image processing

Same pattern shows up in:
- **DMA descriptor coalescing**: merging contiguous memory regions before issuing transfers (fewer descriptors → less overhead)
- **Dirty-region tracking** in framebuffers: an ISP often tracks which scanlines changed; merging adjacent dirty intervals before re-rendering
- **Time-window aggregation**: sensor frames within a time window get merged for processing

### Common bugs

- **Forgetting `max` on the merged end** — fully-contained intervals lose their wider extent
- **Using `<` instead of `<=`** — fails on touching intervals (LC 56 spec)
- **`emplace_back(intervals[i])`** — works but is misleading; `push_back` is the right verb when the argument is already the right type. `emplace_back` shines when constructing in place: `emplace_back(start, end)`.
- **Signed/unsigned mismatch** with `int i` and `.size()` — prefer range-for or `size_t i`
- **Forgetting to sort** — without sort, the sweep silently produces wrong answers on unsorted input

### Where this pattern generalizes

| Problem | Twist |
|---|---|
| LC 56 Merge Intervals | Vanilla sort + sweep |
| LC 57 Insert Interval | Pre-sorted input + insertion point |
| LC 252 Meeting Rooms | Detect any overlap (don't merge) |
| LC 253 Meeting Rooms II | Count max concurrent intervals (heap of ends, or sweep-line +1/-1) |
| LC 435 Non-overlapping Intervals | Sort by **end** for greedy interval scheduling |
| LC 1288 Remove Covered Intervals | Sort by start asc, end desc; sweep |

**The variant that trips most people**: LC 435 sorts by **end**, not start. Sorting by start is right for "merge what overlaps"; sorting by end is right for "keep as many non-overlapping as possible."

### Memorization aid

> **"Sort by start. Sweep. Extend with max-of-ends or append."**

---

## 10. Rectangle Overlap — LC 836 (Separating Axis, 2D)

### The pattern

Two axis-aligned rectangles overlap iff they overlap on **both** the x-axis AND the y-axis projection. The 2D problem reduces to **two independent 1D interval-overlap checks**.

```cpp
bool isRectangleOverlap(vector<int>& rec1, vector<int>& rec2) {
    bool x_overlap = std::max(rec1[0], rec2[0]) < std::min(rec1[2], rec2[2]);
    bool y_overlap = std::max(rec1[1], rec2[1]) < std::min(rec1[3], rec2[3]);
    return x_overlap && y_overlap;
}
```

**O(1) time, O(1) space.**

### The 1D primitive

Two intervals `[a, b]` and `[c, d]` overlap with positive length iff:

```
max(a, c) < min(b, d)
```

- `<` (strict) → "touching doesn't count" (LC 836 spec)
- `<=` → "touching counts as overlap" (LC 56 spec)

**Read the problem statement** to know which to use.

### Why the formula works

Visualize the two intervals on a number line:
- `max(left edges)` = the rightmost left edge → "where the overlap region starts"
- `min(right edges)` = the leftmost right edge → "where the overlap region ends"

If start < end, there's positive overlap. If start ≥ end, the "overlap region" is empty or inverted (no overlap).

### Equivalent: separation theorem (negation form)

```cpp
return !(rec1[2] <= rec2[0] ||   // rec1 fully left of rec2
         rec2[2] <= rec1[0] ||   // rec2 fully left of rec1
         rec1[3] <= rec2[1] ||   // rec1 fully below rec2
         rec2[3] <= rec1[1]);    // rec2 fully below rec1
```

This is the **Separating Axis Theorem (SAT)** in its simplest form: two convex shapes don't overlap iff there exists an axis along which their projections are disjoint. For axis-aligned rectangles, the only candidate axes are x and y.

For arbitrary convex polygons, you'd test against all face normals — same idea, more axes.

### Why this matters for hardware / CV

- **Bounding-box collision** in physics engines and game loops (axis-aligned phase before tighter checks)
- **IoU (Intersection over Union)** in object detection: numerator is the rectangle overlap area, denominator is union — same overlap-region computation drives modern detectors
- **Tile/clipping in graphics pipelines**: which framebuffer tiles intersect a draw call's bounding box?
- **Dirty-rectangle tracking** in display compositors: which screen regions need redraw?

### Tying it back to LC 56

LC 56 was 1D interval merging with sort + sweep.
LC 836 is 1D interval overlap, **applied twice (x and y)**.

The 1D primitive `max(starts) < min(ends)` (or `<=`) is the building block. Recognizing it across problems is the senior signal.

### Common bugs

- **Wrong inequality** (`<=` when problem says touching doesn't count, or vice-versa)
- **Confusing the rect format** — LC 836 uses `[x1, y1, x2, y2]` with `(x1,y1)` bottom-left. Some problems use `[x, y, w, h]` (origin + width/height) — read carefully.
- **Treating it as a 2D geometric mess** instead of recognizing two 1D problems

### Memorization aid

> **"Two rectangles overlap iff their x-projections overlap AND their y-projections overlap. Each is a 1D max-of-starts < min-of-ends check."**

---

## 11. Sliding Window with Set — LC 3 (Longest Substring Without Repeating)

### The pattern

Maintain a window `[left, right)` over the string with the invariant **all characters inside are unique**. When a duplicate would enter, slide `left` forward (erasing characters along the way) until the duplicate is gone. Then add the new character.

```cpp
int lengthOfLongestSubstring(string s) {
    std::unordered_set<char> seen;
    size_t i = 0, j = 0;
    size_t max_len = 0;

    while (j < s.size()) {
        // Slide left until s[j] is no longer a duplicate
        while (seen.find(s[j]) != seen.end() && i < j) {
            seen.erase(s[i]);
            i++;
        }
        seen.insert(s[j]);
        j++;
        max_len = std::max(seen.size(), max_len);
    }
    return (int)max_len;
}
```

**O(n) time, O(min(n, σ)) space** where σ = alphabet size.

### The invariant (load-bearing)

After the outer body completes, `seen == {s[i], s[i+1], ..., s[j-1]}`. Every operation preserves this:
- Erase from front when sliding `i` → maintains lower bound
- Insert `s[j]` then increment `j` → maintains upper bound

### Why amortized O(n)

Both pointers are **monotonic** — they only advance, never reset. `i` and `j` each cross the string at most once, so total inner-loop work is ≤ 2n across all outer iterations.

The naïve "reset on collision" alternative is O(n²) because each collision throws away progress. **Sliding > resetting** is the whole insight.

### The bug-magnet: variable shadowing

`unordered_set<char> s` clashes with the input `string s`. Always rename the set (e.g., `seen`). Same lesson applies to any helper container — don't reuse the parameter's name.

### The other bug-magnet: `find` vs end

`unordered_set::find` returns an **iterator**, not a count. Compare to `.end()`, never to `0`:
- "in set" → `seen.find(c) != seen.end()`
- "not in set" → `seen.find(c) == seen.end()`

### When the inner-loop guard `i < j` matters

If you ever reach `i == j`, the window is empty, so any duplicate of `s[j]` must be impossible — the guard is technically unreachable in correct code. But it's a cheap defensive check that prevents pathological infinite loops if the invariant ever breaks (e.g., from a future refactor).

### Variant: last-seen-index map (faster constant factor)

Replace the inner erase loop with a direct `left` jump:

```cpp
int lengthOfLongestSubstring(string s) {
    std::unordered_map<char, int> last;
    int left = 0, max_len = 0;
    for (int right = 0; right < (int)s.size(); right++) {
        auto it = last.find(s[right]);
        if (it != last.end() && it->second >= left) {
            left = it->second + 1;   // jump past prior occurrence
        }
        last[s[right]] = right;
        max_len = std::max(max_len, right - left + 1);
    }
    return max_len;
}
```

`left` jumps directly past the prior occurrence (still monotonic — only advances). Same O(n), fewer hash ops in the worst case.

The set version is more general (no need to track positions); the map version is faster when you have positional info.

### Why this matters for hardware / signal processing

Sliding window over time-series data is the basis for:
- **Moving averages / sliding median** in image and audio filters
- **Frame-to-frame deduplication** in video pipelines
- **Burst detection** in sensor streams (max activity in any K-frame window)
- **Cache eviction** in LRU implementations (window of recently-accessed keys)

The "two pointers, both monotonic" structure is the same machinery in all of them.

### Where this pattern generalizes

| Problem | Twist |
|---|---|
| LC 3 | Longest window with unique chars |
| LC 76 Min Window Substring | Smallest window containing all chars of `t` (need + count) |
| LC 209 Min Subarray Sum ≥ k | Smallest window with sum ≥ k |
| LC 438 All Anagrams of `p` | Fixed-size window, hashmap match |
| LC 567 Permutation in String | Fixed-size window, character-frequency match |
| LC 904 Fruit Into Baskets | Longest window with at most 2 distinct values |

**The structural distinction**: "longest window with property X" vs "shortest window with property X" — same two-pointer skeleton, different update rules.

### Common bugs

- **Variable shadowing**: same name for input string and the helper container
- **`find != 0`**: comparing iterator to int instead of `.end()`
- **Resetting the window** (`set.clear()`) on collision instead of sliding `left` — turns it into O(n²) and produces wrong answers (e.g., `"dvdf"` returns 2 instead of 3)
- **Off-by-one on window size**: `right - left + 1` for `[left, right]` inclusive; or `seen.size()` if maintaining a set
- **Signed/unsigned mismatch** in `max(set.size(), int_var)` — both args must be the same type

### Memorization aid

> **"Two pointers, both monotonic. Right adds, left erases on collision. Window size is `right - left + 1` or `set.size()`."**

---

## 12. Top-K via Size-K Heap — LC 215 (Heap Inversion Trick)

### The pattern

For the K-th largest element: maintain a **min-heap of size K**. Each time the heap exceeds size K, pop the smallest. After processing all elements, the heap's top is the K-th largest.

```cpp
int findKthLargest(vector<int>& nums, int k) {
    std::priority_queue<int, vector<int>, greater<int>> q;   // min-heap
    for (int x : nums) {
        q.push(x);
        if ((int)q.size() > k) q.pop();
    }
    return q.top();
}
```

**O(n log k) time, O(k) space.**

### The inversion trick (counterintuitive, load-bearing)

| Want | Heap type | Why |
|---|---|---|
| **K-th largest** | **min-heap** of size K | Top = smallest of the K biggest = the K-th biggest overall |
| **K-th smallest** | **max-heap** of size K | Top = largest of the K smallest = the K-th smallest overall |

Beginners reach for a max-heap to find "the largest." Correct for one element. **For top-K with a size-K bounded heap, you want the *opposite* — a min-heap so the weakest of your survivors sits at the top, ready to be evicted when something better shows up.**

### Why bounded-heap, not full-heap

Three approaches with their trade-offs:

| Approach | Time | Space | Note |
|---|---|---|---|
| Sort + index `nums[n-k]` | O(n log n) | O(1) extra | One-line fallback, works |
| Max-heap of all + pop k-1 | O(n + k log n) | O(n) | Build heap once, pop k-1 |
| **Size-K min-heap** | **O(n log k)** | **O(k)** | Canonical top-K pattern |
| Quickselect | O(n) avg, O(n²) worst | O(1) | Optimal asymptote, bug-prone |

**Why size-K wins for streaming**: when n is unknown ahead of time (data arrives online), you can't sort or build a full heap. The size-K bounded heap processes elements one at a time and stays at O(K) memory. Same code works for batch and streaming.

### C++ heap API gotchas

```cpp
priority_queue<int>                              q1;   // MAX-heap (default)
priority_queue<int, vector<int>, less<int>>      q2;   // MAX-heap (explicit)
priority_queue<int, vector<int>, greater<int>>   q3;   // MIN-heap
```

The third template arg is the **comparator**. The comparator is the *opposite* of what's at the top:
- `less<>` (a < b) → max at top
- `greater<>` (a > b) → min at top

Operations:
- `q.top()` — access (NOT `q.front()`; that's `std::queue`'s API)
- `q.push(x)` — insert
- `q.pop()` — remove top
- `q.size()`, `q.empty()` — usual

**Constructor speedup**: passing a range to the constructor builds the heap in O(n) instead of O(n log n) sequential pushes:
```cpp
priority_queue<int> q(nums.begin(), nums.end());   // O(n) heapify
```

### Mental model

> **"Min-heap surfaces the loser. Max-heap surfaces the champion. To track top-K, use a min-heap so the weakest of your survivors is at the top — ready to be evicted when something better arrives."**

### Common bugs

- **Wrong heap direction**: max-heap-of-size-k for "k-th largest" — gives k-th smallest instead. Single most common bug on this problem.
- **`q.front()` vs `q.top()`**: `priority_queue` doesn't have `front`. That's `queue`'s API.
- **Off-by-one on K**: `k=1` means the largest, not second largest. The heap top is the answer when heap size == k.
- **Default comparator confusion**: `priority_queue<int>` is max-heap; `set<int>` is sorted asc. Different defaults. Don't assume.
- **Signed/unsigned**: `q.size()` is `size_t`, `k` is `int`. Cast one or compiler may warn.

### Where this pattern generalizes

| Problem | Heap setup |
|---|---|
| LC 215 K-th Largest | Size-K min-heap |
| LC 703 K-th Largest in Stream | Same — size-K min-heap, persists across calls |
| LC 347 Top K Frequent | Size-K min-heap on (freq, val) pairs |
| LC 692 Top K Frequent Words | Size-K min-heap with custom comparator |
| LC 973 K Closest Points to Origin | Size-K **max**-heap on distance (we want K **smallest** distances) |
| LC 295 Median from Data Stream | Two heaps — max-heap for lower half, min-heap for upper half |
| LC 23 Merge K Sorted Lists | Min-heap of list heads (top = next element to emit) |

**LC 973 is the "max-heap for K-smallest" mirror image** — useful sanity check that you've internalized the inversion rule.

### Why this matters for hardware / streaming

Size-K bounded heaps appear in:
- **Top-K monitoring** in observability pipelines (top 10 slowest queries, top 5 noisiest sensors)
- **K-NN search** in vector retrieval (heap of K nearest neighbors as you scan)
- **Outlier detection** (top K outliers by score)
- **Embedded / DSP**: bounded-memory algorithms when full-array sort isn't feasible

The key property is **constant memory in K**, regardless of input size — critical for streaming/embedded contexts where you can't buffer the whole input.

### Memorization aid

> **"K-th largest = min-heap of size K. Push, evict if too big. The top is the K-th best because it's the smallest of the K winners."**

---

## 13. Tree Level-Order BFS — LC 102 (Snapshot Trick)

### The pattern

BFS with a queue, but at the start of each level, **snapshot `queue.size()`** to know how many nodes belong to the current level. Process exactly that many — children pushed during the inner loop become next level's snapshot.

```cpp
vector<vector<int>> levelOrder(TreeNode* root) {
    vector<vector<int>> result;
    if (!root) return result;

    queue<TreeNode*> q;
    q.push(root);

    while (!q.empty()) {
        int level_size = q.size();             // snapshot BEFORE the inner loop
        vector<int> level;
        for (int i = 0; i < level_size; i++) {
            TreeNode* curr = q.front();
            q.pop();
            level.push_back(curr->val);
            if (curr->left)  q.push(curr->left);
            if (curr->right) q.push(curr->right);
        }
        result.push_back(level);
    }
    return result;
}
```

**O(n) time, O(w) space** where w = max width of the tree.

### The snapshot trick (load-bearing)

At the moment you snapshot `queue.size()`, the queue contains **exactly the nodes of the current level** — none of their children have been enqueued yet. By processing exactly `level_size` pops, you finish the current level even though the queue grows during the loop.

If you instead checked `q.size()` inside the loop (`while (q.size() > 0)`), you'd never advance levels — the queue keeps being non-empty as children get added.

### The two children → two ifs (not else-if)

```cpp
if (curr->left)  q.push(curr->left);
if (curr->right) q.push(curr->right);
```

A node can have both children. `if/else` would push only one of them — silent data loss. **Two independent ifs.**

### Why this matters

Level-order is the foundation pattern for an entire family of tree problems:

| Problem | Tweak |
|---|---|
| LC 102 Level Order | Vanilla — group by level |
| LC 107 Level Order Bottom-Up | Same code, `std::reverse(result)` at end |
| LC 199 Right Side View | Last node of each level (when `i == level_size - 1`) |
| LC 116/117 Next Right Pointers | Connect siblings within a level using prev pointer |
| LC 103 Zigzag Level Order | Alternate push direction per level (or reverse every other level) |
| LC 515 Largest Value Each Row | Track max within the level |
| LC 637 Average of Levels | Track sum / count within the level |
| LC 314 Vertical Order | BFS keyed by column index instead of level |

Once the BFS-with-snapshot skeleton is muscle memory, all of these become 2-line tweaks.

### Why this matters for hardware / graphs

BFS with level demarcation is **shortest-path in unweighted graphs** — the level number = distance from source in hops. Used in:
- Image flood-fill / connected components by distance
- Network broadcast (which nodes are reachable in K hops)
- ISP pipeline scheduling (what stages run in parallel at each "depth")
- Game AI pathfinding (BFS for unit moves on a grid)

### Common bugs

- **No null check on root** → enqueueing `nullptr` then `nullptr->val` segfaults
- **Re-evaluating `q.size()` inside the for loop** → queue grows as children enqueue, so you never advance levels (or process wrong counts)
- **`else if` between left and right children** → loses one child when both exist
- **`q.top()` instead of `q.front()`** → `priority_queue`'s API; `queue` uses `front()`
- **Missing per-level vector** → flat list output instead of grouped-by-level
- **Using `stack` instead of `queue`** → DFS order, not BFS

### C++ queue API gotchas

```cpp
std::queue<T> q;
q.push(x);        // enqueue
q.front();        // peek (NOT q.top — that's priority_queue)
q.pop();          // returns void, doesn't return the popped element
q.empty();
q.size();
```

The `pop()` returns void — read `q.front()` before calling `q.pop()`. (Different from `std::priority_queue` which has the same `pop()` quirk but uses `top()` for peek.)

### Mental model

> **"Snapshot the queue size at the start of each level. Process exactly that many pops. Children enqueued during processing become the next level — they don't contaminate the current count."**

### Memorization aid

> **"BFS with snapshot. Outer loop = levels. Inner loop bounded by `q.size()` snapshot. Two `if`s for children, never `if/else`."**

---

## 14. In-Place Merge from the Back — LC 88

### The pattern

When merging two sorted arrays into one **in place** with trailing slack space at the end of the destination, walk both arrays from the **back**, writing the larger element into the back of the destination. This guarantees the write pointer never overwrites unread data.

```cpp
void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
    int i = m - 1;          // last valid in nums1
    int j = n - 1;          // last valid in nums2
    int k = m + n - 1;       // write position

    while (i >= 0 && j >= 0) {
        if (nums1[i] > nums2[j]) {
            nums1[k--] = nums1[i--];
        } else {
            nums1[k--] = nums2[j--];
        }
    }
    // Drain any leftover nums2 (nums1 leftovers are already in place)
    while (j >= 0) {
        nums1[k--] = nums2[j--];
    }
}
```

**O(m + n) time, O(1) space.**

### The key insight: front-merge fails, back-merge succeeds

Front-merge would overwrite `nums1[0]` with `min(nums1[0], nums2[0])` — but you haven't read all of `nums1` yet. You'd clobber unread data.

Back-merge works because **the free space is at the back of `nums1`**. The write pointer `k` always stays at or ahead of the read pointer `i` (in absolute index terms), so writes never trample unread cells.

### Why nums1 leftovers don't need copying

If `nums2` exhausts first (`j < 0`), the remaining elements of `nums1` are already in their final positions. They're below `k`, untouched, and sorted. **Skip the copy.**

If `nums1` exhausts first (`i < 0`), `nums2` still has its smallest elements waiting — those need to be drained into the front of `nums1`. That's the second `while (j >= 0)` loop.

### Where this trick generalizes

| Problem | Application |
|---|---|
| LC 88 | Merge sorted arrays in place |
| Merge step of merge-sort | When destination buffer is the input + scratch |
| `memmove` with forward overlap | Direction is back-to-front to avoid clobbering |
| In-place insertion into sorted array | Shift right side back-to-front to make room |
| Firmware ring-buffer compaction | Move tail elements forward over consumed slots |

The general lemma: **when source and destination overlap, copy in the direction that keeps reads ahead of writes**. For merge-into-tail, that direction is back-to-front.

### Why this matters for hardware / DMA

- **`memmove` correctness**: `memcpy` is undefined for overlapping regions; `memmove` chooses copy direction based on whether `dst > src` (forward overlap → copy backward) or `dst < src` (backward overlap → copy forward). Same principle as LC 88.
- **DMA descriptor coalescing** with overlapping regions follows the same direction logic
- **Image scanline shift** in software pipelines: shifting the entire image left/right uses backward copy when shift > 0

### Common bugs

- **Forgetting nums2 leftover drain** — if `nums2 = [1,2,3]` and `nums1 = [4,5,6,0,0,0]`, the main loop runs 3 times pulling from nums1, leaving nums2 untouched. The drain loop is mandatory.
- **Trying to drain nums1 leftovers** — wasted work, but not incorrect. Cleaner to skip.
- **Off-by-one on indices**: `m-1`, `n-1`, `m+n-1` because they're zero-based indices. Confusing `m` (count) with `m-1` (last index) is the most common slip.
- **Using `<` instead of `>=` in the loop guard** — both pointers must be checked or you'll deref negative indices.

### Mental model

> **"Trailing zeros are scratch space. Walk from the back, write the larger into the back of nums1. Drain nums2 leftovers at the end; nums1 leftovers stay put."**

### Memorization aid

> **"Three pointers from the back: i, j, k. Pick the larger, write to k, advance. Drain nums2 at the end."**

---

## 15. Two-Pass Prefix/Suffix Products — LC 238 (Product of Array Except Self)

### The pattern

For each index `i`, the answer is `(product of everything left of i) × (product of everything right of i)`. Compute both halves in two sweeps, reusing the output array to avoid an extra buffer.

```cpp
vector<int> productExceptSelf(vector<int>& nums) {
    int n = nums.size();
    vector<int> answer(n, 1);

    // Pass 1: answer[i] = product of nums[0..i-1]  (left products)
    for (int i = 1; i < n; i++) {
        answer[i] = answer[i-1] * nums[i-1];
    }

    // Pass 2: multiply in the right products using a scalar
    int right = 1;
    for (int i = n - 1; i >= 0; i--) {
        answer[i] *= right;
        right *= nums[i];
    }
    return answer;
}
```

**O(n) time, O(1) extra space** (output not counted).

### Why this is the canonical "no division" answer

The naive solution is `total_product / nums[i]`, which:
- The problem **forbids division**
- Breaks for arrays containing zero (division by zero, or wrong handling of "zero in product")

The two-pass approach handles zeros correctly without special-casing — a zero in `nums[k]` means every `answer[i ≠ k]` will pick it up via either the left or right sweep, and `answer[k]` will be the product of everything else.

### Order of operations in Pass 2 (load-bearing)

```cpp
answer[i] *= right;     // use current 'right' (product strictly right of i)
right *= nums[i];       // THEN include nums[i] for the next iteration
```

If you reverse the order, `right` would include `nums[i]`, contaminating `answer[i]` with itself.

### Why this matters — the two-sweep DP pattern

"For each index, the answer depends on stuff to the left + stuff to the right" → **two-pass with prefix array, then suffix scalar/array**:

| Problem | Forward sweep | Backward sweep |
|---|---|---|
| LC 238 Product Except Self | left-products | right-products |
| LC 42 Trapping Rain Water | maxLeft per index | maxRight per index, water = min - height |
| LC 84 Largest Rectangle in Histogram | left-bound (smaller bar to left) | right-bound (smaller bar to right) |
| LC 135 Candy | ratings increases left | ratings increases right, take max |
| LC 152 Maximum Product Subarray | max/min product ending here | (variant — single pass with min/max) |

### Why this matters for image processing

The two-sweep pattern is the foundation of **summed-area tables (integral images)**:
- Forward sweep computes prefix sums in 2D
- Any rectangular region's sum is then O(1) via 4 corner lookups
- Used in face detection (Viola-Jones), box filters, fast convolution

Same idea: precompute a left-info array, then resolve each query with a constant-time combination. **Two passes can replace an O(n²) brute force with O(n).**

### Common bugs

- **`answer[0]` initialized to `nums[0]`** instead of 1 — prefix product of empty range is 1
- **Using a second array for right products** — works but wastes O(n) space; the scalar trick is the optimization
- **Wrong order of `answer[i] *= right` vs `right *= nums[i]`** — must use `right` for `answer[i]` BEFORE updating `right`
- **Off-by-one on prefix indexing**: `answer[i] = answer[i-1] * nums[i-1]` (NOT `* nums[i]`) — the left product *excludes* the current index

### Mental model

> **"Two sweeps. Forward sweep fills in 'product of everything before me'. Backward sweep multiplies in 'product of everything after me' using a single scalar. Output array doubles as the prefix-product scratch."**

### Memorization aid

> **"Forward sweep: left product. Backward sweep with a scalar: right product. Order in pass 2: read first, update second."**

---

## 16. Three-Reverse In-Place Rotation — LC 189

### The pattern

Right-rotation by `k` decomposes into three in-place reversals:

```cpp
void rotate(vector<int>& nums, int k) {
    int n = nums.size();
    k %= n;                                      // normalize: k can be > n
    std::reverse(nums.begin(), nums.end());     // 1. reverse all
    std::reverse(nums.begin(), nums.begin() + k);   // 2. reverse first k
    std::reverse(nums.begin() + k, nums.end());     // 3. reverse last n-k
}
```

**O(n) time, O(1) extra space.**

### Why it works (geometric intuition)

Right-rotation by k splits the array into two pieces that swap places:
- Last k elements → must move to the front
- First n-k elements → must move to the back

Reversing the **whole array** flips both pieces' positions (tail-to-front, head-to-back) but also reverses their internal order. The two sub-reverses fix the internal order of each piece.

```
[A_head | B_tail]                              (original)
→ reverse all
[B_tail_reversed | A_head_reversed]
→ reverse first k (= length of B)
[B_tail | A_head_reversed]
→ reverse last n-k (= length of A)
[B_tail | A_head]                              (rotated)
```

This is the **block-swap-via-reversal** identity. Equivalent in any context where two adjacent regions need to swap.

### Why `k %= n` is mandatory

Without it: `k = 10` on length-7 array → trying to reverse `nums.begin() + 10` is out-of-bounds. Single most common LC 189 bug.

`k %= n` collapses any number of full rotations into the equivalent ≤ n shift.

### Comparison of approaches

| Approach | Time | Space | Notes |
|---|---|---|---|
| Extra array, write to `(i+k)%n` | O(n) | **O(n)** | Simplest. Accepted but doesn't earn the senior signal. |
| Rotate-by-1, k times | O(n·k) | O(1) | Too slow for large k. |
| **Three reverses** | **O(n)** | **O(1)** | **Canonical answer.** |
| Cyclic juggling (GCD cycles) | O(n) | O(1) | Same complexity, more bug-prone. Mention as follow-up if asked. |

### The "destination formula" trap

If you derive `new_position = (i + k) % n` and try to do it in-place with `nums[(i+k)%n] = nums[i]`, **you overwrite values you haven't read yet**. Trace `[1,2,3,4,5,6,7], k=3`:
- i=0: `nums[3] = 1` → array is `[1,2,3,1,5,6,7]`. Original `nums[3]=4` is destroyed.

The formula is correct as a *destination map*, but you need either:
- An extra output array (O(n) space), or
- **Cyclic juggling** that walks gcd(n,k) chains, swapping along each cycle

The three-reverse trick sidesteps the entire bookkeeping problem.

### Where this trick generalizes

| Use case | Application |
|---|---|
| Right-rotate array | LC 189 |
| Left-rotate array | Reverse first n-k, reverse last k, reverse all |
| Adjacent block swap | Two regions A and B → reverse A, reverse B, reverse all (yields BA) |
| String rotation | "hello world" ↔ "world hello" via three reverses on chars |
| Circular buffer time-shift | DSP / signal processing |
| Image scanline horizontal pan | Camera ISP with wraparound |
| Memory region rotation | OS / firmware buffer compaction |

The principle: **swapping two adjacent regions in-place = three reverses**. Cheapest in-place block-swap.

### Mental model

> **"Right-rotate-by-k = reverse all, reverse first k, reverse last n-k. The whole-reverse cuts the deck; the two sub-reverses straighten each half."**

### Common bugs

- **Forgetting `k %= n`** — out-of-bounds for `k > n`
- **Off-by-one on the sub-reverse bounds**: must be `[0, k)` and `[k, n)` — half-open intervals matching `std::reverse`'s convention
- **Wrong order**: must be all → first-k → last-(n-k). Different orders produce wrong results.
- **Using the destination-formula in place** without extra storage → overwrites unread data

### Memorization aid

> **"Three reverses, in this order: whole, first k, last n-k. Don't forget `k %= n` at the top."**

---

## 17. Modified Binary Search on Rotated Sorted Array — LC 33

### The pattern

Standard binary search needs a monotonic array. A rotated sorted array isn't monotonic globally, **but at every midpoint, at least one of the two halves is sorted**. Detect which half is sorted, check if target lies in its range, and recurse into the appropriate half.

```cpp
int search(vector<int>& nums, int target) {
    int lo = 0, hi = nums.size() - 1;
    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (nums[mid] == target) return mid;

        if (nums[lo] <= nums[mid]) {
            // Left half is sorted
            if (nums[lo] <= target && target < nums[mid]) {
                hi = mid - 1;
            } else {
                lo = mid + 1;
            }
        } else {
            // Right half is sorted
            if (nums[mid] < target && target <= nums[hi]) {
                lo = mid + 1;
            } else {
                hi = mid - 1;
            }
        }
    }
    return -1;
}
```

**O(log n) time, O(1) space.**

### The key insight

After rotation, `nums` looks like two sorted runs joined at a pivot:
```
[4, 5, 6, 7 | 0, 1, 2]      pivot between idx 3 and 4
```

For any midpoint `mid`, the pivot is either in `[lo..mid)` or `(mid..hi]` — not both. **The half without the pivot is sorted.**

To detect which half is sorted: compare `nums[lo]` to `nums[mid]`:
- `nums[lo] <= nums[mid]` → no pivot crossed in left half → **left is sorted**
- otherwise → pivot is in left half → **right is sorted**

### The bracket asymmetry in range checks

Once a half is known sorted, decide if `target` is in it using its endpoints:
```
left sorted:  nums[lo] <= target < nums[mid]
right sorted: nums[mid] < target <= nums[hi]
```

The asymmetric `<=`/`<` is deliberate:
- `target == nums[mid]` is already handled at the top (returns mid)
- `target == nums[lo]` or `target == nums[hi]` are valid hits — must include them with `<=`

### Why O(log n)

Each iteration eliminates half the range. The two-step decision rule (which half is sorted, is target in it) is constant-time, so the recurrence is `T(n) = T(n/2) + O(1)` → O(log n).

### When binary search generalizes beyond "monotonic array"

The unlock is: **binary search works whenever a constant-time check at any midpoint can eliminate half the search space**, even if the array structure isn't strictly monotonic.

| Variant | Constant-time check |
|---|---|
| LC 33 Search Rotated Sorted | "Which half is sorted?" |
| LC 153 Find Min in Rotated Sorted | "Which half contains the pivot?" |
| LC 162 Find Peak Element | "Is mid going up or down?" |
| LC 4 Median of Two Sorted Arrays | "Are the partitions valid?" |
| LC 875 Koko Eating Bananas | "Can she finish at speed mid?" |
| LC 1011 Capacity to Ship Packages | "Is capacity mid sufficient?" |

The last two are **answer-space binary search**: search over the answer range, not the input. Same machinery, different domain.

### Mental model

> **"Two sorted runs glued at a pivot. At every step, one half is sorted — find it, check if target lies in it, search there or the other half."**

### Common bugs

- **`<` vs `<=` on `nums[lo] <= nums[mid]`**: must use `<=` to handle `lo == mid` (single-element range). With strict `<`, the algorithm fails when the range collapses.
- **`while (lo < hi)` instead of `<=`**: single-element ranges miss the check, returning `-1` for valid targets.
- **`mid = (lo + hi) / 2`**: integer overflow risk for large arrays. Always use `lo + (hi - lo) / 2`.
- **Forgetting `nums[mid] == target` check at the top**: leads to infinite loops when `mid` doesn't move.
- **Off-by-one on lo/hi updates**: must be `mid - 1` and `mid + 1`, never `mid`.
- **Wrong inequality direction in the in-range check**: confusing `<=` and `<` flips inclusivity at the wrong endpoint.

### What changes if duplicates are allowed (LC 81)

If duplicates exist, the check `nums[lo] <= nums[mid]` becomes ambiguous when `nums[lo] == nums[mid]` (e.g., `[2,2,2,3,4,2]`) — neither half is provably sorted. The fix: when `nums[lo] == nums[mid]`, advance `lo++` and retry. **Worst-case O(n)** for all-duplicate arrays.

LC 33 has unique values → the `<=` check is always informative.

### Why this matters for hardware

- **Memory-mapped register search** when address space has been remapped or rotated (boot ROM trampolines)
- **Cyclic ring-buffer search** when reads can start mid-cycle
- **Time-series search in cyclic logs**: find an entry by timestamp in a wrap-around buffer
- **Pivot detection** in calibration data that's been rotated by sensor mounting orientation

### Memorization aid

> **"At every mid, one half is sorted. Detect via `nums[lo] <= nums[mid]`. Check if target is in the sorted half's range with asymmetric brackets. Search there or the other half. O(log n)."**

