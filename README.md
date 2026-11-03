# Arask

**A cross-platform, high-performance programming language and compiler ecosystem.**

Arask is an experimental general-purpose programming language project focused on native performance, predictable memory management, fast compilation, and an excellent developer experience.

The long-term goal is to build a modern systems programming language with a powerful compiler architecture and a reusable ecosystem.

> **Status:** Early development — currently includes a lexer, arithmetic expression parser, and a basic tree-walking interpreter.

## Vision

Arask aims to bring together five core engineering goals:

- **Fast compilation:** Incremental compilation, dependency tracking, and parallel builds.
- **Predictable memory:** Explore ownership, borrowing, and efficient memory management without mandatory garbage collection.
- **Hardware-aware execution:** SIMD optimization, parallel execution, and data-oriented programming.
- **Developer experience:** Clear diagnostics, useful tooling, and a streamlined development workflow.
- **Native performance:** Optimized native machine code with minimal runtime overhead.

These are long-term design goals, not claims of currently implemented capabilities.

## Current Features

| Component | Status |
|---|---|
| Lexer and tokenization | Implemented |
| Parser and AST | Early implementation |
| Arithmetic expressions | Implemented |
| Operator precedence | Implemented |
| Unary `+` and `-` | Implemented |
| Integer variables | Basic implementation |
| Variable lookup | Implemented |
| Sequential statement execution | Implemented |
| `cout()` output | Implemented |
| `.ara` file execution | Implemented |
| Automated tests | Implemented |
| Semantic analysis | Planned |
| LLVM integration | Planned |
| Native code generation | Planned |
| Incremental compilation | Planned |
| Parallel compilation | Planned |

## Example

An early Arask program:

```rust
let a = 10;
let b = 20;
let result = a + b;

cout(result);
```

Output:

```text
30
```

## Project Structure

```text
Arask/
├── compiler/
│   ├── ast/
│   ├── interpreter/
│   ├── lexer/
│   ├── parser/
│   ├── semantic/
│   └── main.cpp
├── examples/
│   └── hello.ara
├── include/
│   └── newlang/
├── tests/
│   ├── parser/
│   ├── lexer_tests.cpp
│   └── interpreter_tests.cpp
├── docs/
├── CMakeLists.txt
└── README.md
```

## Building from Source

### Requirements

- C++20 compatible compiler
- CMake 3.20 or newer
- Git

### Build

Clone the repository:

```bash
git clone https://github.com/AyushAnasane/Arask.git
cd Arask
```

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

### Run an Arask program

```bash
./build/newlang examples/hello.ara
```

## Running Tests

Run the automated test suite:

```bash
ctest --test-dir build --output-on-failure
```

The current test suites cover lexer, parser, and interpreter functionality.

## Cross-Platform Direction

Arask is being designed with cross-platform compilation in mind.

Planned targets:

- macOS — Apple Silicon and Intel
- Windows — x86-64 and ARM64
- Linux — x86-64 and ARM64

LLVM is being considered as the eventual native code-generation backend.

Cross-platform native compilation is a design goal and is not implemented yet.

## Development Roadmap

1. Complete the language grammar and core syntax.
2. Expand expression and statement support.
3. Implement semantic analysis and type checking.
4. Design a compiler intermediate representation.
5. Integrate LLVM for native code generation.
6. Develop incremental and parallel compilation.
7. Explore advanced memory and hardware-aware optimizations.
8. Build a standard library and developer tooling ecosystem.

## Philosophy

Arask is being developed around a few principles:

- Modular compiler architecture.
- Stable language semantics.
- Benchmark-driven optimization.
- Clear and actionable compiler diagnostics.
- Experimental features backed by testing.
- Cross-platform design from the beginning.

## License

License to be determined.
