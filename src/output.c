#include "lib/fmtstr/fmtstr.h"
#include <stdio.h>
#include <string.h>

typedef enum ReplacementPolicy { RP_ROUND_ROBIN, RP_RANDOM } ReplacementPolicy;

typedef struct CacheInputParameters {
    unsigned int cacheSize;
    unsigned int blockSize;
    unsigned int associativity;
    ReplacementPolicy replacementPolicy;
    unsigned int physicalMemoryMB;
    float percentMemoryUsedBySystem;
    unsigned int instructionsPerTimeSlice;

} CacheInputParameters;

char *ReplacementPolicy_to_string(ReplacementPolicy *rp) {
    if (*rp == RP_ROUND_ROBIN) {
        return "Round Robin";
    }
    return "Random";
}

char *build_section_title(char *section_title) {
    return str_fmt("***** %s *****", section_title);
}

char *_build_column_aligned_string(char *col1, char *col2) {
    return str_fmt("%-32s%s", col1, col2);
}

char *CacheInputParameters_to_string(CacheInputParameters *params) {
    char *str =
        str_fmt("%s\n\n", build_section_title("Cache Input Parameters"));
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
                      ReplacementPolicy_to_string(&params->replacementPolicy)));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string("Physical Memory:",
                                     str_fmt("%d MB", params->physicalMemoryMB)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Percent Memory Used by System:",
                      str_fmt("%.1f%%", params->percentMemoryUsedBySystem)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Instructions / Time Slice:",
                      str_fmt("%d", params->instructionsPerTimeSlice)));
    return str;
}

char *build_trace_files_string(char **trace_files, size_t trace_file_count) {
    char *str = "Trace File(s):\n";
    for (size_t i = 0; i < trace_file_count; i++) {
        str = str_fmt("%s\t%s\n", str, trace_files[i]);
    }
    return str;
};

typedef struct CacheCalculatedValues {
    unsigned int totalBlocks;
    unsigned int tagSize;
    unsigned int indexSize;
    unsigned int numRows;
    unsigned int overheadSize;
    unsigned int implementationMemorySize;
    float costPerKb;
} CacheCalculatedValues;

char *CacheCalculdatedValues_to_string(CacheCalculatedValues *vals) {
    char *str =
        str_fmt("%s\n\n", build_section_title("Cache Calculated Values"));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Total # Blocks:", str_fmt("%d", vals->totalBlocks)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Tag Size:", str_fmt("%d bits", vals->tagSize)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Index Size:", str_fmt("%d bits", vals->indexSize)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string("Total # Rows:",
                                               str_fmt("%d", vals->numRows)));
    str =
        str_fmt("%s%s\n", str,
                _build_column_aligned_string(
                    "Overhead Size:", str_fmt("%d bytes", vals->overheadSize)));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string("Implementation Memory Size:",
                                     str_fmt("%d KB", vals->overheadSize)));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string(
            "Cost", str_fmt("$.2f @ $.2f per KB", vals->costPerKb, 12.12)));
    return str;
}

typedef struct PhysicalMemoryCalculatedValues {
    unsigned int numPhysicalPages;
    unsigned int numSystemPages;
    unsigned int pageTableEntrySize;
    unsigned int pageTableRamUsage;
} PhysicalMemoryCalculatedValues;

char *
PhysicalMemoryCalculatedValues_to_string(PhysicalMemoryCalculatedValues *vals) {
    char *str =
        str_fmt("%s\n\n", build_section_title("Cache Calculated Values"));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string("Number of Physical Pages:",
                                     str_fmt("%d", vals->numPhysicalPages)));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string("Number of Pages for System:",
                                     str_fmt("%d", vals->numSystemPages)));
    str = str_fmt("%s%s\n", str,
                  _build_column_aligned_string(
                      "Size of Page Table Entry:",
                      str_fmt("%d bits", vals->pageTableEntrySize)));
    str = str_fmt(
        "%s%s\n", str,
        _build_column_aligned_string("Total RAM for Page Table(s):)",
                                     str_fmt("%d", vals->pageTableRamUsage)));
    return str;
}
