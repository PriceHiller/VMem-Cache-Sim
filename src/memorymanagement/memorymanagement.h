#ifndef MEMORYMANAGEMENT_H
#define MEMORYMANAGEMENT_H

#include "../instructions/address.h"
#include "../instructions/inst.h"
#include "../lib/types/cache.h"
#include "../lib/types/types.h"
#include "../performance/performance.h"

#define PAGE_SIZE 4096
#define VIR_SPACE 524288

typedef int PTE;

typedef struct {
    PTE *pageTable;
    int pageTableCount;
    Instruction_Arr instructions;
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
} VMStats;

void initPhysMem(Byte physMemMB, int osPercent, VMStats *stats);
void freePhysMem();
void initProc(Proc *proc);
int vmemAccessMemory(Proc *proc, Cache *cache, int address, VMStats *stats,
                     PerfStats *ps);
int lookupPage(Proc *proc, int vPage);
int count_used_pte_entries(PTE *table);
void unmapPage(Proc *proc, int page);
#endif
