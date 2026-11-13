"""Read-only: split the 2,124 aligned code-range dwords of file-backed .rdata/.data by mechanical evidence.
Usage: py -3.12 codeptr_split.py <exe> <pc_funcs.csv> <CODEPTR_SLOTS.tsv> <PortStateSeed.inc>
"""
import bisect, csv, re, struct, sys
exe, funcs_csv, slots_tsv, seed_inc = sys.argv[1:5]
b = open(exe, 'rb').read()
SECS = {'.rdata': (0x005cc000, 0x01d044, 0x1cc000), '.data': (0x005ea000, 0x04d000, 0x1ea000)}
T0, T1 = 0x00401000, 0x005ca8e0
funcs = []
for r in csv.DictReader(open(funcs_csv, encoding='utf-8')):
    funcs.append((int(r['entry'], 16), int(r['body_end'], 16)))
funcs.sort()
starts = [f[0] for f in funcs]
entryset = set(starts)
def in_body(v):
    i = bisect.bisect_right(starts, v) - 1
    return i >= 0 and funcs[i][0] <= v < funcs[i][1]
def asciiish(bs):
    return all(32 <= c < 127 or c in (0, 9, 10, 13) for c in bs) and any(32 <= c < 127 for c in bs)
hits = []
for s, (lo, n, rp) in SECS.items():
    for a in range(lo, lo + n - 3, 4):
        bs = b[rp + a - lo: rp + a - lo + 4]
        v = struct.unpack('<I', bs)[0]
        if T0 <= v < T1:
            # neighbourhood: is the previous or next dword also a code-range value (table shape)?
            def nb(x):
                if not (lo <= x < lo + n - 3):
                    return False
                w = struct.unpack_from('<I', b, rp + x - lo)[0]
                return T0 <= w < T1
            hits.append(dict(a=a, v=v, s=s, entry=v in entryset, body=in_body(v), asc=asciiish(bs),
                             table=nb(a - 4) or nb(a + 4)))
cls = {}
for h in hits:
    if h['entry']:
        k = 'A entry'
    elif h['asc'] and not h['table']:
        k = 'C ascii, isolated'
    elif h['body']:
        k = 'B inside a body, not an entry' + (' (ascii)' if h['asc'] else '')
    else:
        k = 'D outside every pc_funcs body' + (' (ascii)' if h['asc'] else '')
    cls[k] = cls.get(k, 0) + 1
print('hits', len(hits))
for k in sorted(cls):
    print('  %-40s %d' % (k, cls[k]))
print('  entry targets that are also table-shaped:', sum(1 for h in hits if h['entry'] and h['table']))
slots = {int(r['slot'], 16) for r in csv.DictReader(open(slots_tsv, newline=''), delimiter='\t')}
hs = {h['a']: h for h in hits}
print('CODEPTR_SLOTS (188) by class: entry %d, body-nonentry %d, ascii %d' % (
    sum(1 for s in slots if hs[s]['entry']), sum(1 for s in slots if not hs[s]['entry'] and hs[s]['body']),
    sum(1 for s in slots if hs[s]['asc'])))
t = open(seed_inc).read()
ents = [(int(a, 16), int(n)) for a, n, o in re.findall(r'\{ 0x([0-9a-f]+)u,\s*(\d+)u,\s*(\d+)u \}', t)]
ov = [(hex(a), n) for a, n in ents if any(a <= x < a + n for x in hs)]
print('PortStateSeed entries containing a code-range dword: %d %s' % (len(ov), ov[:12]))
print('PortStateSeed entries containing a CODEPTR slot: %d' % sum(1 for a, n in ents if any(a <= x < a + n for x in slots)))
