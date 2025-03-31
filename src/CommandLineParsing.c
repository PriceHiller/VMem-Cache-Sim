#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int cacheSize;
    int blockSize;
    int associativity;
    char replacementPolicy[4];
    int physicalMemory;
    int percentMemoryUsed;
    int instructionsPerSlice;
    char traceFiles[3][100];
    int traceFileCount;
} Parameters;

//parses and validates the command line agruments and stores them in the parameters struct
void inputs(int argc, char *argv[], Parameters *parameters) {
    if (argc < 9) {
        printf("Error: Not enough args\n");
        exit(1);
    }

    parameters->traceFileCount = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0) {
            parameters->cacheSize = atoi(argv[++i])*1024;
            if (parameters->cacheSize < 8192 || parameters->cacheSize > 8388608) {
                printf("Error: Cache size must be between 8 KB and 8 MB\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-b") == 0) {
            parameters->blockSize = atoi(argv[++i]);
            if (parameters->blockSize < 8 || parameters->blockSize > 64) {
                printf("Error: Block size must be between 8 and 64 bytes\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-a") == 0) {
            parameters->associativity = atoi(argv[++i]);
            if (parameters->associativity != 1 && parameters->associativity != 2 && parameters->associativity != 4 && parameters->associativity != 8 && parameters->associativity != 16) {
                printf("Error: Associativity must be 1, 2, 4, 8, or 16\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-r") == 0) {
            strncpy(parameters->replacementPolicy, argv[++i], 3);
            parameters->replacementPolicy[3] = '\0';
            if (strcmp(parameters->replacementPolicy, "RR") != 0 && strcmp(parameters->replacementPolicy, "RND") != 0) {
                printf("Error: Replacement policy must be RR or RND\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-p") == 0) {
            parameters->physicalMemory = atoi(argv[++i])*1024*1024;
            if (parameters->physicalMemory < 1048576 || parameters->physicalMemory > 4294967296) {
                printf("Error: Physical memory must be between 1MB and 4GB\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-u") == 0) {
            parameters->percentMemoryUsed = atoi(argv[++i]);
            if (parameters->percentMemoryUsed < 0 || parameters->percentMemoryUsed > 100) {
                printf("Error: Percent of physical memory used by OS must be between 0 and 100 percent\n");
                exit(1);
            }
        } else if (strcmp(argv[i], "-n") == 0) {
            parameters->instructionsPerSlice = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-f") == 0) {
            if (parameters->traceFileCount < 3) {
                strncpy(parameters->traceFiles[parameters->traceFileCount], argv[++i], 99);
                parameters->traceFiles[parameters->traceFileCount][99] = '\0';
                parameters->traceFileCount++;
            } else {
                printf("Error: Maximum of 3 trace files are allowed\n");
                exit(1);
            }
        } else {
            printf("Error: Not a valid argument\n");
            exit(1);
        }
    }
}

//initializes parameters and processes the command line inputs
int main(int argc, char *argv[]) {
    Parameters parameters;
    inputs(argc, argv, &parameters);
    return 0;
}
