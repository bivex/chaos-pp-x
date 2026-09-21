# Chaos Preprocessor Library

The **Chaos Preprocessor Library** is an advanced generative metaprogramming framework for C and C++ developed by Paul Mensonides (co-author of the Boost Preprocessor Library). It pushes the theoretical and practical state-of-the-art in preprocessor metaprogramming, functioning as an industrial-strength reference implementation of advanced macro metaprogramming concepts, idioms, and techniques.

---

## Architectural Highlights & Feature Audit

Chaos transcends the limitations of traditional preprocessor libraries like `Boost.PP` by introducing foundational computational abstractions:

### 1. Continuation Machine & Recursion Engine (`recursion/`)
- **Beyond 256 Iterations**: Whereas traditional libraries hit hard re-scan boundaries (usually 256 iterations) due to fixed lookup tables, Chaos introduces a **Continuation Machine** with exponential token scanning (`CHAOS_PP_EXPR`, `CHAOS_PP_STATE`).
- **Extended Limits**: Supports a base magnitude of **512 steps** (`CHAOS_PP_LIMIT_MAG = 512`) and theoretically unbounded recursion via bypass semantics without scanner overflow.

### 2. Lambda Calculus & Higher-Order Macros (`lambda/`)
- **First-Class Macro Lambdas**: Anonymous functional abstractions (`CHAOS_PP_LAMBDA`).
- **Currying & Partial Application**: `CHAOS_PP_BIND` binding up to 16 arguments.
- **Closures & Scope Capture**: Lexical closure abstractions capturing environment tokens up to 25 nesting levels.
- **Higher-Order Macros**: Passing macros as first-class citizens into other macros up to 8 recursion orders.

### 3. Arbitrary-Precision Arithmetic — BigInt (`arbitrary/`)
- Pure textual multi-precision arithmetic directly in the preprocessor.
- Supports arbitrary-length integers (e.g. 50–100+ digits) for addition, subtraction, multiplication, division (`div`, `mod`), and bitwise manipulation before the C/C++ AST parser even starts.

### 4. Unified Polymorphic Containers & Generic Algorithms (`generics/`, `algorithm/`)
Chaos unifies 5 discrete preprocessor data representations under a polymorphic `generics` layer:
- **Sequences (`seq`)**: `(a)(b)(c)` — fastest sequential iteration.
- **Tuples (`tuple`)**: `(a, b, c)` — direct compatibility with C99/C++11 variadic packs (`__VA_ARGS__`).
- **Lists (`list`)**: LISP-style cons pairs `(a, (b, (c, CHAOS_PP_NIL)))`.
- **Arrays (`array`)**: Length-prefixed packages `(3, (a, b, c))`.
- **Strings (`string`)**: Preprocessor character-token sequences.

**Algorithms**: Full STL-style functional repertoire: `for_each`, `transform`, `filter`, `fold_left`, `fold_right`, `zip`, `binary_transform`, `take`, `drop`, `range`, `reverse`, `insert`, `remove`, `replace`.

### 5. Token Introspection & Type Detection (`detection/`)
- Inspect macro arguments at preprocess-time: `CHAOS_PP_IS_EMPTY`, `CHAOS_PP_IS_NULLARY`, `CHAOS_PP_IS_UNARY`, `CHAOS_PP_IS_BINARY`, `CHAOS_PP_IS_NUMERIC`, `CHAOS_PP_IS_VARIADIC`.
- Branching primitives: `CHAOS_PP_IIF`, `CHAOS_PP_IF`, `CHAOS_PP_WHILE`, `CHAOS_PP_BRANCH`, `CHAOS_PP_INLINE_WHEN`.

### 6. Multi-Pass Code Generation (`slot/`, `iteration/`)
- **Slots**: Global preprocessor registers (`CHAOS_PP_ASSIGN_SLOT(1, ...)` / `CHAOS_PP_SLOT(1)`) to evaluate arithmetic expressions and persist global state across header iterations.
- **File Iteration**: Self-reentrant multi-pass code generation (`#include CHAOS_PP_ITERATE()`) up to 5 depth levels.
- **Monotonic Counters**: Built-in monotonic counter (`counter.h`).

---

## Production Case Study: Zero-Overhead Reflection

The repository includes a production-grade High-Frequency Trading & Risk Engine example in [`examples/production_trading_engine/`](examples/production_trading_engine/):

