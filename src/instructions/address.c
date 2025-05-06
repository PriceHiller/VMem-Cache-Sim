#include "address.h"
#include "../lib/fmtstr/fmtstr.h"

AddrParts get_addr_parts(Address address, NumBits tag_bits, NumBits index_bits,
                         NumBits offset_bits) {
    AddrParts addr_parts;
    addr_parts.address = address;
    addr_parts.offset.addr = address & ((1 << offset_bits) - 1);
    addr_parts.offset.bits = offset_bits;

    addr_parts.index.addr = (address >> offset_bits) & ((1 << index_bits) - 1);
    addr_parts.index.bits = index_bits;

    addr_parts.tag.addr = (address >> (offset_bits + index_bits));
    addr_parts.tag.bits = tag_bits;
    return addr_parts;
}

char *addr_part_str(AddrPart part) {
    return str_fmt("AddrPart(addr=0x%X, bits=%d)", part.addr, part.bits);
}

VAddrParts get_vaddr_parts(AddrParts addr_parts) {
    VAddrParts vaddr_parts;
    vaddr_parts.page_number.addr = addr_parts.address >> addr_parts.offset.bits;
    vaddr_parts.page_number.bits = addr_parts.index.bits + addr_parts.tag.bits;
    vaddr_parts.parts = addr_parts;
    return vaddr_parts;
}

char *addr_parts_str(AddrParts *attr_parts) {
    return str_fmt("AddrParts(address=0x%X, tag=0x%X, index=0x%X, offset=0x%X)",
                   attr_parts->address, attr_parts->tag.addr,
                   attr_parts->index.addr, attr_parts->offset.addr);
}

char *vaddr_parts_str(VAddrParts *vattr_parts) {
    return str_fmt("VAddrParts(page_number=0x%X, %s)", vattr_parts->page_number,
                   addr_parts_str(&vattr_parts->parts));
}
