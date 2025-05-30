#ifndef CACHESIM_H
#define CACHESIM_H

#include "../instructions/address.h"
#include "../lib/types/cache.h"
#include "../memorymanagement/memorymanagement.h"
#include "../performance/performance.h"

Cache Cache_init(CacheValues c_info, ReplacementPolicy rp);
void Cache_free(Cache *cache);
void Cache_lookup_addr(Cache *cache, Address addr, unsigned int len,
                       PerfStats *ps);
void Cache_clear(Cache *cache);
void Cache_invalidate_index(Cache *cache, unsigned int index);
void Cache_invalidate_pte(Cache *cache, PTE pte);

#endif
