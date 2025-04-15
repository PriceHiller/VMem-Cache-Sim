#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE 4096
#define VIR_SPACE 524288

typedef struct {
    int valid;
    int physPage;
} PTE;

typedef struct {
    PTE *pageTable;
} Proc;

typedef struct {
    int used;
    double usage;
    int waste;
} PageTableUsage;

typedef struct {
    int totalPhys;
    int *freeList;
    int freeCount;
    int sysPages;
    int freePages;
} PhysMem;

typedef struct {
    int sysPages;
    int freePages;
    int mapped;
    int hits;
    int pagesFromFree;
    int faults;
    PageTableUsage perProc[3];
} vmstats;

PhysMem physmem;

void initPhysMem(int physMemMB, int osPercent, vmstats *stats) {
    int physBytes = physMemMB * 1024 * 1024;
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

int mapPage(Proc *proc, int vPage, vmstats *stats) {
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

int accessMemory(Proc *proc, int address, vmstats *stats, int ProcIndex) {
    int vPage = address / PAGE_SIZE;
    int offset = address % PAGE_SIZE;
    stats->mapped++;

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

int countused(PTE *table) {
    int used = 0;
    for (int i = 0; i < VIR_SPACE; i++) {
        if (table[i].valid)
            used++;
    }
    return used;
}
