#include "../cachesim/cachesim.h"
#include "../integration/integration.h"
#include "../lib/fmtstr/fmtstr.h"
#include "../lib/types/cache.h"
#include "../lib/types/physicalmemory.h"
#include "../performance/performance.h"
#include "../virtmemresults/virtMemResults.h"
#include <stdio.h>
#include <string.h>

char *build_section_title(char *section_title) {
    return str_fmt("***** %s *****", section_title);
}

char *build_column_aligned_string(char *col1, char *col2) {
    return str_fmt("%-32s%s", col1, col2);
}

char *build_trace_files_string(TraceFiles *trace_files) {
    char *str = "Trace File(s):\n";
    size_t i;
    for (i = 0; i < trace_files->count; i++) {
        str = str_fmt("%s\t%s\n", str, trace_files->files[i]);
    }
    return str;
}

char *CacheParameters_to_string(CacheParameters *params) {
    char *str = build_trace_files_string(&params->traceFiles);
    str = str_fmt("%s\n%s\n\n", str,
                  build_section_title("Cache Input Parameters"));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Cache Size:", str_fmt("%d KB", params->cacheSize)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Block Size:", str_fmt("%d bytes", params->blockSize)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Associativity:", str_fmt("%d", params->associativity)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Replacement Policy:",
                      ReplacementPolicy_to_string(params->replacementPolicy)));
    str = str_fmt(
        "%s%s\n", str,
        build_column_aligned_string("Physical Memory:",
                                    str_fmt("%d MB", params->physicalMemory)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Percent Memory Used by System:",
                      str_fmt("%.1f%%", params->percentMemoryUsed)));
    str = str_fmt("%s%s", str,
                  build_column_aligned_string(
                      "Instructions / Time Slice:",
                      str_fmt("%u", params->instructionsPerTimeSlice)));
    return str;
}

char *CacheValues_to_string(CacheValues *cache_vals) {
    char *str =
        str_fmt("%s\n\n", build_section_title("Cache Calculated Values"));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Total # Blocks:", str_fmt("%d", cache_vals->numBlocks)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Tag Size:", str_fmt("%d bits", cache_vals->tagBits)));
    str =
        str_fmt("%s%s\n", str,
                build_column_aligned_string(
                    "Index Size:", str_fmt("%d bits", cache_vals->indexBits)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string("Total # Rows:",
                                              str_fmt("%d", cache_vals->sets)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Overhead Size:",
                      str_fmt("%-5d %6s", cache_vals->overheadSize, "bytes")));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Implementation Memory Size:",
                      str_fmt("%.2f KB  (%u bytes)",
                              (((float)cache_vals->memorySize / BYTES_KB)),
                              cache_vals->memorySize)));
    str = str_fmt("%s%s", str,
                  build_column_aligned_string(
                      "Cost", str_fmt("$%.2f @ $%.2f per KB", cache_vals->cost,
                                      cache_vals->costPerKb)));
    return str;
}

char *PhysicalMemory_to_string(PhysicalMemory *phys_mem) {
    char *str = str_fmt(
        "%s\n\n", build_section_title("Physical Memory Calculated Values"));
    str =
        str_fmt("%s%s\n", str,
                build_column_aligned_string("Number of Physical Pages:",
                                            str_fmt("%d", phys_mem->numPages)));
    str = str_fmt(
        "%s%s\n", str,
        build_column_aligned_string("Number of Pages for System:",
                                    str_fmt("%d", phys_mem->numOSPages)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Size of Page Table Entry:",
                      str_fmt("%d bits", phys_mem->pageTableEntryBits)));
    str = str_fmt("%s%s", str,
                  build_column_aligned_string(
                      "Total RAM for Page Table(s):",
                      str_fmt("%d bytes", phys_mem->pageTableBytes)));
    return str;
}
char *VirtualMemory_to_string(SimulationStats *sim) {
    char *str = str_fmt(
        "%s\n\n", build_section_title("VIRTUAL MEMORY SIMULATION RESULTS"));
    str = str_fmt(
        "%s%s\n", str,
        build_column_aligned_string("Physical Pages Used By SYSTEM:",
                                    str_fmt("%u", sim->pagesUsedForOS)));
    str = str_fmt(
        "%s%s\n", str,
        build_column_aligned_string("Pages Available to User:",
                                    str_fmt("%u", sim->pagesAvailable)));
    str = str_fmt("%s\n", str);

    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Virtual Pages Mapped:",
                      str_fmt("%u", sim->totalVirtualPagesMapped)));
    str = str_fmt("%s\t------------------------------\n", str);
    str = str_fmt("%s\tPage Table Hits: %u\n", str, sim->pageTableHits);
    str = str_fmt("%s\tPages from Free: %u\n", str, sim->pagesFromFree);
    str = str_fmt("%s\tTotal Page Faults: %u\n\n", str, sim->totalPageFaults);

    str = str_fmt(
        "%sPage Table Usage Per Process:\n------------------------------\n",
        str);
    for (int i = 0; i < sim->processCt; i++) {
        PageTableProcess *proc = &sim->pageTableProcesses[i];
        float usage_percent =
            ((float)proc->usedPTE / (float)proc->totalPTE) * 100.0f;
        str = str_fmt("%s[%d] %s:\n", str, i, proc->processName);
        str = str_fmt("%s\tUsed Page Table Entries: %u  ( %.2f%% )\n", str,
                      proc->usedPTE, usage_percent);
        str = str_fmt("%s\tPage Table Wasted: %u bytes\n\n", str,
                      proc->wastedPageTable);
    }
    return str;
}

