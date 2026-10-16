"""Cause-2 attribution reducer (PREREG_d2engage.md 4.6 / amendment 1). REPORT ONLY.
For a port arm: reads <arm>.a6a.log (MASHED_A6ADUMP, slot 0, one line per frame) and prints, from the first frame
whose speed drops by > 50 % in one frame (the first wall impact) for --n frames:
  speed, horizontal velocity split on the body axes (fwd = snap.bf, lat = the horizontal perpendicular),
  lateral retention lat[t]/lat[t-1], clamp #6 arm (0 LOW, 1 HIGH, -1 not run) and k (act.kvel), grip.
Then a band summary over the whole post-impact window: per speed band, n, median k, LOW-arm share, median retention.
For the original: the same velocity split from orig_solo3.msd (+0x9b0/+0x9b8 vel, +0x9d4/+0x9dc fwd), from its
first impact (idx 981) -- the original's k is not in the .msd; D2_REOPEN section 26.3 gives 0.1 at 100-150.
usage: py -3.12 cause2.py <arm> [<arm> ...] [--n 40] [--orig]"""
import math, os, re, struct, sys
D = os.path.dirname(os.path.abspath(__file__))
ORIG = r'C:\Users\maria\Desktop\Proyectos\Mashed\verify\d2_reopen_20260929\orig_solo3.msd'
args = [a for a in sys.argv[1:] if not a.startswith('--')]
N = int(sys.argv[sys.argv.index('--n') + 1]) if '--n' in sys.argv else 40
if '--n' in sys.argv: args = [a for a in args if a != sys.argv[sys.argv.index('--n') + 1]]
BANDS = [(0, 50), (50, 100), (100, 150), (150, 260), (260, 500)]


def split(vx, vz, fx, fz):
    h = math.hypot(fx, fz) or 1.0
    fx, fz = fx / h, fz / h
    return vx * fx + vz * fz, vx * fz - vz * fx          # forward, lateral (signed)


def med(v):
    v = sorted(v); return v[len(v) // 2] if v else float('nan')


def report(rows, label, has_k):
    i0 = next((i for i in range(1, len(rows)) if rows[i - 1]['sp'] > 500 and rows[i]['sp'] < 0.5 * rows[i - 1]['sp']), None)
    print(f'== {label}: first impact at row {i0}' + ('' if i0 is None else f" (frame {rows[i0]['f']}, sp {rows[i0 - 1]['sp']:.1f} -> {rows[i0]['sp']:.1f})"))
    if i0 is None: return
    print('   frame      sp      fwd      lat   lat_ret' + ('  arm        k        grip' if has_k else ''))
    for i in range(i0, min(i0 + N, len(rows))):
        r, p = rows[i], rows[i - 1]
        ret = abs(r['lat']) / abs(p['lat']) if abs(p['lat']) > 1e-6 else float('nan')
        extra = f"  {r['arm']:3d} {r['k']:8.4f} {r['grip']:11.1f}" if has_k else ''
        print(f"   {r['f']:5d} {r['sp']:8.2f} {r['fwd']:8.2f} {r['lat']:8.2f} {ret:8.4f}{extra}")
    post = rows[i0 + 1:]
    print('   band       n   med_ret' + ('   med_k  LOW_share' if has_k else ''))
    for lo, hi in BANDS:
        sel = [(rows[j - 1], rows[j]) for j in range(i0 + 1, len(rows)) if lo <= rows[j]['sp'] < hi and abs(rows[j - 1]['lat']) > 1.0]
        if not sel: continue
        rets = [abs(b['lat']) / abs(a['lat']) for a, b in sel]
        extra = ''
        if has_k:
            ks = [b['k'] for _, b in sel if b['arm'] >= 0]
            low = sum(1 for _, b in sel if b['arm'] == 0)
            extra = f'  {med(ks):7.4f}  {low}/{len(sel)}'
        print(f'   {lo:4d}-{hi:<4d} {len(sel):4d}  {med(rets):8.4f}{extra}')


VEC = r'([-0-9.eE+]+),([-0-9.eE+]+),([-0-9.eE+]+)'
for arm in args:
    p = os.path.join(D, arm + '.a6a.log')
    if not os.path.exists(p):
        print(f'== {arm}: MISSING {p}'); continue
    rows = []
    for line in open(p, errors='replace'):
        mv = re.search(r'snap\.vel=' + VEC, line); mf = re.search(r'snap\.bf=' + VEC, line)
        ms = re.search(r'snap\.sp=([-0-9.eE+]+)', line); mfr = re.match(r'f=(\d+)', line)
        mk = re.search(r'act\.kvel=([-0-9.eE+]+)', line); ma = re.search(r'act\.arm=(-?\d+)', line)
        mg = re.search(r'act\.grip=([-0-9.eE+]+)', line)
        if not (mv and mf and ms and mfr): continue
        vx, vz = float(mv.group(1)), float(mv.group(3)); fx, fz = float(mf.group(1)), float(mf.group(3))
        fw, la = split(vx, vz, fx, fz)
        rows.append(dict(f=int(mfr.group(1)), sp=math.hypot(vx, vz), fwd=fw, lat=la,
                         k=float(mk.group(1)) if mk else float('nan'), arm=int(ma.group(1)) if ma else -9,
                         grip=float(mg.group(1)) if mg else float('nan')))
    report(rows, arm, True)

if '--orig' in sys.argv:
    b = open(ORIG, 'rb').read(); rec, base, _ = struct.unpack_from('<III', b, 4)
    off, fr = 16, []
    while off + 4 + rec <= len(b):
        fi, = struct.unpack_from('<I', b, off); fr.append((fi, off + 4)); off += 4 + rec
    fr.sort(); rows = []
    for fi, po in fr:
        vx, vz = struct.unpack_from('<f', b, po + 0x9b0)[0], struct.unpack_from('<f', b, po + 0x9b8)[0]
        fx, fz = struct.unpack_from('<f', b, po + 0x9d4)[0], struct.unpack_from('<f', b, po + 0x9dc)[0]
        fw, la = split(vx, vz, fx, fz)
        rows.append(dict(f=fi, sp=math.hypot(vx, vz), fwd=fw, lat=la))
    report(rows, 'ORIGINAL orig_solo3', False)
