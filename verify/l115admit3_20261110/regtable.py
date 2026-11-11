"""L115-ADMIT-3-TABLE: full per-byte table for a register-only near miss (generalises
verify/particle3_20261110/bytetable_0048e820.py to any register mapping).

Whole obj symbol compared byte k to byte k with MASHED.exe.unpatched at <rva> (the instruction counts and lengths are
equal for a register-only residual), relocation sites masked. For every instruction holding a differing byte:
  - the register mapping ours -> original is read from the operand text (same mnemonic, operands differing only in
    register names);
  - every differing byte must differ only in one or both 3-bit register fields (bits 0-2 = ModRM r/m or opcode +r,
    bits 3-5 = ModRM reg), and each changed field must map ours' register code to the original's by that mapping.
Label: "explained by register X<->Y" when it holds, else "other".
usage: py -3.12 verify/l115admit3_20261110/regtable.py <rva> <src.c>
"""
import pathlib, re, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[2] / 're' / 'tools' / 'match'))
import capstone
import match as M

ADDR = int(sys.argv[1], 16)
SRC = pathlib.Path(sys.argv[2])
text = SRC.read_text(encoding='utf-8')
flags = text.split('// FLAGS:', 1)[1].split('\n', 1)[0].strip()
obj = M.compile_obj(SRC, flags, toolchain=M.file_toolchain(text))
secs, syms = M.coff(obj)
sym = re.search(r'// MATCH: 0x[0-9a-f]+ (\S+)', text).group(1).lstrip('_')
mine, relocs, full, local = M.function_from_obj(secs, syms, sym)
while mine and mine[-1] in (0xCC, 0x90):
    mine = mine[:-1]
orig = M.original_bytes(ADDR, len(mine))
masked = {r + k for r in relocs for k in range(4)} | {s + k for s in local for k in range(4)}
bad = [i for i in range(len(mine)) if i not in masked and mine[i] != orig[i]]

CODE = {'eax': 0, 'ecx': 1, 'edx': 2, 'ebx': 3, 'esp': 4, 'ebp': 5, 'esi': 6, 'edi': 7}
NAME = {v: k for k, v in CODE.items()}
REG = re.compile(r'\b(e[abcd]x|e[sd]i|e[sb]p)\b')
cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
im = list(cs.disasm(mine, 0))
io = list(cs.disasm(orig, 0))
same_layout = [(a.address, a.size) for a in im] == [(b.address, b.size) for b in io]


def containing(lst, off):
    return next((i for i in lst if i.address <= off < i.address + i.size), None)


rows, other, mapping, seen = [], 0, {}, set()
for b in bad:
    a, o = containing(im, b), containing(io, b)
    if a is None or o is None or a.address in seen:
        continue
    seen.add(a.address)
    ra, ro = REG.findall(a.op_str), REG.findall(o.op_str)
    textok = a.mnemonic == o.mnemonic and len(ra) == len(ro) and REG.sub('R', a.op_str) == REG.sub('R', o.op_str)
    local_map = {}
    for x, y in zip(ra, ro):
        if x != y:
            local_map[x] = y
    for x, y in local_map.items():
        if mapping.setdefault(x, y) != y:
            textok = False            # the same register maps two ways: not one mapping
    nb = [k for k in range(a.size) if a.address + k in bad]
    bitok = True
    for k in nb:
        x, y = mine[a.address + k], orig[a.address + k]
        d = x ^ y
        if d & ~0x3f:
            bitok = False; continue
        for sh in (0, 3):
            fx, fy = (x >> sh) & 7, (y >> sh) & 7
            if fx != fy and local_map.get(NAME[fx]) != NAME[fy]:
                bitok = False
    ok = textok and bitok and a.size == o.size
    other += 0 if ok else 1
    pairs = ', '.join('%s->%s' % kv for kv in sorted(local_map.items()))
    for k in nb:
        x, y = mine[a.address + k], orig[a.address + k]
        fld = []
        if (x ^ y) & 7: fld.append('bits 0-2 (%s)' % ('ModRM r/m' if k > 0 else 'opcode +r'))
        if (x ^ y) & 0x38: fld.append('bits 3-5 (ModRM reg)')
        rows.append((ADDR + a.address + k, y, x, ' + '.join(fld) or '-', '%s %s' % (o.mnemonic, o.op_str),
                     '%s %s' % (a.mnemonic, a.op_str), ('explained by register ' + pairs) if ok else 'other'))

print('0x%08x %s: %d B whole symbol, %d relocation-masked bytes, %d differing bytes, instruction layout %s, '
      'mapping ours->original: %s' % (ADDR, full, len(mine), len(masked), len(bad),
                                      'identical' if same_layout else 'DIFFERS',
                                      ', '.join('%s->%s' % kv for kv in sorted(mapping.items())) or '-'))
print()
print('| # | address | original byte | our byte | field | original instruction | our instruction | label |')
print('|---|---|---|---|---|---|---|---|')
for n, r in enumerate(rows, 1):
    print('| %d | 0x%08x | `%02x` | `%02x` | %s | %s | %s | %s |' % ((n,) + r))
print()
print('differing bytes outside any instruction: %d' % sum(1 for b in bad if containing(im, b) is None))
print('verdict: %s' % ('all explained' if other == 0 and rows else 'other: %d instruction(s)' % other))
