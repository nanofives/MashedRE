"""Disassemble a VA range of original/MASHED.exe.unpatched with capstone (d3b lane, read-only).
usage: py -3.12 disasm.py <va> <nbytes>"""
import struct, sys, capstone
P = r'C:\Users\maria\Desktop\Proyectos\Mashed\original\MASHED.exe.unpatched'
b = open(P, 'rb').read()
pe = struct.unpack_from('<I', b, 0x3c)[0]
nsec = struct.unpack_from('<H', b, pe + 6)[0]
opt = struct.unpack_from('<H', b, pe + 20)[0]
base = struct.unpack_from('<I', b, pe + 24 + 28)[0]
sec = pe + 24 + opt
va, n = int(sys.argv[1], 16), int(sys.argv[2])
for i in range(nsec):
    vsz, vaddr, rsz, raw = struct.unpack_from('<IIII', b, sec + i * 40 + 8)
    if vaddr <= va - base < vaddr + max(vsz, rsz):
        off = raw + (va - base - vaddr)
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        for ins in md.disasm(b[off:off + n], va):
            print('%08x  %-8s %s' % (ins.address, ins.mnemonic, ins.op_str))
        break
