#include "performance.h"

static inline int missPenalty(unsigned int blk) { return 4 * (blk / 4); }

void perf_init(PerfStats *ps) { *ps = (PerfStats){0}; }

void perf_recordInstrStart(PerfStats *ps, int instrBytes) {
    ps->totalCycles += 2; // +2 cycles to execute
    ps->totalInstructions++;
    ps->instrBytes += instrBytes; // count bytes fetched
}

void perf_recordCacheLookup(PerfStats *ps, bool cacheHit, unsigned int blockSize, unsigned int len) {
    int cc = cacheHit ? 1 : missPenalty(blockSize);
    ps->totalCycles += cc;
    if (cacheHit)
        ps->hitCycles += cc;
    else
        ps->missCycles += cc;
    ps->dataBytes += len;
}

void perf_recordPageFault(PerfStats *ps) {
    ps->totalCycles += 100;
    ps->pageFaultCycles += 100;
}

void perf_finalize(PerfStats *ps) {
    if (ps->totalInstructions)
        ps->CPI = (float)ps->totalCycles / (float)ps->totalInstructions;
}
