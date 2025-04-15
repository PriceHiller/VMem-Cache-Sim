#include <stdbool.h>
#include <stdio.h>
#ifndef TRACE_FILE_READER_H

typedef struct {
    unsigned int address;
    bool valid;
} Maddr;

typedef struct {
    unsigned int len;
    unsigned int start_address;
    Maddr dest;
    Maddr source;
} Instruction;

typedef struct {
    unsigned int count;
    unsigned int capacity;
    Instruction *instructions;
} Instruction_Arr;

bool read_trace_instruction(Instruction *instruction, char *inst_part_one,
                            char *inst_part_two);
Instruction_Arr read_trace_file(FILE *trc_file);
char *Instruction_get_str(Instruction *instruction);

#endif
