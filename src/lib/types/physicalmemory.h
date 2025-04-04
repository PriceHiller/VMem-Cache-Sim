#ifndef LIBTYPES_PHYSMEM_H
#define LIBTYPES_PHYSMEM_H

#include "types.h"
typedef struct {
    Byte physBytes;         // Physical memory in bytes
    unsigned int numPages;           //  # of physical pages
    unsigned int numOSPages;         // # of OS pages
    unsigned int validBits;          // 1 valid bit
    unsigned int physicalPageBits;   // Bits required for the physical page number
    unsigned int pageTableEntryBits; // Total PTE size
    Byte pageTableBytes;    // Total RAM  for  page table
} PhysicalMemory;
#endif
