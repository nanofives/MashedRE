"""Compare symbol addresses across MSVC map files: how many non-RwCore symbols moved."""
import re, sys
def load(p):
    d = {}
    for l in open(p, errors='replace'):
        m = re.match(r'\s*(000[1-9]):([0-9a-f]{8})\s+(\S+)\s+([0-9a-f]{8})\s+(f\s+)?(\S.*)$', l)
        if m:
            d[m.group(3)] = (int(m.group(4), 16), m.group(6).strip())
    return d
maps = [load(p) for p in sys.argv[1:]]
a = maps[0]
for i, b in enumerate(maps[1:], 1):
    moved = {}; same = 0; total = 0
    for s, (va, obj) in a.items():
        if s not in b: continue
        if 'RwCore' in obj: continue
        total += 1
        if b[s][0] == va: same += 1
        else: moved.setdefault(obj.split(':')[-1], []).append((s, va, b[s][0]))
    print(f'{sys.argv[1]} vs {sys.argv[i+1]}: non-RwCore symbols {total}, same address {same}, moved {total-same}')
    for obj, lst in sorted(moved.items(), key=lambda x: -len(x[1]))[:12]:
        s, va, vb = lst[0]
        print(f'   {obj}: {len(lst)} moved, e.g. {s[:60]} {va:08x}->{vb:08x}')
    for s in ['_CamEntryType0Step445aa0', '_g_b17_low_arena_pad']:
        if s in a and s in b: print('   ', s, hex(a[s][0]), hex(b[s][0]))
