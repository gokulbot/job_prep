# C++ Concepts

---

## Rule of Three (Raw Pointer Members)

If your class owns a raw pointer, you **must** define all three:

| Method | Why |
|--------|-----|
| Destructor | `delete` the pointer — prevents memory leak |
| Copy constructor | Allocate new memory and copy the value — prevents double-free |
| Copy assignment | Delete old, allocate new, copy value — prevents leak + double-free |

```cpp
class Test {
public:
    Test() = default;

    // Copy constructor
    Test(const Test& other) {
        if (other.x_ != nullptr) {
            x_ = new int;
            *x_ = *(other.x_);
        } else {
            x_ = nullptr;
        }
    }

    // Copy assignment
    Test& operator=(const Test& other) {
        if (this == &other) return *this;   // self-assignment guard
        delete x_;                           // free old memory first
        if (other.x_ != nullptr) {
            x_ = new int;
            *x_ = *(other.x_);
        } else {
            x_ = nullptr;
        }
        return *this;
    }

    // Destructor
    ~Test() {
        delete x_;   // delete nullptr is safe (no-op)
    }

private:
    int* x_ = nullptr;
};
```

**Key gotchas:**
- Always `delete x_` before reassigning in copy assignment — otherwise you leak the old memory
- Always check `this == &other` in copy assignment — `a = a` would delete then read freed memory
- `delete nullptr` is safe — no need to null-check before deleting

---

## Rule of Three with `unique_ptr`

Replacing a raw pointer with `unique_ptr` eliminates the destructor — cleanup is automatic. But copy constructor and copy assignment still need manual definitions since `unique_ptr` is **not copyable**.

```cpp
class Test {
public:
    Test() = default;

    // Copy constructor — allocate new and copy value
    Test(const Test& other) {
        if (other.x_ != nullptr)
            x_ = std::make_unique<int>(*other.x_);
        else
            x_ = nullptr;
    }

    // Copy assignment
    Test& operator=(const Test& other) {
        if (this == &other) return *this;
        if (other.x_ != nullptr)
            x_ = std::make_unique<int>(*other.x_);
        else
            x_ = nullptr;
        return *this;
    }

    ~Test() = default;   // unique_ptr cleans up automatically

private:
    std::unique_ptr<int> x_;
};
```

**What disappears vs raw pointer:**
- No `delete x_` anywhere — `unique_ptr` destructor handles it
- No null-check before delete — `unique_ptr` does it internally

**Key gotcha — don't move in a copy:**
```cpp
x_ = std::move(other.x_);       // WRONG in copy — steals from other, leaving it null
x_ = std::make_unique<int>(*other.x_);  // CORRECT — allocates new, copies value
```

**Header:** `#include <memory>`

---

## RAII — Resource Acquisition Is Initialization

**Core idea:** Acquire a resource in the constructor, release it in the destructor. C++ guarantees the destructor runs when the object goes out of scope — even if an exception is thrown.

**Why it matters:** Without RAII, if an exception is thrown between acquiring and releasing a resource, the release never happens → resource leak.

```cpp
// Without RAII — leak if exception thrown between open and close
FILE* f = fopen("log.txt", "r");
doSomething();   // exception here → fclose never reached
fclose(f);

// With RAII — destructor always runs
{
    RAIIfile f("log.txt");   // opens in constructor
    doSomething();           // exception here...
}                            // destructor runs anyway → file closed
```

**Rules for a correct RAII class:**
1. Acquire resource in constructor — throw if it fails
2. Release resource in destructor — check for null first
3. Delete copy constructor and copy assignment — two objects can't own the same resource
4. Optionally add move constructor + move assignment to transfer ownership

**Implementation — RAII file handle:**
```cpp
#include <cstdio>
#include <stdexcept>
#include <string>

class RAIIfile {
public:
    RAIIfile(const std::string& path) {
        file_ = fopen(path.c_str(), "r");
        if (!file_) throw std::runtime_error("Failed to open file: " + path);
    }

    ~RAIIfile() {
        if (file_) fclose(file_);
    }

    RAIIfile(const RAIIfile&) = delete;
    RAIIfile& operator=(const RAIIfile&) = delete;

private:
    FILE* file_;
};
```

