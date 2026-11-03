# Arask
## Project overview and language syntax

Arask is a general-purpose programming language designed for native performance, predictable memory use, hardware-aware programming, fast development cycles, and a strong developer experience.

The language combines a readable syntax with low-level control. Its compiler is designed around a shared language frontend and an LLVM-based native-code backend. Arask is intended for systems software, applications, tools, networking, graphics, numerical computing, and other performance-sensitive software.

## Language design

Arask is built around these ideas:

- **Native performance:** compile programs to native machine code.
- **Predictable memory:** give programmers clear control over allocation, ownership, references, and resource lifetimes.
- **Hardware awareness:** support SIMD, parallel execution, and data-oriented programming.
- **Fast compilation:** use incremental compilation, dependency tracking, caching, and parallel compilation to avoid rebuilding unaffected code.
- **Compiler intelligence:** analyze dependencies and changes so the compiler can determine what actually needs to be rechecked, recompiled, and relinked.
- **Readable code:** keep common programming constructs straightforward and consistent.
- **Cross-platform development:** target macOS, Windows, and Linux through platform-specific code generation and linking.

## Basic program structure

```arask
fn main() {
    cout("Hello, Arask!");
}
```

Executable programs start at `main`. A program can define additional functions and types outside `main`.

## Variables and constants

```arask
let count = 10;
let age: i32 = 19;
let ratio: f64 = 0.75;

const limit = 100;
const title = "Arask";
```

- `let` declares a mutable variable.
- `const` declares an immutable binding.
- A type can be inferred or written explicitly after `:`.
- Assignment changes a mutable variable.

```arask
let total = 10;
total = 25;
```

## Built-in and user-defined types

### Primitive types

```arask
let signed: i32 = -10;
let large: i64 = 900000;
let decimal: f32 = 1.5;
let precise: f64 = 3.14159;
let active: bool = true;
let letter: char = 'A';
let message: String = "Hello";
```

Integer and floating-point widths, signedness, and additional numeric types can be expressed through type names such as `i32`, `i64`, `f32`, and `f64`.

### Structs

```arask
struct Point {
    x: i32,
    y: i32,
}

let origin = Point { x: 0, y: 0 };
cout(origin.x);
```

### Arrays

```arask
let numbers = [10, 20, 30];
cout(numbers[0]);
cout(numbers.length);
```

### Pointers and references

```arask
let value = 10;
let ptr: *i32 = &value;
cout(*ptr);
```

- `&value` obtains an address/reference to a value.
- `*i32` describes a pointer to an `i32`.
- `*ptr` dereferences a pointer.

## Functions

```arask
fn add(a: i32, b: i32) -> i32 {
    return a + b;
}

fn greet(name: String) {
    cout("Hello");
    cout(name);
}

fn main() {
    let result = add(10, 20);
    cout(result);
    greet("Arask");
}
```

Function parameters have declared types. `-> Type` specifies a return type. A function without an explicit return type does not return a value.

## Operators and expressions

### Arithmetic

```arask
let a = 10 + 5;
let b = 10 - 5;
let c = 10 * 5;
let d = 10 / 5;
let r = 10 % 3;
```

Operators: `+`, `-`, `*`, `/`, `%`.

### Comparison

```arask
a == b;
a != b;
a < b;
a <= b;
a > b;
a >= b;
```

Operators: `==`, `!=`, `<`, `<=`, `>`, `>=`.

### Logical

```arask
let result = (a > 0) && (b > 0);
let either = (a == 0) || (b == 0);
let opposite = !active;
```

Operators: `&&`, `||`, `!`.

### Assignment

```arask
value = 10;
value += 5;
value -= 2;
value *= 3;
value /= 2;
value %= 4;
```

Compound assignment operators: `+=`, `-=`, `*=`, `/=`, `%=`.

Bitwise compound forms:

```arask
value &= mask;
value |= mask;
value ^= mask;
value <<= 1;
value >>= 1;
```

### Increment and decrement

```arask
i++;
i--;
```

### Unary operators

```arask
let negative = -value;
let positive = +value;
let address = &value;
let original = *ptr;
```

### Operator grouping

Parentheses control evaluation order:

```arask
let result = (a + b) * c;
```

## Conditions

```arask
if (score >= 50) {
    cout("Pass");
} else {
    cout("Retry");
}
```

