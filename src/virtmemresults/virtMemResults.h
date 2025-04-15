#ifndef VIRTMEMRESULTS_H
#define VIRTMEMRESULTS_H

#include "../lib/types/virtualmemory.h"

//methods

SimulationStats* virtualMemResults(int percentMemoryUsed, int numPages, int processCt);

int addProcess(SimulationStats* sim, char* processName, unsigned int totalPageTableEntries);

void recordPTHit(SimulationStats* sim, int pid);
void recordPageFault(SimulationStats* sim, int pid);
void recordPageFromFree(SimulationStats* sim, int pid);
void recordVirtualPagesMapped(SimulationStats* sim, int pid);
void calculateWastedPageTable(SimulationStats* sim);
void freeSim(SimulationStats* sim);

#endif
