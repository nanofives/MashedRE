"""PREREG_U9684.md section 4: build the counterfactual G-REPLAY inputs from o_tg1.msd.tgt.csv (read-only on it).
usage: py -3.12 make_cf.py <o_tg1.msd.tgt.csv> <outdir>
Keeps the rows with frame < 45150 (before the player first moves). Replaces the player's (car 0) columns:
  CF_NONE unmodified | CF_PR pr0 = 0x00000000 | CF_Z z0 = bits(-1.9990371465682983) | CF_SP sp0 = bits(0.63804620504379272)
  CF_ALL all three. Values are written as unsigned u32, the form the original file uses."""
import csv, os, struct, sys
def bits(x): return struct.unpack('<I', struct.pack('<f', x))[0]
src, out = sys.argv[1], sys.argv[2]
rows = list(csv.reader(open(src, newline='')))
h, body = rows[0], [r for r in rows[1:] if int(r[1]) < 45150]
ix = {k: h.index(k) for k in ('pr0', 'z0', 'sp0')}
mods = {'CF_NONE': {}, 'CF_PR': {'pr0': 0}, 'CF_Z': {'z0': bits(-1.9990371465682983)},
        'CF_SP': {'sp0': bits(0.63804620504379272)}}
mods['CF_ALL'] = {**mods['CF_PR'], **mods['CF_Z'], **mods['CF_SP']}
for name, m in mods.items():
    p = os.path.join(out, name + '.tgt.csv')
    with open(p, 'w', newline='') as f:
        w = csv.writer(f, lineterminator='\n'); w.writerow(h)
        for r in body:
            r = list(r)
            for k, v in m.items(): r[ix[k]] = str(v)
            w.writerow(r)
    print(name, len(body), 'rows ->', p, {k: v for k, v in m.items()})