A condition can use comparison and logical expressions:

```arask
if (age >= 18 && active) {
    cout("Allowed");
} else {
    cout("Not allowed");
}
```

## Loops

Arask uses both compact range loops and controlled loops.

### Ascending range — exclusive end

```arask
for (let i = 0, 5) {
    cout(i);
}
```

Values: `0, 1, 2, 3, 4`.

### Ascending range — inclusive end

```arask
for (let i = 0, =5) {
    cout(i);
}
```

Values: `0, 1, 2, 3, 4, 5`.

### Descending range — exclusive end

```arask
for (let i = 5, 0) {
    cout(i);
}
```

Values: `5, 4, 3, 2, 1`.

### Descending range — inclusive end

```arask
for (let i = 5, =0) {
    cout(i);
}
```

Values: `5, 4, 3, 2, 1, 0`.

### Custom step

```arask
for (let i = 0, 20, 2) {
    cout(i);
}
```

Values: `0, 2, 4, ... 18`.

The step is a positive magnitude. The direction is inferred from the start and end values.

### Descending custom step

```arask
for (let i = 20, 0, 5) {
    cout(i);
}
```

Values: `20, 15, 10, 5`.

### Inclusive custom-step range

```arask
for (let i = 20, =0, 5) {
    cout(i);
}
```

Values: `20, 15, 10, 5, 0`.

An inclusive endpoint is included only when the step reaches it.

### Runtime bounds

```arask
let limit = 10;

for (let i = 0, limit) {
    cout(i);
}
```

### Controlled loop

```arask
for (let i = 1; i < 100; i++) {
    cout(i);
}
```

### Controlled loop with an expression update

```arask
for (let i = 1; i < 100; i = i * 2) {
    cout(i);
}
```

### Multiple initialization/update variables

```arask
for (let i = 0, j = 10; i < 10; i++, j--) {
    cout(i);
    cout(j);
}
```

### Nested loops

```arask
for (let i = 0, 3) {
    for (let j = 0, 3) {
        cout(i * j);
    }
}
```

### Break

```arask
for (let i = 0, 10) {
    if (i == 5) {
        break;
    }
    cout(i);
}
```

### Continue

```arask
for (let i = 0, 5) {
    if (i == 2) {
        continue;
    }
    cout(i);
}
```

### Return from a function

```arask
fn find_value() -> i32 {
    for (let i = 0, 10) {
        if (i == 5) {
            return i;
        }
    }
    return -1;
}
```

### Index-based collection iteration

```arask
let numbers = [10, 20, 30];

for (let i = 0, numbers.length) {
    cout(numbers[i]);
}
```

### Element iteration

```arask
for (let item : numbers) {
    cout(item);
}
```

### Reference iteration

```arask
for (let &item : numbers) {
    item += 1;
}
```

### Labeled loops

```arask
outer: for (let i = 0, 5) {
    for (let j = 0, 5) {
        if (i * j == 6) {
            break outer;
        }
    }
}
```

### Custom iterators

```arask
for (let item : primeSequence) {
    cout(item);
}
```

### Floating-point ranges

```arask
for (let x = 0.0, =1.0, 0.1) {
    cout(x);
}
```

Floating-point iteration needs defined precision and termination rules.

### Parallel loops

```arask
parallel for (let i = 0, 1000000) {
    results[i] = calculate(i);
}
```

Parallel execution requires clear rules for shared data, data races, and synchronization.

### SIMD loops

```arask
simd for (let i = 0, 1024) {
    output[i] = a[i] + b[i];
}
```

### Automatic loop vectorization

Ordinary loops can be analyzed by the compiler and vectorized when the operation is safe and profitable.

### Loop rules

- Range bounds are evaluated once when the loop begins.
- A range without `=` before the end value excludes the endpoint.
- A range with `=` before the end value includes the endpoint if reached.
- The step is a positive magnitude; direction follows the range.
- A zero step is invalid.
- Equal start and end values produce zero iterations for an exclusive range and one iteration for an inclusive range.
- Integer overflow must not silently create an infinite loop.
- Nested loops are supported.
- `break`, `continue`, and `return` control loop execution.
- Index, element, and reference iteration use the same underlying loop model.

## Output

```arask
cout("Hello");
cout(100);
cout(total);
```

`cout` is the language's output construct.

