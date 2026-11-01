"""Print the un-armed path of a RunTracked instantiation: the je target right after `call Armed`, up to its ret."""
import sys, struct, re, capstone
dll, mapf, name = sys.argv[1], sys.argv[2], sys.argv[3]
d = open(dll, 'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]
nsec = struct.unpack_from('<H', d, pe + 6)[0]; opt = struct.unpack_from('<H', d, pe + 20)[0]
base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
secs = []; o = pe + 24 + opt
for i in range(nsec):
    vs, va, rs, ra = struct.unpack_from('<IIII', d, o + 8); secs.append((va, vs, ra, rs)); o += 40


def off(v):
    r = v - base
    for va, vs, ra, rs in secs:
        if va <= r < va + max(vs, rs): return r - va + ra


syms = {}
for l in open(mapf, encoding='latin-1'):
    m = re.match(r'\s*\w+:\w+\s+(\S+)\s+([0-9a-f]{8})\s', l)
    if m: syms[m.group(1)] = int(m.group(2), 16)
rev = {v: k for k, v in syms.items()}
md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
a = syms[name]
ins = list(md.disasm(d[off(a):off(a) + 0x1400], a))
tgt = None
for i, x in enumerate(ins):
    if x.mnemonic == 'call' and 'Armed' in rev.get(int(x.op_str, 16) if x.op_str.startswith('0x') else -1, ''):
        for y in ins[i + 1:i + 5]:
            if y.mnemonic == 'je':
                tgt = int(y.op_str, 16); break
        break
print('==', name, hex(a), 'unarmed path at', hex(tgt) if tgt else None)
on = False
for x in ins:
    if x.address == tgt: on = True
    if on:
        t = ''
        if x.mnemonic == 'call' and x.op_str.startswith('0x'): t = '  ; ' + rev.get(int(x.op_str, 16), '')
        print(f'  {x.address:08x} {x.mnemonic} {x.op_str}{t}')
        if x.mnemonic == 'ret': break
