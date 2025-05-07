#ifndef LIBTYPES_VIRTMEM_H
#define LIBTYPES_VIRTMEM_H

// structs, go in lib/types folder?
#include "../../instructions/inst.h"
#include "../../memorymanagement/memorymanagement.h"
#include "types.h"

typedef struct {
    char *processName;
    unsigned int virtualPagesMapped;
    unsigned int pageTableHits;
    unsigned int pageFaults;
    unsigned int usedPTE;
    unsigned int totalPTE;
    unsigned int wastedPageTable;
    Proc proc;
} PageTableProcess;

typedef struct {
    unsigned int numPages;
    unsigned int pagesUsedForOS;
    unsigned int pagesAvailable;
    unsigned int totalVirtualPagesMapped;
    unsigned int pageTableHits;
    unsigned int pagesFromFree;
    unsigned int totalPageFaults;

    PageTableProcess *pageTableProcesses;
    int processCt;
} SimulationStats;

#endif