## Modules and imports

```arask
import math;
import networking;

fn main() {
    let result = math.sqrt(25.0);
    cout(result);
}
```

Modules organize code and allow programs to use reusable libraries.

## Memory, resources, and concurrency

Arask's design includes explicit pointers and references, predictable resource lifetimes, and hardware-aware execution. The language is intended to support ownership and borrowing concepts, resource-aware types, parallel tasks, and deterministic concurrency.

## Incremental compilation and compiler intelligence

Incremental compilation is a core part of Arask's compiler design. The compiler should avoid rebuilding an entire project when only a small part has changed.

The intended compilation workflow includes:

- **Dependency tracking:** record relationships between modules, files, declarations, and generated outputs.
- **Change detection:** identify which source files and declarations have changed.
- **Selective recompilation:** recompile only the affected parts of a project rather than every source file.
- **Incremental semantic analysis:** reuse valid results from earlier analysis and repeat type checking or other semantic work only where changes require it.
- **Compilation caching:** retain reusable intermediate results and compiled artifacts when their inputs and compiler settings have not changed.
- **Dependency-aware invalidation:** when a declaration or interface changes, rebuild the code that depends on it while keeping unrelated outputs.
- **Parallel compilation:** compile independent modules or compilation units concurrently when their dependencies allow it.
- **Incremental linking:** where the platform and output format permit it, avoid repeating unnecessary linking work.

The compiler must invalidate cached results whenever relevant source code, imported interfaces, compiler options, target settings, or dependencies change. Correctness takes priority over reusing stale results.

The goal is to make edit-build-test cycles fast in both small and large projects. Performance should be measured using clean-build time, no-change rebuild time, and rebuild time after changing one file or module.

## Compiler and ecosystem

The Arask compiler is designed with a language frontend, an intermediate representation, optimization passes, and a native-code backend using LLVM. The architecture supports target-specific code generation and platform linking.

The Arask ecosystem is intended to provide a standard library, package manager, development tools, and a collection of reusable, general-purpose libraries and engines. These engines are intended to give developers ready-to-use building blocks instead of requiring them to repeatedly create complex infrastructure from scratch.

### Engine and library ecosystem

- **Graphics engine:** rendering, shaders, graphics resources, and visual applications.
- **Physics engine:** collision detection, rigid-body simulation, and physical interactions.
- **Networking engine:** sockets, protocols, asynchronous communication, and network services.
- **Math engine:** linear algebra, numerical methods, geometry, and SIMD-accelerated operations.
- **Database engine:** storage, indexing, data management, and query execution.
- **AI/ML engine:** tensor operations, model execution, and machine-learning inference.
- **Audio engine:** audio processing, mixing, playback, and real-time audio workloads.
- **Parallel computing engine:** thread pools, task scheduling, and parallel workload execution.

These engines are ecosystem components designed to work with Arask. They are not separate language features; they are reusable libraries and development tools built around the language.

### Vectra — optional separate project

Vectra is a separate vector-search project, not the purpose or core component of Arask. If Arask and its ecosystem mature, Vectra may optionally become a project that uses Arask or integrates with its libraries and tooling. Arask does not depend on Vectra.

## Example program

```arask
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
    const limit = 3;
    let total = 0;
    let origin = Point { x: 0, y: 0 };

    for (let i = 1, =limit) {
        let p = Point { x: i, y: i };
        total += distance_sq(origin, p);
    }

    if (total > 20) {
        cout("big");
    } else {
        cout("small");
    }

    let ptr: *i32 = &total;
    cout(*ptr);
}
```

## Syntax reference

