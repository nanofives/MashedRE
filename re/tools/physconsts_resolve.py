import re, os, struct
from collections import Counter

ROOT = r'C:\Users\maria\Desktop\Proyectos\Mashed'
SRC = os.path.join(ROOT, 'mashedmod', 'src', 'mashed_re')

bat = open(os.path.join(ROOT, 'mashedmod', 'build.bat'), encoding='utf-8', errors='replace').read()
start = bat.find('mashed_re.exe"')
end = bat.find('/link', start)
block = bat[start:end]
rels = sorted(set(re.findall(r'"([^"]+\.cpp)"', block)))

pat = re.compile(r'#define\s+(\w+)\s+\(\s*\*\s*\(\s*const\s+float\s*\*\s*\)\s*(0x[0-9a-fA-F]+)u?\s*\)(.*)')
hits = []
missing = []
for rel in rels:
    p = os.path.join(SRC, rel)
    if not os.path.exists(p):
        missing.append(rel)
        continue
    for i, ln in enumerate(open(p, encoding='utf-8', errors='replace'), 1):
        m = pat.search(ln)
        if m:
            hits.append([rel, i, m.group(1), int(m.group(2), 16), m.group(3).strip()])

exe = open(os.path.join(ROOT, 'original', 'MASHED.exe.unpatched'), 'rb').read()
pe = struct.unpack_from('<I', exe, 0x3c)[0]
nsec = struct.unpack_from('<H', exe, pe + 6)[0]
optsz = struct.unpack_from('<H', exe, pe + 20)[0]
base = struct.unpack_from('<I', exe, pe + 24 + 28)[0]
secs = []
off = pe + 24 + optsz
for i in range(nsec):
    s = off + i * 40
    nm = exe[s:s + 8].rstrip(b'\x00').decode(errors='replace')
    vsz = struct.unpack_from('<I', exe, s + 8)[0]
    va = struct.unpack_from('<I', exe, s + 12)[0]
    rsz = struct.unpack_from('<I', exe, s + 16)[0]
    ptr = struct.unpack_from('<I', exe, s + 20)[0]
    secs.append((nm, base + va, vsz, ptr, rsz))

def read_f32(va):
    for nm, sva, vsz, ptr, rsz in secs:
        if sva <= va < sva + vsz:
            d = va - sva
            if d < rsz and ptr + d + 4 <= len(exe):
                return struct.unpack_from('<f', exe, ptr + d)[0], struct.unpack_from('<I', exe, ptr + d)[0], nm
            return None, None, nm + '(bss)'
    return None, None, 'OUTSIDE'

print('exe TUs parsed from build.bat : %d  (missing on disk: %d)' % (len(rels), len(missing)))
print('float RVA macros found        : %d' % len(hits))
print()

resolved = 0
unres = []
mism = []
for h in hits:
    v, b, sec = read_f32(h[3])
    h += [v, b, sec]
    if v is None:
        unres.append(h)
    else:
        resolved += 1
        g = re.search(r'(-?\d+(?:\.\d+)?(?:e[-+]?\d+)?)', h[4], re.I)
        if g:
            try:
                gv = float(g.group(1))
                if abs(gv - v) > max(1e-6, abs(v) * 1e-4):
                    mism.append((h, gv, v))
            except ValueError:
                pass

print('resolved from binary : %d' % resolved)
print('unresolved (bss)     : %d' % len(unres))
print('gloss mismatches     : %d' % len(mism))
print()
c = Counter(h[0] for h in hits)
print('files (%d):' % len(c))
for f, n in c.most_common(14):
    print('  %3d  %s' % (n, f))
print()
print('sample:')
for h in hits[:10]:
    v = ('%.7g' % h[5]) if h[5] is not None else 'BSS'
    b = ('0x%08X' % h[6]) if h[6] is not None else '--------'
    print('  0x%08X = %-13s %s [%s] %s:%d' % (h[3], v, b, h[7], h[0], h[1]))
if mism:
    print()
    print('GLOSS MISMATCHES (comment vs binary):')
    for h, gv, v in mism[:15]:
        print('  0x%08X  comment=%-12g binary=%-14.9g %s:%d' % (h[3], gv, v, h[0], h[1]))
if unres:
    print()
    print('UNRESOLVED (%d):' % len(unres))
    for h in unres[:10]:
        print('  0x%08X [%s] %s:%d  %s' % (h[3], h[7], h[0], h[1], h[4][:46]))

import json
json.dump([{'file': h[0], 'line': h[1], 'macro': h[2], 'addr': h[3],
            'comment': h[4], 'value': h[5], 'bits': h[6], 'sec': h[7]} for h in hits],
          open(os.path.join(ROOT, 'verify', 'physconsts_20260821.json'), 'w'), indent=1)
print()
print('wrote verify/physconsts_20260821.json')
