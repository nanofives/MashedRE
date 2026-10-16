"""List instructions in MASHED.exe (.unpatched anchor) .text whose memory displacement or immediate
falls in [lo, hi). Static witness. Usage: dispref.py <lo> <hi> [text_lo text_hi]"""
import sys
from pathlib import Path
import capstone, pefile
EXE = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched")
lo = int(sys.argv[1], 16); hi = int(sys.argv[2], 16)
pe = pefile.PE(str(EXE)); base = pe.OPTIONAL_HEADER.ImageBase; img = pe.get_memory_mapped_image()
t = [s for s in pe.sections if s.Name.startswith(b'.text')][0]
tlo = base + t.VirtualAddress; thi = tlo + t.Misc_VirtualSize
if len(sys.argv) > 4: tlo = int(sys.argv[3], 16); thi = int(sys.argv[4], 16)
needles = set()
for v in range(lo, hi):
    needles.add(v)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32); md.detail = True
code = bytes(img[tlo-base:thi-base])
# byte prefilter: any 4-byte little-endian value in range
import struct
cands = []
for i in range(len(code)-3):
    v = struct.unpack_from('<I', code, i)[0]
    if lo <= v < hi: cands.append(i)
seen = set()
for c in cands:
    for back in range(1, 9):
        s = c - back
        if s < 0: continue
        for ins in md.disasm(code[s:s+16], tlo+s, count=1):
            if ins.address + ins.size < tlo + c + 4 or ins.address in seen: continue
            ok = False
            for op in ins.operands:
                if op.type == capstone.x86.X86_OP_MEM and lo <= (op.mem.disp & 0xffffffff) < hi: ok = True
                if op.type == capstone.x86.X86_OP_IMM and lo <= (op.imm & 0xffffffff) < hi: ok = True
            if ok:
                seen.add(ins.address)
                print(f"0x{ins.address:08x}  {ins.mnemonic} {ins.op_str}")
