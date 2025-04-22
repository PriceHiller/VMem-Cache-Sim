#include <stdbool.h>
#include <stdio.h>
#ifndef TRACE_FILE_READER_H

typedef unsigned int Address;

typedef struct {
    unsigned int address;
    bool valid;
} VAddr;

typedef struct {
    unsigned int len;
    Address start;
    VAddr dest;
    VAddr source;
} Instruction;

typedef struct {
    unsigned int count;
    unsigned int capacity;
    Instruction *items;
} Instruction_Arr;

bool read_trace_instruction(Instruction *instruction, char *inst_part_one,
                            char *inst_part_two);
Instruction_Arr read_trace_file(FILE *trc_file);
char *Instruction_get_str(Instruction *instruction);

#endif
