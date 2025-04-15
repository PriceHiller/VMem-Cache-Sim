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

### Part 1: Trace File Parsing and Virtual Address Extraction

- **Responsibilities:**
  - Read each trace file line by line using functions like `fgets` or equivalent.
  - Extract the key data from the structured trace lines (e.g., instruction length and virtual addresses from the EIP line, and data addresses from the dstM/srcM lines).
  - Filter out invalid accesses (addresses equal to zero or marked with `--------`).
  - Convert extracted string representations of addresses into numerical values.
- **Deliverables:**
  - A module (or function library) that reliably parses trace files and returns a list of virtual addresses (and possibly instruction lengths).
  - Unit tests to ensure the parser correctly interprets the sample trace file format.

### Part 2: Virtual Memory Manager & Page Table Simulation

- **Responsibilities:**
  - Implement the virtual-to-physical address translation using a page table for each process (trace file).
  - Initialize and manage a page table with 512K entries (as specified).
  - Handle page faults:
  - - Check if a free physical page is available.
  - - If not, implement a replacement policy (or selection) to decide which process's page to free.
  - Update page table entries when a new virtual-to-physical mapping is created.
  - **Note:** Use the calculated physical memory values (from Milestone 1) to determine available pages.
- **Deliverables:**
  - A module that provides functions to:
    - Look up a virtual address in the page table.
    - Update the page table on a page fault.
    - Simulate mapping of virtual pages to physical pages.
  - Functions that return counters for page table hits, free page mappings, and page faults.

### Part 3: Simulation Metrics & Statistics Calculation

- **Responsibilities:**
  - During the simulation run, collect all metrics such as:
    - **Physical Pages Used by SYSTEM:** Derived from the OS percentage.
    - **Pages Available to User:** (Total physical pages minus OS-reserved pages).
    - **Virtual Pages Mapped:** Total number of virtual pages that were successfully mapped.
    - **Page Table Hits:** Count of accesses where the virtual page was already mapped.
    - **Pages from Free:** Count of mappings that came from free physical pages.
    - **Total Page Faults:** Count of times no physical page was available and a page fault occurred.
  - For each process (trace file), track:
    - The number of used page table entries.
    - The “wasted” page table memory (unused entries, if applicable).
- **Deliverables:**
  - A statistics module that aggregates simulation data.
  - Data structures (possibly structs) to hold per-process and overall simulation metrics.
  - Functions that compute the final numbers needed for the formatted output.

### Part 4: Output Formatting, Integration & Testing

- **Responsibilities:**
  - Integrate the results from Milestone 1 (input parameters and calculated values) with the new virtual memory simulation results.
  - Format and print the simulation results exactly as specified. This includes headers like:
    - `***** VIRTUAL MEMORY SIMULATION RESULTS *****`
    - The calculated values for physical pages used by the system, pages available to the user, virtual pages mapped, page table hits, pages from free, and total page faults.
  - For each trace file, print the per-process page table usage (used entries and wasted bytes).
  - Ensure that the output is consistent across different simulation runs.
  - Conduct end-to-end testing by running the complete simulation with a variety of trace files and parameters.
- **Deliverables:**
  - A final output function that collects all data from the parsing, virtual memory manager, and statistics modules, and prints the formatted results.
  - Test runs that generate output files matching the required naming and format (e.g., `Team_XX_Sim_n_M#2.txt`).

## Milestone 3: Cache Simulation Results
