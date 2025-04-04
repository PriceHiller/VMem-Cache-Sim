#include "../lib/fmtstr/fmtstr.h"
#include "../lib/types/cache.h"
#include "../lib/types/physicalmemory.h"
#include <stdio.h>
#include <string.h>

char *build_section_title(char *section_title) {
    return str_fmt("***** %s *****", section_title);
}

char *_build_column_aligned_string(char *col1, char *col2) {
    return str_fmt("%-32s%s", col1, col2);
}

char *build_trace_files_string(TraceFiles *trace_files) {
    char *str = "Trace File(s):\n";
    for (size_t i = 0; i < trace_files->count; i++) {
        str = str_fmt("%s\t%s\n", str, trace_files->files[i]);
    }
    return str;
};

char *CacheParameters_to_string(CacheParameters *params) {
    char *str = build_trace_files_string(&params->traceFiles);
    str = str_fmt("%s\n%s\n\n", str,
                  build_section_title("Cache Input Parameters"));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Cache Size:", str_fmt("%d KB", params->cacheSize)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Block Size:", str_fmt("%d bytes", params->blockSize)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Associativity:", str_fmt("%d", params->associativity)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Replacement Policy:",
                      ReplacementPolicy_to_string(params->replacementPolicy)));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string("Physical Memory:",
                                     str_fmt("%d MB", params->physicalMemory)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Percent Memory Used by System:",
                      str_fmt("%.1f%%", params->percentMemoryUsed)));
    str = str_fmt("%s%s", str,
                  _build_column_aligned_string(
                      "Instructions / Time Slice:",
                      str_fmt("%d", params->instructionsPerTimeSlice)));
    return str;
}

char *CacheValues_to_string(CacheValues *cache_vals) {
    char *str =
        str_fmt("%s\n\n", build_section_title("Cache Calculated Values"));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Total # Blocks:", str_fmt("%d", cache_vals->numBlocks)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Tag Size:", str_fmt("%d bits", cache_vals->tagSize)));
    str =
        str_fmt("%s%s\n", str,
                _build_column_aligned_string(
                    "Index Size:", str_fmt("%d bits", cache_vals->indexSize)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Total # Rows:", str_fmt("%d", cache_vals->sets)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Overhead Size:",
                      str_fmt("%-5d %6s", cache_vals->overheadSize, "bytes")));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Implementation Memory Size:",
                      str_fmt("%.2f KB  (%u bytes)",
                              (((float)cache_vals->memorySize / BYTES_KB)),
                              cache_vals->memorySize)));
    str = str_fmt("%s%s", str,
                  _build_column_aligned_string(
                      "Cost", str_fmt("$%.2f @ $%.2f per KB", cache_vals->cost,
                                      cache_vals->costPerKb)));
    return str;
}

char *PhysicalMemory_to_string(PhysicalMemory *phys_mem) {
    char *str = str_fmt(
        "%s\n\n", build_section_title("Physical Memory Calculated Values"));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string("Number of Physical Pages:",
                                     str_fmt("%d", phys_mem->numPages)));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string("Number of Pages for System:",
                                     str_fmt("%d", phys_mem->numOSPages)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Size of Page Table Entry:",
                      str_fmt("%d bits", phys_mem->pageTableEntryBits)));
    str = str_fmt(
        "%s%s", str,
        _build_column_aligned_string("Total RAM for Page Table(s):)",
                                     str_fmt("%d", phys_mem->pageTableBytes)));
    return str;
}
