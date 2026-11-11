"""POWERUPINIT-B2: full byte-diff table of re/match/s29/00455400.c against MASHED.exe.unpatched 0x00455400 (L-115
order), on ALIGNED instruction pairs (difflib over mnemonic + operands; relocated operands written as their target
address, jump targets as function offsets), so a length change does not shift every later byte. For each aligned
pair the raw bytes are compared with relocation sites masked, and a relocation site's target is checked against the
original's absolute value at the aligned position (the check relocheck.py does positionally).
usage: py -3.12 verify/powerupinit_b2_20261110/bytetable_00455400.py
"""
import difflib, pathlib, re, struct, sys
sys.path.insert(0, str(pathlib.Path(__file__).resolve().parents[2] / 're' / 'tools' / 'match'))
import capstone
import match as M

ADDR = 0x00455400
SRC = pathlib.Path(__file__).resolve().parents[2] / 're' / 'match' / 's29' / '00455400.c'
text = SRC.read_text(encoding='utf-8')
flags = text.split('// FLAGS:', 1)[1].split('\n', 1)[0].strip()
obj = M.compile_obj(SRC, flags, toolchain=M.file_toolchain(text))
secs, syms = M.coff(obj)
mine, relocs, full, local = M.function_from_obj(secs, syms, 'FUN_00455400')
while mine and mine[-1] in (0xCC, 0x90):
    mine = mine[:-1]
size = M.ghidra_size(ADDR)
orig = M.original_bytes(ADDR, size)
while orig and orig[-1] in (0xCC, 0x90):
    orig = orig[:-1]

# relocation targets in ours: symbol address (the DAT_/FUN_ name) + stored addend; strings resolve to 0 (ours) so
# they are compared by the original's value being inside .rdata
s = [x for x in syms if x['sec'] > 0 and x['type'] == 0x20 and x['name'] == full][0]
sec = secs[s['sec'] - 1]
by_index = {x['index']: x for x in syms}
rtarget = {}
for off, symidx, rtype in sec['relocs']:
    o = off - s['value']
    if not (0 <= o < len(mine)):
        continue
    name = by_index.get(symidx, {}).get('name', '')
    m = re.search(r'_(?:DAT|FUN)_([0-9a-f]{8})', name)
    add = struct.unpack_from('<i', mine, o)[0]
    if rtype == 0x14:   # REL32 call
        rtarget[o] = (int(m.group(1), 16) if m else None, 'rel')
    else:
        rtarget[o] = ((int(m.group(1), 16) + add) if m else None, 'abs')

# ours with every resolvable relocation written as the resolved target (absolute sites) or as the displacement to it
# (REL32 calls), disassembled at the original's base, so both sides print the same operands
patched = bytearray(mine)
unresolved = set()
for o, (tgt, kind) in rtarget.items():
    if tgt is None:
        unresolved.add(o)          # string literal: compared as <STR> on both sides
        continue
    v = tgt if kind == 'abs' else tgt - (ADDR + o + 4)
    struct.pack_into('<i', patched, o, v if v < 0x80000000 else v - 0x100000000)
cs = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
im = list(cs.disasm(bytes(patched), ADDR))
io = list(cs.disasm(orig, ADDR))
RDATA = (0x005c0000, 0x005e0000)

def norm(i, unresolved_here):
    t = '%s %s' % (i.mnemonic, i.op_str)
    if i.mnemonic.startswith('j') and i.op_str.startswith('0x'):
        return '%s +0x%x' % (i.mnemonic, int(i.op_str, 16) - ADDR)
    if i.mnemonic == 'push' and (unresolved_here or (i.op_str.startswith('0x') and RDATA[0] <= int(i.op_str, 16) < RDATA[1])):
        return 'push <STR>'
    return t

def key_m(i):
    return norm(i, any(i.address - ADDR <= o < i.address - ADDR + i.size for o in unresolved))

def key_o(i):
    return norm(i, False)

km = [key_m(i) for i in im]
ko = [key_o(i) for i in io]
sm = difflib.SequenceMatcher(None, km, ko, autojunk=False)
rows = []
for tag, a1, a2, b1, b2 in sm.get_opcodes():
    if tag == 'equal':
        for x, y in zip(range(a1, a2), range(b1, b2)):
            mi, oi = im[x], io[y]
            mb, ob = bytes(mi.bytes), bytes(oi.bytes)
            masked = {k for k in range(mi.size) for o in unresolved if o <= mi.address - ADDR + k < o + 4}
            diff = [k for k in range(min(len(mb), len(ob))) if k not in masked and mb[k] != ob[k]]
            if len(mb) != len(ob) or diff:
                rows.append(('pair-bytes', mi, oi, len(diff)))
        continue
    rows.append((tag, [im[x] for x in range(a1, a2)], [io[y] for y in range(b1, b2)], None))

eq = sum(a2 - a1 for t, a1, a2, b1, b2 in sm.get_opcodes() if t == 'equal')
print('ours %d B / %d ins, original %d B / %d ins, aligned equal pairs %d' % (len(mine), len(im), len(orig), len(io), eq))
print('aligned pairs with a byte difference (relocation sites masked; their targets are in the key): %d' %
      sum(1 for r in rows if r[0] == 'pair-bytes'))
print()
print('| # | kind | ours (offset: bytes / instruction) | original (address: bytes / instruction) |')
print('|---|---|---|---|')
n = 0
for r in rows:
    n += 1
    if r[0] == 'pair-bytes':
        _, mi, oi, nd = r
        print('| %d | pair, %d bytes differ | +0x%03x `%s` %s %s | 0x%08x `%s` %s %s |' % (n, nd, mi.address - ADDR, bytes(mi.bytes).hex(' '), mi.mnemonic, mi.op_str, oi.address, bytes(oi.bytes).hex(' '), oi.mnemonic, oi.op_str))
        continue
    tag, ms, os_, _ = r
    fm = '<br>'.join('+0x%03x `%s` %s %s' % (i.address - ADDR, bytes(i.bytes).hex(' '), i.mnemonic, i.op_str) for i in ms) or '(none)'
    fo = '<br>'.join('0x%08x `%s` %s %s' % (i.address, bytes(i.bytes).hex(' '), i.mnemonic, i.op_str) for i in os_) or '(none)'
    print('| %d | %s | %s | %s |' % (n, tag, fm, fo))
