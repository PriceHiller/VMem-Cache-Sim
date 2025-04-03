#ifndef CMDLINE_PARSER_H
#define CMDLINE_PARSER_H

#include "../lib/types/cache.h"

CacheParameters read_cmdline_args(int argc, char *argv[]);
#endif
