"""Disassemble original bytes: py -3.12 orig_disasm.py <exe> <va_hex> <len>"""
import sys, pefile, capstone
pe = pefile.PE(sys.argv[1]); va = int(sys.argv[2], 16); n = int(sys.argv[3])
data = pe.get_memory_mapped_image()[va - 0x400000: va - 0x400000 + n]
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for i in md.disasm(data, va):
    print(f"0x{i.address:08x}  {i.mnemonic} {i.op_str}")
