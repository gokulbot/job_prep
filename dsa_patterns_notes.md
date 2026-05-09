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

