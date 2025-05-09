#include "cachecalculator/cachecalculator.h"
#include "cachesim/cachesim.h"
#include "cmdline/parser.h"
#include "instructions/reader.h"
#include "lib/types/cache.h"
#include "lib/types/physicalmemory.h"
#include "memorymanagement/memorymanagement.h"
#include "output/output.h"
#include "performance/performance.h"
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

    unsigned int proc_num = 0;
    bool procs_finished = false;
    PerfStats ps;
    perf_init(&ps);

    while (!procs_finished) {
        PageTableProcess *ptp = &proc_ptps[proc_num];
        Proc *proc = &ptp->proc;
        VMStats last_vm_stats;
        memcpy(&last_vm_stats, &vm_stats, sizeof(vm_stats));

        for (int i = 0; i < InputParams.instructionsPerTimeSlice; i++) {
            // If an instruction pointer is at the number of actual instructions
            // we have, then we have reached the end of a process's execution.
            //
            // We need to evict its tracked pages from the cache.
            if (proc->instructions.ip >= proc->instructions.count) {
                for (int i = 0; i < VIR_SPACE; i++) {
                    PTE pte = proc->pageTable[i];
                    int page = pte;
                    unmapPage(proc, pte);
                    Cache_invalidate_pte(&cache, pte);
                }
                break;
            }

            Instruction inst = proc->instructions.items[proc->instructions.ip];
            proc->instructions.ip += 1;

            int addr =
                vmemAccessMemory(proc, &cache, inst.start, &vm_stats, &ps);
            if (addr != -1) {
                inst_bytes += inst.len;
                perf_recordInstrStart(&ps, inst.len);
                Cache_lookup_addr(&cache, addr, inst.len, &ps);
            }

            if (inst.source.valid) {
                // +1 cycle for EA calc
                ps.totalCycles += 1;
                int source_addr = vmemAccessMemory(
                    proc, &cache, inst.source.address, &vm_stats, &ps);
                if (source_addr != -1) {
                    Cache_lookup_addr(&cache, source_addr, 4, &ps);
                    src_dst_bytes += 4;
                }
            }

            if (inst.dest.valid) {
                // +1 cycle for EA calc
                ps.totalCycles += 1;
                int dest_addr = vmemAccessMemory(
                    proc, &cache, inst.dest.address, &vm_stats, &ps);
                if (dest_addr != -1) {
                    Cache_lookup_addr(&cache, dest_addr, 4, &ps);
                    src_dst_bytes += 4;
                }
            }

            if (inst.source.valid && inst.dest.valid) {
                // Handle mem reads (MOV)
                ps.totalCycles += 2;
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

        procs_finished = true;
        for (int i = 0; i < proc_count; i++) {
            proc_num = (proc_num + 1) % proc_count;
            ptp = &proc_ptps[proc_num];
            if (ptp->proc.instructions.ip < ptp->proc.instructions.count) {
                procs_finished = false;
                break;
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
    CacheAccess access = {.totalCacheAccesses = cache.stat.accesses,
                          .totalRowsAccessed = cache.stat.accesses,
                          .eipBytes = inst_bytes,
                          .srcDstBytes = src_dst_bytes,
                          .totalAddresses = cache.stat.addresses};
    char *cache_sim_str =
        CacheSimulation_to_string(&access, &ps, &cache, &InputParams);
    printf("\n%s\n", cache_sim_str);
    return EXIT_SUCCESS;
}