**Real-world RAII in C++ stdlib:**
- `std::unique_ptr` / `std::shared_ptr` — memory
- `std::lock_guard` / `std::unique_lock` — mutex
- `std::fstream` — file streams

---

## RAII Mutex Lock Guard

The mutex must be **passed by reference** — `std::mutex` is non-copyable by design (copying a mutex makes no sense since the whole point is a single shared lock).

```cpp
#include <mutex>

class RAIIMutex {
public:
    RAIIMutex(std::mutex& mtx) : mtx_(mtx) { mtx_.lock(); }
    ~RAIIMutex() { mtx_.unlock(); }

    RAIIMutex(const RAIIMutex&) = delete;
    RAIIMutex& operator=(const RAIIMutex&) = delete;

private:
    std::mutex& mtx_;  // reference, not owner
};

// Usage
std::mutex shared_mtx;  // one mutex shared across threads

void criticalSection() {
    RAIIMutex lock(shared_mtx);  // locks here
    // do work...
}                                // unlocks automatically
```

This is exactly what `std::lock_guard` does in the stdlib.

---

## RAII Timer

Record start time in constructor, compute and log elapsed in destructor.

```cpp
#include <chrono>
#include <iostream>
#include <string>

class RAIITimer {
public:
    RAIITimer(const std::string& label)
        : label_(label), start_(std::chrono::high_resolution_clock::now()) {}

    ~RAIITimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start_).count();
        std::cout << label_ << " took " << elapsed << " ms\n";
    }

    RAIITimer(const RAIITimer&) = delete;
    RAIITimer& operator=(const RAIITimer&) = delete;

private:
    std::string label_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};

// Usage
void expensiveFunction() {
    RAIITimer t("expensiveFunction");
    // do work...
}  // prints: "expensiveFunction took X ms"
```

**Key `std::chrono` types:**
- `high_resolution_clock::now()` — current time point
- `duration_cast<milliseconds>` — convert duration to ms
- `.count()` — extract numeric value

---

## std::runtime_error / Exceptions

**Core idea:** `throw` stops execution immediately and unwinds the stack until a matching `catch` is found. If nothing catches it, the program terminates.

**What happens when an exception is not caught:**
```
throw → unwind stack looking for catch → finds none → std::terminate() → crash
```
Destructors **may not run** during `terminate()` — so RAII cleanup is not guaranteed. Always catch exceptions if you need destructors to fire.

**What happens when it IS caught:**
```
throw → unwind stack → destructors run during unwind → catch handles it → program continues
```
Destructors are guaranteed to run during stack unwinding — that's what makes RAII exception-safe.

**Syntax:**
```cpp
throw std::runtime_error("message");
```

**Catching:**
```cpp
try {
    RAIIfile f("missing.txt");
} catch (const std::runtime_error& e) {
    std::cout << e.what();  // prints the message
}
```

**Common standard exception types** (all from `<stdexcept>`):
| Exception | When to use |
|---|---|
| `std::runtime_error` | Errors detectable only at runtime (file not found, bad input) |
| `std::invalid_argument` | Bad argument passed to a function |
| `std::out_of_range` | Index or value out of valid range |
| `std::logic_error` | Programming errors, violated preconditions |

**Why throw in a constructor?**
If construction fails, the object is never fully created — so the destructor won't run. Throwing is the only way to signal failure from a constructor. The caller gets an exception instead of a half-constructed object.

```cpp
RAIIfile f("missing.txt");  // throws → f never exists
// execution jumps to catch block, no need to worry about f's destructor
```

**Header:** `#include <stdexcept>`

**When exceptions are not possible (e.g. embedded / real-time / ROS):**

Exceptions are often disabled in robotics (`-fno-exceptions`) — they add overhead and unpredictable latency. Two alternatives:

**Option 1 — Invalid state + isValid():**
```cpp
class RAIIfile {
public:
    RAIIfile(const std::string& path) {
        file_ = fopen(path.c_str(), "r");
        // don't throw, leave file_ as nullptr on failure
    }

    bool isValid() const { return file_ != nullptr; }

    ~RAIIfile() { if (file_) fclose(file_); }

private:
    FILE* file_;
};

// caller checks
RAIIfile f("log.txt");
if (!f.isValid()) { /* handle error */ }
```

