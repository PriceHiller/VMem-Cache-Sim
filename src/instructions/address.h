#ifndef ADDRESS_H
#define ADDRESS_H

#include "../lib/types/types.h"

typedef unsigned int Address;

typedef struct {
    Address addr;
    NumBits bits;
} AddrPart;

typedef struct {
    Address address;
    AddrPart tag;
    AddrPart index;
    AddrPart offset;
} AddrParts;

typedef struct {
    AddrPart page_number;
    AddrParts parts;
} VAddrParts;

AddrParts get_addr_parts(Address address, NumBits tag_bits, NumBits index_bits,
                         NumBits offset_bits);

VAddrParts get_vaddr_parts(AddrParts addr_parts);

char *addr_parts_str(AddrParts *attr_parts);
char *vaddr_parts_str(VAddrParts *vattr_parts);
char *addr_part_str(AddrPart part);
#endif
