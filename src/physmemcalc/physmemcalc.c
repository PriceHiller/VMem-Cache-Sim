#include "../lib/types/physicalmemory.h"
#include <math.h>

static const Byte PAGE_SIZE_BYTES = 4 * BYTES_KB;
static const Byte VIRTUAL_ADDRESS_SPACE_BYTES = 2ULL * BYTES_GB;

PhysicalMemory PhysicalMemory_calculate_stats(Byte physical_memory_bytes,
                                              int percent_memory_used,
                                              int files) {
    PhysicalMemory stats;

    // Convert physmem to bytes
    stats.physBytes = physical_memory_bytes;

    // # of physical pages
    stats.numPages = stats.physBytes / PAGE_SIZE_BYTES;

    // # of OS pages
    stats.numOSPages = (stats.numPages * percent_memory_used) / 100;

    // # of virtual pages
    int vPage = VIRTUAL_ADDRESS_SPACE_BYTES / PAGE_SIZE_BYTES;

    // # of bits for  physical page number
    int physPageBits = (int)ceil(log2((double)stats.numPages));

    // page table entry in bits
    int pteSizeBits = 1 + physPageBits;

    // total RAM  for page table
    int pageTableBytes = (vPage * pteSizeBits / 8) * files;

    // Fill in the struct.
    stats.validBits = 1;
    stats.physicalPageBits = physPageBits;
    stats.pageTableEntryBits = pteSizeBits;
    stats.pageTableBytes = pageTableBytes;

    return stats;
}
