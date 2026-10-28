"""Score the original's FUN_00446520 per-call rows (scenario_launch --p3cam-probe) against the port's
MASHED_LEG2_TRACE rows (one per SubStateCameraChain call). PREREG_d3subph3.md sections 1-2.

usage: py -3.12 score_p3cam.py <orig.p3cam.csv> [<port.trace.csv>] [--n 12] [--offset dx,dy,dz]

Prints: the census of original calls from the last sub-state-2 call through the first phase-3 frames
(call, sdframe, caller, reset, ba8c, t1030, t1008, cam pos in -> out), then call-by-call cam pos
(+4..+0xc after the call) port - orig, raw and with the steady car offset removed.
"""
import csv, struct, sys


def hf(h):
    return struct.unpack('<f', struct.pack('<I', int(h, 16) & 0xffffffff))[0]


def hi(h):
    return struct.unpack('<i', struct.pack('<I', int(h, 16) & 0xffffffff))[0]


def main():
    a = sys.argv[1:]
    n, off = 12, (0.0, 0.0, 0.0)
    if '--n' in a:
        i = a.index('--n'); n = int(a[i + 1]); del a[i:i + 2]
    if '--offset' in a:
        i = a.index('--offset'); off = tuple(float(x) for x in a[i + 1].split(',')); del a[i:i + 2]
    orig = list(csv.DictReader(open(a[0])))
    port = list(csv.DictReader(open(a[1]))) if len(a) > 1 else []
    k3 = next((i for i, r in enumerate(orig) if hi(r['ba8c']) == 3), None)
    print('original calls: %d, first with ba8c==3 at call %s' % (len(orig), k3))
    if k3 is None:
        return 1
    from collections import Counter
    print('sub-state census of all captured calls:', dict(Counter(hi(r['ba8c']) for r in orig)))
    print('callers in sub-state 3:', dict(Counter(r['ret'] for r in orig if hi(r['ba8c']) == 3)))
    lo = max(0, k3 - 3)
    print('\ncall sdf  ret       rst ba8c s29b8   t1030       t1008 in(+4..+c)                    out(+4..+c)                   out+1c  e0(x,z)')
    sel = orig[lo:k3 + n]
    for r in sel:
        pin = [hf(r['i%02x' % o]) for o in (4, 8, 0xc)]
        pout = [hf(r['o%02x' % o]) for o in (4, 8, 0xc)]
        print('%4s %4s %8s %3s %4d %6d %11d %5d (%8.5f,%8.5f,%8.5f) (%8.5f,%8.5f,%8.5f) %5d (%7.3f,%7.3f)' % (
            r['call'], r['sdframe'], r['ret'], r['reset'], hi(r['ba8c']), hi(r['s29b8']), hi(r['t1030']) & 0xffffffff,
            hi(r['t1008']), *pin, *pout, hi(r['o1c']), hf(r['e0x']), hf(r['e0z'])))
    if not port:
        return 0
    o3 = [r for r in orig if hi(r['ba8c']) == 3]
    print('\ncall-by-call cam +4..+0xc after the call (port row k vs original phase-3 call k), offset removed = %s' % (off,))
    print('  k  port_rst orig_rst  port_ticks orig_t1030   d_raw(x,y,z)                  d_minus_offset(x,y,z)')
    for k in range(min(n, len(port), len(o3))):
        p, o = port[k], o3[k]
        pp = [float(p[c]) for c in ('tx', 'ty', 'tz')]
        op = [hf(o['o%02x' % x]) for x in (4, 8, 0xc)]
        d = [pp[i] - op[i] for i in range(3)]
        dm = [d[i] - off[i] for i in range(3)]
        print('%3d %4s %8s %12s %11d  (%+.5f,%+.5f,%+.5f)   (%+.5f,%+.5f,%+.5f)' % (
            k, p['reset'], o['reset'], p['cam_ticks'], hi(o['t1030']) & 0xffffffff, *d, *dm))
    return 0


if __name__ == '__main__':
    sys.exit(main())
