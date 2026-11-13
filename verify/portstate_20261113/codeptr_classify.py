"""Read-only: classify the aligned dwords of file-backed .rdata/.data whose value lands in a code section of
original/MASHED.exe.unpatched. Usage: py -3.12 pe_codeptr_classify.py <exe> <hooks.csv> <CODEPTR_SLOTS.tsv> <SEED_CANDIDATES.tsv>
"""
import csv, struct, sys
from collections import Counter

exe, hooks, slots_tsv, seedcand = sys.argv[1:5]
b = open(exe, 'rb').read()
IB = 0x00400000
SECS = {  # from pe_image_survey.py output (section headers at file 0x208..0x2d0)
    '.text': (0x00401000, 0x1c98e0, 0x001000),
    '_rwcseg': (0x005cb000, 0x000451, 0x1cb000),
    '.rdata': (0x005cc000, 0x01d044, 0x1cc000),
    '.data': (0x005ea000, 0x04d000, 0x1ea000),
}
def off(va):
    for lo, n, rp in SECS.values():
        if lo <= va < lo + n:
            return rp + va - lo
def dw(va):
    return struct.unpack_from('<I', b, off(va))[0]

entries = set()
names = {}
for row in csv.DictReader(open(hooks, newline='', encoding='utf-8')):
    r = (row.get('RVA') or row.get('rva') or '').strip()
    try:
        v = int(r, 16)
    except ValueError:
        continue
    entries.add(v)
print('hooks.csv function entries parsed:', len(entries))

code = [('.text',) + SECS['.text'][:2], ('_rwcseg',) + SECS['_rwcseg'][:2]]
def incode(v):
    for n, lo, sz in code:
        if lo <= v < lo + sz:
            return n

iat_lo, iat_hi = 0x005cc000, 0x005cc000 + 0x2e8
rows = []
for sname in ('.rdata', '.data'):
    lo, n, rp = SECS[sname]
    for a in range(lo, lo + n - 3, 4):
        v = dw(a)
        c = incode(v)
        if c:
            rows.append((a, v, sname, c, v in entries, iat_lo <= a < iat_hi))
print('aligned dwords into a code section:', len(rows))
print('  by (section, target section):', dict(Counter((r[2], r[3]) for r in rows)))
print('  target is a hooks.csv function entry:', sum(r[4] for r in rows), ' not an entry:', sum(not r[4] for r in rows))
print('  slot inside IAT 0x005cc000..0x005cc2e8:', sum(r[5] for r in rows))
print('  distinct targets:', len({r[1] for r in rows}), ' distinct entry targets:', len({r[1] for r in rows if r[4]}))
# unaligned candidates (value in .text at an odd offset), file-backed only
un = 0
for sname in ('.rdata', '.data'):
    lo, n, rp = SECS[sname]
    for a in range(lo, lo + n - 3):
        if a % 4 and incode(struct.unpack_from('<I', b, rp + a - lo)[0]):
            un += 1
print('unaligned dword positions whose value lands in code (upper bound, overlaps aligned ones):', un)
nonentry = [r for r in rows if not r[4]]
print('  first 12 non-entry slots:', ' '.join('%08x->%08x' % (r[0], r[1]) for r in nonentry[:12]))
# IAT content
iat_nz = [struct.unpack_from('<I', b, off(a))[0] for a in range(iat_lo, iat_hi, 4)]
print('IAT dwords %d, nonzero %d, sample %s' % (len(iat_nz), sum(1 for x in iat_nz if x),
      ' '.join('%08x' % x for x in iat_nz[:4])))
# cross-check with PORT-OT-SEED-CODEPTR inventory
cs = [int(r['slot'], 16) for r in csv.DictReader(open(slots_tsv, newline=''), delimiter='\t')]
scan = {r[0] for r in rows}
print('CODEPTR_SLOTS.tsv slots %d, in this scan %d, missing %s' % (len(cs), len(set(cs) & scan),
      [hex(x) for x in sorted(set(cs) - scan)][:10]))
# B16 scale constants
for va, want in ((0x005cd5a8, 0x3ACCCCCD), (0x005cc560, 0x3B088889)):
    print('B16 const %08x image %08x written-by-B16 %08x equal=%s' % (va, dw(va), want, dw(va) == want))
# seed candidates TSV head count
with open(seedcand, newline='', encoding='utf-8') as f:
    n = sum(1 for _ in f) - 1
print('SEED_CANDIDATES.tsv rows:', n)
