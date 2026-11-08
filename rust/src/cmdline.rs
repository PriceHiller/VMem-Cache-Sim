use crate::cache::ReplacementPolicy;
use std::path::PathBuf;

use clap::Parser;

use crate::{NumBytes, NumKilobytes, NumMegaBytes};

fn replacement_policy_parser(s: &str) -> Result<ReplacementPolicy, String> {
    let s = s.to_uppercase();

    match &s[..] {
        "RR" => Ok(ReplacementPolicy::RoundRobin),
        "RND" => Ok(ReplacementPolicy::Random),
        _ => Err(format!(
            "`{s}` isn't a valid replacement policy! Valid replacement policies: [rr, rnd]",
        )),
    }
}

fn instr_time_slice_parser(s: &str) -> Result<usize, String> {
    let tslice_max = 0xFFFFFFFF;
    let err_msg = format!(
        "Invalid value for timeslice given! Must be -1 or from 0 to 0x{tslice_max:08x}, received: {s}"
    );
    let tslice = s
        .parse::<isize>()
        .or_else(|_| isize::from_str_radix(s, 16).map_err(|_| err_msg.clone()))?;

    if tslice == -1 {
        Ok(tslice_max)
    } else if tslice < 0 {
        Err(err_msg)
    } else {
        Ok(tslice as usize)
    }
}

#[derive(Parser)]
#[command(version, about)]
pub struct CliArgs {
    #[arg(short = 's', long = "cache-size")]
    pub cache_size_kb: NumKilobytes,

    #[arg(short = 'b', long = "block-size")]
    pub block_size_bytes: NumBytes,

    #[arg(short = 'a', long = "associativity")]
    pub associativity: usize,

    #[arg(short = 'r', long = "replacement-policy", value_enum, value_parser = replacement_policy_parser)]
    pub replacement_policy: ReplacementPolicy,

    #[arg(short = 'p', long = "phys-mem")]
    pub phys_mem_mb: NumMegaBytes,

    #[arg(short = 'u', long = "os-use-percent")]
    pub phys_mem_used_by_os: f32,

    #[arg(short = 'n', long = "timeslice", value_parser = instr_time_slice_parser)]
    pub instr_time_slice: usize,

    #[arg(short = 'f', long = "trace-file", required = true)]
    pub trace_files: Vec<PathBuf>,
}
