# Virtual Memory Cache Simulation

This is a C implementation of a Virtual Memory and Level 1 cache simulation for a 32-bit CPU. See the [Project-Assignment.pdf](./docs/Project-Assignment.pdf) file for the project instructions.

# Building

## With `Make`

Run `make` in the top level directory, the generated binary should be in the top level as well.


## With `Cmake`

Create a `build` directory and change directory into it. From that directory type `cmake ..` and then run `cmake --build .` to build.

The built binary should be in the generated `bin/` directory.

## With Nix

Run `nix build`. If you'd prefer to use `clang` or `cmake` we also expose those as `".#clang"` and `".#cmake"`.

# Milestones

## Milestone 1: Input Parameters and Calculated Values

We chose to divide Milestone 1 into four parts.

### Part 1: Command-Line Parsing & Data Structures

**Main Goals**

1. Collect and validate input parameters:

   - `-s` (cache size, KB)
   - `-b` (block size)
   - `-a` (associativity)
   - `-r` (replacement policy)
   - `-p` (physical memory, MB)
   - `-u` (percent memory used by OS)
   - `-n` (instructions/time slice)
   - `-f` (trace file names, up to 3)

2. Store the parameters in a structured way (e.g., a `Config` struct/class).

**Deliverables**

- Functions or code sections that parse command-line arguments.
- Validation checks (e.g., does the user provide valid values like 8 ≤ cache size ≤ 8192?).
- A single data object or set of variables that the rest of the simulator can reference.

### Part 2: Cache Parameter Calculations

**Main Goals**

1.  Compute **block offset bits** = log₂(block size).
2.  Compute the **total number of cache blocks** = (cache size in bytes) / (block size in bytes).
    - Note that if `-s 512` → 512 KB, that’s `512 * 1024` bytes total.
3.  Compute the **number of rows** = (total number of blocks) ÷ (associativity).
4.  Compute **index bits** = log₂(number of rows).
5.  Compute **tag bits** = 32 − (index bits + offset bits).
6.  Calculate the **overhead** for storing tags, valid bits, etc.
7.  Calculate the **implementation memory size** (cache data + overhead).
8.  Estimate the **cost** (given a rate, e.g., $0.12 per KB).

**Deliverables**

- A set of calculated integers/doubles:
  - `totalBlocks`
  - `indexSize` (bits)
  - `tagSize` (bits)
  - `overheadSize` (bytes)
  - `implementationMemorySize` (bytes)
  - `cost` (in dollars)

### Part 3: Physical Memory & Page Table Calculations

**Main Goals**

1.  Convert `-p` (physical memory in MB) into bytes, then into **physical pages**.
    - The page size might be assumed (often 4 KB) or given by the instructor.
2.  Compute how many of those pages the OS uses, given `-u` (the OS percentage).
3.  Calculate the **size of the page table entry** (in bits) and total entries needed:
    - For a 32-bit virtual address space, total virtual pages = (2^32 / page size).
    - For multiple trace files (processes), you might have up to 3 separate page tables.
4.  Multiply to find the **total page table size** in bytes.

**Deliverables**

- `numPhysicalPages` (e.g., 1 GB / 4 KB = 262,144 pages).
- `numPagesForOS` (percentage of the above).
- `pageTableEntrySize` (bits).
- `totalPageTableSize` (bytes).

### Part 4: Output Formatting & Testing

**Main Goals**

1.  Print all **input parameters** in the required format.
2.  Print all **calculated** cache and memory values (tag bits, index bits, etc.).
3.  Verify with **sample inputs** to ensure the numbers match expectations.
4.  Keep the output structure exactly as stated in the spec.

**Deliverables**

- A neat, correctly formatted console output matching the instructions.
- Quick manual or automated tests (e.g., try `-s 512 -b 16 -a 4 …`) to confirm.

## Milestone 2: Virtual Memory Simulation Results

## Milestone 3: Cache Simulation Results
