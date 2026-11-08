use clap::ValueEnum;

use crate::{
    NumBits, NumBytes,
    address::{AddrMask, Address},
};

#[derive(Debug, Clone, ValueEnum, Copy)]
pub enum ReplacementPolicy {
    RoundRobin,
    Random,
}

#[derive(Debug, Clone, Copy)]
struct CacheBlock {
    tag: Option<Address>,
}

impl From<Address> for CacheBlock {
    fn from(value: Address) -> Self {
        CacheBlock { tag: Some(value) }
    }
}

impl CacheBlock {
    pub fn invalidate(&mut self) {
        self.tag = None;
    }

    pub fn is_valid(&self) -> bool {
        self.tag.is_some()
    }
}

#[derive(Debug, Clone)]
pub struct CacheSet(Vec<CacheBlock>);

#[derive(Debug)]
pub enum CacheLookup {
    Hit(usize),
    ConflictMiss,
    CompulsoryMiss(usize),
}

impl CacheSet {
    fn lookup_block(&self, tag: Address) -> CacheLookup {
        let mut hole_idx: Option<usize> = None;
        for (idx, blk) in self.0.iter().enumerate() {
            match blk.tag {
                Some(blk_tag) if blk_tag == tag => return CacheLookup::Hit(idx),
                None if blk.is_valid() || hole_idx.is_none() => hole_idx = Some(idx),
                _ => {}
            };
        }

        if let Some(hole_idx) = hole_idx {
            return CacheLookup::CompulsoryMiss(hole_idx);
        }

        CacheLookup::ConflictMiss
    }

    fn invalidate(&mut self) {
        for blk in &mut self.0 {
            blk.invalidate();
        }
    }

    fn len(&self) -> usize {
        self.0.len()
    }

    fn replace_block(&mut self, rp: &ReplacementPolicy, blk: CacheBlock) {
        let idx = match rp {
            ReplacementPolicy::RoundRobin => {
                // To achieve round robin, we're just going to rotate elements
                // leftwards making the first element the next element to be
                // replaced.
                //
                // Less efficient than hanging onto a round robin counter
                // somewhere else, but certainly does simplify things.
                self.0.rotate_left(1);
                self.len() - 1
            }
            ReplacementPolicy::Random => std::random::random::<usize>() % self.len(),
        };
        self.0[idx] = blk;
    }
}

#[derive(Debug, Clone)]
pub struct Cache {
    tag: NumBits,
    index: NumBits,
    offset: NumBits,
    rp: ReplacementPolicy,
    associativity: usize,
    sets: Vec<CacheSet>,
}

#[derive(Debug, Default, Clone, Copy)]
pub struct CacheLookupStat {
    pub hits: usize,
    pub compulsory_misses: usize,
    pub conflict_misses: usize,
    pub bytes: NumBytes,
}

impl std::ops::Add for CacheLookupStat {
    type Output = Self;

    fn add(self, rhs: Self) -> Self::Output {
        Self {
            hits: self.hits + rhs.hits,
            compulsory_misses: self.compulsory_misses + rhs.compulsory_misses,
            conflict_misses: self.conflict_misses + rhs.conflict_misses,
            bytes: self.bytes + rhs.bytes,
        }
    }
}

impl std::ops::AddAssign for CacheLookupStat {
    fn add_assign(&mut self, rhs: Self) {
        self.hits = self.hits + rhs.hits;
        self.compulsory_misses = self.compulsory_misses + rhs.compulsory_misses;
        self.conflict_misses = self.conflict_misses + rhs.conflict_misses;
        self.bytes = self.bytes + rhs.bytes;
    }
}

impl std::ops::AddAssign<&CacheLookupStat> for CacheLookupStat {
    fn add_assign(&mut self, rhs: &Self) {
        *self += *rhs;
    }
}

impl CacheLookupStat {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn misses(&self) -> usize {
        self.compulsory_misses + self.conflict_misses
    }

    pub fn accesses(&self) -> usize {
        self.hits + self.misses()
    }
}

impl Cache {
    pub fn new(
        rp: ReplacementPolicy,
        tag: NumBits,
        index: NumBits,
        offset: NumBits,
        associativity: usize,
    ) -> Self {
        let s = Self {
            rp,
            tag,
            index,
            offset,
            associativity,
            sets: Vec::with_capacity(0),
        };

        let sets = vec![CacheSet(vec![CacheBlock { tag: None }; associativity]); s.sets()];

        Self {
            rp,
            tag,
            index,
            offset,
            associativity,
            sets,
        }
    }

    pub fn lookup(&mut self, mut addr: Address, mut len: usize) -> CacheLookupStat {
        let addr_mask = self.addr_mask();
        let blk_size = self.block_size();
        let rp = self.rp;
        let mut stat = CacheLookupStat {
            bytes: len,
            ..Default::default()
        };

        while len > 0 {
            let addr_parts = addr_mask.parts(addr);
            let offset_addr = blk_size - addr_parts.offset;
            let bytes_to_process = if len <= offset_addr { len } else { offset_addr };
            match self.lookup_set_mut(addr_parts.index) {
                Some(set) => match set.lookup_block(addr_parts.tag) {
                    CacheLookup::Hit => stat.hits += 1,
                    CacheLookup::ConflictMiss => {
                        stat.conflict_misses += 1;
                        set.replace_block(&rp, CacheBlock::from(addr_parts.tag));
                    }
                    CacheLookup::CompulsoryMiss(open_idx) => {
                        stat.compulsory_misses += 1;
                        set.0[open_idx] = CacheBlock::from(addr_parts.tag)
                    }
                },
                None => break,
            }
            len -= bytes_to_process;
            addr += bytes_to_process;
        }
        stat
    }

    pub fn lookup_set(&self, index: usize) -> Option<&CacheSet> {
        self.sets.get(index)
    }

    pub fn lookup_set_mut(&mut self, index: usize) -> Option<&mut CacheSet> {
        self.sets.get_mut(index)
    }

    pub fn addr_mask(&self) -> AddrMask {
        AddrMask::new(self.tag, self.index, self.offset)
    }

    pub fn address_space(&self) -> NumBits {
        self.tag + self.index + self.offset
    }

    pub fn sets(&self) -> usize {
        2 << (self.index - 1)
    }

    pub fn blocks(&self) -> usize {
        self.sets() * self.associativity
    }

    pub fn size(&self) -> NumBytes {
        self.blocks() * self.block_size()
    }

    pub fn block_size(&self) -> NumBytes {
        2 << (self.offset - 1)
    }
}
