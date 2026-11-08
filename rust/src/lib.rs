#![feature(random)]
pub mod cache;
pub mod cmdline;
pub mod trace;
pub mod address;
pub mod vmem;

type NumBits = usize;
type NumBytes = usize;
type NumKilobytes = usize;
type NumMegaBytes = usize;

