# Chaos Preprocessor Library

The Chaos Preprocessor Library is a generative metaprogramming framework for C and C++ that builds on the techniques used in the Boost Preprocessor Library. It pushes the state-of-the-art in preprocessor metaprogramming and functions as a reference implementation of advanced macro metaprogramming concepts, idioms, and techniques.

## Modern Build System (CMake)

Chaos provides a modern CMake build system supporting `INTERFACE` library targets, CTest smoke test suites, installation, package export (`find_package`), and automated documentation builds.

### Quick Start

```bash
# Configure and build smoke tests
cmake -B build
cmake --build build

# Run test suite
ctest --test-dir build --output-on-failure
```

Or using the convenience `Makefile`:

```bash
make          # Configure and build
make test     # Run smoke tests
make example  # Build and run the production trading engine example
make docs     # Build documentation
make install  # Install library headers and CMake package configs
make clean    # Remove build directory
```

### CMake Options

| Option | Default | Description |
|---|---|---|
| `CHAOS_ENABLE_TESTING` | `ON` (top-level) | Build C/C++ verification smoke tests |
| `CHAOS_ENABLE_EXAMPLES` | `ON` (top-level) | Build production-grade trading engine example |
| `CHAOS_ENABLE_DOCS` | `ON` (top-level) | Add `docs` target using `xsltproc` |
| `CHAOS_INSTALL` | `ON` (top-level) | Install targets and export `chaosConfig.cmake` |

### Integrating with Your CMake Project

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

## Manual Usage & Compiler Verification

Chaos is a header-only library. To use it directly without CMake, add the repository root to your compiler's include path:

```c
#include <chaos/preprocessor.h>
```

Verify header compatibility manually:

- **C**:
  ```bash
  clang -I. -x c -c -o /dev/null chaos/preprocessor.h
  ```
- **C++ (C++11 or newer)**:
  ```bash
  clang++ -std=c++11 -I. -x c++ -c -o /dev/null chaos/preprocessor.h
  ```

## Building Documentation

Documentation sources are located in `documentation/` (XML format) and compiled to HTML in `built-docs/`.

### Using CMake Target
```bash
cmake --build build --target docs
```

### Using Script Directly
```bash
cd pp-book && ./build ../documentation/library.xml ../documentation ../built-docs
```

The output HTML files are placed in `built-docs/` (e.g. `built-docs/overview.html`).

## License

Unless otherwise specified, terms and conditions of use of Chaos Preprocessor software and documentation are described in the accompanying license. See the file named [LICENSE](LICENSE) in the top-level directory (Boost Software License, Version 1.0).
