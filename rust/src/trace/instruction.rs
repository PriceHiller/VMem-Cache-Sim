use std::{ops::Deref, str::FromStr};

use crate::address::Address;

/// A single Extended Instruction pointer's information
///
/// NOTE: If you need the length of the instruction, check `op_bytes.len()`.
///
/// * `address`: The start address of the op bytes (op_bytes)
/// * `op_bytes`: The actual op b oytes corresponding for the instruction
/// * `operation`: The raw assembly instruction executed
#[derive(PartialEq)]
pub struct TraceEIP {
    pub address: Address,
    pub op_bytes: Vec<u8>,
    pub operation: String,
}

impl std::fmt::Debug for TraceEIP {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("TraceEIP")
            .field("address", &format!("0x{:08x}", &self.address))
            .field("op_bytes", &self.op_bytes.iter().map(|b| format!("{b:02x}")).collect::<Vec<_>>())
            .field("operation", &self.operation)
            .finish()
    }
}

/// A single trace address, either source or destination.
///
/// WARN: Avoid using this directly, prefer `TraceSourceAddress` and
/// `TraceDestAddress` instead!
///
/// * `len`: The length of the data to be read/written
/// * `address`: The address being read/written from/to
/// * `data`: The data being read/written
#[derive(PartialEq)]
pub struct TraceAddress {
    pub len: usize,
    pub address: Address,
    pub data: Option<usize>,
}

impl std::fmt::Debug for TraceAddress {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("TraceAddress")
            .field("len", &self.len)
            .field("address", &format!("0x{:08x}", self.address))
            .field("data", &self.data)
            .finish()
    }
}

#[derive(Debug, PartialEq)]
pub struct TraceSourceAddress(TraceAddress);

impl Deref for TraceSourceAddress {
    type Target = TraceAddress;
    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

#[derive(Debug, PartialEq)]
pub struct TraceDestAddress(TraceAddress);

impl Deref for TraceDestAddress {
    type Target = TraceAddress;
    fn deref(&self) -> &Self::Target {
        &self.0
    }
}

/// Holds a full trace instruction
///
/// * `eip`: The Extended Instruction pointer info
/// * `src`: The source address info
/// * `dst`: The destination address info
#[derive(Debug, PartialEq)]
pub struct TraceInstruction {
    pub eip: TraceEIP,
    pub src: TraceSourceAddress,
    pub dst: TraceDestAddress,
}

impl FromStr for TraceAddress {
    type Err = ();
    fn from_str(s: &str) -> Result<Self, ()> {
        match s.split_whitespace().collect::<Vec<_>>()[..] {
            [_, address, data] => {
                let (address, data) = (usize::from_str_radix(address, 16).map_err(|_| ())?, {
                    if data != "--------" {
                        Some(usize::from_str_radix(data, 16).map_err(|_| ())?)
                    } else {
                        None
                    }
                });

                Ok(TraceAddress {
                    len: 4,
                    address,
                    data,
                })
            }
            _ => Err(()),
        }
    }
}

fn build_data_str(data: Option<usize>) -> String {
    match data {
        Some(d) => format!("{d:08x}"),
        None => "--------".to_string(),
    }
}
impl From<TraceSourceAddress> for String {
    fn from(val: TraceSourceAddress) -> Self {
        format!("srcM: {:08x} {}", val.address, build_data_str(val.data))
    }
}

impl From<TraceDestAddress> for String {
    fn from(val: TraceDestAddress) -> Self {
        format!("dstM: {:08x} {}", val.address, build_data_str(val.data))
    }
}

impl FromStr for TraceEIP {
    type Err = ();

    /// Parses an EIP instruction from the format:
    /// EIP (<len>): <address> <opcode bytes> <operation asm>
    ///
    /// * `s`: The EIP instruction to parse
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s.splitn(4, " ").collect::<Vec<_>>()[..] {
            [_, eip_len, address, code_op] => {
                // If the length from the instruction is too short (invalid),
                // then we have a bad string.
                //
                // Should look like `(04):`
                if eip_len.len() < 5 {
                    return Err(());
                }

                // If the length doesn't match the expected format, then we have
                // an error. Searching for the last parens.
                let split_pos = eip_len.char_indices().nth_back(1).ok_or(())?;
                if split_pos.1 != ')' {
                    return Err(());
                }

                let len = eip_len[1..split_pos.0].parse::<usize>().map_err(|_| ())?;

                // Next we want to parse out the address if available
                let address = usize::from_str_radix(address, 16).map_err(|_| ())?;

                // Now we need to parse out the actual machine code bytes from
                // the EIP instruction and its related operation
                //
                // E.g.
                //  - `83 60 34 00` for the machine code bytes
                //  - `and dword [eax+0x34],0x0` for the operation (just the
                //    rest of the string after the bytes end basically)
                //
                //  We will use the `len` calculated from earlier to determine
                //  how many bytes we have to grab

                // Operations come in as two hex values with a space on the end.
                // since we know how many operation codes we should have from
                // the `len` variable earlier we can determine the index at
                // which those operation codes should terminate before the
                // actual raw operation text.
                //
                // We're adding on the total number of spaces used for the
                // opcodes in total so we can actually get at the end index.
                let code_ops_end_idx = (len * 2) + (len - 1);
                if code_op.len() < code_ops_end_idx {
                    return Err(());
                }

                let op_bytes: Vec<_> = code_op[..code_ops_end_idx]
                    .split_whitespace()
                    .map(|s| {
                        if s.len() == 2
                            && let Ok(byte) = u8::from_str_radix(s, 16)
                        {
                            Ok(byte)
                        } else {
                            Err(())
                        }
                    })
                    .collect::<Result<Vec<u8>, _>>()?;

                if op_bytes.len() != len {
                    return Err(());
                }

                // Now grab the operation, which is the remaining part of the
                // string
                let operation = code_op[code_ops_end_idx..].trim().to_string();

                Ok(TraceEIP {
                    address,
                    op_bytes,
                    operation,
                })
            }
            _ => Err(()),
        }
    }
}

