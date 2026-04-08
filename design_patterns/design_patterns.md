# Design Pattern Questions (Extracted)

### Day 1 — Apr 3 | Singleton ✅
**Design Pattern:** Singleton — lazy init, thread-safe variant, Meyers' singleton  
**SLAM Question:**  
In a SLAM system, how would you ensure that the global map manager is implemented as a singleton so that multiple threads (e.g., LiDAR and camera pipelines) do not create conflicting map instances?

---

### Day 2 — Apr 4 | Factory Method ✅
**Design Pattern:** Factory Method — virtual creator, concrete products, open/closed principle  
**SLAM Question:**  
How would you design a factory method to dynamically create different sensor processing modules (LiDAR, Stereo Camera, RGB-D) in a SLAM pipeline?

---

### Day 3 — Apr 5 | Abstract Factory ✅
**Design Pattern:** Abstract Factory — families of related objects, swappable factories  
**SLAM Question:**  
How would you implement an abstract factory to switch between different SLAM backends (e.g., ORB-SLAM vs RTAB-Map), each requiring its own feature extractor, matcher, and optimizer?

---

### Day 4 — Apr 6 | Builder ✅
**Design Pattern:** Builder — step-by-step construction, fluent interface, director class  
**SLAM Question:**  
How would you use the Builder pattern to construct configurable SLAM pipelines (e.g., lightweight vs high-accuracy) step by step?

---

### Day 5 — Apr 7 | Prototype ✅
**Design Pattern:** Prototype — clone interface, deep vs shallow copy, registry  
**SLAM Question:**  
How would you use the Prototype pattern to efficiently clone SLAM keyframes while handling deep vs shallow copies?

---

### Day 6 — Apr 8 | Adapter ✅
**Design Pattern:** Adapter — class adapter vs object adapter, legacy interface wrapping  
**SLAM Question:**  
How would you use the Adapter pattern to integrate a legacy GPS module with a different interface into your SLAM sensor framework?

---

### Day 7 — Apr 9 | Bridge
**Design Pattern:** Bridge — abstraction decoupled from implementation, composition over inheritance  
**SLAM Question:**  
How would you decouple SLAM visualization abstraction from different rendering engines (e.g., OpenGL, Vulkan) using the Bridge pattern?

---

### Day 8 — Apr 10 | Composite
**Design Pattern:** Composite — tree of uniform objects, leaf vs composite nodes  
**SLAM Question:**  
How would you represent a hierarchical SLAM map structure (map → regions → landmarks) using the Composite pattern?

---

### Day 9 — Apr 11 | Decorator
**Design Pattern:** Decorator — dynamic behavior wrapping, open/closed, stacking decorators  
**SLAM Question:**  
How would you dynamically add features like noise filtering or loop closure detection to a SLAM pipeline using the Decorator pattern?

---

### Day 10 — Apr 12 | Facade
**Design Pattern:** Facade — simplified interface to subsystem, hide complexity  
**SLAM Question:**  
How would you design a Facade that exposes a simple `runSLAM()` interface while hiding complex subsystems like sensor fusion and optimization?

---

### Day 11 — Apr 13 | Flyweight
**Design Pattern:** Flyweight — shared intrinsic state, extrinsic state separation  
**SLAM Question:**  
How would you use the Flyweight pattern to reduce memory usage when storing millions of similar landmarks in large-scale SLAM?

---

### Day 12 — Apr 14 | Proxy
**Design Pattern:** Proxy — virtual proxy, protection proxy, lazy initialization  
**SLAM Question:**  
How would you use a Proxy to lazily load large SLAM maps or restrict access to high-resolution map data?

---

### Day 13 — Apr 15 | Chain of Responsibility
**Design Pattern:** Chain of Responsibility — handler chain, pass or handle, logging pipeline  
**SLAM Question:**  
How would you implement a sensor data processing pipeline (noise removal → outlier rejection → normalization) using Chain of Responsibility?

---

