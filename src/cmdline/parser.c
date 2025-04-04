#include "../lib/fmtstr/fmtstr.h"
#include "../lib/types/cache.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// parses and validates the command line agruments and stores them in the
// parameters struct
CacheParameters read_cmdline_args(int argc, char *argv[]) {
    CacheParameters parameters;
    TraceFiles traceFiles;
    if (argc < 9) {
        printf("Error: Not enough args\n");
        exit(1);
    }

    parameters.traceFiles.count = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            parameters.cacheSize = (atol(argv[++i]) * BYTES_KB);
            if (parameters.cacheSize < (unsigned long)(8 * BYTES_KB) ||
                parameters.cacheSize > 16 * BYTES_MB) {
                printf("Error: Cache size must be between 8 KB and 8 MB\n");
                exit(1);
            }
            parameters.cacheSize = parameters.cacheSize / BYTES_KB;
        } else if (strcmp(argv[i], "-b") == 0) {
            parameters.blockSize = atoi(argv[++i]);
            if (parameters.blockSize < 8 || parameters.blockSize > 64) {
                printf("Error: Block size must be between 8 and 64 bytes\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            parameters.associativity = atoi(argv[++i]);
            if (parameters.associativity != 1 &&
                parameters.associativity != 2 &&
                parameters.associativity != 4 &&
                parameters.associativity != 8 &&
                parameters.associativity != 16) {
                printf("Error: Associativity must be 1, 2, 4, 8, or 16\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-r") == 0) {
            char *replacement_policy = str_fmt("%s", argv[++i]);
            parameters.replacementPolicy =
                ReplacementPolicy_from_string(replacement_policy);
            if (parameters.replacementPolicy == RP_UNKNOWN) {
                printf("Error: Replacement policy must be one of RR or RND\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-p") == 0) {
            parameters.physicalMemory = atoi(argv[++i]) * BYTES_MB;
            if (parameters.physicalMemory * BYTES_MB < 128 * BYTES_MB ||
                parameters.physicalMemory > 4 * BYTES_GB) {
                printf("Error: Physical memory must be between 1MB and 4GB\n");
                exit(1);
            }
            parameters.physicalMemory = parameters.physicalMemory / BYTES_MB;

        } else if (strcmp(argv[i], "-u") == 0) {
            parameters.percentMemoryUsed = atoi(argv[++i]);
            if (parameters.percentMemoryUsed < 0 ||
                parameters.percentMemoryUsed > 100) {
                printf("Error: Percent of physical memory used by OS must be "
                       "between 0 and 100 percent\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-n") == 0) {
            unsigned int timeslice = atoi(argv[++i]);
            if (timeslice == -1) {
                timeslice = 0xFFFFFFFF;
            }
            parameters.instructionsPerTimeSlice = timeslice;
        } else if (strcmp(argv[i], "-f") == 0) {
            if (parameters.traceFiles.count < 3) {
                parameters.traceFiles.files[parameters.traceFiles.count] =
                    str_fmt("%s", argv[++i]);
                parameters.traceFiles.count++;
            } else {
                printf("Error: Maximum of 3 trace files are allowed\n");
                exit(1);
            }
        } else {
            printf("Error, invalid argument `%s`\n", argv[i]);
            exit(1);
        }
    }

    return parameters;
}
