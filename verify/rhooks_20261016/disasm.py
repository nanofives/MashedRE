"""Linear capstone disassembly of MASHED.exe (.unpatched anchor). Usage: disasm.py <lo> <hi>"""
import sys
from pathlib import Path
import capstone, pefile
EXE = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
lo = int(sys.argv[1], 16); hi = int(sys.argv[2], 16)
pe = pefile.PE(str(EXE)); base = pe.OPTIONAL_HEADER.ImageBase; img = pe.get_memory_mapped_image()
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for ins in md.disasm(bytes(img[lo-base:hi-base]), lo):
    print(f"0x{ins.address:08x}  {ins.bytes.hex():16s} {ins.mnemonic} {ins.op_str}")
