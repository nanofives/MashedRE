# Disassemble one named export of a built PE (the dev .asi): prints the first instructions up to the first ret/jmp.
# Usage: py -3.12 disasm_export.py <pe> <export>
import struct, sys, capstone
d = open(sys.argv[1], 'rb').read(); name = sys.argv[2].encode()
pe = struct.unpack_from('<I', d, 0x3c)[0]; n = struct.unpack_from('<H', d, pe + 6)[0]; osz = struct.unpack_from('<H', d, pe + 20)[0]
base = struct.unpack_from('<I', d, pe + 52)[0]; s0 = pe + 24 + osz
secs = [struct.unpack_from('<IIII', d, s0 + i * 40 + 8) for i in range(n)]
def off(rva):
    for vs, va, rs, ro in secs:
        if va <= rva < va + max(vs, rs): return ro + rva - va
erva = struct.unpack_from('<I', d, pe + 24 + 96)[0]; e = off(erva)
nn, fa, na, oa = struct.unpack_from('<IIII', d, e + 20)[0], *struct.unpack_from('<III', d, e + 28)
for i in range(struct.unpack_from('<I', d, e + 24)[0]):
    s = off(struct.unpack_from('<I', d, off(na) + 4 * i)[0]); en = d[s:d.index(b'\0', s)]
    if en == name:
        ordi = struct.unpack_from('<H', d, off(oa) + 2 * i)[0]; frva = struct.unpack_from('<I', d, off(fa) + 4 * ordi)[0]
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        for ins in md.disasm(d[off(frva):off(frva) + 64], base + frva):
            print(f'  {ins.address:08x}: {ins.mnemonic} {ins.op_str}')
            if ins.mnemonic == 'jmp' and not ins.op_str.startswith('0x') or ins.mnemonic == 'int3': break
        break
else: sys.exit('export not found')
