"""lk21: one car's record, original capture (scenario_launch --statediff-car V) against a port MSD1 dump
(MASHED_RECDUMP_SLOT=V), aligned on the +0x958/+0x960 position bits: the port index whose position equals the original
record at window call --anchor (default 2) bit for bit. For each k in [k0, k1) prints the float/dword offsets that differ.
usage: py -3.12 recdiff1.py <orig.msd> <orig.aistep.csv> <port.rec.msd> <car> <k0> <k1> [--anchor=2] [--rel=0] [--offs=a-b] [--first]"""
import csv, struct, sys, os
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 're', 'tools', 'statediff'))
import msd_fields as m

args = [a for a in sys.argv[1:] if not a.startswith('--')]
opts = dict((a[2:].split('=', 1) + [''])[:2] for a in sys.argv[1:] if a.startswith('--'))
o_msd, o_ais, p_msd, v, k0, k1 = args[0], args[1], args[2], args[3], int(args[4]), int(args[5])
ANCH = int(opts.get('anchor', '2'))
REL = float(opts.get('rel') or 0)
LO, HI = (int(x, 16) for x in (opts.get('offs') or '0-d04').split('-'))

rows = [r for r in csv.DictReader(open(o_ais, newline='')) if r['v'] == v]
i0 = next(i for i, r in enumerate(rows) if int(r['c4']) != 0)
f0 = int(rows[i0]['frame'])
orig = {fi - f0: rec for fi, rec in m.parse(o_msd)}
port = [rec for fi, rec in m.parse(p_msd)]
pos = lambda r: r[0x958:0x95c] + r[0x960:0x964]
a = pos(orig[ANCH])
hits = [i for i, r in enumerate(port) if pos(r) == a]
if not hits:
    sys.exit('no port record with the original position bits at k=%d' % ANCH)
shift = hits[0] - ANCH
print('orig window f0=%d, port index = k + %d (anchor k=%d, %d hit(s))' % (f0, shift, ANCH, len(hits)))
for k in range(k0, k1):
    ro, j = orig.get(k), k + shift
    if ro is None or not (0 <= j < len(port)):
        continue
    rp = port[j]
    n = min(len(ro), len(rp), HI)
    diffs = []
    for off in range(LO, n - 3, 4):
        bo, bp = ro[off:off + 4], rp[off:off + 4]
        if bo == bp:
            continue
        fo, fp = struct.unpack('<f', bo)[0], struct.unpack('<f', bp)[0]
        if REL and fo == fo and fp == fp and abs(fp - fo) <= REL * max(1.0, abs(fo)):
            continue
        diffs.append('+%03x %.9g/%.9g' % (off, fo, fp))
    print('k=%3d ndiff=%d  %s' % (k, len(diffs), '  '.join(diffs[:14])))
    if '--first' in opts and diffs:
        break
