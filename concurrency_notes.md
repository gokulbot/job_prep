# Concurrency Notes — Qualcomm Prep

C++ multi-threading patterns, pitfalls, and idioms. Companion to `cpp_pointer_notes.md` (language depth) and `dsa_patterns_notes.md` (algorithms).

---

## 1. `std::lock_guard` vs Manual `mutex.lock()` / `unlock()`

### TL;DR

> **Always use `std::lock_guard` (or `std::scoped_lock`/`std::unique_lock`) instead of manual `m.lock()` / `m.unlock()` calls.**

**Why: it follows RAII.** Manual locking is a bug factory; RAII-based lock guards are exception-safe, scoped, and impossible to mismatch.

### What RAII means (the foundational principle)

**RAII = Resource Acquisition Is Initialization.**

The C++ idiom: **tie resource lifetimes to object lifetimes**. The constructor acquires the resource; the destructor releases it. Destructors **always fire** when an object goes out of scope — including during exception unwinding, early returns, or any other exit path.

Applied to mutexes:
- **Resource** = the locked mutex
- **Acquire** = `lock_guard` constructor calls `m.lock()`
- **Release** = `lock_guard` destructor calls `m.unlock()`
- **Guarantee** = destructor fires no matter how the scope ends → mutex always released

This is why you **never have to write `unlock()`** in RAII style. The compiler enforces it via the destructor.

### Other RAII examples in standard C++

| Resource | RAII wrapper |
|---|---|
| Heap memory | `std::unique_ptr`, `std::shared_ptr` |
| File handles | `std::ifstream`, `std::ofstream` |
| Mutex locks | `std::lock_guard`, `std::unique_lock`, `std::scoped_lock` |
| Threads | `std::thread` (must join/detach before destruction) |
| Network sockets | (your own wrapper, or libraries like Boost.Asio) |
| Vector / containers | Manage their own storage in destructors |

**Pattern**: `T resource(args); /* use it */ } // resource auto-cleaned`

The opposite of RAII is **manual resource management** — `malloc`/`free`, `lock()`/`unlock()`, `fopen`/`fclose`. These are bug-prone because every exit path must explicitly clean up. Forget once → leak / deadlock / file handle exhausted.

### The two styles side-by-side

```cpp
// MANUAL — DON'T DO THIS
void increment() {
    m_.lock();
    count_++;
    m_.unlock();
}

// RAII — DO THIS
void increment() {
    std::lock_guard<std::mutex> lock(m_);
    count_++;
}   // lock auto-released when `lock` goes out of scope
```

### Why RAII wins — five concrete failure modes of manual locking

#### 1. Exception safety

```cpp
void manual_buggy() {
    m_.lock();
    do_thing_that_might_throw();   // throws → unlock NEVER called → DEADLOCK on next acquire
    m_.unlock();
}
```

vs

```cpp
void raii_safe() {
    std::lock_guard<std::mutex> lock(m_);
    do_thing_that_might_throw();   // throws → stack unwinds → ~lock_guard() runs → mutex released
}
```

`lock_guard`'s destructor runs **even during exception propagation**, releasing the mutex. With manual `unlock()`, the call is skipped → mutex stays locked → next thread to call `lock()` deadlocks forever.

#### 2. Early returns

```cpp
int manual_buggy(int x) {
    m_.lock();
    if (x < 0) return -1;          // forgot to unlock → DEADLOCK
    int result = compute(x);
    m_.unlock();
    return result;
}
```

vs

```cpp
int raii_safe(int x) {
    std::lock_guard<std::mutex> lock(m_);
    if (x < 0) return -1;          // ~lock_guard() runs → mutex released
    return compute(x);
}
```

Every `return` path automatically destroys the lock_guard. Manual code requires you to **remember to unlock at every exit point** — error-prone.

#### 3. Multiple unlocks / forgotten unlocks

```cpp
void manual_buggy() {
    m_.lock();
    if (cond_a) {
        m_.unlock();
        return;
    }
    if (cond_b) {
        return;                    // BUG: forgot unlock
    }
    m_.unlock();                   // only fires if neither branch taken
}
```

`lock_guard` makes this impossible — there's no `unlock()` to forget; lifetime ends at scope close.

#### 4. Scope confusion

```cpp
void manual_buggy() {
    m_.lock();
    {
        int x = read_shared();
    }                              // intent: release lock here?
    // ... but lock is still held
    m_.unlock();
}
```

