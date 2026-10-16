"""disasm.py <start-va> <length> : capstone listing from the anchored original (main checkout's MASHED.exe.unpatched)."""
import sys

import capstone
import pefile

EXE = r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched"
pe = pefile.PE(EXE)
base = pe.OPTIONAL_HEADER.ImageBase
start = int(sys.argv[1], 16)
n = int(sys.argv[2], 0)
off = pe.get_offset_from_rva(start - base)
data = open(EXE, "rb").read()[off:off + n]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for i in md.disasm(data, start):
    print(f"0x{i.address:08x}  {i.bytes.hex():16s} {i.mnemonic} {i.op_str}")
