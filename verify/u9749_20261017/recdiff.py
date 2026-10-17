"""u9749: align a port MSD1 record dump with an original capture on car 2's (b) window, then list every dword that
differs at the given window calls. Alignment: original frame = (car 2 window start in the original's aistep) + k; port
frame = the port dump index whose +0x9e4 sequence matches the original's first frozen coast (1237.9x, 9 frames).
usage: py -3.12 recdiff.py <orig.msd> <orig.aistep.csv> <port.rec.msd> <k> [<k> ...] [--rel=1e-4] [--offs=a-b]"""
import csv, struct, sys
sys.path.insert(0, r'C:\Users\maria\Desktop\Proyectos\Mashed\re\tools\statediff')
import msd_fields as m

args = [a for a in sys.argv[1:] if not a.startswith('--')]
opts = dict(a[2:].split('=', 1) for a in sys.argv[1:] if a.startswith('--'))
REL = float(opts.get('rel', '1e-4'))
LO, HI = (int(x, 16) for x in opts.get('offs', '0-d04').split('-'))
o_msd, o_ais, p_msd = args[:3]
ks = [int(k) for k in args[3:]]

rows = [r for r in csv.DictReader(open(o_ais, newline='')) if r['v'] == '2']
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
f0 = int(rows[i0]['frame'])
orig = {fi - f0: rec for fi, rec in m.parse(o_msd)}
port = [rec for fi, rec in m.parse(p_msd)]
spd = lambda r: struct.unpack_from('<f', r, 0x9e4)[0]
# original coast: first k with a 9-frame run of equal +0x9e4 above 1000
ko = next(k for k in sorted(orig) if k >= 0 and spd(orig[k]) > 1000 and all(k + j in orig and spd(orig[k + j]) == spd(orig[k]) for j in range(9)))
kp = next(i for i in range(len(port) - 9) if spd(port[i]) > 1000 and all(abs(spd(port[i + j]) - spd(port[i])) == 0 for j in range(9)))
shift = kp - ko
print(f"orig coast starts k={ko} ({spd(orig[ko]):.3f}); port coast starts at dump index {kp} ({spd(port[kp]):.3f}); port index = k + {shift}")
for k in ks:
    o, p = orig.get(k), port[k + shift] if 0 <= k + shift < len(port) else None
    if o is None or p is None:
        print('k', k, 'missing'); continue
    print(f"=== k={k}  orig 9e4 {spd(o):.3f}  port 9e4 {spd(p):.3f}")
    for off in range(LO, HI, 4):
        a, b = o[off:off + 4], p[off:off + 4]
        if a == b: continue
        fa, fb = struct.unpack('<f', a)[0], struct.unpack('<f', b)[0]
        ia, ib = struct.unpack('<i', a)[0], struct.unpack('<i', b)[0]
        isf = all(abs(x) < 1e9 and (x == 0 or abs(x) > 1e-12) for x in (fa, fb))
        if isf:
            d = abs(fa - fb); s = max(abs(fa), abs(fb), 1e-9)
            if d / s < REL: continue
            print(f"  +0x{off:03x}  orig {fa:14.6f}  port {fb:14.6f}  (i {ia} / {ib})")
        else:
            print(f"  +0x{off:03x}  orig i {ia:12d}  port i {ib:12d}  (hex {a[::-1].hex()} / {b[::-1].hex()})")
