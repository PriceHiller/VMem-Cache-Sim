#include "cachesim.h"
#include "../instructions/address.h"
#include "../lib/types/cache.h"
#include <stdbool.h>
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
    CacheBlock block = {
        .index = addr_parts.index.addr,
        .tag = addr_parts.tag.addr,
        .valid = false,
    };
    return block;
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

void Cache_replace_round_robin(Cache *cache, CacheBlock block) {
    CacheSet *set = &cache->sets[block.index];
    set->blocks[set->next_block_rp] = block;
    set->next_block_rp++;
    if (set->next_block_rp == cache->info.associativity) {
        set->next_block_rp = 0;
    }
}

void Cache_replace_random(Cache *cache, CacheBlock block) {
    cache->sets[block.index].blocks[(rand() % cache->info.associativity)] =
        block;
}

void Cache_replace_block(Cache *cache, CacheBlock block) {
    switch (cache->rp) {
    case RP_ROUND_ROBIN:
        Cache_replace_round_robin(cache, block);
        break;
    case RP_RANDOM:
    case RP_UNKNOWN:
        Cache_replace_random(cache, block);
        break;
    }
};

void Cache_handle_lookup(Cache *cache, AddrParts addr_parts) {
    cache->stat.accesses++;

    CacheBlock block;
    CacheBlock addr_block = CacheBlock_init(cache, addr_parts);
    CacheSet *set = &cache->sets[addr_parts.index.addr];
    for (unsigned int offset = 0; offset < cache->info.associativity;
         offset++) {
        block = set->blocks[offset];
        if (block.valid && block.tag == addr_block.tag) {
            // Hit!
            cache->stat.hits++;
            return;
        }

        if (!block.valid) {
            // We have an empty location to place the block, and we haven't
            // touched this block yet since we started lookuping up blocks in
            // our cache
            //
            // Compulsory miss
            cache->stat.misses.compulsory++;
            addr_block.valid = true;
            set->blocks[offset] = addr_block;
            return;
        }
    }

    // If we didn't find an empty location in the set for the address and we
    // didn't get a hit, then we have a conflict.
    cache->stat.misses.conflict++;
    addr_block.valid = true;
    Cache_replace_block(cache, addr_block);
}

void Cache_lookup_addr(Cache *cache, Address addr, unsigned int len) {
    cache->stat.addresses++;
    unsigned int block_size_bytes = 1 << cache->info.offsetBits;

    while (len > 0) {
        AddrParts addr_parts =
            get_addr_parts(addr, cache->info.tagBits, cache->info.indexBits,
                           cache->info.offsetBits);

        Cache_handle_lookup(cache, addr_parts);

        unsigned int bytes_remaining_in_block =
            block_size_bytes;
        unsigned int bytes_to_process =
            (len < bytes_remaining_in_block + 1) ? len : bytes_remaining_in_block;
        len -= bytes_to_process;
        addr += bytes_to_process;
    };
}
