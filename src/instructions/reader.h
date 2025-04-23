#ifndef TRACE_FILE_READER_H
#define TRACE_FILE_READER_H

#include "inst.h"
#include <stdio.h>

Instruction_Arr read_trace_file(FILE *trc_file);

#endif
