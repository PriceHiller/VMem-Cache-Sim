#include "performance.h"
#include <stdio.h>

static inline int missPenalty(int blk) {
    return 4 * (blk / 4);
}

void perf_init(PerfStats *ps) {
    *ps = (PerfStats){0};
}

void perf_recordInstrRow(PerfStats *ps,
                         bool cacheHit,
                         bool firstRow,
                         int instrBytes,
                         int blockSize)
{
    int cc = cacheHit ? 1 : missPenalty(blockSize);
    ps->totalCycles += cc;
    if (cacheHit) ps->hitCycles += cc;
    else          ps->missCycles += cc;

    if (firstRow) {
        ps->totalCycles      += 2;           // +2 cycles to execute
        ps->totalInstructions++;
        ps->instrBytes       += instrBytes;  // count bytes fetched
    }
}

void perf_recordDataRow(PerfStats *ps,
                        bool cacheHit,
                        int blockSize)
{
    int cc = cacheHit ? 1 : missPenalty(blockSize);
    ps->totalCycles += cc + 1;               // +1 cycle for EA calc
    if (cacheHit) ps->hitCycles += cc;
    else          ps->missCycles += cc;
    ps->dataBytes += 4;                      // always 4 bytes
}

void perf_recordPageFault(PerfStats *ps) {
    ps->totalCycles     += 100;
    ps->pageFaultCycles += 100;
}

void perf_finalize(PerfStats *ps) {
    if (ps->totalInstructions)
        ps->CPI = (double)ps->totalCycles / ps->totalInstructions;
}