**Option 2 — Factory method (named constructor):**
Constructor is private and always valid. Factory returns `nullptr` on failure.
```cpp
class RAIIfile {
public:
    static std::unique_ptr<RAIIfile> create(const std::string& path) {
        FILE* f = fopen(path.c_str(), "r");
        if (!f) return nullptr;
        return std::unique_ptr<RAIIfile>(new RAIIfile(f));
    }

private:
    RAIIfile(FILE* f) : file_(f) {}
    FILE* file_;
};

// caller checks
auto f = RAIIfile::create("log.txt");
if (!f) { /* handle error */ }



## Virtual Functions, Pure Virtual Functions & Interfaces

### Virtual Function

A function marked `virtual` in a base class can be **overridden** by a derived class. The correct version is called based on the **actual runtime type**, not the declared type.

```cpp
class Base {
public:
    virtual void speak() { std::cout << "Base\n"; }
};

class Derived : public Base {
public:
    void speak() override { std::cout << "Derived\n"; }
};

Base* obj = new Derived();
obj->speak();  // prints "Derived" — runtime dispatch, not "Base"
```

Without `virtual`, `obj->speak()` would print "Base" (static dispatch based on pointer type).

**How it works — vtable:**
- The compiler creates a vtable (table of function pointers) per class
- Each object has a hidden vptr pointing to its class's vtable
- `obj->speak()` → follow vptr → look up speak → call correct version
- Cost: one pointer indirection per virtual call — negligible in most code, avoid in tight inner loops

**Always use `override`** on derived class methods — compiler error if you accidentally mistype the signature.

---

### Pure Virtual Function

A virtual function with `= 0` — **no implementation in the base class**. Derived classes **must** override it or they too become abstract.

```cpp
class SLAMBackendInterface {
public:
    virtual void getPose() = 0;       // pure virtual
    virtual ~SLAMBackendInterface() = default;
};
```

A class with at least one pure virtual function is **abstract** — you cannot instantiate it directly:
```cpp
SLAMBackendInterface b;  // compile error — abstract class
```

---

### Interface

C++ has no `interface` keyword (unlike Java). An interface is just a convention: **an abstract class with only pure virtual functions and a virtual destructor**.

```cpp
class SLAMBackendInterface {
public:
    virtual void initialize()                    = 0;
    virtual bool processFrame(const Frame& f)    = 0;
    virtual Pose getPose()                 const = 0;
    virtual void reset()                         = 0;
    virtual ~SLAMBackendInterface()              = default;
};
```

**Why virtual destructor on the interface?**
If you delete a derived object through a base pointer, without a virtual destructor the derived destructor never runs → resource leak.

```cpp
SLAMBackendInterface* b = new VINSFusion();
delete b;  // without virtual ~: only ~SLAMBackendInterface runs → leak
           // with virtual ~:    ~VINSFusion runs first → correct
```

**Rule:** Any class with virtual functions must have a virtual destructor.

---

### How this connects to Strategy pattern

```
SLAMBackendInterface        ← interface (pure virtual)
    ↑           ↑
VINSFusion   ORBSLAM3       ← concrete strategies (implement interface)

SLAMBackendContext          ← holds unique_ptr<SLAMBackendInterface>
                              delegates to it, never knows the concrete type
```

`setBackend(unique_ptr<SLAMBackendInterface>)` — caller passes in any concrete type. Context just calls the interface. This is why virtual dispatch enables Strategy.

---

### Quick reference

| Term | Meaning |
|------|---------|
| `virtual void f()` | Can be overridden; has a default implementation |
| `virtual void f() = 0` | Must be overridden; no implementation in base |
| Abstract class | Has at least one pure virtual function; cannot instantiate |
| Interface | All pure virtual + virtual destructor; defines a contract |
| `override` | Compiler checks you're actually overriding a base virtual |
| vtable | Per-class table of function pointers enabling runtime dispatch |

---

## vtable and vptr — How Virtual Dispatch Actually Works

When the compiler sees a class with any virtual function, it does two things:

**1. Creates a vtable (per class)**
A static array of function pointers — one entry per virtual function. Created once at compile time.

```
VINSFusion vtable:
  [0] → &VINSFusion::initialize
  [1] → &VINSFusion::processFrame
  [2] → &VINSFusion::getPose

ORBSLAM3 vtable:
  [0] → &ORBSLAM3::initialize
  [1] → &ORBSLAM3::processFrame
  [2] → &ORBSLAM3::getPose
