#ifndef LIBTYPES_PHYSMEM_H
#define LIBTYPES_PHYSMEM_H

#include "types.h"
typedef struct {
    Byte physBytes;         // Physical memory in bytes
    int numPages;           //  # of physical pages
    int numOSPages;         // # of OS pages
    int validBits;          // 1 valid bit
    int physicalPageBits;   // Bits required for the physical page number
    int pageTableEntryBits; // Total PTE size
    Byte pageTableBytes;    // Total RAM  for  page table
} PhysicalMemory;
#endif
