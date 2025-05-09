#ifndef INTEGRATION_H
#define INTEGRATION_H
#include "../instructions/address.h"
#include "../lib/types/types.h"

/* cache.h, remove later */
// #define BLOCK_SIZE 16
// #define CACHE_SETS 8192
// #define ASSOCIATIVITY 4
// #define CACHE_SIZE (CACHE_SETS * ASSOCIATIVITY * BLOCK_SIZE)
// #define COST_PER_KB 0.12
/* end */

/* types.h, remove later */
// #define BYTES_KB 1024
// #define BYTES_MB (1024ULL * BYTES_KB)
// #define BYTES_GB (1024ULL * BYTES_MB)

// typedef unsigned long long Byte;
/* end */

// typedef unsigned int Address; //address.h, remove later

typedef enum { EIP, SRC, DST } AccessType;

typedef struct {
    unsigned int totalCacheAccesses;
    unsigned int totalRowsAccessed;
    Byte eipBytes; // instruction
    Byte srcDstBytes;
    unsigned int totalAddresses;
} CacheAccess;

extern CacheAccess access;

unsigned int rowsAccessed(Address address, Byte addrSize, Byte blockSize);

#endif