```

**2. Adds a vptr (per object)**
Every object of a class with virtual functions gets a hidden pointer (`vptr`) as its first member. It points to its class's vtable. Set in the constructor.

```
VINSFusion object layout in memory:
  [ vptr ] → points to VINSFusion vtable
  [ x_   ]
  [ y_   ]
  ...
```

**What happens at a virtual call:**
```cpp
SLAMBackendInterface* b = new VINSFusion();
b->getPose();
// 1. follow b's vptr → VINSFusion vtable
// 2. look up getPose entry → &VINSFusion::getPose
// 3. call it
```

**Cost:**
- Memory: +8 bytes per object (one pointer)
- Call: one extra pointer indirection vs a direct call
- Negligible for most code — matters only in tight inner loops (e.g., per-keypoint processing at 20 Hz)

**Why this matters in SLAM:**
In your feature extraction loop (thousands of keypoints per frame), if you called a virtual function per keypoint the vtable overhead would add up. This is why templates / CRTP are preferred for per-element policies.

---

## Derived Class Memory Layout

A derived object is **one contiguous block of memory** — the base members are embedded inside it, not a separate allocation.

```
Derived object in memory:
┌─────────────────────┐
│  vptr               │  ← hidden pointer to Derived's vtable
├─────────────────────┤
│  Base members       │  ← Base's data embedded here
│    int base_x_      │
├─────────────────────┤
│  Derived members    │
│    int* data_       │
│    float sensor_y_  │
└─────────────────────┘
```

When you do `Base* b = new Derived()` — `b` points to the start of this block. The full Derived object is there in memory, but `b` only "sees" the Base portion by type.

---

## Why Virtual Destructor — Always

Without a virtual destructor, deleting a derived object through a base pointer only calls the base destructor. The derived destructor never runs.

```cpp
class Base {
public:
    ~Base() { std::cout << "~Base\n"; }  // NOT virtual
};

class Derived : public Base {
    int* data_ = new int[100];
public:
    ~Derived() { delete[] data_; std::cout << "~Derived\n"; }
};

Base* b = new Derived();
delete b;
// prints: ~Base only
// ~Derived never runs → data_ leaked
```

Fix — make destructor virtual:
```cpp
class Base {
public:
    virtual ~Base() = default;  // now ~Derived runs first, then ~Base
};
```

**Why only `~Base` runs without virtual — static dispatch:**

`b` is a `Base*`. Without `virtual`, the compiler does static dispatch at compile time — it only knows `b` is a `Base*`, so it calls `~Base` directly. The Derived portion of the object is never cleaned up.

With `virtual ~Base()`:
- `delete b` → vtable lookup → finds `~Derived` → calls it
- `~Derived` runs first (cleans up Derived's members)
- C++ automatically chains up → `~Base` runs next
- Full object destroyed correctly

```
Without virtual ~:     With virtual ~:
delete b               delete b
    ↓                      ↓
~Base() only           vtable lookup
  (Derived portion          ↓
   never cleaned)      ~Derived() runs  ← cleans data_
                           ↓
                       ~Base() runs     ← C++ auto-chains
```

**Why C++ doesn't make all destructors virtual by default:**
Cost — every object would get a vptr (+8 bytes) even when never used polymorphically. For a `Point3D` with millions of instances in a point cloud, that's significant wasted memory.

**Rule: if a class has any virtual function, give it a virtual destructor.**

Why? Because if it has virtual functions, someone will use it polymorphically (base pointer to derived object). And then they'll delete it. And then you leak.

**`= default` vs `{}`:**
Both work. `= default` is preferred — tells the compiler to generate the default destructor, allows the class to remain trivially destructible in some contexts.

---

## Compile-time Polymorphism (Templates) vs Runtime Polymorphism (Virtual)

### Runtime polymorphism — virtual functions
- Type resolved at **runtime** via vtable lookup
- Requires heap allocation + pointer/reference to base
- Flexible — swap implementations at runtime (Strategy pattern)
- Cost: vtable indirection, can't be inlined, harder for compiler to optimize

### Compile-time polymorphism — templates
- Type resolved at **compile time** — compiler generates separate code per type
- No vtable, no vptr, no indirection
- Calls can be **inlined** — zero overhead abstraction
- Rigid — cannot swap at runtime, each type baked in at compile time

```cpp
// Runtime — virtual dispatch, one pointer indirection per call
void process(SLAMBackendInterface* b) {
    b->processFrame(f);  // vtable lookup every call
}

