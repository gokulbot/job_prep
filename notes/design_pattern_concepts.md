# Design Pattern Concepts

---

## Strategy Pattern

**Intent:** Define a family of algorithms, encapsulate each one, and make them interchangeable at runtime.

**When to use:** You need to swap behavior at runtime based on config, sensor state, or user input.

**Structure:**
- **Interface** — pure virtual base class defining the algorithm contract
- **Concrete strategies** — implementations of the interface
- **Context** — holds a pointer to the interface, delegates work to it

```cpp
class SLAMBackendInterface {
public:
    virtual void getPose() = 0;
    virtual ~SLAMBackendInterface() = default;
};

class VINSFusion : public SLAMBackendInterface {
public:
    void getPose() override { std::cout << "VINS Pose\n"; }
};

class ORBSLAM3 : public SLAMBackendInterface {
public:
    void getPose() override { std::cout << "ORBSLAM3 Pose\n"; }
};

class SLAMBackendContext {
public:
    void setBackend(std::unique_ptr<SLAMBackendInterface> backend) {
        slam_backend_ = std::move(backend);
    }
    void getPose() {
        if (slam_backend_) slam_backend_->getPose();
    }
private:
    std::unique_ptr<SLAMBackendInterface> slam_backend_;
};

// Usage — swap at runtime
SLAMBackendContext ctx;
ctx.setBackend(std::make_unique<VINSFusion>());
ctx.getPose();   // VINS Pose

ctx.setBackend(std::make_unique<ORBSLAM3>());
ctx.getPose();   // ORBSLAM3 Pose
```

**Key points:**
- Context doesn't know which strategy it's using — only the interface
- `unique_ptr` for ownership, `std::move` to transfer into context
- Swapping strategy = just call `setBackend` with a new object

---

## Factory Method Pattern

**Intent:** Create objects without exposing the instantiation logic — caller specifies *what* it wants, factory decides *how* to create it.

**When to use:** You need to create different concrete types based on a condition (config, sensor type, runtime input) without the caller knowing the concrete class.

**How it differs from Strategy:**
- **Factory** — about **creation** — who creates the object and how
- **Strategy** — about **ownership and behavior** — who holds the object and delegates to it
- Factory hands you the object. Strategy uses it.
- They're often used together: Factory creates, Strategy uses.

```cpp
enum class SLAMTypes { eORBSLAM3, eViNSFusion };

class SLAMBackendInterface {
public:
    virtual void getPose() = 0;
    virtual ~SLAMBackendInterface() = default;
};

class VINSFusion : public SLAMBackendInterface {
public:
    void getPose() override { std::cout << "VINS Pose\n"; }
};

class ORBSLAM3 : public SLAMBackendInterface {
public:
    void getPose() override { std::cout << "ORBSLAM3 Pose\n"; }
};

class SLAMBackendContext {
public:
    void setBackend(SLAMTypes type) {
        switch (type) {
            case SLAMTypes::eORBSLAM3:
                slam_backend_ = std::make_unique<ORBSLAM3>();
                break;
            case SLAMTypes::eViNSFusion:
                slam_backend_ = std::make_unique<VINSFusion>();
                break;
        }
    }
    void getPose() {
        if (slam_backend_) slam_backend_->getPose();
    }
private:
    std::unique_ptr<SLAMBackendInterface> slam_backend_;
};

// Usage
SLAMBackendContext ctx;
ctx.setBackend(SLAMTypes::eORBSLAM3);
ctx.getPose();    // ORBSLAM3 Pose

ctx.setBackend(SLAMTypes::eViNSFusion);
ctx.getPose();    // VINS Pose
```

**Key points:**
- Caller passes a type (enum), gets back the right object — no `new` in caller code
- `enum class` is preferred over plain `enum` — scoped, no implicit int conversion
- Switch `default` case should handle unknown types gracefully
- `switch` fall-through: always end each `case` with `break` or `return`, otherwise execution continues into the next case

---

## Singleton Pattern

**Intent:** Ensure only one instance of a class exists, and provide a global access point to it.

**When to use:** Shared global state that must be consistent across threads — e.g. map manager, logger, config.

**Meyers' Singleton (preferred — thread-safe in C++11):**

```cpp
class MapManager {
private:
    MapManager() = default;

public:
    static MapManager& getInstance() {
        static MapManager instance;   // created once, thread-safe
        return instance;
    }

    MapManager& operator=(const MapManager&) = delete;
    MapManager(const MapManager&) = delete;
};

// Usage
MapManager& m1 = MapManager::getInstance();
MapManager& m2 = MapManager::getInstance();
// &m1 == &m2 → true, same instance
```

**Key points:**
- Constructor is `private` — no one can create an instance directly
- Copy constructor and copy assignment deleted — can't duplicate the singleton
- `static` local variable inside `getInstance` — C++11 guarantees initialized exactly once, even across threads
- `static` on the method — call it without an object: `MapManager::getInstance()`

**`static` inside a function vs `static` on a method:**
- `static MapManager instance` inside function → "create once, persist for lifetime of program"
- `static MapManager& getInstance()` on method → "call this without an object instance"

---
