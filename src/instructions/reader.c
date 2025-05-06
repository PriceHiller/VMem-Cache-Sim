#include "reader.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bool read_trace_instruction(Instruction *instruction, char *inst_part_one,
                            char *inst_part_two) {
    if (sscanf(inst_part_one, "EIP (%d): %x", &instruction->len,
               &instruction->start) != 2) {
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
        // Continue skipping the first n blank lines
        while (line_number == 0 &&
               (!strcmp("\n", line) || !strcmp("\r\n", line))) {
            if (!fgets(line, sizeof(line), trc_file)) {
                printf("Reached EOF when attempting to read a trace file, is "
                       "the content correct?\n");
                exit(1);
            }
        }
        if (line_number % 3 == 0) {
            memcpy(inst_part_one, line, 1024);
        } else if (line_number % 3 == 1) {
            instructions.count++;

            // Ensure our array has enough capacity to store the next
            // instruction
            if (instructions.count >= instructions.capacity) {
                unsigned int desired_capacity = (instructions.capacity + 1) * 2;
                Instruction *realloced_instructions = realloc(
                    instructions.items, sizeof(Instruction) * desired_capacity);
                if (realloced_instructions == NULL) {
                    printf("Failed to allocate enough memory for reading "
                           "instructions from trace file!\n");
                    exit(1);
                }
                instructions.items = realloced_instructions;
                instructions.capacity = desired_capacity;
            }

            Instruction new_instruction;
            if (!read_trace_instruction(&new_instruction, inst_part_one,
                                        line)) {
                printf("Failed to read an instruction from line %d\n",
                       line_number);
                printf("Line:\n%s\n", line);

                exit(1);
            }
            instructions.items[instructions.count - 1] = new_instruction;
        }
        line_number++;
    }
    return instructions;
}