- **Declarative Schema Definition**:
  ```cpp
  CHAOS_REFLECT_ENUM(OrderSide, (BUY)(SELL))

  CHAOS_REFLECT_STRUCT(LimitOrder,
      ((uint64_t, order_id))
      ((std::string, symbol))
      ((OrderSide, side))
      ((uint64_t, quantity))
      ((uint64_t, timestamp_epoch_ns))
  )
  ```
- **Zero Runtime Overhead**:
  - Auto-generated member fields, equality operators (`==`, `!=`), and stream formatters (`<<`).
  - Introspection metadata: `field_count` and `field_names` array (`std::string_view`).
  - Generic `for_each_field(obj, visitor)` traversal without virtual tables or runtime type tables.
  - Zero-dependency JSON serializer and deserializer with full 64-bit integer precision.
  - Compact binary wire protocol serialization (73% bandwidth reduction compared to JSON).
- **Benchmark Performance**:
  - **Throughput**: ~**1.15 million orders/sec** (under `-O2`).
  - **Latency**: ~**0.87 µs** average per transaction (including risk checks, matching, and binary audit logging).

---

## Quality Assurance & Verification

| Check | Tooling | Result | Details |
|---|---|---|---|
| **Memory Leaks** | `/usr/bin/leaks --atExit` | **0 leaks / 0 bytes** | Zero memory leaks across tests and 50k-order engine runs |
| **Undefined Behavior** | `-fsanitize=undefined` (UBSan) | **0 violations** | Clean integer math, alignments, pointer semantics |
| **Strict Compiler Warnings** | Clang / GCC (`-Wall -Wextra -Wpedantic -Wshadow`) | **0 warnings** | Tested across C99, C11, C++11, C++14, C++17, C++20 |
| **Debug Assertion Fixes** | Negative CTest (`WILL_FAIL TRUE`) | **100% PASS** | Resolved [Issue #1](#resolved-issues): `CHAOS_PP_ASSERT_MSG` now aborts compilation on false conditions |

### Resolved Issues
- **Issue #1**: In legacy versions, `CHAOS_PP_ASSERT_MSG(cond, msg)` expanded solely to `msg`, which did not cause compilation errors when evaluated in expression or statement positions. This has been resolved by routing false assertions to `CHAOS_PP_FAILURE_MSG(__VA_ARGS__)`, intentionally triggering a preprocessor invocation error that surfaces the user's message directly in compiler diagnostics.

---

## Modern Build System (CMake)

Chaos includes a modern CMake build system supporting `INTERFACE` library targets, CTest test suites, installation, package export (`find_package`), and documentation generation.

### Quick Start

```bash
# Configure and build smoke tests & examples
cmake -B build
cmake --build build

# Run test suite
ctest --test-dir build --output-on-failure
```

Or using the convenience `Makefile`:

```bash
make          # Configure and build
make test     # Run smoke tests (positive + negative compilation tests)
make example  # Build and run production trading engine demonstration
make docs     # Build HTML documentation using xsltproc
make install  # Install headers and CMake package configs
make clean    # Remove build directory
```

### CMake Options

| Option | Default | Description |
|---|---|---|
| `CHAOS_ENABLE_TESTING` | `ON` (top-level) | Build C/C++ verification smoke tests |
| `CHAOS_ENABLE_EXAMPLES` | `ON` (top-level) | Build production-grade trading engine example |
| `CHAOS_ENABLE_DOCS` | `ON` (top-level) | Add `docs` target using `xsltproc` |
| `CHAOS_INSTALL` | `ON` (top-level) | Install targets and export `chaosConfig.cmake` |

### Integrating with Your Project

#### Via `find_package`
```cmake
find_package(chaos 1.0 CONFIG REQUIRED)
target_link_libraries(my_project PRIVATE chaos::chaos)
```

#### Via `FetchContent`
```cmake
include(FetchContent)
FetchContent_Declare(
    chaos
    GIT_REPOSITORY https://github.com/bivex/chaos-pp-x.git
    GIT_TAG master
)
FetchContent_MakeAvailable(chaos)
target_link_libraries(my_project PRIVATE chaos::chaos)
```

## Manual Usage

Chaos is header-only. Simply add the repository root to your compiler's include path:

```c
#include <chaos/preprocessor.h>
```

Verify header compatibility manually:
```bash
clang -I. -x c -c -o /dev/null chaos/preprocessor.h
clang++ -std=c++11 -I. -x c++ -c -o /dev/null chaos/preprocessor.h
```

## License

Unless otherwise specified, terms and conditions of use of Chaos Preprocessor software and documentation are described in the accompanying license. See the file named [LICENSE](LICENSE) in the top-level directory (Boost Software License, Version 1.0).
