#include "cachecalculator.h"
#include <math.h>

int calculateBlocks(Byte cacheSize, int blockSize) {
    return (cacheSize) / blockSize;
}
int calculateSets(Byte cacheSize, int blockSize, int associativity) {
    return (cacheSize) / (blockSize * associativity);
}
int calculateOffsetSize(Byte blockSize) { return log2f(blockSize); }
int calculateIndexSize(int sets) { return log2f(sets); }
double calculateImplementationMemorySize(Byte cacheSize, int overheadSize) {
    return cacheSize + overheadSize;
}

int calculateTagSize(Byte physMem, int indexSize, int offsetSize) {
    return log2f(physMem) - indexSize - offsetSize;
}

int calculateOverheadSize(int numBlocks, int tagSize) {
    int overhead = numBlocks * (1 + tagSize);
    return overhead / 8;
}

CacheValues Cache_calculate_values(Byte cache_size_bytes, Byte block_size_bytes,
                                   int associativity,
                                   int physical_memory_bytes) {
    CacheValues cv;

    int cacheBits = log2f(cache_size_bytes);
    int sets = calculateSets(cache_size_bytes, block_size_bytes, associativity);
    int numBlocks = calculateBlocks(cache_size_bytes, block_size_bytes);
    int indexSize = calculateIndexSize(sets);
    int offsetSize = calculateOffsetSize(block_size_bytes);
    int tagSize =
        calculateTagSize(physical_memory_bytes, indexSize, offsetSize);
    int overheadSize = calculateOverheadSize(numBlocks, tagSize);
    int implementMemorySize =
        calculateImplementationMemorySize(cache_size_bytes, overheadSize);
    double implementMemorySizeInKB = (double)(implementMemorySize / pow(2, 10));
    double cost = (double)(COST_PER_KB * implementMemorySizeInKB);

    cv.cacheBits = cacheBits;
    cv.sets = sets;
    cv.numBlocks = numBlocks;
    cv.indexSize = indexSize;
    cv.offsetSize = offsetSize;
    cv.tagSize = tagSize;
    cv.overheadSize = overheadSize;
    cv.memorySize = implementMemorySize;
    cv.costPerKb = COST_PER_KB;
    cv.cost = cost;

    return cv;
}
