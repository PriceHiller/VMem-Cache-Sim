#include "integration.h"

CacheAccess access;

unsigned int rowsAccessed(Address address, Byte addrSize, Byte blockSize) {
    return ((address + addrSize - 1) / blockSize) - (address / blockSize) + 1;
}