| Area | Syntax |
|---|---|
| Function | `fn name(args) -> Type { ... }` |
| Entry point | `fn main() { ... }` |
| Mutable variable | `let name = value;` |
| Immutable binding | `const name = value;` |
| Type annotation | `let name: Type = value;` |
| Struct | `struct Name { field: Type }` |
| Struct value | `Name { field: value }` |
| Function call | `name(args)` |
| Return | `return expression;` |
| Output | `cout(expression);` |
| Condition | `if (condition) { ... } else { ... }` |
| Range loop | `for (let i = start, end) { ... }` |
| Inclusive range | `for (let i = start, =end) { ... }` |
| Stepped range | `for (let i = start, end, step) { ... }` |
| Controlled loop | `for (let i = start; condition; update) { ... }` |
| Element iteration | `for (let item : collection) { ... }` |
| Reference iteration | `for (let &item : collection) { ... }` |
| Break / continue | `break;` / `continue;` |
| Array access | `array[index]` |
| Pointer type | `*Type` |
| Address-of | `&value` |
| Dereference | `*pointer` |
| Assignment | `=` |
| Compound assignment | `+=`, `-=`, `*=`, `/=`, `%=` |
| Bitwise compound assignment | `&=`, `|=`, `^=`, `<<=`, `>>=` |
| Increment / decrement | `++`, `--` |
| Arithmetic | `+`, `-`, `*`, `/`, `%` |
| Comparison | `==`, `!=`, `<`, `<=`, `>`, `>=` |
| Logical | `&&`, `||`, `!` |
| Field access | `value.field` |
| Import | `import module;` |

## Arask project scope and feature overview

The following overview summarizes the language design, compiler capabilities, research directions, ecosystem, engines, and project quality requirements. Inclusion here describes the intended project scope; it does not imply that every feature is already implemented.

### Language foundation

- General-purpose programming language
- Readable syntax with low-level control
- Native machine-code compilation
- Cross-platform support for macOS, Windows, and Linux
- Consistent language semantics across platforms
- Modular language and compiler architecture

### Language syntax and features

**Variables and types**

- Mutable variables (`let`)
- Immutable bindings (`const`)
- Type inference
- Explicit type annotations
- Signed integer types (`i32`, `i64`)
- Floating-point types (`f32`, `f64`)
- Boolean type
- Character and string types
- Arrays and collections
- User-defined structures (`struct`)
- Generics
- Enums and additional user-defined types

**Functions**

- Function declarations (`fn`)
- Typed parameters
- Return types
- Function calls
- Return statements
- `main` entry point
- Recursion
- Function overloading (design consideration)

**Operators**

- Arithmetic: `+`, `-`, `*`, `/`, `%`
- Comparison: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Logical: `&&`, `||`, `!`
- Assignment: `=`
- Compound assignment: `+=`, `-=`, `*=`, `/=`, `%=`
- Bitwise: `&`, `|`, `^`, `~`
- Bitwise shifts: `<<`, `>>`
- Bitwise compound assignment: `&=`, `|=`, `^=`, `<<=`, `>>=`
- Increment and decrement: `++`, `--`
- Unary operators
- Operator precedence and associativity
- Single evaluation of compound-assignment targets

**Conditions and control flow**

- `if`
- `else`
- Nested conditions
- `break`
- `continue`
- `return`
- Lexical variable scoping
- Error handling and exceptions (design decision)

### Complete loop system

**Range loops**

- Ascending range — exclusive endpoint
- Ascending range — inclusive endpoint
- Descending range — exclusive endpoint
- Descending range — inclusive endpoint
- Custom step
- Descending custom step
- Inclusive custom-step range
- Runtime-defined bounds
- Equal start/end behaviour
- Zero-step rejection
- Overflow-safe iteration

**Controlled loops**

- Traditional initialization, condition, and update
- Arbitrary update expressions
- Multiplication and division updates
- Multiple initialization variables
- Multiple update expressions
- Nested loops

**Loop control**

- `break`
- `continue`
- `return` inside loops
- Labeled loops
- Labeled break
- Correct loop-variable scope

**Collection iteration**

- Index-based iteration
- Element iteration using `:`
- Reference iteration using `&`
- Collection length support
- Custom iterators

**Advanced loops**

- Floating-point ranges
- Parallel loops
- SIMD loops
- Automatic loop vectorization

**Loop compiler requirements**

- Common internal loop representation
- Consistent semantics between interpreter and native backend
- Correct endpoint handling
- Safe integer overflow behaviour
- Correct nested-loop execution
- Comprehensive loop tests

### Memory management

- Pointer support
- Reference support
- Address-of operator
- Dereference operator
- Ownership model
- Borrowing model
- Resource-aware types
- Predictable resource lifetimes
- Region-based memory management
- Memory allocation strategies
- Explicit resource management
- No mandatory garbage collector

### Compiler architecture

**Core compiler**

- Lexer
- Parser
- Abstract Syntax Tree (AST)
- Semantic analysis
- Symbol tables
- Type checker
- Intermediate Representation (IR)
- Optimization passes
- LLVM integration
- Native machine-code generation
- Platform-specific linking

