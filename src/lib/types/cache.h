#ifndef LIBTYPES_CACHE_H
#define LIBTYPES_CACHE_H

#include "types.h"

typedef enum { RP_ROUND_ROBIN, RP_RANDOM, RP_UNKNOWN } ReplacementPolicy;

char *ReplacementPolicy_to_string(ReplacementPolicy rp);
ReplacementPolicy ReplacementPolicy_from_string(char *replacement_policy);

typedef struct {
    char files[3][100];
    unsigned int count;
} TraceFiles;

typedef struct {
  Kilobyte cacheSize;
  Byte blockSize;
  unsigned int associativity;
  ReplacementPolicy replacementPolicy;
  Megabyte physicalMemory;
  float percentMemoryUsed;
  unsigned int instructionsPerTimeSlice;
  TraceFiles traceFiles;
} CacheParameters;


#define COST_PER_KB 0.12

typedef struct {
  int cacheBits;
  int sets;
  int numBlocks;
  int indexSize;
  int offsetSize;
  int tagSize;
  int overheadSize;
  Byte memorySize;
  double cost;
  double costPerKb;
} CacheValues;

#endif
