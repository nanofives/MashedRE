"""u9778: capstone listing of MASHED.exe.unpatched between two VAs. usage: cslist.py <va_from> <va_to>"""
import sys, struct, capstone
B = open(r'C:/Users/maria/Desktop/Proyectos/Mashed/original/MASHED.exe.unpatched', 'rb').read()
pe = struct.unpack_from('<I', B, 0x3c)[0]; ns = struct.unpack_from('<H', B, pe + 6)[0]; oh = struct.unpack_from('<H', B, pe + 20)[0]
ib = struct.unpack_from('<I', B, pe + 24 + 28)[0]; st = pe + 24 + oh
def off(va):
    for i in range(ns):
        vs, vaddr, rs, rp = struct.unpack_from('<IIII', B, st + i * 40 + 8)
        if vaddr <= va - ib < vaddr + max(vs, rs): return va - ib - vaddr + rp
a, b = int(sys.argv[1], 16), int(sys.argv[2], 16)
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
o = off(a)
for ins in md.disasm(B[o:o + (b - a)], a):
    print(f'{ins.address:08x}  {ins.mnemonic:6s} {ins.op_str}')
