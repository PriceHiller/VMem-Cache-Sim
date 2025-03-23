#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// cache calculation

int calculateBlocks(int cacheSize, int blockSize) {
    return cacheSize / blockSize;
}
int calculateSets(int cacheSize, int blockSize, int associativity) {
    return cacheSize / (blockSize * associativity);
}
int calculateOffsetSize(int blockSize) { return log2f(blockSize); }
int calculateIndexSize(int sets) { return log2f(sets); }
double calculateImplementationMemorySize(int cacheSize, int overheadSize) {
    return cacheSize + overheadSize;
}

int calculateTagSize(int cacheBits, int indexSize, int offsetSize) {
    if (0 < cacheBits && cacheBits <= 16) { // 16-bit address space
        return 16 - indexSize - offsetSize;
    } else if (16 < cacheBits && cacheBits <= 32) { // 32-bit address space
        return 32 - indexSize - offsetSize;
    }
    return 64 - indexSize - offsetSize; // 64-bit address space
}

int calculateOverheadSize(int numBlocks, int tagSize) {
    int overhead = numBlocks * (1 + tagSize);
    return overhead / 8;
}

void calculateCacheValues(int cacheSize, int blockSize, int associativity) {
    int cacheBits = log2f(cacheSize);
    int sets = calculateSets(cacheSize, blockSize, associativity);
    int numBlocks = calculateBlocks(cacheSize, blockSize);
    int indexSize = calculateIndexSize(sets);
    int offsetSize = calculateOffsetSize(blockSize);
    int tagSize = calculateTagSize(cacheBits, indexSize, offsetSize);
    int overheadSize = calculateOverheadSize(numBlocks, tagSize);
    int implementMemorySize =
        calculateImplementationMemorySize(cacheSize, overheadSize);
    double implementMemorySizeKB = (double)(implementMemorySize / pow(2, 10));
    double cost = (double)(0.12 * implementMemorySizeKB);

    printf("Total # Blocks: \t\t%d\n", calculateBlocks(cacheSize, blockSize));
    // printf("Cache Bits: \t\t\t%d bits\n", cacheBits);
    printf("Tag Size: \t\t\t%d bits\n", tagSize);
    printf("Index Size: \t\t\t%d bits\n", indexSize);
    // printf("Offset Size: \t\t\t%d bits\n", offsetSize);
    printf("Total # Rows: \t\t\t%d\n", sets);
    printf("Overhead Size: \t\t\t%d bytes\n", overheadSize);
    printf("Implementation Memory Size: \t%.2lf KB (%d bytes)\n",
           implementMemorySizeKB, implementMemorySize);
    printf("Cost: \t\t\t\t$%.2lf @ $0.12 per KB\n", cost);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Not enough args.");
        return 1;
    }

    int cacheSize = atoi(argv[1]) * pow(2, 10);
    int blockSize = atoi(argv[2]);
    int associativity = atoi(argv[3]);

    calculateCacheValues(cacheSize, blockSize, associativity);

    return 0;
}
