# DSA Concepts

## Backtracking

### Classic Permutation Template

```
void f(n, nums, result, current, chosen):
    if current.size() == n:
        result.append(current)
        return
    
    for i in 0..n:
        if not chosen[i]:
            chosen[i] = true
            current.push(nums[i])
            
            f(n, nums, result, current, chosen)
            
            current.pop()
            chosen[i] = false
```

- `chosen[]` tracks which elements are already in `current`
- push before recurse, pop + unmark after — this is the backtrack step
- base case: when `current` has all `n` elements, save it

---

### Classic Combination Template

```
void f(start, k, nums, result, current):
    if current.size() == k:
        result.append(current)
        return
    
    for i in start..n:
        current.push(nums[i])
        
        f(i + 1, k, nums, result, current)
        
        current.pop()
```

- **No `chosen` array** — use `start` index instead
- Loop starts at `start`, not `0` — avoids reusing earlier elements
- Pass `i + 1` to move forward (each element used at most once)

---

## Two Pointers

### 3Sum (LC 15)

- Sort first — enables two pointers and adjacent duplicate detection
- Outer loop fixes `nums[i]`; inner two-pointer finds pairs summing to `-nums[i]`
- Skip duplicate `nums[i]`: `if (i > 0 && nums[i] == nums[i-1]) continue`
- After a match, move both pointers then skip duplicates on each side:
```cpp
left++; right--;
while (left < right && nums[left] == nums[left - 1]) left++;
while (left < right && nums[right] == nums[right + 1]) right--;
```
- **Time:** O(n²) — O(n log n) sort + O(n²) two-pointer scan
- **Space:** O(1)

---

## Heap

### What is a Heap?
- A **complete binary tree** stored as an array
- **Min-heap rule:** parent is always smaller than its children
- **Max-heap rule:** parent is always larger than its children
- No ordering between siblings — only the parent-child relationship matters

```
        1          ← smallest always at top (min-heap)
       / \
      4    3
    /  \
   5    10
```

Array representation: `[1, 4, 3, 5, 10]`
- Left child of index `i` → `2i + 1`
- Right child of index `i` → `2i + 2`
- Parent of index `i` → `(i-1) / 2`

---

### Time Complexities

| Operation | Time | Why |
|-----------|------|-----|
| Insert | O(log n) | bubble up from leaf to root = log n levels |
| Delete (extract min/max) | O(log n) | sift down from root to leaf = log n levels |
| Peek (min/max) | O(1) | always at index 0 |
| Build heap | O(n) | see below |

---

### Why Build Heap is O(n) — not O(n log n)

**Key insight:** Most nodes are near the bottom and do very little work.

Each node sifts **down** — it can only travel as far as its height allows.

| Level | # Nodes | Max swaps |
|-------|---------|-----------|
| Leaves (h=0) | n/2 | 0 |
| h=1 | n/4 | 1 |
| h=2 | n/8 | 2 |
| Root | 1 | log n |

Total work:
```
= n/2×0 + n/4×1 + n/8×2 + n/16×3 + ...
= n × S,  where S = 1/4 + 2/8 + 3/16 + ...
```

**Trick to solve S (shift and subtract):**
```
S   = 1/4 + 2/8 + 3/16 + ...
S/2 =       1/8 + 2/16 + ...
─────────────────────────────
S/2 = 1/4 + 1/8 + 1/16 + ... = 1/2

→ S = 1/2
```

Total work = `n × 1/2` = **O(n)**

**Why not O(n log n)?**
- A sorted array requires strict ordering of every element → O(n log n)
- A heap only requires *"be smaller than your kids"* → much more relaxed → O(n)
- Half the nodes are leaves doing **zero work** — that's what makes it linear

---

## Bit Manipulation

### Reverse Bits (LC 190)

**Core trick — shift left and append bit:**
```cpp
result = (result << 1) | (n & 1);
n >>= 1;
```

- `result << 1` → shift left, making room on the right
- `n & 1`       → grab the last bit of n (LSB)
- `|`           → drop it into result's empty slot
- `n >>= 1`     → remove that bit from n, move to next

**Visually:**
```
result = 0 1 1 0 _     ← shift left creates empty slot
n & 1  =         1     ← grab bit
result = 0 1 1 0 1     ← OR drops it in
```

Repeat 32 times — streaming bits from n into result one at a time.

**Why it works:** You're reading n LSB→MSB while writing into result LSB→MSB, which naturally reverses the bit order.

**Time:** O(32) = O(1) — always exactly 32 iterations.

---

## C++ Gotchas

### Single vs Double Quotes
```cpp
char c = 'a';

c == "("   // WRONG — "(" is a string literal (const char*), comparing pointer to char
c == '('   // CORRECT — '(' is a char literal
```
- **Single quotes** `' '` → `char`
- **Double quotes** `" "` → `const char*` (string literal)


---

### String Character Checks (`#include <cctype>`)

| Function | Matches | Example |
|----------|---------|---------|
| `isalpha(c)` | letters only (a-z, A-Z) | `'a'` ✓, `'3'` ✗ |
| `isdigit(c)` | digits only (0-9) | `'3'` ✓, `'a'` ✗ |
| `isalnum(c)` | letters **or** digits | `'a'`, `'3'` ✓, `' '` ✗ |
| `isspace(c)` | whitespace | `' '`, `'\n'` ✓ |
| `tolower(c)` | lowercase version | `'A'` → `'a'` |
| `toupper(c)` | uppercase version | `'a'` → `'A'` |

**Header:** `#include <cctype>`

**LC 125 gotcha:** use `isalnum`, not `isalpha` — digits count as valid chars.

**`toupper`/`tolower` return `int`, not `char`** — cast explicitly when building strings:
```cpp
temp + (char)toupper(s[i])   // correct
temp + toupper(s[i])         // works but implicit — be explicit
```

**Out-of-bounds guard** when skipping non-alnum in two-pointer:
```cpp
while (start <= end && !isalnum(s[start])) start++;
while (start <= end && !isalnum(s[end]))   end--;
```
Without the `start <= end` check, an all-symbol string like `" "` will blow past the array bounds.

---

### Common Bit Tricks

| Operation | Code | What it does |
|-----------|------|--------------|
| Get last bit | `n & 1` | 0 if even, 1 if odd |
| Remove last bit | `n >>= 1` | divide by 2 |
| Make room on right | `result << 1` | shift left, LSB becomes 0 |
| Count set bits | `n &= (n-1)` | clears lowest set bit |
