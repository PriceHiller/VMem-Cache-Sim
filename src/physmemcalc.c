#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 4096  // bytes / page
#define VIRTUAL_ADDRESS_SPACE_BYTES (2ULL * 1024 * 1024 * 1024)

void calculateMemoryStats(int physmem, int os, int files) {
    // physical memory to bytes
    int physBytes = physmem * 1024 * 1024;
    // total # of physical pages
    int numPages = physBytes / PAGE_SIZE;

    //# of pages  for  OS
    int numOSPages = (numPages * os) / 100;

    // # of virtual pages per process
    int pages = VIRTUAL_ADDRESS_SPACE_BYTES / PAGE_SIZE;

    // Page Table Entry (PTE) size is given as 19 bits.
    int pteSizeBits = 19;
    // total RAM needed 
    int pageTable = (pages * pteSizeBits / 8) * files;

    printf("----- Physical Memory & Page Table Calculations -----\n");
    printf("Physical Memory: %d MB (%d bytes)\n", physmem, physBytes);
    printf("Number of Physical Pages: %d\n", numPages);
    printf("OS Percentage: %d%%\n", os);
    printf("Number of Pages Reserved for OS: %d\n", numOSPages);
    printf("Effective Virtual Pages per Process: %d\n", pages);
    printf("Page Table Entry Size: %d bits\n", pteSizeBits);
    printf("Number of Trace Files (Processes): %d\n", files);
    printf("Total RAM Required for Page Table(s): %d bytes\n", pageTable);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <physical_memory_MB> <os_percentage> <num_trace_files>\n", argv[0]);
        return 1;
    }

    int physmem = atoi(argv[1]);
    int os = atoi(argv[2]);
    int files = atoi(argv[3]);

    calculateMemoryStats(physmem, os, files);

    return 0;
}