// Compile-time — template, inlined, zero overhead
template <typename Backend>
void process(Backend& b) {
    b.processFrame(f);  // direct call, compiler can inline
}
```

### CRTP — Curiously Recurring Template Pattern
Get polymorphic behavior at compile time with zero vtable cost:

```cpp
template <typename Derived>
class SLAMBackendBase {
public:
    void processFrame(const Frame& f) {
        static_cast<Derived*>(this)->processFrameImpl(f);  // compile-time dispatch
    }
};

class VINSFusion : public SLAMBackendBase<VINSFusion> {
public:
    void processFrameImpl(const Frame& f) { /* ... */ }
};
```

No vtable. No vptr. The compiler resolves `processFrameImpl` at compile time.

### When to use which

| Situation | Use |
|-----------|-----|
| Need to swap at runtime (config, sensor fallback) | Virtual / Strategy |
| Fixed type known at compile time, performance critical | Template / CRTP |
| Per-element inner loop (keypoints, particles) | Template — vtable overhead adds up |
| Plugin system, factory, user-selectable algorithm | Virtual |
| Generic container or algorithm (sort, filter) | Template |

**In your SLAM code:**
- `SLAMBackendContext` swaps backends at runtime based on sensor state → virtual is correct
- Per-keypoint descriptor computation in a tight loop → template would be better
- `std::vector<T>`, `std::sort` → templates, zero overhead

---

## lvalue vs rvalue — References and Temporaries

**lvalue** — has a name and an address, persists beyond the expression:
```cpp
int i = 5;   // i is an lvalue
```

**rvalue** — a temporary, no address, exists only for the expression:
```cpp
i + 1        // temporary — rvalue
```

**The rule:** a non-const reference (`int&`) can only bind to an lvalue. Passing a temporary to `int&` is a compile error:

```cpp
void f(int& x) { ... }

int i = 0;
f(i);      // OK — i is an lvalue
f(i + 1);  // ERROR — i+1 is a temporary
```

**Fix:** pass by value when you need to pass expressions like `i+1`:
```cpp
void f(int x) { ... }
f(i + 1);  // OK — creates a copy
```

**Backtracking gotcha:** in recursive backtracking, always pass `index` and `current` by value, not by reference — each call needs its own copy:
```cpp
// WRONG
void solve(int& i, string& temp, ...)
    solve(i+1, temp + s[i], ...)  // ERROR — temporaries can't bind to references

// CORRECT
void solve(int i, string temp, ...)
    solve(i+1, temp + s[i], ...)  // OK — copies made for each recursive call
```

---

## How Data is Stored — Bits, Hex, and Type Casting

**At the hardware level, everything is binary (bits).** Only 0s and 1s exist. All types — `int`, `float`, `char` — are just bits in memory.

**Hex is just human shorthand** for reading binary — every 4 bits = 1 hex digit:
```
binary:   1111 1111
hex:      FF
decimal:  255
char:     '\xff'
```
Same bits, four different ways to look at them.

### char is just an integer with an ASCII mapping

`char` is an 8-bit integer. ASCII is a standard agreement that maps numbers to symbols:
```
65 → 'A'
97 → 'a'
48 → '0'
```

```cpp
char c = 65;
cout << c;        // prints: A
cout << (int)c;   // prints: 65  — same bits, different interpretation
```

### Type casting: two kinds

**1. Reinterpretation (no conversion) — `char` ↔ `int`:**
Both are plain integers, same binary format. Casting just changes how the compiler reads them — bits don't change:
```cpp
(int)'A'    // → 65
(char)65    // → 'A'
```

**2. Actual conversion — `float` ↔ `int`:**
`float` uses IEEE 754 (sign + exponent + mantissa), `int` is plain binary — completely different formats. The CPU must actually convert, and the bits change:
```cpp
float f = 3.7;
int i = (int)f;   // i = 3 — decimal truncated (not rounded)
```

### `toupper`/`tolower` return `int`
They do ASCII math (`'a' - 32 = 'A'`) and return `int` for historical C reasons (to handle `EOF = -1` which doesn't fit in a `char`). Cast explicitly:
```cpp
temp + (char)toupper(s[i])   // correct — cast back to char
```

---

## Pointer Arithmetic

An array name decays to a pointer to its first element. You can traverse it using `+` and `*` instead of `[]`:

```cpp
int arr[] = {1, 2, 3, 4, 5};

