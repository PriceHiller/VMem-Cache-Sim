#ifndef OUTPUT_H
#define OUTPUT_H
#include "../lib/types/cache.h"
#include "../lib/types/physicalmemory.h"
#include "../lib/types/types.h"
#include "../virtmemresults/virtMemResults.h"
char *CacheParameters_to_string(CacheParameters *params);
char *CacheValues_to_string(CacheValues *cache_vals);
char *PhysicalMemory_to_string(PhysicalMemory *phys_mem);
char *VirtualMemory_to_string(SimulationStats *sim);
#endif
