# C++ Practice Track (Extracted)

### Day 1 — Apr 3
1. Implement a class with a raw pointer member — follow Rule of 3 (copy ctor, copy assign, destructor)
2. Replace raw pointer with `unique_ptr` — notice what disappears
3. Pointer arithmetic: traverse an array using `int*` without `[]`

---

### Day 2 — Apr 4
1. Stack vs heap: benchmark allocating 1M ints on stack vs heap, explain the difference
2. Write a function that deliberately causes a memory leak; then fix it with `delete`
3. Implement `make_unique` from scratch using `new` + template

---

### Day 3 — Apr 5
1. Build an RAII `FileHandle` class — opens file in ctor, closes in dtor, no manual cleanup
2. Demonstrate that RAII works even when an exception is thrown mid-function
3. Write a `ScopedTimer` using RAII that prints elapsed time on scope exit

---

### Day 4 — Apr 6
1. Create a `shared_ptr` cycle between two nodes; verify it leaks with a custom destructor print
2. Break the cycle using `weak_ptr`; verify destructor fires
3. Implement a reference-counted smart pointer from scratch (simplified `shared_ptr`)

---

### Day 5 — Apr 7
1. Write a class with Rule of 5: copy ctor, copy assign, move ctor, move assign, destructor
2. Prove move is called (not copy) when returning a local object from a function
3. Implement `std::swap` for your class using move semantics

---

### Day 6 — Apr 8
1. Write a base class with a virtual function; derive two classes — call via base pointer
2. Add a pure virtual function; make the base abstract; verify you can't instantiate it
3. Implement a vtable manually using function pointers in a struct

---

### Day 7 — Apr 9
1. Overload `+`, `-`, `==` for a `Vector2D` class
2. Overload `<<` for `std::ostream` to print your class
3. Overload `[]` for a simple `Matrix` class with bounds checking

---

### Day 8 — Apr 10
1. Write a function template `max_val<T>(T a, T b)` — test with int, double, string
2. Write a class template `Pair<T, U>` with `first`, `second`, and `swap()`
3. Specialize your `Pair` template for `bool, bool` to use bit storage

---

### Day 9 — Apr 11
1. Implement SFINAE: write a template that only compiles for arithmetic types
2. Use `std::enable_if` to restrict a template function to integral types
3. Write `is_pointer<T>` type trait from scratch using template specialization

---

### Day 10 — Apr 12
1. `std::vector` internals: measure capacity doubling, write your own `push_back` loop
2. Compare `std::list` vs `std::vector` for random insert — explain cache locality
3. Use `std::deque` as both a stack and queue; explain when to prefer it over vector

---

### Day 11 — Apr 13
1. `std::map` vs `std::unordered_map`: measure insert/lookup for 1M keys, explain BST vs hash
2. Use `std::set` to find the k-th distinct element in a stream
3. Implement a frequency counter using `std::unordered_map<string, int>`

---

### Day 12 — Apr 14
1. Sort a `vector<Person>` by age using a lambda comparator
2. Use `std::sort` with a custom `operator<` in the struct
3. Implement `std::find_if` from scratch as a template function

---

### Day 13 — Apr 15
1. Write a generic `transform` using function pointers — then replace with `std::function`
2. Use `std::accumulate` to reduce a vector of strings into one concatenated string
3. Use `std::partition` + `std::stable_partition` — explain the difference

---

### Day 14 — Apr 16
1. Write a lambda with `[=]` capture, `[&]` capture, and explicit `[x]` capture — compare behavior
2. Write a generic `memoize(f)` wrapper using `std::unordered_map` and `std::function`
3. Use `std::bind` to partially apply a 3-argument function; replace with a lambda version

---

### Day 15 — Apr 17
1. Use `auto` for iterator type deduction in a range-based for loop over a `map`
2. Use `decltype` to capture the return type of a function without calling it
3. Write a `constexpr` factorial — verify it computes at compile time

---

### Day 16 — Apr 18
1. Launch 3 `std::thread`s that each increment a shared counter — observe the race condition
2. Fix the race with `std::mutex` + `lock_guard`
3. Fix with `std::atomic<int>` — compare performance

---

