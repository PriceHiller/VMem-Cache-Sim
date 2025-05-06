#ifndef CACHESIM_H
#define CACHESIM_H

#include "../instructions/address.h"
#include "../lib/types/cache.h"

Cache Cache_init(CacheValues c_info, ReplacementPolicy rp);
void Cache_free(Cache *cache);
void Cache_lookup_addr(Cache *cache, Address addr, unsigned int len);
void Cache_clear(Cache *cache);

#endif