for (int i = 0; i < 5; i++) {
    std::cout << *(arr + i) << " ";
}
```

**Key fact:** `arr[i]` and `*(arr + i)` are **literally identical** — the compiler generates the same code for both. `[]` is just syntactic sugar for pointer arithmetic.

- `arr` → pointer to first element
- `arr + i` → pointer to i-th element
- `*(arr + i)` → value at i-th element

---

## Stack vs Heap

| | Stack | Heap |
|--|-------|------|
| Management | Automatic | Manual (`new`/`delete`) |
| Size | ~8MB fixed | Bounded by RAM |
| Speed | Very fast (pointer decrement) | Slower (OS allocator) |
| Lifetime | Until scope ends | Until `delete` is called |

```cpp
void foo() {
    int x = 5;           // stack — freed when foo() returns
    int* y = new int;    // heap — lives until delete
    delete y;            // must manually free
}
```

**Fixed-size array → stack:**
```cpp
int arr[5] = {1,2,3,4,5};   // stack
```

**`new` → always heap:**
```cpp
int* arr = new int[5];       // heap — must delete[] arr
```

**`std::vector` → both:**
- The vector object (size, capacity, pointer) → stack
- The actual data → heap (vector internally calls `new`)
- Vector destructor frees the heap data automatically (RAII)

**Key insight:** `unique_ptr` and containers like `vector` exist to make heap behave like stack — automatic cleanup via destructors.

---

## `std::map` vs `std::unordered_map`

| | `std::map` | `std::unordered_map` |
|--|-----------|---------------------|
| Underlying structure | Red-black tree (BST) | Hash table |
| Lookup | O(log n) | O(1) average |
| Insert | O(log n) | O(1) average |
| Order | Sorted by key | No order |
| Worst case | O(log n) — guaranteed | O(n) — hash collisions |

**`map` — no hashing at all.** Keys are compared using `<` to navigate left/right in the tree. Keys must be sortable/comparable.

**`unordered_map` — hash table with buckets:**
```
bucket[0] → null
bucket[3] → "alice" → "carol" → null   ← collision, both hashed to 3
bucket[7] → "bob" → null
```
Worst case: all keys hash to same bucket → linked list scan → O(n).

**`map` worst case is always O(log n)** because the red-black tree self-balances — height is always guaranteed to be O(log n).

**Duplicate keys:**
- `map` / `unordered_map` — no duplicates, inserting same key overwrites value
- `std::multimap` — allows duplicate keys, same red-black tree structure

**Use `map` when:** need sorted order, or want guaranteed O(log n) worst case.
**Use `unordered_map` when:** need fast average lookup, don't care about order.

---

## `enum` vs `enum class`

| | `enum` | `enum class` |
|--|--------|-------------|
| Scope | Global — pollutes namespace | Scoped — must use `Color::RED` |
| Implicit int conversion | Yes | No |
| Name conflicts | Can clash with other enums | Safe |

```cpp
// enum — pollutes namespace, implicit int conversion
enum Color { RED, GREEN, BLUE };
int x = RED;   // allowed — implicit int conversion

// enum class — scoped, no implicit conversion
enum class Color { RED, GREEN, BLUE };
int x = Color::RED;    // ERROR — no implicit conversion
Color c = Color::RED;  // correct
```

**Name clash example:**
```cpp
enum Direction { UP, DOWN };
enum Status { UP, DOWN };      // ERROR — UP already defined!

enum class Direction { UP, DOWN };
enum class Status { UP, DOWN }; // fine — Direction::UP vs Status::UP
```

**Always prefer `enum class`** — safer scoping, no accidental int conversions, no name clashes.

---

## Abstract Data Types (ADT)

**ADT = the contract** — defines what operations are available, not how they're implemented.

| ADT | Operations | Possible Implementations |
|-----|-----------|--------------------------|
| Stack | push, pop, peek | array, linked list |
| Queue | enqueue, dequeue | array, linked list |
| Map | insert, lookup, delete | BST, hash table |

**In C++:** ADT = abstract base class with pure virtual functions. Implementation = concrete derived class.

### Virtual functions approach (runtime polymorphism)

```cpp
class SensorInterface {
public:
    virtual void readData() = 0;
    virtual std::string getSensorType() = 0;
    virtual ~SensorInterface() = default;
};

