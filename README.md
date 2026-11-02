# NewLang

**An experimental high-performance programming language and compiler built in C++.**

NewLang is an independent programming language project focused on exploring how modern compiler engineering, hardware-aware execution, predictable memory management, and developer experience can come together in one language ecosystem.

The long-term goal is to build a language that offers compelling reasons to choose it over established systems languages through measurable performance, efficient compilation, and powerful developer tools.

> NewLang is currently in early development. Its compiler prototype includes a lexer, basic parser, and AST infrastructure. Many language features and optimization goals are still under development or research.

---

## Vision

The goal of NewLang is not simply to create another programming language with different syntax.

It is to explore a language and compiler ecosystem designed around five core engineering goals:

### 1. Fast Compilation
Explore incremental compilation, dependency tracking, and parallel compilation to reduce build times as projects grow.

### 2. Predictable Memory
Investigate ownership, borrowing, and region-based memory management to provide predictable memory behavior without requiring a garbage collector.

### 3. Hardware-Aware Execution
Explore SIMD, parallel execution, and data-oriented programming to make better use of modern hardware.

### 4. Developer Experience
Build a compiler with helpful diagnostics, efficient development workflows, and straightforward language design.

### 5. Native Performance
Target optimized native machine code with minimal runtime overhead.

---

## Language Design

NewLang is being designed as a statically typed, general-purpose programming language.

The proposed syntax takes inspiration from modern systems languages while aiming to remain approachable.

### Example

```rust
struct Point {
    x: i32,
    y: i32,
}

fn distance_sq(a: Point, b: Point) -> i32 {
    let dx = a.x - b.x;
    let dy = a.y - b.y;
    return dx * dx + dy * dy;
}

fn main() {
    let origin = Point { x: 0, y: 0 };
    let point = Point { x: 3, y: 4 };

    print(distance_sq(origin, point));
}
```

*Illustrative language design example. The complete syntax shown here is not yet implemented in the compiler.*

---

## Compiler Architecture

NewLang is being developed in C++20 with a modular compiler architecture.

The planned compilation pipeline is:

```text
       NewLang Source Code
                |
                v
              Lexer
                |
                v
              Parser
                |
                v
          Abstract Syntax Tree
                |
                v
        Semantic Analysis
                |
                v
          Intermediate IR
                |
                v
          Optimization
                |
                v
        Native Code Generation
                |
                v
          Native Executable
```

The architecture is intended to keep compiler components modular so that new language features and optimization passes can be introduced without repeatedly redesigning the entire compiler.

---

## Current Development Status

NewLang is in its early compiler-development stage.

| Component | Status |
|---|---|
| C++20 project setup | Implemented |
| Lexer | Initial implementation |
| Abstract Syntax Tree | Initial implementation |
| Basic parser | Initial implementation |
| Semantic analysis | Planned |
| Type checking | Planned |
| Intermediate representation | Planned |
| Optimization pipeline | Planned |
| Native code generation | Planned |
| Standard library | Planned |

The current priority is to establish a reliable compiler foundation before introducing advanced language features.

---

## Research Directions

Beyond the core engineering goals, NewLang explores several potential compiler capabilities:

- Adaptive memory allocation
- Compile-time performance analysis
- Resource-aware types
- Automatic SIMD specialization
- Adaptive task scheduling
- Incremental type checking
- Deterministic concurrency
- Optimized binary generation
- Hardware-aware data layouts

These are experimental research directions, not guaranteed language features. Each will be evaluated based on practical usefulness, implementation complexity, and measurable results.

---

## Ecosystem Vision

The long-term vision extends beyond the compiler itself.

NewLang aims to develop a modular ecosystem of reusable, high-performance engines and libraries, potentially including:

- Graphics and rendering
- Physics simulation
- Networking
- Mathematical computing
- Database infrastructure
- AI and machine learning
- Audio processing
- Parallel computing

The objective is to give developers access to powerful reusable infrastructure rather than requiring them to rebuild complex systems for every project.

---

## Technology Stack

- **Language:** C++20
- **Build System:** CMake
- **Compiler Backend:** LLVM (planned)
- **Development Environment:** Visual Studio Code

---

## Building the Project

Clone the repository:

```bash
git clone https://github.com/AyushAnasane/NewLang.git
cd NewLang
```

Configure the project:

```bash
cmake -S . -B build
```

Build:

```bash
cmake --build build
```

Run the current compiler prototype:

```bash
./build/newlang
```

---

## Project Philosophy

NewLang follows a few development principles:

- **Modularity:** Keep compiler components independent and extensible.
- **Measured performance:** Validate optimization claims through benchmarks.
- **Stability:** Avoid breaking existing functionality unnecessarily.
- **Experimentation:** Test ambitious ideas before making them permanent.
- **Practicality:** Prioritize features that solve real engineering problems.

---

## Project Status

NewLang is an independent, experimental project under active development.

The immediate objective is to build a working compiler pipeline, establish the language's core semantics, and progressively work toward native code generation.

**The long-term ambition is to build a language ecosystem that earns its place alongside established systems programming languages.**

---

*Created and maintained by [Ayush Anasane](https://github.com/AyushAnasane).*  
