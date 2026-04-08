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

## Prototype Pattern

**Intent:** Clone an existing object instead of creating a new one from scratch.

**When to use:** Object creation is expensive (e.g. loading a map, initializing a sensor) and you need multiple similar copies.

```
Normal:    new Keyframe() → initialize everything from scratch
Prototype: existing_kf.clone() → copy already-initialized object
```

```cpp
// Abstract base with clone()
class Keyframe {
public:
    virtual Keyframe* clone() = 0;
    virtual void print() = 0;
};

// Concrete — clones itself via copy constructor
class SLAMKeyframe : public Keyframe {
    int id_;
    float x_, y_;
public:
    SLAMKeyframe(int id, float x, float y) : id_(id), x_(x), y_(y) {}

    SLAMKeyframe* clone() override { return new SLAMKeyframe(*this); }

    void print() override {
        std::cout << "Keyframe id=" << id_ << " pos=(" << x_ << "," << y_ << ")\n";
    }
};

int main() {
    SLAMKeyframe kf(1, 3.5, 2.1);
    SLAMKeyframe* kf_clone = kf.clone();  // same data, different object

    kf.print();
    kf_clone->print();

    delete kf_clone;
}
```

**Why return a pointer (not by value):**
- Returning by value loses polymorphism — can't store as `Keyframe*`
- Heap allocation persists beyond the function call
- Caller holds `Keyframe*` but gets the correct derived type at runtime

**Key points:**
- `clone()` uses the copy constructor internally — deep vs shallow copy matters here
- Always returns a heap-allocated pointer for polymorphism to work
- The cloned object is independent — modifying it doesn't affect the original

**SLAM use case:** Efficiently clone keyframes when creating map snapshots or branching hypotheses without re-initializing from scratch.

---

## Abstract Factory Pattern

**Intent:** Create families of related objects without specifying their concrete classes. One factory produces a whole group of compatible products.

**When to use:** You need to swap entire families of related objects together — swapping one should swap all.

**vs Factory Method:**
- Factory Method — creates one product
- Abstract Factory — creates a family of related products that work together

```
SLAMFactory
    ├── ClassicalSLAMFactory → GFTTExtractor + CeresOptimizer
    └── DeepSLAMFactory      → SuperPointExtractor + CeresOptimizer
```

```cpp
// Abstract products
class Optimizer       { public: virtual void optimize() = 0; };
class FeatureExtractor { public: virtual void extract()  = 0; };

// Concrete products
class CeresOptimizer : public Optimizer {
    void optimize() override { std::cout << "Ceres optimizing\n"; }
};
class GFTTExtractor : public FeatureExtractor {
    void extract() override { std::cout << "GFTT extracting\n"; }
};
class SuperPointExtractor : public FeatureExtractor {
    void extract() override { std::cout << "SuperPoint extracting\n"; }
};

// Abstract factory
class SLAMFactory {
public:
    virtual Optimizer*        createOptimizer() = 0;
    virtual FeatureExtractor* createExtractor() = 0;
};

// Concrete factories
class ClassicalSLAMFactory : public SLAMFactory {
    Optimizer*        createOptimizer() override { return new CeresOptimizer(); }
    FeatureExtractor* createExtractor() override { return new GFTTExtractor(); }
};
class DeepSLAMFactory : public SLAMFactory {
    Optimizer*        createOptimizer() override { return new CeresOptimizer(); }
    FeatureExtractor* createExtractor() override { return new SuperPointExtractor(); }
};

// Usage — swap factory to switch entire pipeline
SLAMFactory* factory = new ClassicalSLAMFactory();
factory->createOptimizer()->optimize();
factory->createExtractor()->extract();

factory = new DeepSLAMFactory();  // switch whole family
factory->createOptimizer()->optimize();
factory->createExtractor()->extract();
```

**Key points:**
- Factory creates products; products do the work
- Swapping the factory swaps the entire family of products
- Products from one factory are guaranteed compatible with each other
- Caller only depends on abstract interfaces — never on concrete classes

**SLAM use case:** Switch between ORB-SLAM and RTAB-Map backends, each requiring its own feature extractor, matcher, and optimizer.

---

## Builder Pattern

**Intent:** Construct a complex object step by step. Separate the construction from the representation so the same process can create different configurations.

**When to use:** Object has many optional parameters — instead of a constructor with 10 arguments, use a fluent builder.

```cpp
// Product
class SLAMPipeline {
public:
    bool with_loop_closure = false;
    bool use_imu = false;
    bool with_stereo = false;

    void print() {
        std::cout << "LoopClosure: " << with_loop_closure
                  << " IMU: " << use_imu
                  << " Stereo: " << with_stereo << "\n";
    }
};

// Builder
class SLAMBuilder {
    SLAMPipeline slam_;
public:
    SLAMBuilder& withLoopClosure(bool val) { slam_.with_loop_closure = val; return *this; }
    SLAMBuilder& withIMU(bool val)         { slam_.use_imu = val;           return *this; }
    SLAMBuilder& withStereo(bool val)      { slam_.with_stereo = val;       return *this; }
    SLAMPipeline build()                   { return slam_; }
};

// Usage
SLAMPipeline p = SLAMBuilder()
    .withLoopClosure(true)
    .withIMU(true)
    .withStereo(false)
    .build();
```

**Key points:**
- Each setter returns `*this` — enables method chaining (fluent interface)
- `build()` returns the final object
- Product and Builder are separate classes
- Caller only sets what they need — everything else stays at default

**SLAM use case:** Build lightweight vs high-accuracy pipelines from the same builder by toggling flags.

---

## Adapter Pattern

**Intent:** Wrap an incompatible interface so it works with your existing code — like a power plug adapter.

**When to use:** You have a legacy class with the wrong function name/interface and can't modify it.

```
Your system → expects getData()
                    ↓
               [GPSAdapter]       ← you write this
                    ↓
          LegacyGPS.readRAW()     ← can't touch this
```

```cpp
// What your system expects
class SensorInterface {
public:
    virtual std::string getData() = 0;
};

// Legacy class — can't modify
class LegacyGPS {
public:
    std::string readRAW() { return "$GPGGA, 123456..."; }
};

// Adapter — inherits new interface, wraps old class
class GPSAdapter : public SensorInterface {
    LegacyGPS legacy_gps_;
public:
    std::string getData() override { return legacy_gps_.readRAW(); }
};

// Usage — caller only knows SensorInterface
SensorInterface* sensor = new GPSAdapter();
std::cout << sensor->getData() << "\n";
```

**Key points:**
- Adapter inherits from the new interface (`SensorInterface`)
- Adapter holds the legacy object as a member
- Caller has no idea `LegacyGPS` exists underneath
- No modification to legacy code needed

**SLAM use case:** Integrate a legacy GPS module with a different interface into your sensor framework without touching the original code.

---
