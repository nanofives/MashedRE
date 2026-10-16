"""Print every `call <target>` site in MASHED.exe.unpatched with the 8 preceding instructions.
usage: py -3.12 callsites.py <exe> <target_va> [<target_va> ...]   (VA = image base 0x400000 absolute)"""
import sys, struct
import capstone

path = sys.argv[1]
targets = [int(a, 16) for a in sys.argv[2:]]
d = open(path, 'rb').read()
e = struct.unpack_from('<I', d, 0x3c)[0]
coff = e + 4
nsec, = struct.unpack_from('<H', d, coff + 2)
opt = coff + 20
sect_off = opt + struct.unpack_from('<H', d, coff + 16)[0]
ib, = struct.unpack_from('<I', d, opt + 28)
secs = []
for i in range(nsec):
    so = sect_off + i * 40
    vsz, vrva, rsz, rptr = struct.unpack_from('<IIII', d, so + 8)
    secs.append((vrva, vsz, rptr, rsz))
text = secs[0]
vrva, vsz, rptr, rsz = text
code = d[rptr:rptr + min(vsz, rsz)]
base = ib + vrva
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.skipdata = True
ins = list(md.disasm(code, base))
for idx, i in enumerate(ins):
    if i.mnemonic == 'call' and i.op_str.startswith('0x'):
        try:
            t = int(i.op_str, 16)
        except ValueError:
            continue
        if t in targets:
            print('--- call %#x at %#x' % (t, i.address))
            for j in ins[max(0, idx - 9):idx + 3]:
                print('  %#x  %-8s %s' % (j.address, j.mnemonic, j.op_str))
