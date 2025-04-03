#include "cachecalculator/cachecalculator.h"
#include "cmdline/parser.h"
#include "lib/types/cache.h"
#include "lib/types/physicalmemory.h"
#include "output/output.h"
#include "physmemcalc/physmemcalc.h"
#include <stdio.h>
#include <stdlib.h>

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
    return EXIT_SUCCESS;
}
