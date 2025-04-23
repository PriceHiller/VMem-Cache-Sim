#include "inst.h"
#include "../lib/fmtstr/fmtstr.h"

char *Instruction_get_str(Instruction *instruction) {
    return str_fmt("Length: %d bytes | Start: [0x%-8X] | Source: "
                   "[0x%-8X], Valid: %d "
                   "| Dest: [0x%-8X], Valid: %d",
                   instruction->len, instruction->start,
                   instruction->source.address, instruction->source.valid,
                   instruction->dest.address, instruction->dest.valid);
}
