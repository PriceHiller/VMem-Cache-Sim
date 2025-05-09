#ifndef LIBTYPES_CACHE_H
#define LIBTYPES_CACHE_H

#include "../../instructions/address.h"
#include "types.h"
#include <stdbool.h>
#include <sys/types.h>

typedef enum { RP_ROUND_ROBIN, RP_RANDOM, RP_UNKNOWN } ReplacementPolicy;

char *ReplacementPolicy_to_string(ReplacementPolicy rp);
ReplacementPolicy ReplacementPolicy_from_string(char *replacement_policy);

typedef struct {
    char *files[3];
    unsigned int count;
} TraceFiles;

typedef struct {
    Kilobyte cacheSize;
    Byte blockSize;
    unsigned int associativity;
    ReplacementPolicy replacementPolicy;
    Megabyte physicalMemory;
    float percentMemoryUsed;
    unsigned int instructionsPerTimeSlice;
    TraceFiles traceFiles;
} CacheParameters;

#define COST_PER_KB 0.12

typedef struct {
    int cacheBits;
    int sets;
    int numBlocks;
    int associativity;
    NumBits indexBits;
    NumBits offsetBits;
    NumBits tagBits;
    int overheadBitsPerBlock;
    int overheadSize;
    int memorySize;
    double cost;
    double costPerKb;
} CacheValues;

typedef Address CacheBlock;

typedef struct {
    Address index;
    CacheBlock *blocks;
    unsigned int next_block_rp;
} CacheSet;

typedef struct {
    unsigned int compulsory;
    unsigned int conflict;
} CacheStatMisses;

typedef struct {
    CacheStatMisses misses;
    unsigned int accesses;
    unsigned int addresses;
    unsigned int hits;
} CacheStat;

typedef struct {
    CacheValues info;
    ReplacementPolicy rp;
    CacheSet *sets;
    CacheStat stat;
} Cache;

#endif
