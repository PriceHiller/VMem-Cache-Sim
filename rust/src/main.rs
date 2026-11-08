use clap::Parser;
use vmem_cache_sim::cache::{Cache, CacheLookupStat};
use vmem_cache_sim::{
    cmdline::CliArgs,
    trace::read::TraceFile,
};

#[derive(Debug, Default)]
struct CacheStats {
    inst: CacheLookupStat,
    dst: CacheLookupStat,
    src: CacheLookupStat,
    lookups: usize,
}

impl CacheStats {
    fn combined(&self) -> CacheLookupStat {
        self.inst + self.dst + self.src
    }
}

impl std::fmt::Display for CacheStats {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let combined = self.combined();
        write!(
            f,
            r#"
***** Cache Simulation Results *****

Total Cache Accesses:           {}    ({} addresses)
--- Instruction Bytes:          {}
--- SrcDst Bytes:               {}
Cache Hits:                     {}
Cache Misses:                   {}
--- Compulsory Misses:          {}
--- Conflict Misses:            {}
"#,
            combined.accesses(),
            self.lookups,
            self.inst.bytes,
            self.src.bytes + self.dst.bytes,
            combined.hits,
            combined.misses(),
            combined.compulsory_misses,
            combined.conflict_misses
        )
    }
}

fn main() -> Result<(), String> {
    let cli = CliArgs::parse();
    let mut traces = vec![];
    for path in &cli.trace_files {
        let trace_file = TraceFile::try_from(path.clone())
            .map_err(|e| format!("Failed to open {path:?}, {e}"))?;
        traces.push(trace_file);
    }

    let trace_readers = traces;
    println!("Cache Simulator - CS 3853 - Price Hiller\n");
    println!("Trace File(s):");
    for f in &trace_readers {
        println!("        {f}")

    }
    let mut cache = Cache::from(&cli);

    let mut stats = CacheStats::default();
    for trace_reader in trace_readers {
        for trc in trace_reader {
            let trc = trc.unwrap();
            stats.inst += cache.lookup(trc.eip.address, trc.eip.op_bytes.len());
            stats.lookups += 1;

            if trc.dst.data.is_some() {
                stats.dst += cache.lookup(trc.dst.address, trc.dst.len);
                stats.lookups += 1;
            }

            if trc.src.data.is_some() {
                stats.src += cache.lookup(trc.src.address, trc.src.len);
                stats.lookups += 1;
            }
        }
    }
    println!("{cache}");
    println!("{stats}");
    Ok(())
}
