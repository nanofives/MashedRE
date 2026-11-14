"""RO-ORACLE-GATE run 46: linear disassembly of VA ranges from a PE (capstone).

py -3.12 verify/replayoracle_20261113/oraclegate/disrange.py <exe> <start_va>:<length> [<start_va>:<length> ...]
"""
import sys
import capstone, pefile

pe = pefile.PE(sys.argv[1])
base = pe.OPTIONAL_HEADER.ImageBase
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
print("exe", sys.argv[1])
for spec in sys.argv[2:]:
    s, n = spec.split(":")
    s, n = int(s, 16), int(n, 16)
    print(f"--- 0x{s:08x} +0x{n:x}")
    for i in md.disasm(pe.get_data(s - base, n), s):
        print(f"0x{i.address:08x}  {i.bytes.hex():<20} {i.mnemonic} {i.op_str}")
