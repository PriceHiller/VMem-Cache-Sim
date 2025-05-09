#include "cachesim.h"
#include "../instructions/address.h"
#include "../lib/types/cache.h"
#include "../memorymanagement/memorymanagement.h"
#include "../performance/performance.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CacheSet CacheSet_init(Cache *cache, Address index) {
    CacheSet set = {.index = index,
                    .next_block_rp = 0,
                    .blocks = (CacheBlock *)calloc(cache->info.associativity,
                                                   sizeof(CacheBlock))};
    return set;
}

CacheBlock CacheBlock_init(Cache *cache, AddrParts addr_parts) {
    return addr_parts.tag.addr;
}

Cache Cache_init(CacheValues c_info, ReplacementPolicy rp) {
    Cache cache = {.info = c_info, .rp = rp, .stat = {0}};

    cache.sets = (CacheSet *)calloc(cache.info.sets, sizeof(CacheSet));
    for (unsigned int i = 0; i < cache.info.sets; i++) {
        cache.sets[i] = CacheSet_init(&cache, i);
    }
    return cache;
}

void Cache_clear(Cache *cache) {
    for (unsigned int i = 0; i < cache->info.sets; i++) {
        cache->sets[i] = CacheSet_init(cache, i);
    }
}

void Cache_free(Cache *cache) {
    if (cache->sets) {
        for (unsigned int i = 0; i < cache->info.sets; i++) {
            CacheSet *set = &cache->sets[i];
            if (set->blocks) {
                free(set->blocks);
                set->blocks = NULL;
            }
        }
        free(cache->sets);
        cache->sets = NULL;
    }
}

void Cache_replace_round_robin(Cache *cache, CacheSet *set, CacheBlock block) {
    set->blocks[set->next_block_rp] = block;
    set->next_block_rp = (set->next_block_rp + 1) % cache->info.associativity;
}

void Cache_replace_random(Cache *cache, CacheSet *set, CacheBlock block) {
    set->blocks[(rand() % cache->info.associativity)] = block;
}

void Cache_replace_block(Cache *cache, CacheSet *set, CacheBlock block) {
    switch (cache->rp) {
    case RP_ROUND_ROBIN:
        Cache_replace_round_robin(cache, set, block);
        break;
    case RP_RANDOM:
    case RP_UNKNOWN:
        Cache_replace_random(cache, set, block);
        break;
    }
};

bool Cache_handle_lookup(Cache *cache, AddrParts addr_parts) {
    cache->stat.accesses++;

    CacheBlock block;
    CacheBlock addr_block = CacheBlock_init(cache, addr_parts);
    CacheSet *set = &cache->sets[addr_parts.index.addr];
    for (unsigned int offset = 0; offset < cache->info.associativity;
         offset++) {
        block = set->blocks[offset];
        if (block == addr_block) {
            // Hit!
            cache->stat.hits++;
            return true;
        }

        if (!block) {
            // We have an empty location to place the block, and we haven't
            // touched this block yet since we started lookuping up blocks in
            // our cache
            //
            // Compulsory miss
            cache->stat.misses.compulsory++;
            set->blocks[offset] = addr_block;
            return false;
        }
    }

    // If we didn't find an empty location in the set for the address and we
    // didn't get a hit, then we have a conflict.
    cache->stat.misses.conflict++;
    Cache_replace_block(cache, set, addr_block);
    return false;
}

void Cache_lookup_addr(Cache *cache, Address addr, unsigned int len,
                       PerfStats *ps) {
    cache->stat.addresses++;
    unsigned int block_size_bytes = 1 << cache->info.offsetBits;

    while (len > 0) {
        AddrParts addr_parts =
            get_addr_parts(addr, cache->info.tagBits, cache->info.indexBits,
                           cache->info.offsetBits);

        unsigned int offset_addr = block_size_bytes - addr_parts.offset.addr;
        unsigned int bytes_to_process =
            (len <= offset_addr) ? len : offset_addr;

        bool hit = Cache_handle_lookup(cache, addr_parts);

        perf_recordCacheLookup(ps, hit, bytes_to_process, len);

        if (len <= offset_addr)
            break;
        len -= bytes_to_process;
        addr += bytes_to_process;
    };
}

void Cache_invalidate_pte(Cache *cache, PTE pte) {
    if (!pte) {
        return;
    }

    int page = pte * PAGE_SIZE;
    int block_size_bytes = 1 << cache->info.offsetBits;
    AddrParts stuff =
        get_addr_parts(page, cache->info.tagBits, cache->info.indexBits,
                       cache->info.offsetBits);
    for (int offset = 0; offset < VIR_SPACE; offset += block_size_bytes - 1) {
        Address addr = offset;
        AddrParts parts =
            get_addr_parts(page + offset, cache->info.tagBits,
                           cache->info.indexBits, cache->info.offsetBits);
        CacheSet *set = &cache->sets[parts.index.addr];
        for (unsigned int blk_off = 0; blk_off < cache->info.associativity;
             blk_off++) {
            CacheBlock *block = &set->blocks[blk_off];
            if (*block == parts.tag.addr) {
                block[blk_off] = CacheBlock_init(cache, parts);
            }
        }
    }
}
