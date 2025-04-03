#ifndef CACHECALCULATOR_H
#define CACHECALCULATOR_H

#include "../lib/types/cache.h"
CacheValues Cache_calculate_values(Byte cacheSize, Byte blockSize, int associativity, int physMem);

#endif