vs

```cpp
void raii_clear() {
    int x;
    {
        std::lock_guard<std::mutex> lock(m_);
        x = read_shared();
    }                              // lock released here, exactly
    expensive_unlocked_work(x);
}
```

The lock_guard's lifetime **is** the critical section. Visible at a glance.

#### 5. Mismatched lock/unlock pairs

```cpp
void manual_buggy() {
    m1_.lock();
    m2_.lock();
    // ...
    m1_.unlock();                  // BUG: should release m2 first (or use std::lock for deadlock-free)
    m2_.unlock();
}
```

With `std::scoped_lock` (C++17, the multi-mutex sibling of lock_guard):

```cpp
void raii_two_mutexes() {
    std::scoped_lock lock(m1_, m2_);   // deadlock-free acquisition + ordered release
    // ...
}                                       // both released at scope exit, in reverse order
```

`std::scoped_lock` uses an internal deadlock-avoidance algorithm. Manual ordering is your responsibility.

### What about `std::unique_lock`?

`std::unique_lock` is the more flexible cousin. Use it when you need:
- **Defer locking**: `std::unique_lock<std::mutex> lock(m_, std::defer_lock); ... lock.lock();`
- **Try-lock**: `lock.try_lock_for(std::chrono::ms(100))`
- **Manual unlock mid-scope**: `lock.unlock(); ... lock.lock();`
- **Use with condition variables**: `cv.wait(lock, predicate)` requires a `unique_lock`

Otherwise, prefer `lock_guard` (smaller, no overhead from extra state).

| Tool | Use when |
|---|---|
| `std::lock_guard<M>` | Default. Acquire on construction, release on destruction. |
| `std::scoped_lock<Ms...>` (C++17) | Multiple mutexes simultaneously. Deadlock-free. |
| `std::unique_lock<M>` | Need to unlock/relock mid-scope, or use with `condition_variable`. |
| Manual `m.lock()`/`unlock()` | **Almost never.** Only if implementing a custom RAII wrapper. |

### Performance

`lock_guard` has **zero runtime overhead** vs manual lock/unlock — it's a header-only class with a constructor and destructor that just call `m.lock()` / `m.unlock()`. The compiler inlines it away.

You're only paying for what `lock()`/`unlock()` themselves cost (typically tens of nanoseconds for an uncontended mutex). The RAII wrapping is free.

### Common bugs from this session