class Camera : public SensorInterface {
public:
    void readData() override { std::cout << "Reading RGB image\n"; }
    std::string getSensorType() override { return "Camera"; }
};

class LiDAR : public SensorInterface {
public:
    void readData() override { std::cout << "Reading point cloud\n"; }
    std::string getSensorType() override { return "LiDAR"; }
};

int main() {
    std::vector<std::unique_ptr<SensorInterface>> sensors;
    sensors.push_back(std::make_unique<Camera>());
    sensors.push_back(std::make_unique<LiDAR>());

    for (auto& s : sensors) {
        std::cout << s->getSensorType() << ": ";
        s->readData();
    }
}
```

### `std::variant` approach (compile-time polymorphism)

No inheritance, no virtual functions, no vtable overhead:

```cpp
#include <variant>

struct Camera {
    void readData()        { std::cout << "Reading RGB image\n"; }
    std::string getSensorType() { return "Camera"; }
};

struct LiDAR {
    void readData()        { std::cout << "Reading point cloud\n"; }
    std::string getSensorType() { return "LiDAR"; }
};

int main() {
    std::vector<std::variant<Camera, LiDAR>> sensors;
    sensors.push_back(Camera{});
    sensors.push_back(LiDAR{});

    for (auto& s : sensors) {
        std::visit([](auto& sensor) {
            std::cout << sensor.getSensorType() << ": ";
            sensor.readData();
        }, s);
    }
}
```

**`std::visit` explained:**
- Applies the lambda to whatever the variant currently holds
- `auto&` makes it a generic lambda — compiler generates a version for each type
- Internally equivalent to:
```cpp
if s holds Camera → call lambda(get<Camera>(s))
if s holds LiDAR  → call lambda(get<LiDAR>(s))
```
But resolved at compile time via templates — no runtime overhead.

**When to use which:**

| | Virtual functions | `std::variant` |
|--|------------------|----------------|
| Types known at compile time | No | Yes — fixed set |
| Add new types at runtime | Yes | No |
| Performance | vtable overhead | Faster — compile time |
| Inheritance needed | Yes | No |

---

## `static` Keyword — All Uses

| Where | Meaning |
|-------|---------|
| Local variable in function | Persist across calls, created once |
| Method in class | Call without an object instance |
| Member variable in class | Shared across all objects of that class |
| Global/file scope | Private to this translation unit only |

**1. Local variable in function — persist across calls:**
```cpp
void foo() {
    static int count = 0;
    count++;
    std::cout << count << "\n";
}
foo();   // 1
foo();   // 2
foo();   // 3  — not reset, persists
```

**2. Static method — call without an object:**
```cpp
class Foo {
public:
    static void bar() { ... }
};
Foo::bar();   // no object needed
```

**3. Static member variable — shared across all objects:**
```cpp
class Foo {
    static int count;   // one copy shared by all Foo instances
};
```

**4. Static at file/global scope — private to translation unit:**
```cpp
// file1.cpp
static int x = 5;   // only visible in file1.cpp — not exported to linker

// file2.cpp
static int x = 10;  // separate variable, no conflict
```
Without `static`, `int x` at global scope is visible across all files — duplicate definitions cause linker errors.

---

## `shared_ptr` — Internals and Cycle Problem

### How `shared_ptr` Works Internally

`shared_ptr` keeps a **control block** — a separate heap allocation created alongside the object. The control block holds:
- **ref count** — number of `shared_ptr`s pointing to the object
- **weak count** — number of `weak_ptr`s pointing to the object
- **custom deleter** (if any)

```
shared_ptr<A> a = make_shared<A>();

Memory layout:
  a  →  [ control block | ref=1, weak=0 ]
         [ A object                      ]
```

When you copy a `shared_ptr`, both copies point to the same control block and increment the ref count:
```cpp
shared_ptr<A> b = a;   // ref count → 2
```

When a `shared_ptr` is destroyed, ref count decrements. When it hits 0, the object is destroyed. When weak count also hits 0, the control block is freed.

### Cycle Problem

```cpp
class A { public: shared_ptr<B> b; ~A() { cout << "A destroyed\n"; } };
class B { public: shared_ptr<A> a; ~B() { cout << "B destroyed\n"; } };