### Day 17 — Apr 19
1. Implement producer-consumer with `std::queue`, `std::mutex`, `std::condition_variable`
2. Add a stop signal using an `std::atomic<bool>` flag
3. Use `std::unique_lock` with `wait_for` to add a timeout to the consumer

---

### Day 18 — Apr 20
1. Launch an async task with `std::async(std::launch::async, ...)` — get result via `std::future`
2. Use `std::promise`/`std::future` pair to send a value between threads
3. Chain two futures: run task B only after task A's result is ready

---

### Day 19 — Apr 21
1. Write a variadic template `print_all(args...)` that prints any number of arguments
2. Use `std::integer_sequence` to index into a `std::tuple` at runtime
3. Write `tuple_for_each` using fold expressions (C++17)

---

### Day 20 — Apr 22
1. Use C++20 `concept` to constrain a template to types with `operator+`
2. Write a `requires` clause that checks a type has a `.size()` method
3. Write a constrained `print_range` that works for any range (has `begin`/`end`)

---

### Day 21 — Apr 23
1. Use `std::ranges::sort` and `std::ranges::filter` (C++20)
2. Write a custom range view that yields only even numbers from a vector
3. Chain `std::views::transform` + `std::views::filter` + `std::views::take`

---

### Day 22 — Apr 24
1. Write a coroutine generator (C++20) that yields Fibonacci numbers lazily
2. Use `co_yield` to implement a range over a recursive tree structure
3. Compare coroutine vs callback vs iterator for the same problem

---

### Day 23 — Apr 25
1. Write a thread pool with a task queue using `std::thread`, `std::queue`, `std::mutex`
2. Add graceful shutdown: drain the queue before joining threads
3. Add `std::future` return from submitted tasks so callers can await results

---

### Day 24 — Apr 26
1. Profile two sorting implementations with `std::chrono::high_resolution_clock`
2. Use `perf` (or valgrind) to find a cache miss hotspot in a naive matrix multiply
3. Rewrite the naive O(n²) matrix multiply to be cache-friendly (row-major access)

---

### Day 25 — Apr 27
1. Use `std::variant<int, double, string>` to model a heterogeneous value type
2. Use `std::visit` with a visitor to dispatch on variant type
3. Implement a simple `JSON`-like value type using `std::variant` + `std::map` + `std::vector`

---

### Day 26 — Apr 28
1. Implement `std::optional<T>` from scratch (simplified) — model a nullable value
2. Use `std::expected<T, E>` (C++23) or simulate it with `std::variant<T, Error>`
3. Chain optional operations with monadic style (`.and_then`, `.transform`)

---

### Day 27 — Apr 29
1. Write a lock-free stack using `std::atomic<T*>` and compare-and-swap
2. Detect and fix ABA problem in the lock-free stack
3. Use `std::memory_order` explicitly — compare `relaxed` vs `seq_cst` performance

---

### Day 28 — Apr 30
1. Write a compile-time `static_assert` test suite for your type traits from earlier days
2. Use `<type_traits>` to write a generic `clamp(T val, T lo, T hi)` with constraints
3. Final challenge: implement a mini `std::function<R(Args...)>` from scratch using type erasure

---

# Additional Essential Topics (Must Cover)

### Build Systems & Compilation
1. Create a multi-file C++ project using CMake (library + executable)
2. Understand compilation pipeline: preprocessing → compilation → linking
3. Build both static and shared libraries and link them

---

### Debugging & Tooling
1. Use GDB to debug a segmentation fault step-by-step
2. Detect memory leaks using AddressSanitizer (`-fsanitize=address`)
3. Use Valgrind to analyze memory issues

---

### Undefined Behavior
1. Create examples of dangling pointers and fix them
2. Demonstrate iterator invalidation in `vector` and `map`
3. Write code that compiles but causes UB — explain why

---

### Memory Layout & Performance
1. Explore struct padding using `sizeof` and `alignof`
2. Reorder struct members to minimize padding
3. Benchmark aligned vs unaligned memory access

---

### Exception Safety
1. Implement functions with basic, strong, and no-throw guarantees
2. Show how RAII ensures exception safety
3. Write exception-safe copy assignment (copy-and-swap idiom)

---

### Low-Level I/O (Optional but Valuable)
1. Use memory-mapped files (`mmap`) for fast file access
2. Compare standard file I/O vs memory-mapped I/O performance