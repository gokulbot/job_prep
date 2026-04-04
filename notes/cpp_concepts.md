# C++ Concepts

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

---

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
