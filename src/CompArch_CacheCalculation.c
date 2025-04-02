#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// cache calculation

typedef struct {
    int cacheBits;
    int sets;
    int numBlocks;
    int indexSize;
    int offsetSize;
    int tagSize;
    int overheadSize;
    double implementMemorySize;
    double implementMemorySizeInKB;
    double cost;
} CacheValues;

int calculateBlocks(int cacheSize, int blockSize) { return cacheSize / blockSize; }
int calculateSets(int cacheSize, int blockSize, int associativity) { return cacheSize / (blockSize * associativity); }
int calculateOffsetSize(int blockSize) { return log2f(blockSize); }
int calculateIndexSize(int sets) { return log2f(sets); }
double calculateImplementationMemorySize(int cacheSize, int overheadSize) { return cacheSize + overheadSize; }
int calculateTagSize(int physMem, int indexSize, int offsetSize) { return log2f(physMem) - indexSize - offsetSize; }

int calculateOverheadSize(int numBlocks, int tagSize) {
    int overhead = numBlocks * (1 + tagSize);
    return overhead / 8;
}

CacheValues calculateCacheValues(int cacheSize, int blockSize, int associativity, int physMem) {
    CacheValues cv;

    int cacheBits = log2f(cacheSize);
    int sets = calculateSets(cacheSize, blockSize, associativity);
    int numBlocks = calculateBlocks(cacheSize, blockSize);
    int indexSize = calculateIndexSize(sets);
    int offsetSize = calculateOffsetSize(blockSize);
    int tagSize = calculateTagSize(physMem, indexSize, offsetSize);
    int overheadSize = calculateOverheadSize(numBlocks, tagSize);
    int implementMemorySize =
        calculateImplementationMemorySize(cacheSize, overheadSize);
    double implementMemorySizeInKB = (double)(implementMemorySize / pow(2, 10));
    double cost = (double)(0.12 * implementMemorySizeInKB);

    cv.cacheBits = cacheBits;
    cv.sets = sets;
    cv.numBlocks = numBlocks;
    cv.indexSize = indexSize;
    cv.offsetSize = offsetSize;
    cv.tagSize = tagSize;
    cv.overheadSize = overheadSize;
    cv.implementMemorySize = implementMemorySize;
    cv.implementMemorySizeInKB = implementMemorySizeInKB;
    cv.cost = cost;


    return cv;    
}