- **Missing variable name**: `std::lock_guard<std::mutex>(m_);` constructs a temporary that destructs immediately at the `;`, releasing the lock before the protected code runs. **Same as no lock at all.** Always name it: `std::lock_guard<std::mutex> lock(m_);`
- **Typo**: `lock_gurad` → `lock_guard` (it's g-u-a-r-d).
- **Protecting only readers, not writers**: `get()` had the lock, but `increment()` (the actual race) didn't. **Lock the writer; lock the reader for memory ordering.**

### Memorization aid

> **"Construct in scope, destruct at brace. The lock lives exactly as long as the object."**

### Interview gold framing

> *"In production C++ I always use `std::lock_guard` or `std::scoped_lock` over manual `lock()`/`unlock()` calls. RAII gives exception safety, prevents missed unlocks on early returns, and makes the critical section visible at a glance. Manual locking is a bug factory — every exit path needs an unlock, and any throw between acquire and release deadlocks the next thread."*

That's a senior-engineer answer to "how do you protect shared state in multithreaded C++?"

---

## 2. `std::atomic<T>` vs `std::mutex` — When to Use Which

### The deeper "why" — hardware vs software synchronization

Both make code thread-safe, but **the safety lives in completely different places**:

| Mechanism | Where safety lives | Cost (uncontended) | Cost (contended) |
|---|---|---|---|
| `std::mutex` | **Software/OS-level** lock (futex on Linux, kernel mutex on Windows) | ~20-30 ns | Hundreds of ns (kernel syscall, scheduling) |
| `std::atomic<int>` | **Hardware-level** atomic CPU instruction | ~5-10 ns | Tens of ns (CAS retry in user space) |

For a counter incremented millions of times per second, **atomic is 3-10× faster** under contention.

### What the compiler actually emits

```cpp
std::atomic<int> count_{0};
count_++;
```

Compiles to (one instruction):

| Platform | Instruction |
|---|---|
| x86-64 | `LOCK XADD [count_], 1` |
| ARM64 | `LDADDAL` or `LDXR/STXR` loop |

vs

```cpp
std::mutex m_;
int count_;
{ std::lock_guard<std::mutex> lock(m_); count_++; }
```

Compiles to ~50 instructions — set lock owner via CAS, increment, release lock, possibly futex syscall + scheduling on contention.

**The atomic operation is already atomic at the hardware level.** No software locks, no kernel involvement, no blocking — you get correctness "for free" (single CPU instruction), not "for cheap" (mutex acquisition path).

### Why atomics beat mutex for single-variable updates

#### 1. Performance (covered above)
Single CPU instruction vs mutex acquire/release path.

#### 2. Lock-free → no deadlock, no priority inversion

A thread doing an atomic op **never blocks another thread**. With mutex:
- Thread A holds lock, gets preempted → thread B waits → wasted cycles
- Two locks in different orders → deadlock
- Low-priority thread holds lock → high-priority thread can't run (priority inversion)

Atomics sidestep all of this. Either succeed or retry — no "waiting on another thread."

#### 3. Simpler code

```cpp
class CounterAtomic {
public:
    void increment() { count_++; }
    int get() { return count_.load(); }   // implicit conversion also works
private:
    std::atomic<int> count_{0};
};
```

No mutex member, no lock_guard, no headers (`<atomic>` only). The variable IS the protection.

#### 4. Memory ordering control (advanced)

`std::atomic` lets you choose **how strict** the synchronization is:

```cpp
count_.fetch_add(1, std::memory_order_relaxed);    // weakest, fastest (just atomicity, no ordering)
count_.fetch_add(1, std::memory_order_acq_rel);    // medium (acquire + release semantics)
count_.fetch_add(1, std::memory_order_seq_cst);    // strongest (default, sequential consistency)
```

For pure counters where ordering across other ops doesn't matter, `relaxed` is fastest. Mutex always gives you full sequential consistency — no knob.

### When mutex IS better than atomic

Atomics aren't a silver bullet.

#### 1. Multi-variable invariants

```cpp
// CAN'T be done atomically — two separate fields must update together
void deposit(double amount) {
    balance_ += amount;
    last_update_ = std::chrono::system_clock::now();
}
```

`std::atomic` only protects **one variable at a time**. To keep `balance_` and `last_update_` consistent (a reader sees both updated together), you need a mutex.

#### 2. Large critical sections

```cpp
void process() {
    std::lock_guard<std::mutex> lock(m_);
    // 50 lines of code that need to be atomic w.r.t. other threads
}
```

A long CAS-retry loop on contended atomics burns CPU. Mutex's blocking wait is more efficient when the critical section is large.

#### 3. Wait conditions (condition_variable)

```cpp
cv_.wait(lock, [&]{ return !queue_.empty(); });
```

`condition_variable` requires `unique_lock<mutex>`. Atomics can't wait — they can only retry.

#### 4. Non-trivially-copyable types

`std::atomic<T>` only works for types that are trivially copyable (typically integers, pointers, small POD structs). For `std::string`, `std::vector`, `std::map`, etc. → must use mutex.

### Decision matrix

| Scenario | Use |
|---|---|
| Single counter / flag / pointer | `std::atomic<T>` ✓ |
| Lock-free queue heads / tails | `std::atomic<T*>` ✓ |
| Multi-field updates (need consistency) | `std::mutex` |
| Wait-for-condition (queue empty, event ready) | `std::mutex` + `std::condition_variable` |
| Large critical section | `std::mutex` |
| Read-heavy, write-rare | `std::shared_mutex` (reader/writer lock) |
| Status flag (`stop_requested`, `ready`) | `std::atomic<bool>` |

### Real-world Qualcomm relevance

In camera / HW model code, you'll see atomics in:

- **Frame counters** (`std::atomic<uint64_t>`) — incremented from ISR or driver thread
- **Reference counts** (`std::atomic<int>`) — for shared image / tensor buffers
- **Lock-free SPSC ring buffers** — atomic head/tail indices for queues to/from DSP / Hexagon
- **Status flags** (`std::atomic<bool>`) — "shutdown requested," "frame ready," "device idle"
- **Lazy initialization** (`std::atomic<T*>` + double-checked locking)

You'd use mutex when:
- Updating **multiple buffer fields** atomically (e.g., `width`, `height`, `stride` together when a frame arrives)
- Threads need to **wait** for events (e.g., consumer waits for next frame)
- Protecting STL containers (`std::vector`, `std::queue`, `std::map`)

### Performance reality check

For a single-variable counter incremented in a hot loop:

```
1M increments, 4 threads, modern x86:

   plain int:          BROKEN (race condition, undefined output)
   atomic<int>++:      ~80 ms   (hardware LOCK XADD, contended)
   mutex+lock_guard:   ~250 ms  (3× slower due to lock acquire/release)
```

For a multi-field critical section:

```
Atomic with multiple atomics: BROKEN (no consistency guarantee across them)
Single mutex over the section: CORRECT, ~modest overhead
```

Use the right tool for the shape of your protection.

### Common bugs

- **Reaching for atomic when you have multi-field invariants** → reads can see partially-updated state
- **Using mutex for a single counter** → 3-10× slower than necessary
- **Forgetting that `atomic<T>` reads need `.load()` semantically** (implicit conversion works but is invisible at call site)
- **Wrong memory ordering** → `relaxed` when you needed `acquire/release` → reordering across the atomic breaks invariants

### Memorization aid

> **"One variable, simple op → atomic. Multiple variables, complex section, or wait → mutex."**

### Interview gold framing

> *"For single-variable updates I prefer `std::atomic<T>` because it pushes thread safety to the CPU instruction level — `LOCK XADD` on x86, `LDADDAL` on ARM. No kernel involvement, no blocking, much faster than mutex acquire/release for hot counters. I'd reach for `std::mutex` when I need to update multiple fields together (single-variable atomicity isn't enough for multi-field invariants), or when I need to wait on a condition_variable. So: simple ops → atomic; multi-field consistency or wait → mutex."*

That's a senior-engineer answer that demonstrates hardware-level understanding.

---

## 3. `unique_lock` vs `lock_guard` — When and Why

### The single technical reason `unique_lock` exists

`cv.wait(lock, predicate)` must internally **unlock the mutex while sleeping** and **re-lock on wake**. `lock_guard` doesn't expose `lock()`/`unlock()` methods. `unique_lock` does. **That's the only reason.**

### What `cv.wait()` actually does internally

```cpp
// Pseudo-implementation of cv.wait
while (!predicate()) {
    lock.unlock();        // release the mutex so others can mutate state
    /* sleep on this CV until notified */
    lock.lock();          // re-acquire the mutex on wakeup
}
// Now: predicate is true AND mutex is held
```

Two critical operations: **`lock.unlock()` and `lock.lock()`**. Only `unique_lock` has these methods.

### Interface comparison

```cpp
// lock_guard — minimal, lock-on-construction only
class lock_guard {
    lock_guard(mutex&);   // constructor: locks
    ~lock_guard();        // destructor: unlocks
    // NO unlock() / lock() — can't be manipulated mid-scope
};

// unique_lock — full lock manipulation
class unique_lock {
    unique_lock(mutex&);
    ~unique_lock();
    void lock();
    void unlock();
    bool try_lock();
    template<class Rep, class Period>
    bool try_lock_for(...);
    bool owns_lock();
    void release();
    // movable (transfer lock ownership)
    // can be constructed with defer_lock / try_to_lock / adopt_lock
};
```

### Why not just use `unique_lock` everywhere

You could — it works correctly. But:

| Property | `lock_guard` | `unique_lock` |
|---|---|---|
| Footprint | 1 pointer | 1 pointer + 1 bool (state tracking) |
| Constructor cost | Just `mutex.lock()` | Same + state init |
| Functionality | Minimal | Full lock manipulation |
| Reader sees | "Lock for whole scope, no funny business" | "Lock might be released/moved/used with CV" |

For 99% of cases (lock the whole function, no manipulation), `lock_guard` is slightly cheaper and **the intent is clearer at the type level**. Pay the cost only when you need flexibility.

### Decision matrix

| Want | Use |
|---|---|
| Lock for the whole scope, no manipulation | `std::lock_guard` (default) |
| Lock unlocked/relocked mid-scope (CV wait, deferred lock) | `std::unique_lock` |
| Multiple mutexes at once, deadlock-free | `std::scoped_lock` (C++17) |
| Reader/writer pattern (many readers, one writer) | `std::shared_mutex` + `shared_lock`/`unique_lock` |

### Memorization aid

> **"`cv.wait()` needs to unlock and relock; `lock_guard` can't, `unique_lock` can. Use `lock_guard` everywhere except CV-wait paths."**

### Common bug from this session

- **Using `lock_guard` with `cv.wait()`** → compile error. The CV's `wait` overload requires `unique_lock<mutex>&` (or `unique_lock<basic_mutex>&`). Won't accept `lock_guard`.

---

## 4. Condition Variables — `cv.wait()` Semantics

### The pattern

A **condition variable** lets a thread sleep until another thread signals a state change. The OS scheduler removes the waiting thread from the run queue → zero CPU usage while waiting.

```cpp
std::mutex m;
std::condition_variable cv;
std::queue<int> q;

// Consumer thread
void consumer() {
    std::unique_lock<std::mutex> lock(m);
    cv.wait(lock, [&]{ return !q.empty(); });   // sleep until queue has items
    int x = q.front();
    q.pop();
    // ... process x ...
}

// Producer thread
void producer(int item) {
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(item);
    }
    cv.notify_one();   // wake one waiting consumer
}
```

### The predicate is "GO-AHEAD condition", not "STOP condition"

`cv.wait(lock, pred)` is equivalent to:

```cpp
while (!pred()) {
    cv.wait(lock);
}
```

It **sleeps WHILE predicate is FALSE**, **exits when predicate becomes TRUE**.

> **The predicate answers: "When may I proceed?" — not "When should I wait?"**

| Waiting for... | Predicate is... |
|---|---|
| "queue has items" | `!q.empty()` ✓ |
| "queue has space" | `q.size() < capacity` ✓ |
| "shutdown not requested" | `!shutdown_` ✓ |

This is the **#1 confusion point** for new C++ multithreading code. Predicates are inverted constantly.

### Why the predicate form (not bare `cv.wait(lock)`)

Three things the predicate handles for you:

1. **Spurious wakeups**: CVs can wake threads even without `notify_*` (OS quirk on some platforms). Without checking the predicate, you'd proceed with the predicate still false → bug. The `while (!pred())` re-checks after every wake.

2. **Lost wakeups**: if the predicate became true *before* you started waiting, `notify_*` was already called and missed. The predicate check at entry ensures you don't sleep when you shouldn't.

3. **Thundering herd / contention**: if `notify_all` wakes 10 threads but only 1 can proceed, the other 9 re-check the predicate and go back to sleep cleanly — no corrupted state.

### `notify_one()` vs `notify_all()`

| | When to use |
|---|---|
| `notify_one()` | One waiter can proceed (e.g., one item added → wake one consumer) |
| `notify_all()` | Multiple waiters might proceed (e.g., shutdown signal — wake everyone) |

For producer-consumer, `notify_one()` is correct — one push wakes one consumer; one pop wakes one producer.

### The lock release timing for `notify`

```cpp
{
    std::lock_guard<std::mutex> lock(m);
    q.push(item);
}                       // ← lock released here
cv.notify_one();        // notify AFTER releasing the lock
```

Notifying **after** unlocking is preferred:
- Wake-up'd thread immediately gets the lock (vs trying, failing, sleeping again)
- Slightly less contention

But notifying while locked also works correctly — just slightly slower. Don't lose sleep over it.

### Common bugs

- **Predicate inverted** → either deadlock (waits forever) or proceeds before condition is true (crash)
- **Using `lock_guard` instead of `unique_lock`** → compile error
- **Wrong CV notified** in producer-consumer (push notifies producers instead of consumers, etc.)
- **Forgetting to hold the lock when modifying shared state** before notify → race condition

### Memorization aid

> **"Predicate = go-ahead. wait sleeps until it's true. notify after unlock."**

---

## 5. Producer-Consumer / Bounded Buffer Pattern

### Canonical implementation

```cpp
class BoundedBuffer {
public:
    BoundedBuffer(size_t capacity) : capacity_(capacity) {}

    void push(int item) {
        std::unique_lock<std::mutex> lock(m_);
        not_full_.wait(lock, [&]{ return q_.size() < capacity_; });
        q_.push(item);
        not_empty_.notify_one();   // wake a waiting consumer
    }

    int pop() {
        std::unique_lock<std::mutex> lock(m_);
        not_empty_.wait(lock, [&]{ return !q_.empty(); });
        int x = q_.front();
        q_.pop();
        not_full_.notify_one();    // wake a waiting producer
        return x;
    }

private:
    std::queue<int> q_;
    size_t capacity_;
    std::mutex m_;
    std::condition_variable not_full_;
    std::condition_variable not_empty_;
};
```

### Why TWO condition variables (not one)

| With ONE CV (`cv`) | With TWO CVs (`not_full_`, `not_empty_`) |
|---|---|
| `notify_all` wakes everyone | `notify_one` wakes only producers OR only consumers |
| Wrong-side waiters re-check predicate, go back to sleep | Right side is woken, no wasted wakeups |
| "Thundering herd" — burst of CPU on each notify | Cleaner, no false wakes |
| Easier to write but slower under high contention | Slightly more code, much better behavior |

**Rule**: one CV per "kind of waiting." Producers wait on `not_full_`; consumers wait on `not_empty_`. Push notifies the consumer side; pop notifies the producer side.

### Action → notification mapping

| Action | Effect on queue | Wake which CV |
|---|---|---|
| **push** (added item) | Queue has more items | `not_empty_` (consumers care) |
| **pop** (removed item) | Queue has more space | `not_full_` (producers care) |

**The CV you notify is for the OTHER side.** push doesn't notify producers — it notifies consumers.

### Why predicate (not bare wait) is critical here

Producer-consumer has **two waiters per side**. Without the predicate:
- Producer wakes from `not_full_`, but another producer was faster and queue is full again → must re-wait
- Predicate auto-handles this re-check; bare `wait()` would proceed to push and overflow

Always use the predicate form.

### Why predicate uses `< capacity_` (strict less-than)

```cpp
not_full_.wait(lock, [&]{ return q_.size() < capacity_; });
```

If you wrote `q_.size() <= capacity_`, the predicate is true even when full → push proceeds → buffer overflow. **Off-by-one matters here.**

Same rule for `!q_.empty()` (vs e.g., wrong `q_.size() >= 0` which is always true).

### Multi-producer / multi-consumer scaling

This pattern works for N producers and M consumers without modification:
- Each producer's push competes for the mutex, then waits on `not_full_` if buffer full
- Each consumer's pop competes for the mutex, then waits on `not_empty_` if buffer empty
- `notify_one()` wakes one of the waiters, which proceeds
- The mutex serializes the actual push/pop step

### Real-world Qualcomm relevance

Producer-consumer queues are everywhere in HW-adjacent code:

| Use case | Pattern |
|---|---|
| **Camera frames**: ISP produces frames → consumer threads process | Bounded buffer (drop frames if consumer slow) |
| **DSP-to-CPU messages**: Hexagon writes results → CPU reads | Bounded buffer (lock-free SPSC ring is even faster) |
| **Worker pools**: producer queues tasks → worker threads pop | Bounded buffer with notify_all on shutdown |
| **Logging**: app threads log → background writer drains | Bounded buffer (drop logs if full) |

For single-producer/single-consumer ultra-fast paths, **lock-free SPSC ring buffer** with atomic head/tail beats this pattern by 10-100× — but is much harder to write correctly. Mutex+CV is the safe default.

### Common bugs from this session

- **Predicate inverted** (`q.size() == capacity` instead of `< capacity`) → push waits while there's space
- **Wrong CV notified** (push notifies `not_full_` instead of `not_empty_`) → consumers never wake
- **`lock_guard` instead of `unique_lock`** → compile error
- **Trailing-underscore omitted** on member access (`q.size()` instead of `q_.size()`) → compile error

### Memorization aid

> **"Wait until I can proceed. Notify the OTHER side after I act. Two CVs, two predicates, one mutex."**

### Interview gold framing

> *"For producer-consumer I use a bounded buffer with two condition variables — one for 'not full' (producers wait), one for 'not empty' (consumers wait). Predicate form of `cv.wait` handles spurious and lost wakeups. Push notifies the not_empty CV (waking a consumer); pop notifies not_full (waking a producer). For ultra-low-latency single-producer-single-consumer paths I'd reach for a lock-free SPSC ring buffer with atomic head/tail, but for general multi-producer-multi-consumer this mutex+CV pattern is the safe default."*

---