shared_ptr<A> a = make_shared<A>();  // A ref count = 1
shared_ptr<B> b = make_shared<B>();  // B ref count = 1
a->b = b;   // B ref count → 2
b->a = a;   // A ref count → 2

// main returns:
// a goes out of scope → A ref count → 1 (b->a still holds it)
// b goes out of scope → B ref count → 1 (a->b still holds it)
// Neither hits 0 → neither destructor fires → LEAK
```

Symptom: run the program and neither "A destroyed" nor "B destroyed" prints.

### Fix — `weak_ptr`

`weak_ptr` observes an object without incrementing the ref count. Break the cycle by making one side `weak_ptr`:

```cpp
class B { public: weak_ptr<A> a; };  // weak — doesn't increment A's ref count

// Now when main returns:
// a goes out of scope → A ref count → 0 → A destroyed ✓
// b goes out of scope → B ref count → 0 → B destroyed ✓
```

To use the object held by a `weak_ptr`, call `.lock()` — returns a `shared_ptr` if the object still exists, `nullptr` if it's been destroyed:
```cpp
if (auto locked = wp.lock()) {
    locked->doSomething();
}
```

### Custom Deleters

By default, `shared_ptr` calls `delete` when ref count hits 0. You can pass a custom function instead — useful for non-heap resources:

```cpp
shared_ptr<FILE> f(fopen("x.txt", "r"), fclose);   // fclose called instead of delete
shared_ptr<int> p(new int[10], [](int* p){ delete[] p; });  // array delete
```

The custom deleter is stored in the control block.

### Implementing `shared_ptr` from Scratch

Key insight: ref count must be **heap allocated** and shared across all copies — if it were a plain `int` member, each object would have its own copy.

```cpp
template <typename T>
class shr_ptr {
public:
    int* ref_count = nullptr;
    T* t_;

    // Constructor — allocate object and ref count, start at 1
    shr_ptr(T val) {
        t_ = new T(val);
        ref_count = new int(1);
    }

    // Copy constructor — share pointer + ref count, just increment
    // No decrement — this object never owned anything before
    shr_ptr(const shr_ptr& other) {
        ref_count = other.ref_count;
        (*ref_count)++;
        t_ = other.t_;
    }

    // Copy assignment — release old, acquire new
    shr_ptr& operator=(const shr_ptr& other) {
        if (&other == this) return *this;
        (*ref_count)--;                        // release old
        if (*ref_count == 0) { delete t_; delete ref_count; }
        ref_count = other.ref_count;           // acquire new
        (*ref_count)++;
        t_ = other.t_;
        return *this;
    }

    // Destructor — decrement, delete if last owner
    ~shr_ptr() {
        (*ref_count)--;
        if (*ref_count == 0) { delete t_; delete ref_count; }
    }

    T& operator*()  { return *t_; }
    T* operator->() { return t_; }
};
```

**Key rules:**
- Copy constructor — only increment, never decrement (nothing was owned before)
- Copy assignment — decrement old first, then increment new
- Destructor — decrement then check; delete both object and ref count if 0
- `(*ref_count)++` not `*ref_count++` — postfix `++` binds tighter than `*`, would increment the pointer

### Quick Reference

| | `shared_ptr` | `weak_ptr` |
|--|-------------|------------|
| Increments ref count | Yes | No |
| Keeps object alive | Yes | No |
| Can access object | Yes (`->`) | Only via `.lock()` |
| Use case | Shared ownership | Break cycles, optional observer |

---

## `make_unique` from Scratch

`std::make_unique<T>(value)` is just a template function that calls `new` and wraps the result in a `unique_ptr`:

```cpp
#include <memory>

template<typename T>
std::unique_ptr<T> my_make_unique(T value) {
    return std::unique_ptr<T>(new T(value));
}

// Usage
auto p = my_make_unique<int>(42);
std::cout << *p << "\n";   // 42
```

**Why it's a function, not a class:** it just allocates and wraps — no state needed.
**Why use it over `new` directly:** safer — if an exception is thrown, the `unique_ptr` still cleans up. Raw `new` in complex expressions can leak.
