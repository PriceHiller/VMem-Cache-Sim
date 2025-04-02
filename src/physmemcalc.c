#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PAGE_SIZE 4096  // bytes per page
#define VIRTUAL_ADDRESS_SPACE_BYTES (2ULL * 1024 * 1024 * 1024)

typedef struct {
    int physBytes;                // Physical memory in bytes
    int numPages;                 //  # of physical pages
    int numOSPages;               // # of OS pages
    int validBits;                // 1 valid bit
    int physPageBits;             // Bits required for the physical page number
    int pteBits;                 // Total PTE size 
    int ptByte;                  // Total RAM  for  page table
} MemoryStats;

MemoryStats calculateMemoryStats(int physmem, int os, int files) {
    MemoryStats stats;
    
    // Convert physmem to bytes
    stats.physBytes = physmem * 1024 * 1024;
    
    // # of physical pages
    stats.numPages = stats.physBytes / PAGE_SIZE;
    
    // # of OS pages
    stats.numOSPages = (stats.numPages * os) / 100;
    
    // # of virtual pages 
    int vPage = VIRTUAL_ADDRESS_SPACE_BYTES / PAGE_SIZE;
    
    // # of bits for  physical page number
    int physPageBits = (int)ceil(log2((double)stats.numPages));
    
    // page table entry in bits
    int pteSizeBits = 1 + physPageBits;
    
    // total RAM  for page table
    int pageTableBytes = (vPage * pteSizeBits / 8) * files;
    
    // Fill in the struct.
    stats.validBits = 1;
    stats.physPageBits = physPageBits;
    stats.pteBits = pteSizeBits;
    stats.ptByte = pageTableBytes;
    
    return stats;
}

