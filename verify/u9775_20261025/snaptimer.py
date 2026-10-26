"""u9775: per .data snapshot of one run: aligned record k, DAT_007f0ff8 (A5 grip-ramp race timer), DAT_007f0ff4,
DAT_007f1008, and the AI bank-switch words 0x0089a368 / 0x0089a370. usage: snaptimer.py <msd>"""
import sys, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
BASE, SZ, REC = 0x005ea000, 0x32a800, 0x008815a0
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
OFFS = [o for a, b in ((0x928, 0x9f0), (0xb00, 0xb30)) for o in range(a, b, 4) if o not in {0x954, 0x984, 0x994, 0x9a4, 0x9d4, 0x9d8, 0x9dc}]
key = lambda r: b''.join(r[o:o + 4] for o in OFFS)
L = [r for _, r in m.parse(sys.argv[1])]; a0 = next(i for i, r in enumerate(L) if f(r, 0x9e4) > 0.01)
idx = {}
for k in range(len(L) - a0): idx.setdefault(key(L[a0 + k]), k)
raw = open(sys.argv[1] + '.snap.bin', 'rb').read()
g = lambda img, va, t='<i': struct.unpack_from(t, img, va - BASE)[0]
for i in range(len(raw) // SZ):
    img = raw[i * SZ:(i + 1) * SZ]; k = idx.get(key(img[REC - BASE:REC - BASE + 0xd04]))
    t = g(img, 0x007f0ff8)
    print('snap %2d k=%s  7f0ff8=%d (60000-t=%d)  7f0ff4=%d  7f1008=%d  89a368=%d  89a370=%.7g'
          % (i, k, t, 60000 - t, g(img, 0x007f0ff4), g(img, 0x007f1008), g(img, 0x0089a368), g(img, 0x0089a370, '<f')))
