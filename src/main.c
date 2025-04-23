#include "cachecalculator/cachecalculator.h"
#include "cmdline/parser.h"
#include "instructions/address.h"
#include "instructions/reader.h"
#include "lib/types/cache.h"
#include "lib/types/physicalmemory.h"
#include "memorymanagement/memorymanagement.h"
#include "output/output.h"
#include "physmemcalc/physmemcalc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    CacheParameters cache_params = read_cmdline_args(argc, argv);
    CacheValues cache_vals = Cache_calculate_values(
        cache_params.cacheSize * BYTES_KB, cache_params.blockSize,
        cache_params.associativity, cache_params.physicalMemory * BYTES_MB);
    PhysicalMemory physical_memory = PhysicalMemory_calculate_stats(
        cache_params.physicalMemory * BYTES_MB, cache_params.percentMemoryUsed,
        cache_params.traceFiles.count);

    char *cache_param_str = CacheParameters_to_string(&cache_params);
    char *cache_values_str = CacheValues_to_string(&cache_vals);
    char *physical_mem_str = PhysicalMemory_to_string(&physical_memory);

    printf("Cache Simulator - CS 3853 - Team #6\n\n%s\n\n%s\n\n%s\n",
           cache_param_str, cache_values_str, physical_mem_str);

    VMStats vm_stats;
    initPhysMem(physical_memory.physBytes, cache_params.percentMemoryUsed,
                &vm_stats);
    int proc_count = cache_params.traceFiles.count;
    PageTableProcess procs[cache_params.traceFiles.count];

    for (int i = 0; i < cache_params.traceFiles.count; i++) {
        FILE *trace_file = fopen(cache_params.traceFiles.files[i], "r");
        Instruction_Arr instructions = read_trace_file(trace_file);
        fclose(trace_file);

        Proc proc;
        initProc(&proc);
        VMStats last_vm_stats;
        memcpy(&last_vm_stats, &vm_stats, sizeof(vm_stats));

        for (int i = 0; i < instructions.count; i++) {
            Instruction inst = instructions.items[i];
            accessMemory(&proc, inst.start, &vm_stats);
            if (inst.source.valid) {
                accessMemory(&proc, inst.source.address, &vm_stats);
            }
            if (inst.dest.valid) {
                accessMemory(&proc, inst.dest.address, &vm_stats);
            }
        }
        int used_pte_entries = count_used_pte_entries(proc.pageTable);
        PageTableProcess new_process = {
            .processName = cache_params.traceFiles.files[i],
            .virtualPagesMapped = vm_stats.mapped - last_vm_stats.mapped,
            .pageTableHits = vm_stats.hits - last_vm_stats.hits,
            .pageFaults = vm_stats.faults - last_vm_stats.faults,
            .usedPTE = used_pte_entries,
            .totalPTE = VIR_SPACE,
            .wastedPageTable = ((VIR_SPACE - used_pte_entries) *
                                physical_memory.pageTableEntryBits) /
                               8};
        procs[i] = new_process;
    }

    SimulationStats testStats = {.numPages = physical_memory.numPages,
                                 .pagesUsedForOS = vm_stats.sysPages,
                                 .pagesAvailable = vm_stats.freePages,
                                 .totalVirtualPagesMapped = vm_stats.mapped,
                                 .pageTableHits = vm_stats.hits,
                                 .pagesFromFree = vm_stats.pagesFromFree,
                                 .totalPageFaults = vm_stats.faults,
                                 .pageTableProcesses = procs,
                                 .processCt = proc_count};
    char *virtual_mem_str = VirtualMemory_to_string(&testStats);
    printf("\n%s\n", virtual_mem_str);
    return EXIT_SUCCESS;
}
