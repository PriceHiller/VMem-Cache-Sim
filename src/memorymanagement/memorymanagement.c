#include "memorymanagement.h"
#include "../lib/types/types.h"
#include <stdlib.h>

#define PAGE_SIZE 4096
#define VIR_SPACE 524288

PhysMem physmem;

void initPhysMem(Byte physMemMB, int osPercent, VMStats *stats) {
    Byte physBytes = physMemMB;
    int totalPages = physBytes / PAGE_SIZE;
    physmem.totalPhys = totalPages;

    int sys = (int)(totalPages * (osPercent / 100.0));
    physmem.sysPages = sys;
    physmem.freeCount = totalPages - sys;
    physmem.freePages = physmem.freeCount;

    stats->sysPages = sys;
    stats->freePages = physmem.freePages;

    physmem.freeList = (int *)malloc(physmem.freeCount * sizeof(int));
    for (int i = 0; i < physmem.freeCount; i++) {
        physmem.freeList[i] = sys + i;
    }
}

void freePhysMem() { free(physmem.freeList); }

void initProc(Proc *proc) {
    proc->pageTable = (PTE *)calloc(VIR_SPACE, sizeof(PTE));
}

int lookupPage(Proc *proc, int vPage) {
    if (vPage < 0 || vPage >= VIR_SPACE)
        return -1;
    if (proc->pageTable[vPage].valid)
        return proc->pageTable[vPage].physPage;
    return -1;
}

int replacePage(Proc *proc) {
    for (int i = 0; i < VIR_SPACE; i++) {
        if (proc->pageTable[i].valid) {
            int phys = proc->pageTable[i].physPage;
            proc->pageTable[i].valid = 0;
            return phys;
        }
    }
    return -1;
}

int mapPage(Proc *proc, int vPage, VMStats *stats) {
    int physPage;
    if (physmem.freeCount > 0) {
        physPage = physmem.freeList[--physmem.freeCount];
        stats->pagesFromFree++;
    } else {
        physPage = replacePage(proc);
        if (physPage == -1) {
            stats->faults++;
            return -1;
        }
        stats->faults++;
    }
    proc->pageTable[vPage].valid = 1;
    proc->pageTable[vPage].physPage = physPage;
    return physPage;
}

int accessMemory(Proc *proc, int address, VMStats *stats) {
    int vPage = address / PAGE_SIZE;
    int offset = address % PAGE_SIZE;
    stats->mapped++;
    proc->pageTableCount = physmem.totalPhys;

    int physPage = lookupPage(proc, vPage);
    if (physPage == -1) {
        physPage = mapPage(proc, vPage, stats);
        if (physPage == -1)
            return -1;
    } else {
        stats->hits++;
    }
    return physPage * PAGE_SIZE + offset;
}

int count_used_pte_entries(PTE *table) {
    int used = 0;
    for (int i = 0; i < VIR_SPACE; i++) {
        if (table[i].valid)
            used++;
    }
    return used;
}
