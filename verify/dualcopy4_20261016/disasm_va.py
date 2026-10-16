"""disasm_va.py <va-hex> <nbytes> : capstone disassembly of original\\MASHED.exe.unpatched at a VA (no process, no Ghidra)."""
import struct, sys
from pathlib import Path

import capstone

exe = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched").read_bytes()
va, n = int(sys.argv[1], 16), int(sys.argv[2])
pe = struct.unpack_from("<I", exe, 0x3C)[0]
nsec = struct.unpack_from("<H", exe, pe + 6)[0]
opt = struct.unpack_from("<H", exe, pe + 20)[0]
base = struct.unpack_from("<I", exe, pe + 24 + 28)[0]
sec = pe + 24 + opt
off = None
for i in range(nsec):
    name, vsz, vrva, rsz, roff = struct.unpack_from("<8sIIII", exe, sec + 40 * i)
    if vrva <= va - base < vrva + max(vsz, rsz):
        off = roff + (va - base - vrva)
        break
assert off is not None, "VA not in a section"
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
for ins in md.disasm(exe[off:off + n], va):
    print(f"0x{ins.address:08x}  {ins.bytes.hex():16s} {ins.mnemonic} {ins.op_str}")
