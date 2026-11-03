# Arask

Arask is a high-performance programming language project focused on native performance, predictable memory use, hardware-aware programming, and a straightforward developer experience.

The goal is to combine readable syntax with low-level control, while building a compiler and ecosystem suitable for systems software, applications, tools, networking, graphics, and performance-sensitive workloads.

## Design goals

- **Native performance:** compile programs to native machine code.
- **Predictable memory:** provide clear control over memory, references, and resource lifetimes.
- **Hardware-aware programming:** make SIMD, parallel execution, and data-oriented programming accessible.
- **Fast development:** aim for efficient compilation, useful diagnostics, and practical tooling.
- **Cross-platform support:** target macOS, Windows, and Linux.
- **A reusable ecosystem:** support libraries and engines for common areas of software development.

## Language overview

Arask uses a C-like structure with its own syntax for declarations, functions, conditions, loops, and output.

```arask
fn main() {
    let total = 0;

    for (let i = 1, =5) {
        total += i;
    }

    if (total > 10) {
        cout("The total is greater than 10");
    } else {
        cout("The total is 10 or less");
    }

    cout(total);
}
```

### Variables and constants

```arask
let count = 10;
let age: i32 = 19;

const limit = 100;
```

`let` declares a mutable variable. `const` declares an immutable binding. Types can be inferred or written explicitly.

### Functions

```arask
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn main() {
    cout(add(10, 20));
}
```

### Structs

```arask
struct Point {
    x: i32,
    y: i32,
}

fn main() {
    let origin = Point { x: 0, y: 0 };
    cout(origin.x);
}
```

### Conditions

```arask
if (score >= 50) {
    cout("Pass");
} else {
    cout("Retry");
}
```

### Loops

Arask's language design includes compact range loops and traditional controlled loops.

```arask
for (let i = 0, 5) {
    cout(i);
}

for (let i = 0; =5) {
    cout(i);
}

for (let i = 1; i < 100; i = i * 2) {
    cout(i);
}
```

The comma-based range forms are part of the language design. The current prototype does not yet implement every form shown in the full syntax specification.

### Operators

Arithmetic: `+`, `-`, `*`, `/`, `%`

Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`

Compound assignment: `+=`, `-=`, `*=`, `/=`, `%=`

Increment and decrement: `++`, `--`

The complete syntax design also describes logical, bitwise, pointer, reference, collection, and advanced loop constructs.

## Engine and library ecosystem

A major part of Arask's direction is a collection of reusable, general-purpose libraries and engines. The aim is to give developers ready-to-use building blocks for common and technically demanding areas of software development.

- **Graphics engine:** rendering, shaders, graphics resources, and visual applications.
- **Physics engine:** collision detection, simulation, and physical interactions.
- **Networking engine:** sockets, protocols, asynchronous communication, and network services.
- **Math engine:** linear algebra, numerical methods, geometry, and SIMD operations.
- **Database engine:** storage, indexing, data management, and query execution.
- **AI/ML engine:** tensor operations, model execution, and machine-learning inference.
- **Audio engine:** audio processing, mixing, playback, and real-time workloads.
- **Parallel computing engine:** thread pools, task scheduling, and parallel execution.

These are intended to be reusable libraries and engines in the Arask ecosystem, not a claim that every engine is already implemented.

## Vectra (optional separate project)

Vectra is a separate vector-search project. It is not the purpose of Arask and is not required by the language. If Arask and its ecosystem mature, Vectra may optionally use Arask or integrate with its libraries and tooling.

## Compiler architecture

Arask is being developed in C++20 with CMake. The compiler architecture is organized around separate components:

- **Lexer:** converts source text into tokens.
- **Parser:** converts tokens into an abstract syntax tree.
- **AST:** represents expressions and statements.
- **Interpreter:** executes the currently supported subset of the language.
- **Native-code backend:** LLVM is the intended backend for generating optimized native machine code.

The long-term compiler design includes an intermediate representation, optimization passes, static analysis, and target-specific code generation.

## Current prototype

The current prototype contains an initial lexer, parser, AST, and interpreter. The supported subset includes basic integer expressions, variables, assignment, output through `cout`, comparisons, `if`/`else`, and basic `for` loops.

The full language design is broader than the current prototype. Features such as the complete type system, functions, structs, arrays, strings, memory-safety rules, all range-loop forms, LLVM code generation, and the package ecosystem are not represented as fully implemented compiler features yet.

## Build

Requirements:

- C++20 compiler
- CMake 3.20 or newer

From the project root:

```bash
cmake -S . -B build
cmake --build build
```

Run the test suite:

```bash
ctest --test-dir build --output-on-failure
```

Run a source file with the current executable:

```bash
./build/newlang examples/hello.ara
```

The executable and internal CMake target names may change as the project is renamed and developed.

## Example source file

Create a file named `hello.ara`:

```arask
let a = 10;
let b = 20;

cout(a + b);
```

Run it:

```bash
./build/newlang hello.ara
```

## Project documentation

See **Arask Project & Syntax** for the broader language design, including types, operators, pointers, references, control flow, loop forms, modules, compiler direction, and ecosystem concepts.

## Project direction

Arask aims to grow into a modular, cross-platform language and compiler. Its design emphasizes measurable performance, clear semantics, useful tooling, and a library ecosystem. Performance claims will need to be demonstrated with reproducible benchmarks against relevant alternatives.
