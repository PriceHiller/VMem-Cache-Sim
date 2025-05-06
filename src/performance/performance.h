#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint64_t totalCycles;
    uint64_t totalInstructions;
    uint64_t instrBytes;
    uint64_t dataBytes;
    uint64_t hitCycles;
    uint64_t missCycles;
    uint64_t pageFaultCycles;
    double CPI;
} PerfStats;

// init all counts to zero
void perf_init(PerfStats *ps);

// record one cache‐row of an instruction fetch:
void perf_recordInstrRow(PerfStats *ps, bool cacheHit, bool firstRow,
                         int instrBytes, int blockSize);

// record one cache‐row of a data access (always 4 bytes):
//   cacheHit: true=hit, false=miss
//   blockSize: cache block size in bytes
void perf_recordDataRow(PerfStats *ps, bool cacheHit, int blockSize);

// +100 cycles per page fault
void perf_recordPageFault(PerfStats *ps);

// compute CPI = totalCycles / totalInstructions
void perf_finalize(PerfStats *ps);