#[derive(Debug)]
pub enum InvalidTraceInstruction {
    InvalidEIP,
    InvalidAddressLine,
    InvalidSourceAddress,
    InvalidDestAddress,
    InvalidNumLines,
}

impl FromStr for TraceInstruction {
    type Err = InvalidTraceInstruction;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut lines_iter = s.lines();
        let (eip_line, addr_line) = (lines_iter.next(), lines_iter.next());
        let eip_line = eip_line.ok_or(InvalidTraceInstruction::InvalidNumLines)?;
        let addr_line = addr_line.ok_or(InvalidTraceInstruction::InvalidNumLines)?;

        let eip = TraceEIP::from_str(eip_line).map_err(|_| InvalidTraceInstruction::InvalidEIP)?;

        if addr_line.len() < 24 {
            return Err(InvalidTraceInstruction::InvalidAddressLine);
        }

        let dst_addr_str = addr_line[..=23].trim();
        let dst = TraceDestAddress(
            TraceAddress::from_str(dst_addr_str)
                .map_err(|_| InvalidTraceInstruction::InvalidDestAddress)?,
        );

        let src_addr_str = addr_line[24..].trim();
        let src = TraceSourceAddress(
            TraceAddress::from_str(src_addr_str)
                .map_err(|_| InvalidTraceInstruction::InvalidSourceAddress)?,
        );
        Ok(Self { eip, src, dst })
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn trace_address_from_str_correctly_parses() {
        let ex = "srcM: ffffffff 00000000";
        let trace_addr = TraceAddress::from_str(ex).unwrap();
        assert_eq!(trace_addr.address, 0xffffffff);
        assert_eq!(trace_addr.data, Some(0x0));
    }

    #[test]
    fn trace_address_from_str_handles_empty_data() {
        let ex = "srcM: ffffffff --------";
        let trace_addr = TraceAddress::from_str(ex).unwrap();
        assert_eq!(trace_addr.address, 0xffffffff);
        assert!(trace_addr.data.is_none());
    }

    #[test]
    fn trace_address_from_str_rejects_bad_data() {
        let ex = "srcM: ffffffff -1-0-1--";
        let trace_addr = TraceAddress::from_str(ex);
        assert!(trace_addr.is_err());
    }

    #[test]
    fn trace_eip_from_str_correctly_parses() {
        let ex = "EIP (07): 7c80976b 8b 84 88 10 0e 00 00  mov eax,[eax+ecx*4+0xe10]";
        let eip = TraceEIP::from_str(ex).unwrap();
        assert_eq!(eip.address, 0x7c80976b);
        assert_eq!(eip.op_bytes, vec![0x8b, 0x84, 0x88, 0x10, 0x0e, 0x00, 0x00]);
        assert_eq!(eip.operation, "mov eax,[eax+ecx*4+0xe10]");
    }

    #[test]
    fn trace_instruction_from_str_correctly_parses_with_empty_data() {
        let ex = concat!(
            "EIP (07): 7c80976b 8b 84 88 10 0e 00 00  mov eax,[eax+ecx*4+0xe10]\n",
            "dstM: 00000000 --------    srcM: fffffffe --------"
        );

        let inst = TraceInstruction::from_str(ex).unwrap();
        let eip = inst.eip;
        assert_eq!(eip.address, 0x7c80976b);
        assert_eq!(eip.op_bytes, vec![0x8b, 0x84, 0x88, 0x10, 0x0e, 0x00, 0x00]);
        assert_eq!(eip.operation, "mov eax,[eax+ecx*4+0xe10]");

        assert_eq!(inst.src.address, 0xfffffffe);
        assert!(inst.src.data.is_none());
        assert_eq!(inst.dst.address, 0x0);
        assert!(inst.dst.data.is_none());
    }

    #[test]
    fn trace_instruction_from_str_correctly_parses_with_valid_addresses() {
        let ex = concat!(
            "EIP (07): 7c80976b 8b 84 88 10 0e 00 00  mov eax,[eax+ecx*4+0xe10]\n",
            "dstM: 00000000 AABBCCDD    srcM: fffffffe 11111111"
        );

        let inst = TraceInstruction::from_str(ex).unwrap();
        let eip = inst.eip;
        assert_eq!(eip.address, 0x7c80976b);
        assert_eq!(eip.op_bytes, vec![0x8b, 0x84, 0x88, 0x10, 0x0e, 0x00, 0x00]);
        assert_eq!(eip.operation, "mov eax,[eax+ecx*4+0xe10]");

        assert_eq!(inst.dst.address, 0x0);
        let dst_data = inst.dst.data.unwrap();
        assert_eq!(dst_data, 0xAABBCCDD);

        assert_eq!(inst.src.address, 0xfffffffe);
        let src_data = inst.src.data.unwrap();
        assert_eq!(src_data, 0x11111111);
    }
}
