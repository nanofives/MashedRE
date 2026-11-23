"""Per-frame intro-camera comparison, standalone vs original (PLAYTEST-INTRO-CAMERA, 2026-11-22).

usage: cmp_intro_cam.py <orig camera_probe --frame --early csv> <port MASHED_CAM_DUMP csv> [label]

Original side: re/frida/camera_probe.py --frame --early rows. The RwCamera frame LTM (L_*) is what renders; a row's
L_* is the frame the PREVIOUS director call left, so the rendered basis of call k is read on row k+1. Rows with
ba8c == 3 and flag == 1 are phase 3; the first row with flag == 0 is the release.
Port side: MASHED_CAM_DUMP rows (ex..az = the published render basis, flag = DAT_00897fe0).
Two alignments: (a) from phase-3 start (frame index k = 0 at the first flag == 1 render), (b) on the release
frame (the last flag == 1 render = -1, the first race render = 0). Reports the angle between the rendered
view directions (at row) and the eye distance, per window. Phase 3's length follows the QPC clock on the original
(memory phase3-length-is-the-qpc-clock), so (a) is only meaningful early in phase 3 and (b) near the release.
"""
import csv, math, sys


def unit(v):
    n = math.sqrt(sum(c * c for c in v)) or 1.0
    return [c / n for c in v]


def ang(a, b):
    a, b = unit(a), unit(b)
    d = max(-1.0, min(1.0, sum(x * y for x, y in zip(a, b))))
    return math.degrees(math.acos(d))


def load_orig(p):
    rows = list(csv.DictReader(open(p)))
    out = []   # (flag, eye, at) per rendered frame, call k rendered on row k+1
    for k in range(len(rows) - 1):
        r, nxt = rows[k], rows[k + 1]
        if r['reset'] == '1' and k + 1 < len(rows) and rows[k + 1]['reset'] == '0' and k == 0:
            continue   # first frame: the reset call and the reset-0 call render once
        f = lambda c: float(nxt[c])
        out.append((int(r['flag']), (f('L_px'), f('L_py'), f('L_pz')), (f('L_ax'), f('L_ay'), f('L_az')), int(r['ba8c'])))
    return out


def load_port(p):
    out = []
    for r in csv.DictReader(open(p)):
        f = lambda c: float(r[c])
        out.append((int(r['flag']), (f('ex'), f('ey'), f('ez')), (f('ax'), f('ay'), f('az')), -1))
    return out


def window(o, p, oi, pi, rng, tag):
    worst_a = worst_e = 0.0
    n = 0
    sa = 0.0
    for k in rng:
        a, b = oi + k, pi + k
        if a < 0 or b < 0 or a >= len(o) or b >= len(p):
            continue
        da = ang(o[a][2], p[b][2])
        de = math.dist(o[a][1], p[b][1])
        worst_a, worst_e = max(worst_a, da), max(worst_e, de)
        sa += da
        n += 1
    print(f'  {tag}: n={n} view-angle mean {sa / max(n, 1):.2f} max {worst_a:.2f} deg, eye max {worst_e:.3f}')
    return worst_a, worst_e


def main():
    o, p = load_orig(sys.argv[1]), load_port(sys.argv[2])
    label = sys.argv[3] if len(sys.argv) > 3 else ''
    o0 = next(i for i, x in enumerate(o) if x[0] == 1 and x[1] != (0.0, 0.0, 0.0))
    p0 = next(i for i, x in enumerate(p) if x[0] == 1)
    orl = next(i for i in range(o0, len(o)) if o[i][0] == 0)
    prl = next(i for i in range(p0, len(p)) if p[i][0] == 0)
    print(f'{label} orig phase3 renders {orl - o0}, port {prl - p0}')
    print(' (a) from phase-3 start')
    for lo, hi in ((0, 100), (100, 300), (300, 450), (450, 600)):
        window(o, p, o0, p0, range(lo, hi), f'frames {lo:3d}..{hi - 1:3d}')
    print(' (b) on the release (k = 0 first race render)')
    for lo, hi in ((-200, -100), (-100, -1), (-1, 0), (0, 1), (1, 60)):
        window(o, p, orl, prl, range(lo, hi), f'k {lo:4d}..{hi - 1:4d}')
    # handover jump inside each run: view angle between the last phase-3 render and the first race render
    print(f'  handover jump: orig {ang(o[orl - 1][2], o[orl][2]):.2f} deg, port {ang(p[prl - 1][2], p[prl][2]):.2f} deg')
    for tag, s, i in (('orig', o, orl), ('port', p, prl)):
        for k in (-1, 0):
            e, a = s[i + k][1], s[i + k][2]
            print(f'    {tag} k={k:2d} eye ({e[0]:.3f},{e[1]:.3f},{e[2]:.3f}) at ({a[0]:.3f},{a[1]:.3f},{a[2]:.3f})')


if __name__ == '__main__':
    main()
