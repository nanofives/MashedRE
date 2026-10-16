"""Find instructions in MASHED.exe.unpatched whose memory operand displacement is in a target set,
and map them to the owning function (ranges from log/ghidra_funcfacts.jsonl).
usage: py -3.12 -I dispscan.py <exe> <facts.jsonl> <hexdisp> [<hexdisp> ...]"""
import sys, struct, json, bisect
import capstone
from capstone.x86 import X86_OP_MEM

exe, facts = sys.argv[1], sys.argv[2]
targets = set(int(a, 16) for a in sys.argv[3:])
d = open(exe, 'rb').read()
e = struct.unpack_from('<I', d, 0x3c)[0]
coff = e + 4
nsec, = struct.unpack_from('<H', d, coff + 2)
opt = coff + 20
so0 = opt + struct.unpack_from('<H', d, coff + 16)[0]
ib, = struct.unpack_from('<I', d, opt + 28)
vsz, vrva, rsz, rptr = struct.unpack_from('<IIII', d, so0 + 8)
code = d[rptr:rptr + min(vsz, rsz)]
base = ib + vrva
funcs = []
for ln in open(facts, encoding='utf-8'):
    j = json.loads(ln)
    if 'entry' not in j:
        continue
    for a, b in j['ranges']:
        funcs.append((int(a, 16), int(b, 16), j['entry'], j['name']))
funcs.sort()
starts = [f[0] for f in funcs]


def owner(a):
    i = bisect.bisect_right(starts, a) - 1
    if i >= 0 and funcs[i][0] <= a <= funcs[i][1]:
        return funcs[i][2]
    return '?'


md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
md.detail = True
md.skipdata = True
rows = []
for i in md.disasm(code, base):
    if i.mnemonic == '.byte':
        continue
    if not i.operands:
        continue
    for op in i.operands:
        if op.type == X86_OP_MEM and (op.mem.disp & 0xffffffff) in targets:
            rows.append((owner(i.address), i.address, i.mnemonic, i.op_str))
            break
for r in sorted(rows):
    print('%s  %#x  %-6s %s' % r)
