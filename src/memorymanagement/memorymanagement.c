#include "memorymanagement.h"
#include "../cachesim/cachesim.h"
#include "../lib/types/cache.h"
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
    proc->pageTable = (int *)calloc(VIR_SPACE, sizeof(int));
}

int lookupPage(Proc *proc, int vPage) {
    if (vPage < 0 || vPage >= VIR_SPACE)
        return -1;
    if (proc->pageTable[vPage])
        return proc->pageTable[vPage];
    return -1;
}

int replacePage(Proc *proc, Cache *cache) {
    for (int i = 0; i < VIR_SPACE; i++) {
        if (proc->pageTable[i]) {
            Cache_invalidate_pte(cache, i);
            int phys = proc->pageTable[i];
            return phys;
        }
    }
    return -1;
}

void unmapPage(Proc *proc, int page) {
    if (page < physmem.sysPages) {
        return;
    }
    physmem.freeList[page - physmem.sysPages] = 0;
}

int mapPage(Proc *proc, Cache *cache, int vPage, VMStats *stats) {
    int physPage;
    if (physmem.freeCount > 0) {
        int next_page = 0;
        physmem.freeCount--;
        next_page = physmem.freePages - physmem.freeCount;
        physPage = physmem.freeList[next_page];
        stats->pagesFromFree++;
    } else {
        physPage = replacePage(proc, cache);
        if (physPage == -1) {
            stats->faults++;
            return -1;
        }
        stats->faults++;
    }
    // printf(" > Mapping 0x%X -> 0x%X\n", vPage, physPage);
    proc->pageTable[vPage] = physPage;
    return physPage;
}

int vmemAccessMemory(Proc *proc, Cache *cache, int address, VMStats *stats) {
    int vPage = address / PAGE_SIZE;
    int offset = address % PAGE_SIZE;
    stats->mapped++;
    proc->pageTableCount = physmem.totalPhys;

    int physPage = lookupPage(proc, vPage);
    if (physPage == -1) {
        physPage = mapPage(proc, cache, vPage, stats);
        if (physPage == -1)
            return -1;
    } else {
        stats->hits++;
    }
    int vaddr = physPage * PAGE_SIZE + offset;
    return vaddr;
}

int count_used_pte_entries(PTE *table) {
    int used = 0;
    for (int i = 0; i < VIR_SPACE; i++) {
        if (table[i])
            used++;
    }
    return used;
}
