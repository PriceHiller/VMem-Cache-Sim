#include "reader.h"
#include "../lib/fmtstr/fmtstr.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool read_trace_instruction(Instruction *instruction, char *inst_part_one,
                            char *inst_part_two) {
    if (sscanf(inst_part_one, "EIP (%d): %x", &instruction->len,
               &instruction->start_address) != 2) {
        return false;
    };

    unsigned int data;
    instruction->dest.valid = false;
    if (sscanf(inst_part_two, "dstM: %x %x", &instruction->dest.address,
               &data) == 2) {
        instruction->dest.valid = true;
    }

    instruction->source.valid = false;
    // Offset past the dest address for the source address read
    inst_part_two += 27;
    if (sscanf(inst_part_two, "srcM: %x %x", &instruction->source.address,
               &data) == 2) {
        instruction->source.valid = true;
    }

    return true;
}

Instruction_Arr read_trace_file(FILE *trc_file) {
    char line[1024];
    char inst_part_one[1024];
    Instruction_Arr instructions = {
        .count = 0,
        .capacity = 0,
    };

    unsigned int line_number = 0;
    while (fgets(line, sizeof(line), trc_file)) {
        if (line_number % 3 == 0) {
            memcpy(inst_part_one, line, 1024);
        } else if (line_number % 3 == 1) {
            instructions.count++;

            // Ensure our array has enough capacity to store the next
            // instruction
            if (instructions.count >= instructions.capacity) {
                unsigned int desired_capacity = (instructions.capacity + 1) * 2;
                printf("Desired cap: %d\n", desired_capacity);
                Instruction *realloced_instructions =
                    realloc(instructions.instructions,
                            sizeof(Instruction) * desired_capacity);
                if (realloced_instructions == NULL) {
                    printf("Failed to allocate enough memory for reading "
                           "instructions from trace file!\n");
                    exit(1);
                }
                instructions.instructions = realloced_instructions;
                instructions.capacity = desired_capacity;
            }

            Instruction new_instruction;
            if (!read_trace_instruction(&new_instruction, inst_part_one,
                                        line)) {
                printf("Failed to read an instruction!\n");
                exit(1);
            }
            instructions.instructions[instructions.count - 1] = new_instruction;
        }
        line_number++;
    }
    return instructions;
}

char *Instruction_get_str(Instruction *instruction) {
    return str_fmt(
        "Start Address: [%8x] | Length: %d | Source: [%8x], Valid: [%d] "
        "| Dest: [%8x], Valid: [%d]",
        instruction->start_address, instruction->len,
        instruction->source.address, instruction->source.valid,
        instruction->dest.address, instruction->dest.valid);
}
