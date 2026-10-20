"""d2off: first call where the player's record differs, original MSD vs port MSD1 (aligned on each side's first moving frame),
over a set of dword offsets (default: the dynamic block 0x928..0x9f0 and 0xb00..0xb30). Prints the first N calls with a
difference and the differing offsets (orig / port as float and hex).
usage: d2first.py <orig.msd> <port.rec.msd> [n_calls] [lo:hi,lo:hi]"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
u = lambda r, o: struct.unpack_from('<I', r, o)[0]
def mv(L): return next(i for i, (fi, r) in enumerate(L) if f(r, 0x9e4) > 0.01)
O = list(m.parse(sys.argv[1])); P = list(m.parse(sys.argv[2])); o0, p0 = mv(O), mv(P)
n = int(sys.argv[3]) if len(sys.argv) > 3 else 3
rg = sys.argv[4] if len(sys.argv) > 4 else '928:9f0,b00:b30'
SKIP = {0x954, 0x984, 0x994, 0x9a4, 0x9d4, 0x9d8, 0x9dc}   # RW matrix flag / pad words; +0x9d4..dc = the dump's one-frame read lag
offs = [o for a, b in (x.split(':') for x in rg.split(',')) for o in range(int(a, 16), int(b, 16), 4) if o not in SKIP]
shown = 0
for k in range(0, min(len(O) - o0, len(P) - p0)):
    a, b = O[o0 + k][1], P[p0 + k][1]
    d = [o for o in offs if o + 4 <= len(a) and o + 4 <= len(b) and u(a, o) != u(b, o)]
    if d:
        print('k=%d (orig frame %d, port frame %d): %d offsets' % (k, O[o0 + k][0], P[p0 + k][0], len(d)))
        for o in d[:24]: print('   +0x%03x  %.9g / %.9g  (%08x / %08x)' % (o, f(a, o), f(b, o), u(a, o), u(b, o)))
        shown += 1
        if shown >= n: break
