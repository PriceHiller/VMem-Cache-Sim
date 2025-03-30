#include <math.h>
#include <stdio.h>
#include <stdlib.h>

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

int calculateTagSize(int physMem, int indexSize, int offsetSize) {
    return log2f(physMem) - indexSize - offsetSize;
}

int calculateOverheadSize(int numBlocks, int tagSize) {
    int overhead = numBlocks * (1 + tagSize);
    return overhead / 8;
}

void calculateCacheValues(int cacheSize, int blockSize, int associativity, int physMem) {
    int cacheBits = log2f(cacheSize);
    int sets = calculateSets(cacheSize, blockSize, associativity);
    int numBlocks = calculateBlocks(cacheSize, blockSize);
    int indexSize = calculateIndexSize(sets);
    int offsetSize = calculateOffsetSize(blockSize);
    int tagSize = calculateTagSize(physMem, indexSize, offsetSize);
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
    int physMem = 128 * pow(2, 20); // 128 mb in example

    calculateCacheValues(cacheSize, blockSize, associativity, physMem);

    return 0;
}
