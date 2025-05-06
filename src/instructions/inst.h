#ifndef INST_H
#define INST_H

#include "address.h"
#include <stdbool.h>

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
    unsigned int ip;
    Instruction *items;
} Instruction_Arr;

char *Instruction_get_str(Instruction *instruction);
#endif
