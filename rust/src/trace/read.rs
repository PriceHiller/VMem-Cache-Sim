use std::{
    fs::File,
    io::{self, BufRead, BufReader},
    path::PathBuf,
    str::FromStr as _,
};

use super::instruction::{InvalidTraceInstruction, TraceInstruction};

#[derive(Debug)]
pub struct TraceReader<T>(io::BufReader<T>)
where
    T: std::io::Read;

impl<T> Iterator for TraceReader<T>
where
    T: std::io::Read,
{
    type Item = Result<TraceInstruction, InvalidTraceInstruction>;

    fn next(&mut self) -> Option<Self::Item> {
        let reader = &mut self.0;
        let mut read_lines: Vec<String> = vec![];
        for line in reader.lines().map_while(Result::ok) {
            let line = line.trim();
            if !line.is_empty() {
                read_lines.push(line.to_string());
            } else if !read_lines.is_empty() {
                break;
            }
        }

        if read_lines.len() == 2 {
            Some(TraceInstruction::from_str(&read_lines.join("\n")))
        } else {
            None
        }
    }
}

impl<T> TraceReader<T>
where
    T: std::io::Read,
{
    pub fn new(reader: T) -> Self {
        Self(BufReader::new(reader))
    }
}

#[derive(Debug)]
pub struct TraceFile {
    path: PathBuf,
    instructions: TraceReader<File>,
}

impl TryFrom<PathBuf> for TraceFile {
    type Error = std::io::Error;

    fn try_from(value: PathBuf) -> Result<Self, Self::Error> {
        Ok(Self {
            path: value.clone(),
            instructions: TraceReader::new(File::open(value)?),
        })
    }
}

impl std::fmt::Display for TraceFile {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.path.display())
    }
}

impl Iterator for TraceFile {
    type Item = Result<TraceInstruction, InvalidTraceInstruction>;

    fn next(&mut self) -> Option<Self::Item> {
        self.instructions.next()
    }
}