### Day 14 — Apr 16 | Command
**Design Pattern:** Command — encapsulate action, undo/redo, command queue  
**SLAM Question:**  
How would you model robot movement commands in SLAM to support undo/redo functionality using the Command pattern?

---

### Day 15 — Apr 17 | Iterator
**Design Pattern:** Iterator — external iterator, range interface, custom container traversal  
**SLAM Question:**  
How would you design an iterator to traverse SLAM landmarks or keyframes using different strategies (e.g., time-based, spatial)?

---

### Day 16 — Apr 18 | Mediator
**Design Pattern:** Mediator — central hub reduces coupling, chat room example  
**SLAM Question:**  
How would you use a Mediator to manage communication between SLAM modules like mapping, localization, and loop closure?

---

### Day 17 — Apr 19 | Memento
**Design Pattern:** Memento — save/restore state, undo history, snapshot  
**SLAM Question:**  
How would you implement a Memento to save and restore SLAM states (map + robot pose) for rollback or debugging?

---

### Day 18 — Apr 20 | Observer
**Design Pattern:** Observer — event system, subscribe/notify, event bus  
**SLAM Question:**  
How would you implement an Observer pattern where multiple modules react to incoming sensor data in a SLAM system?

---

### Day 19 — Apr 21 | State
**Design Pattern:** State — state machine, context delegates to state, vending machine  
**SLAM Question:**  
How would you model robot states (initializing, tracking, lost, relocalizing) in SLAM using the State pattern?

---

### Day 20 — Apr 22 | Strategy ✅
**Design Pattern:** Strategy — interchangeable algorithms, sort strategies, payment methods  
**SLAM Question:**  
How would you dynamically switch between localization algorithms (EKF, Particle Filter, Graph-based) using the Strategy pattern?

---

### Day 21 — Apr 23 | Template Method
**Design Pattern:** Template Method — skeleton algorithm, hook methods, invariant steps  
**SLAM Question:**  
How would you define a template for SLAM pipelines where the overall structure is fixed but individual steps vary across algorithms?

---

### Day 22 — Apr 24 | Visitor
**Design Pattern:** Visitor — double dispatch, operations without modifying classes  
**SLAM Question:**  
How would you use the Visitor pattern to perform operations like visualization or optimization on SLAM map elements without modifying their classes?

---

### Day 23 — Apr 25 | Interpreter
**Design Pattern:** Interpreter — grammar → AST → evaluate, expression parser  
**SLAM Question:**  
How would you design an Interpreter to parse and evaluate a DSL for SLAM queries (e.g., “find landmarks near x,y”)?

---

### Day 24 — Apr 26 | MVC
**Design Pattern:** MVC — model/view/controller separation, event-driven UI  
**SLAM Question:**  
How would you design a SLAM visualization dashboard using MVC to separate map data, UI, and control logic?

---

### Day 25 — Apr 27 | Repository
**Design Pattern:** Repository — abstract data access, swap DB without changing business logic  
**SLAM Question:**  
How would you abstract SLAM data storage (maps, keyframes) using a Repository pattern to support multiple databases?

---

### Day 26 — Apr 28 | Dependency Injection
**Design Pattern:** Dependency Injection — constructor injection, interface segregation, testability  
**SLAM Question:**  
How would Dependency Injection improve flexibility and testing in a SLAM system with interchangeable components like sensor models and optimizers?

---

### Day 27 — Apr 29 | CQRS
**Design Pattern:** CQRS — separate read/write models, event sourcing intro, eventual consistency  
**SLAM Question:**  
How would you separate read-heavy (visualization) and write-heavy (map updates) operations in SLAM using CQRS?

---

### Day 28 — Apr 30 | Event-Driven / Pub-Sub
**Design Pattern:** Event-Driven / Pub-Sub — decoupled event bus, async message passing  
**SLAM Question:**  
How would you design an event-driven SLAM system where modules communicate asynchronously via a publish-subscribe mechanism?