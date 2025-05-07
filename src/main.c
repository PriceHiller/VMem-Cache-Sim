#include "cachecalculator/cachecalculator.h"
#include "cachesim/cachesim.h"
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
    CacheParameters InputParams = read_cmdline_args(argc, argv);
    CacheValues cache_vals = Cache_calculate_values(
        InputParams.cacheSize * BYTES_KB, InputParams.blockSize,
        InputParams.associativity, InputParams.physicalMemory * BYTES_MB);
    PhysicalMemory physical_memory = PhysicalMemory_calculate_stats(
        InputParams.physicalMemory * BYTES_MB, InputParams.percentMemoryUsed,
        InputParams.traceFiles.count);

    char *cache_param_str = CacheParameters_to_string(&InputParams);
    char *cache_values_str = CacheValues_to_string(&cache_vals);
    char *physical_mem_str = PhysicalMemory_to_string(&physical_memory);

    printf("Cache Simulator - CS 3853 - Team #6\n\n%s\n\n%s\n\n%s\n",
           cache_param_str, cache_values_str, physical_mem_str);

    VMStats vm_stats;
    initPhysMem(physical_memory.physBytes, InputParams.percentMemoryUsed,
                &vm_stats);
    unsigned int proc_count = InputParams.traceFiles.count;
    PageTableProcess proc_ptps[InputParams.traceFiles.count];
    Proc procs[InputParams.traceFiles.count];

    Cache cache = Cache_init(cache_vals, InputParams.replacementPolicy);
    unsigned long inst_bytes = 0;
    unsigned long src_dst_bytes = 0;
    for (int i = 0; i < InputParams.traceFiles.count; i++) {
        FILE *trace_file = fopen(InputParams.traceFiles.files[i], "r");
        if (trace_file == NULL) {
            printf("Failed to read trace file '%s'!\n",
                   InputParams.traceFiles.files[i]);
            exit(1);
        }
        Instruction_Arr instructions = read_trace_file(trace_file);
        fclose(trace_file);

        Proc proc;
        initProc(&proc);
        procs[i] = proc;
        proc.instructions = instructions;
        PageTableProcess new_process = {
            .processName = InputParams.traceFiles.files[i],
            .proc = proc,
            .virtualPagesMapped = vm_stats.mapped,
            .pageTableHits = vm_stats.hits,
            .pageFaults = vm_stats.faults,
            .usedPTE = 0,
            .totalPTE = VIR_SPACE,
            .wastedPageTable =
                ((VIR_SPACE)*physical_memory.pageTableEntryBits) / 8};
        proc_ptps[i] = new_process;
    }

    unsigned int ptp_num = 0;
    bool procs_finished = false;
    while (!procs_finished) {
        PageTableProcess *ptp = &proc_ptps[ptp_num];
        VMStats last_vm_stats;
        memcpy(&last_vm_stats, &vm_stats, sizeof(vm_stats));

        Proc *proc = &ptp->proc;

        for (int i = 0; i < InputParams.instructionsPerTimeSlice; i++) {
            if (proc->instructions.ip >= proc->instructions.count) {
                break;
            }
            Instruction inst = proc->instructions.items[proc->instructions.ip];
            proc->instructions.ip += 1;

            int addr = vmemAccessMemory(proc, inst.start, &vm_stats);
            if (addr != -1) {
                inst_bytes += inst.len;
                Cache_lookup_addr(&cache, addr, inst.len);
            }

            if (inst.source.valid) {
                int source_addr =
                    vmemAccessMemory(proc, inst.source.address, &vm_stats);
                if (source_addr != -1) {
                    Cache_lookup_addr(&cache, source_addr, 4);
                    src_dst_bytes += 4;
                }
            }

            if (inst.dest.valid) {
                int dest_addr =
                    vmemAccessMemory(proc, inst.dest.address, &vm_stats);
                if (dest_addr != -1) {
                    Cache_lookup_addr(&cache, dest_addr, 4);
                    src_dst_bytes += 4;
                }
            }
        }

        int used_pte_entries = count_used_pte_entries(proc->pageTable);
        ptp->virtualPagesMapped += vm_stats.mapped - last_vm_stats.mapped;
        ptp->pageTableHits = vm_stats.hits - last_vm_stats.hits;
        ptp->pageFaults = vm_stats.faults - last_vm_stats.faults;
        ptp->usedPTE = used_pte_entries;
        ptp->totalPTE = VIR_SPACE;
        ptp->wastedPageTable = ((VIR_SPACE - used_pte_entries) *
                                physical_memory.pageTableEntryBits) /
                               8;

        ptp_num++;
        if (ptp_num >= proc_count) {
            ptp_num = 0;
        }

        for (unsigned int proc_num = 0; proc_num < proc_count; proc_num++) {
            Proc *check_proc = &proc_ptps[proc_num].proc;
            if (check_proc->instructions.count == check_proc->instructions.ip) {
                procs_finished = true;
            } else {
                procs_finished = false;
            }
        }
    }

    SimulationStats testStats = {.numPages = physical_memory.numPages,
                                 .pagesUsedForOS = vm_stats.sysPages,
                                 .pagesAvailable = vm_stats.freePages,
                                 .totalVirtualPagesMapped = vm_stats.mapped,
                                 .pageTableHits = vm_stats.hits,
                                 .pagesFromFree = vm_stats.pagesFromFree,
                                 .totalPageFaults = vm_stats.faults,
                                 .pageTableProcesses = proc_ptps,
                                 .processCt = proc_count};
    char *virtual_mem_str = VirtualMemory_to_string(&testStats);
    printf("\n%s\n", virtual_mem_str);

    printf("***** CACHE SIMULATION RESULTS  *****\n\n");
    printf("Total Cache Accesses:           %d  (%d addresses)\n",
           cache.stat.accesses, cache.stat.addresses);
    printf("--- Instruction Bytes:          %lu\n", inst_bytes);
    printf("--- SrcDst Bytes:               %lu\n", src_dst_bytes);
    printf("Cache Hits:                     %d\n", cache.stat.hits);
    printf("Cache Misses:                   %d\n",
           cache.stat.misses.compulsory + cache.stat.misses.conflict);
    printf("--- Compulsory Misses:          %d\n",
           cache.stat.misses.compulsory);
    printf("--- Conflict Misses:            %d\n", cache.stat.misses.conflict);
    return EXIT_SUCCESS;
}