**Incremental compilation**

- Dependency tracking
- Source change detection
- Selective recompilation
- Incremental semantic analysis
- Incremental type checking
- Compilation caching
- Dependency-aware cache invalidation
- Parallel compilation
- Incremental linking where supported
- Reuse of valid intermediate results
- Fast no-change rebuilds

**Compiler intelligence**

- Static analysis
- Useful compiler diagnostics
- Actionable error messages
- Performance recommendations
- Dependency analysis
- Optimization diagnostics
- Profiling information

### Performance and hardware awareness

- Native performance
- SIMD support
- Automatic vectorization
- Hardware-specific optimization
- Data-oriented programming
- Cache-aware data layouts
- Parallel execution
- Efficient memory allocation
- Low runtime overhead
- Benchmarking infrastructure
- Performance regression testing

### Nine future research features

These are research directions to investigate, not promises that every feature is novel or will necessarily be adopted.

- **Adaptive memory allocation** — choose allocation strategies based on workload.
- **Compile-time performance checks** — detect certain expensive operations before execution.
- **Resource-aware types** — track memory, I/O, and blocking effects.
- **Automatic SIMD specialization** — generate hardware-specific implementations.
- **Adaptive task scheduler** — optimize parallel workload distribution.
- **Incremental type checking** — avoid repeating unaffected semantic analysis.
- **Deterministic concurrency** — make selected concurrent operations reproducible.
- **Smart binary generation** — remove unnecessary runtime and library components.
- **Hardware-aware data layouts** — optimize structures for cache and SIMD access.

### Concurrency and parallel computing

- Thread support
- Task-based concurrency
- Thread pools
- Task scheduling
- Parallel loops
- Synchronization primitives
- Atomic operations
- Race detection or analysis
- Deterministic concurrency
- Adaptive task scheduling

### Developer ecosystem

- Standard library
- Package manager
- Module system
- Import system
- Build system
- Compiler command-line interface
- Debugging support
- IDE integration
- Language server
- Formatter
- Documentation tooling
- Testing framework
- Dependency management

### Arask engine ecosystem

The Arask ecosystem is intended to provide reusable, general-purpose engines and libraries that developers can use as building blocks.

**Graphics engine**
- Rendering
- Shaders
- Graphics resources
- Visual applications

**Physics engine**
- Collision detection
- Rigid-body simulation
- Physical interactions

**Networking engine**
- Sockets
- Protocols
- Asynchronous communication
- Network services

**Math engine**
- Linear algebra
- Numerical computing
- Geometry
- SIMD operations

**Database engine**
- Storage
- Indexing
- Data management
- Query execution

**AI/ML engine**
- Tensor operations
- Model execution
- Machine-learning inference

**Audio engine**
- Audio processing
- Mixing
- Playback
- Real-time audio

**Parallel computing engine**
- Thread pools
- Task scheduling
- Parallel execution

### Cross-platform support

- macOS ARM64
- macOS x86-64
- Windows x86-64
- Windows ARM64
- Linux x86-64
- Linux ARM64
- Platform-specific linker support
- Target-specific system libraries
- Cross-compilation support

### Optional separate project: Vectra

Vectra is an independent vector-search project. It is not a required Arask component or dependency.

- Keep Vectra as a separate project
- Explore optional Arask integration
- Explore using Arask libraries in Vectra
- Maintain independent development and functionality

### Quality, testing, and release

- Lexer tests
- Parser tests
- AST tests
- Type-checker tests
- Interpreter tests
- Loop tests
- Compiler backend tests
- Cross-platform tests
- Standard library tests
- Benchmark suite
- Regression tests
- Documentation
- Versioning and releases

### Immediate development focus

The next development session is dedicated to completing the loop system before moving on to function implementation:

- Range-loop parsing
- Inclusive and exclusive endpoints
- Ascending and descending ranges
- Custom steps
- Runtime bounds
- Controlled loop updates
- Nested loops
- `break` and `continue`
- Scoping and overflow behaviour
- Collection iteration
- Multiple updates and labeled loops
- Tests for every supported form

A feature is considered complete only when its syntax is defined, its semantics are clear, it is implemented, and it has tests. A single successful example is not enough.
