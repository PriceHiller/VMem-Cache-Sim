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

void perf_recordCacheLookup(PerfStats *ps, bool cacheHit,
                            unsigned int blockSize, unsigned int len);
void perf_recordInstrStart(PerfStats *ps, int instrBytes);

// +100 cycles per page fault
void perf_recordPageFault(PerfStats *ps);

// compute CPI = totalCycles / totalInstructions
void perf_finalize(PerfStats *ps);
