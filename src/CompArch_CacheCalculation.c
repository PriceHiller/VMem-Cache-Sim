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
int calculateOffsetSize(int associativity) { return log2f(associativity); }
int calculateIndexSize(int sets) { return log2f(sets); }
int calculateTagSize(int cacheBits, int indexSize, int offsetSize) {
    return cacheBits - indexSize - offsetSize;
}

int calculateOverheadSize(int numBlocks, int tagSize) {
    int overhead = numBlocks * (1 + tagSize);
    return overhead / 8;
}

double calculateImplementationMemorySize(int cacheSize, int overheadSize) {
    return cacheSize + overheadSize;
}

int main(int argc, char *argv[]) {
    // printf("%.2lf", log2f(512));
    if (argc < 2) {
        printf("Not enough args.");
        return 1;
    }

    int cacheSize = 512 * pow(2, 10);
    int cacheBits = log2f(cacheSize);

    int blockSize = atoi(argv[1]);
    int associativity = atoi(argv[2]);
    int sets = calculateSets(cacheSize, blockSize, associativity);
    int numBlocks = calculateBlocks(cacheSize, blockSize);
    int indexSize = calculateIndexSize(sets);
    int offsetSize = calculateOffsetSize(associativity);
    int tagSize = calculateTagSize(cacheBits, indexSize, offsetSize);
    int overheadSize = calculateOverheadSize(numBlocks, tagSize);
    int implementMemSize =
        calculateImplementationMemorySize(cacheSize, overheadSize);
    double implementMemSizeKB = (double)(implementMemSize / pow(2, 10));
    printf("Total # Blocks: \t\t%d\n", calculateBlocks(cacheSize, blockSize));
    printf("Cache Bits: \t\t\t%d\n", cacheBits);
    printf("Tag Size: \t\t\t%d bits\n", tagSize);
    printf("Index Size: \t\t\t%d bits\n", indexSize);
    printf("Offset Size: \t\t\t%d bits\n", offsetSize);
    printf("Total # Rows: \t\t\t%d\n", sets);
    printf("Overhead Size: \t\t\t%d bytes\n", overheadSize);
    printf("Implementation Memory Size: \t%.2lf KB (%d bytes)\n",
           implementMemSizeKB, implementMemSize);

    return 0;
}
