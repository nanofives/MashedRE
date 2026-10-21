"""u9778: velocity (+0x9b0..b8), angular velocity (+0x9bc..c4), speed (+0x9e4) and +0x9e0 per k, orig MSD vs port MSD1 dump
(recpos0.py alignment). usage: velcmp.py <orig.msd> <port.rec.msd> <port.step.csv> k0 k1"""
import sys, csv, struct, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m
f = lambda r, o: struct.unpack_from('<f', r, o)[0]
def onset(c): return int(next(r for r in csv.DictReader(open(c)) if r['v'] == '3' and int(r['c4']) != 0)['frame'])
O = {fi - onset(sys.argv[1] + '.aistep.csv'): r for fi, r in m.parse(sys.argv[1])}
P = {fi - onset(sys.argv[3]): r for fi, r in m.parse(sys.argv[2])}
for k in range(int(sys.argv[4]), int(sys.argv[5]) + 1):
    a, b = O[k], P[k]
    v = ' '.join(f'{f(a,o):.4f}/{f(b,o):.4f}' for o in (0x9b0, 0x9b4, 0x9b8))
    w = ' '.join(f'{f(a,o):.5g}/{f(b,o):.5g}' for o in (0x9bc, 0x9c0, 0x9c4))
    print(f'{k} vel {v} | w {w} | 9e4 {f(a,0x9e4):.4f}/{f(b,0x9e4):.4f} 9e0 {f(a,0x9e0):g}/{f(b,0x9e0):g}')
