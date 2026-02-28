# brkman
## Overview
`brkman` is a simple memory allocator. Unlike existing allocators, it was implemented for personal learning purposes and is therefore **not particularly mature**. As such, it is **not suitable for production use**.

---

## How It Works

### Core Design
`brkman` manages memory using a **free-list** of chunks, where each chunk is a block of memory that can be allocated or freed. It relies on the Unix system calls [`brk`](http://man7.org/linux/man-pages/man2/brk.2.html) and [`sbrk`](http://man7.org/linux/man-pages/man2/sbrk.2.html) to dynamically adjust the **program break** (the end of the process's data segment).

- **Heap Extension**: When the free memory is exhausted, `brkman` extends the heap in **64 KB chunks** using `sbrk`.
- **Free-List Management**: Freed memory blocks are organized in a **doubly-linked free-list**, sorted by memory address.
- **Chunk Splitting/Merging**: Large free chunks are split to fulfill smaller allocation requests, and adjacent free chunks are merged to reduce fragmentation.

---

### Key Features

| Feature                     | Description                                                                                     |
|-----------------------------|-------------------------------------------------------------------------------------------------|
| **Deterministic Behavior** | Uses `brk`/`sbrk` for predictable memory management (no randomness like in `malloc`).   |
| **Free-List Strategy**     | Implements a **next-fit** search with **best-fit** optimization to reduce fragmentation.        |
| **Chunk Splitting**        | Splits large free chunks to fulfill smaller requests, minimizing wasted memory.               |
| **Chunk Merging**           | Merges adjacent free chunks to combat fragmentation.                                          |
| **Debug Mode**              | Enables heap introspection, detailed error messages, and GDB integration (`DEBUG=1`).       |
| **No External Dependencies**| Pure C99 implementation; only depends on the standard C library (`libc`).                     |

## Limitations

| Limitation                     | Description                                                                                     |
|--------------------------------|-------------------------------------------------------------------------------------------------|
| **Linux-only**                 | Relies on `brk`/`sbrk` (not available on Windows/macOS).                                        |
| **No Thread Safety**           | Concurrent calls to `brkman_alloc`/`brkman_free` may corrupt the heap.                         |
| **No `realloc`**               | Does not support resizing allocations.                                                         |
| **Fixed Heap Extension**       | Always extends the heap in 64 KB chunks.                                                     |
| **No Alignment Guarantees**     | May not suit SIMD or special types requiring strict alignment.                               |


## Requirements

### For Users
To use `brkman` in your project, ensure your system meets these **minimum requirements**:

| Requirement       | Details                          | Notes                                  |
|-------------------|----------------------------------|----------------------------------------|
| **OS**            | Linux (kernel ≥ 2.6.32)          | Uses `brk`/`sbrk` (Unix system calls). |
| **Compiler**      | GCC ≥ **4.8** or Clang ≥ **3.4** | C99 support |
| **Libraries**     | `libc6-dev`                     | Standard C Library.                   |

---

## Installation
### How to Build
```sh
git clone https://github.com/felixdngrs/brkman.git
cd brkman
make
```

---

### How to Use (as a User)
#### As a Static Library
```sh
gcc foo.c -I/path/to/brkman/include -L/path/to/brkman -l:libbrkman.a -o foo
```

#### As a Dynamic Library
```sh
gcc foo.c -I/path/to/brkman/include -L/path/to/brkman -lbrkman -o foo
```

---

### How to Use (as a Developer)
#### Building Tests
```sh
export LD_LIBRARY_PATH=/path/to/brkman/:$LD_LIBRARY_PATH
make build-tests DEBUG=1
```
> **Note:** It is important to compile in **DEBUG mode** for testing. This mode enables internal functions required for testing, provides detailed output, and simplifies debugging with `gdb`.

## Example Usage
```c
#include "brkman.h"
#include <stdio.h>

int main() {
    // Allocate 64 bytes of memory
    void* ptr = brkman_alloc(64);
    if (!ptr) {
        fprintf(stderr, "Allocation failed!\n");
        return 1;
    }

    // Use the allocated memory
    snprintf(ptr, 64, "Hello, brkman!");

    // Free the memory
    brkman_free(ptr);
    return 0;
}
```