char *CacheSimulation_to_string(CacheAccess *access, PerfStats *perf,
                                Cache *cache, CacheParameters *params) {

    perf_finalize(perf);

    // hit and miss rates
    float hitRate = 0.0f, missRate = 0.0f;
    unsigned int total_cache_misses =
        cache->stat.misses.compulsory + cache->stat.misses.conflict;
    if (perf->totalCycles > 0) {
        hitRate =
            100.0f * ((float)cache->stat.hits / (float)cache->stat.accesses);
        missRate = 100.0f *
                   ((float)(total_cache_misses) / (float)cache->stat.accesses);
    }

    // unused cache space and unused cache blocks
    unsigned int totalBlocks = cache->info.numBlocks;
    int unusedBlocks = cache->info.numBlocks - cache->stat.misses.compulsory;

    int overheadPerBlock =
        (totalBlocks > 0) ? cache->info.overheadSize / totalBlocks : 0;
    float unusedKB =
        (float)(unusedBlocks * (params->blockSize + overheadPerBlock)) /
        1024.0f;
    float totalKB = cache->info.memorySize / 1024.f;

    float unusedPercent =
        (totalKB > 0.0f) ? (100.0f * unusedKB / totalKB) : 0.0f;
    float wasteCost = unusedKB * 0.12f;

    char *str =
        str_fmt("%s\n\n", build_section_title("CACHE SIMULATION RESULTS"));

    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string("Total Cache Accesses:",
                                              str_fmt("%u (%u addresses)",
                                                      cache->stat.accesses,
                                                      access->totalAddresses)));

    str =
        str_fmt("%s%s\n", str,
                build_column_aligned_string("--- Instruction Bytes:",
                                            str_fmt("%llu", access->eipBytes)));

    str =
        str_fmt("%s%s\n", str,
                build_column_aligned_string(
                    "--- SrcDst Bytes:", str_fmt("%llu", access->srcDstBytes)));

    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string("Cache Hits:",
                                              str_fmt("%u", cache->stat.hits)));

    str = str_fmt(
        "%s%s\n", str,
        build_column_aligned_string(
            "Cache Misses:", str_fmt("%u", cache->stat.misses.compulsory +
                                               cache->stat.misses.conflict)));

    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "--- Compulsory Misses:",
                      str_fmt("%u", cache->stat.misses.compulsory)));
    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "--- Conflict Misses:",
                      str_fmt("%u\n", cache->stat.misses.conflict)));

    str = str_fmt("%s%s\n\n", str,
                  build_section_title("*****  CACHE HIT & MISS RATE:  *****"));
    str = str_fmt(
        "%s%s\n", str,
        build_column_aligned_string("Hit Rate:", str_fmt("%.4f%%", hitRate)));
    str = str_fmt(
        "%s%s\n", str,
        build_column_aligned_string("Miss Rate:", str_fmt("%.4f%%", missRate)));

    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "CPI:", str_fmt("%.4f Cycles/Instruction (%llu)",
                                      perf->CPI, perf->totalInstructions)));

    str = str_fmt("%s%s\n", str,
                  build_column_aligned_string(
                      "Unused Cache Space:",
                      str_fmt("%.2f KB / %.2f KB = %.2f%%  Waste: $%.2f/chip",
                              unusedKB, totalKB, unusedPercent, wasteCost)));

    str = str_fmt("%s%s", str,
                  build_column_aligned_string(
                      "Unused Cache Blocks:",
                      str_fmt("%u / %u", unusedBlocks, totalBlocks)));

    return str;
}
