#include "integration.h"

CacheAccess access;

unsigned int rowsAccessed(Address address, Byte addrSize, Byte blockSize) {
    return ((address + addrSize - 1) / blockSize) - (address / blockSize) + 1;
}

void sendToSim(Address address, Byte addrSize, Byte blockSize,
               AccessType type) {
    unsigned int rows = rowsAccessed(address, addrSize, blockSize);
    access.totalAddresses++;
    access.totalRowsAccessed += rows;

    if (type == EIP) {
        access.eipBytes += addrSize;
        // increment total instructions by 1, e.g. totalInstructions++;
        // increment total cycles by 2, e.g. totalCycles += 2;
    } else if (type == SRC) {
        access.srcBytes += addrSize;
        // increment total instructions by 1, e.g. totalInstructions++;
        // increment total cycles by 1, e.g. totalCycles++;
    } else if (type == DST) {
        access.dstBytes += addrSize;
        // increment total instructions by 1, e.g. totalInstructions++;
        // increment total cycles by 1, e.g. totalCycles++;
    }

    unsigned int i;
    for (i = 0; i < rows; i++) {
        unsigned int rowAddress = (address & ~(blockSize - 1)) + i * blockSize;
        access.totalCacheAccesses++;
    }
}
