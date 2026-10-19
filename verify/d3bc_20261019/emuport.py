"""d3bc: emu15e20.py's arithmetic (LUT, PC_24) on the PORT's inputs: own / look from the port step CSV row k, forward
from the port record dump (MSD1, list index = window call) call k+dk, +0x948/+0x950, or the step CSV's rec_9d4/rec_9dc.
Scored against the port's own hist_dc / hist_d8. usage: emuport.py <port.rec.msd> <port.step.csv> <v> [rows]"""
import csv, os, struct, sys
sys.argv_saved = list(sys.argv)
import importlib.util
spec = importlib.util.spec_from_file_location('emu', os.path.join(os.path.dirname(os.path.abspath(__file__)), 'emu15e20.py'))
pm, pa, v = sys.argv[1:4]; n = int(sys.argv[4]) if len(sys.argv) > 4 else 100
sys.argv = ['emu', '-', '-', '-', '0']
src = open(spec.origin).read().split("om, oa, v = sys.argv[1:4]")[0]
G = {'__file__': spec.origin}; exec(src, G)
m = G['m']; f32 = G['f32']; err = G['err']; V = dict(lut=True, sum='f32', sc=G['SC_EXACT'], r24=True)
rows = [r for r in csv.DictReader(open(pa, newline='')) if r['v'] == v]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0); P = rows[i0:]
rec = [r for fi, r in m.parse(pm)]
g = lambda r, o: struct.unpack_from('<f', r, o)[0]
for src_name in ('rec k-2', 'rec k-1', 'rec k', 'csv 9d4'):
    hits = []
    for k in range(2, min(n, len(P))):
        o = P[k]; h = float(f32(float(o['hist_dc']))) if float(o['hist_d8']) == 360 else float(f32(float(o['hist_d8'])))
        if src_name == 'csv 9d4': fx, fz = float(o['rec_9d4']), float(o['rec_9dc'])
        else:
            j = k + {'rec k-2': -2, 'rec k-1': -1, 'rec k': 0}[src_name]; fx, fz = g(rec[j], 0x948), g(rec[j], 0x950)
        ins = [float(o[c]) for c in ('own_x', 'own_z', 'look_x', 'look_z')] + [fx, fz]
        hits.append((k, err(*ins, V) - h))
    bad = [(k, d) for k, d in hits if d != 0]
    print('%-8s exact %d/%d first misses %s' % (src_name, len(hits) - len(bad), len(hits), ' '.join('%d:%+.1e' % x for x in bad[:8])))
