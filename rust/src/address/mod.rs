use crate::NumBits;

pub type Address = usize;

#[derive(Debug)]
pub struct AddrMask {
    tag: NumBits,
    index: NumBits,
    offset: NumBits,
}

impl AddrMask {
    pub fn new(tag: NumBits, index: NumBits, offset: NumBits) -> Self {
        Self { tag, index, offset }
    }
}

#[derive(Debug)]
pub struct AddrParts {
    pub tag: Address,
    pub index: Address,
    pub offset: Address,
}

impl AddrParts {
    fn new(tag: Address, index: Address, offset: Address) -> Self {
        Self { tag, index, offset }
    }
}

impl AddrMask {
    pub fn parts(&self, address: Address) -> AddrParts {
        let offset = address & ((1 << self.offset) - 1);
        let index = address >> self.offset & ((1 << self.index) - 1);
        let tag = address >> (self.index + self.offset);
        AddrParts::new(tag, index, offset)
    }
}


#[cfg(test)]
mod test {
    use super::*;

    #[test]
    fn parts_correctly_parses() {
        let addr: Address = 0xAAABBBCC;
        let mask = AddrMask::new(12,12,8);
        let parts = mask.parts(addr);

        assert_eq!(parts.tag, 0xAAA);
        assert_eq!(parts.index, 0xBBB);
        assert_eq!(parts.offset, 0xCC);
    }

}
