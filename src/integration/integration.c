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
    } else if (type == SRC) {
        access.srcBytes += addrSize;
    } else if (type == DST) {
        access.dstBytes += addrSize;
    }

    unsigned int i;
    for (i = 0; i < rows; i++) {
        access.totalCacheAccesses++;
    }
}
