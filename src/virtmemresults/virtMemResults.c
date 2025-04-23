#include "virtMemResults.h"
#include "../lib/types/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PAGE_SIZE_BYTES (4ULL * BYTES_KB)      // a page is 4K
#define PAGE_TABLE_ENTRIES (512ULL * BYTES_KB) // number of PTE fixed at 512K

SimulationStats *virtualMemResults(int percentMemoryUsed, int numPages,
                                   int processCt) {
    SimulationStats *sim = (SimulationStats *)malloc(sizeof(SimulationStats));
    if (!sim) {
        perror("Failed to allocate memory to sim.");
        return NULL;
    }

    sim->numPages = numPages;
    sim->pagesUsedForOS = ((float)percentMemoryUsed / 100) * (numPages);
    sim->pagesAvailable = numPages - sim->pagesUsedForOS;

    sim->totalVirtualPagesMapped = 0;
    sim->pageTableHits = 0;
    sim->pagesFromFree = 0;
    sim->totalPageFaults = 0;

    sim->processCt = processCt;
    sim->pageTableProcesses =
        (PageTableProcess *)malloc(processCt * sizeof(PageTableProcess));
    if (!sim->pageTableProcesses) {
        perror("Failed to allocate memory to page table processes.");
        free(sim->pageTableProcesses);
        free(sim);
        return NULL;
    }

    // initialize processes
    int i;
    for (i = 0; i < processCt; i++) {
        sim->pageTableProcesses[i].processName = NULL;
        sim->pageTableProcesses[i].virtualPagesMapped = 0;
        sim->pageTableProcesses[i].pageTableHits = 0;
        sim->pageTableProcesses[i].pageFaults = 0;
        sim->pageTableProcesses[i].usedPTE = 0;
        sim->pageTableProcesses[i].totalPTE = PAGE_TABLE_ENTRIES;
        sim->pageTableProcesses[i].wastedPageTable = 0;
    }

    return sim;
}

int addProcess(SimulationStats *sim, char *processName,
               unsigned int totalPageTableEntries) {
    int i;
    for (i = 0; i < sim->processCt; i++) {
        if (sim->pageTableProcesses[i].processName == NULL) {
            sim->pageTableProcesses[i].processName = strdup(processName);
            sim->pageTableProcesses[i].totalPTE = totalPageTableEntries;
            sim->pageTableProcesses[i].usedPTE = 0;
            sim->pageTableProcesses[i].virtualPagesMapped = 0;
            sim->pageTableProcesses[i].pageTableHits = 0;
            sim->pageTableProcesses[i].pageFaults = 0;
            return i;
        }
    }
    return -1;
}

void recordPTHit(SimulationStats *sim, int pid) {
    if (!sim || pid >= sim->processCt) {
        return;
    }

    sim->pageTableHits++;
    sim->pageTableProcesses[pid].pageTableHits++;
}

void recordPageFault(SimulationStats *sim, int pid) {
    if (!sim || pid >= sim->processCt) {
        return;
    }

    sim->totalPageFaults++;
    sim->pageTableProcesses[pid].pageFaults++;
}

void recordPageFromFree(SimulationStats *sim, int pid) {
    if (!sim || pid >= sim->processCt) {
        return;
    }

    sim->pagesFromFree++;
}

void recordVirtualPagesMapped(SimulationStats *sim, int pid) {
    if (!sim || pid >= sim->processCt) {
        return;
    }

    sim->pageTableProcesses[pid].virtualPagesMapped++;
    sim->pageTableProcesses[pid].usedPTE++;
}

void calculateWastedPageTable(SimulationStats *sim) {
    if (!sim) {
        return;
    }

    int i;
    for (i = 0; i < sim->processCt; i++) {
        if (sim->pageTableProcesses[i].processName !=
            NULL) { // check for valid process
            sim->pageTableProcesses[i].wastedPageTable =
                (sim->pageTableProcesses[i].totalPTE -
                 sim->pageTableProcesses[i].usedPTE) *
                PAGE_SIZE_BYTES;
        }
    }
}

void freeSim(SimulationStats *sim) {
    if (!sim) {
        return;
    }
    int i;
    for (i = 0; i < sim->processCt; i++) {
        free(sim->pageTableProcesses[i].processName);
    }
    free(sim->pageTableProcesses);
    free(sim);
}
