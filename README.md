# Python-Like Interpreter & Memory Manager

A systems programming project exploring how a Python-like language executes statements and stores variables. Developed in C and C++ as part of Northwestern University's CS 211 coursework.

The project includes an execution engine, a typed memory manager, and a debugger component. Each component is maintained in its own directory and builds separately; this repository is not yet a single integrated build.

## Features

### Execution engine

- Traverses a program graph to execute assignments, conditionals, and while loops.
- Evaluates arithmetic and comparison expressions involving integers and floating-point values.
- Supports string concatenation and comparisons, with mixed integer/floating-point arithmetic.
- Implements the built-in functions `print`, `input`, `int`, and `float`.
- Supports pointer-based assignment through the memory interface.
- Reports line-numbered errors for undefined variables, incompatible operands, division by zero, and invalid pointer assignments.

### Memory manager

- Stores integers, floating-point values, strings, pointers, booleans, and `None` using tagged values.
- Maps variable names to memory cells using a sorted name-to-address table and binary search.
- Provides O(log n) name lookup in terms of table comparisons; insertion and resizing have separate costs.
- Grows storage dynamically by doubling capacity.
- Copies string values into separately allocated storage and provides explicit cleanup functions.
- Includes eight GoogleTest cases covering initialization, reads and writes, resizing, multiple value types, and string handling.

### Debugger

The debugger component is included separately. The last reviewed remote version contains a command-loop scaffold with help and quit commands. Stepping and variable inspection are listed in its help text but were not implemented in that version. Its status should be updated after the completed source is added and tested.

## Repository layout

| Path | Contents |
| --- | --- |
| `execution/` | Execution engine, interpreter interfaces, example programs, and its makefile |
| `memory/` | Memory manager, GoogleTest tests, and its makefile |
| `debugger/` | Debugger source, interpreter interfaces, and its makefile |
| `README.md` | Project overview and setup instructions |

Keep each component's original source files, headers, and required object dependencies together in its directory.

## Requirements

- A Linux environment compatible with the provided object files
- GCC/G++ and Make
- C11 support for the execution engine and C++20 support for the memory test build
- GoogleTest development headers and libraries for memory tests
- Valgrind, optionally, for memory diagnostics

Some course-provided parser and runtime components are distributed as precompiled object files. They are required build dependencies, not disposable build output. Select files matching your CPU architecture. The provided ELF objects do not link directly into native macOS executables; use a compatible Linux environment on a Mac.

## Build and run

Run commands from the component directory so relative paths resolve correctly.

### Execution engine

```bash
cd execution
make build
./a.out test-expr.py
./a.out test-funct.py
./a.out test-if.py
./a.out test-while.py
```

The function, conditional, and loop examples prompt for input. Running `./a.out` without a filename accepts source from the keyboard until `$` is entered.

The execution makefile links `ram.o` and `nupython.o`. If those files target a different architecture, replace the local copies with the supplied matching variants before building. For example, on x86-64 Linux:

```bash
cp ram.x86.o ram.o
cp nupython.x86.o nupython.o
make build
```

For ARM64 Linux, use the `.arm.o` variants instead.

### Memory-manager tests

From the repository root:

```bash
cd memory
make build
make run
```

Optional diagnostics:

```bash
make valgrind
```

The makefile also includes a `buildcc` target for compilation with coverage instrumentation. The presence of these targets does not imply a clean Valgrind report or a measured coverage percentage.

### Debugger

Build and usage instructions for the completed debugger will be added after its implementation is verified. Its precompiled dependencies must also match the target architecture.

## Example program

This language uses braces for blocks and supports a subset of Python-like syntax; it is not a full Python implementation.

```python
s = input("Enter an integer> ")
N = int(s)
i = 1

while i <= N:
{
    print(i)
    i = i + 1
}
```

## Validation and current limitations

- The execution engine was compiled on x86-64 Linux and all four supplied example programs ran with sample inputs. This is a smoke check, not exhaustive validation.
- Eight memory-manager test cases are present; a passing test run and leak-free execution have not been verified in this review.
- The reviewed memory manager needs a bounds-check correction in `ram_write_cell_by_addr`: invalid addresses must return before any cell access.
- The execution component currently links a supplied memory object. Consolidating these folders does not automatically link the custom memory-manager source into it.
- User-defined functions and full Python compatibility are outside the implemented execution scope.

## Contributions and acknowledgments

Honoured Chikwava implemented and extended the execution and memory-management functionality described above using course-provided interfaces and starter code.

Starter code and supporting components were provided through Northwestern University's CS 211 course, including work credited to Professor Joe Hummel. The debugger scaffold credits Ata Guvendi and Professor Hummel. Original attribution and notices are retained in the source files.
