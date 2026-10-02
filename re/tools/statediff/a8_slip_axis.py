#!/usr/bin/env python3
"""A8: slip angle measured against the WHEEL-AXIS heading (the basis the tire model
actually uses, p[0x1f..0x21] of a rear wheel) on both sides, per speed band, plus the
axis-vs-forward phase offset and av.y. This is the like-for-like slip: in the original's
render-tick snapshot the wheel axes are one frame older than +0x9d4 (A8 twenty-sixth
follow-up), so slip vs +0x9d4 and slip vs the axes differ by one frame of rotation.

Usage: py -3.12 re/tools/statediff/a8_slip_axis.py --orig <.msd> --port <motion_diag.log> [more --port ...]
Regime: speed >= 1500, full lock (orig steer0 >= 33 deg, port steer >= 0.9), all grounded,
port reseeds dropped and spike frames (any wheel |F| > 3x its median) +-30 excluded.
Read-only. Does not execute the game.
"""
import argparse, math, re, struct, sys, os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m  # noqa: E402

BANDS = ((1500, 2000), (2000, 2600))


def wrap(a):
    while a > math.pi: a -= 2 * math.pi
    while a < -math.pi: a += 2 * math.pi
    return a


def med(v):
    v = sorted(v); return v[len(v) // 2] if v else float('nan')


def orig_rows(path):
    _, _, frames = m.load_msd(path); f = lambda p, o: struct.unpack_from('<f', p, o)[0]
    out = []
    for idx in sorted(frames):
        p = frames[idx]
        if f(p, 0x9e4) < 1500 or f(p, 0x1a8) < 33.0 or f(p, 0x9e0) < 3.5: continue
        b2 = 0x1a4 + 2 * 0xc4
        fwdH = math.atan2(f(p, 0x9dc), f(p, 0x9d4)); axH = math.atan2(f(p, b2 + 0x84), f(p, b2 + 0x7c)); velH = math.atan2(f(p, 0x9b8), f(p, 0x9b0))
        # i = the capture's own frame index, carried so a8_medframe.py can report
        # the MEDIAN FRAME of every scored band (memory band-on-speed-compares-
        # different-moments). Purely additive; no existing reader looks at it.
        out.append(dict(i=idx, sp=f(p, 0x9e4), s_fwd=abs(wrap(velH - fwdH)), s_ax=abs(wrap(velH - axH)), off=wrap(axH - fwdH), avy=f(p, 0x9c0)))
    return out


RE = re.compile(r"reseed=(\d).*?steer=([-+\d.]+) gnd=([\d.]+) sp=([\d.]+) horiz=([\d.]+) velH=([-\d.]+) bodyH=([-\d.]+).*?av=\(([-\d.e+]+),([-\d.e+]+),([-\d.e+]+)\) wf=\[([^\]]*)\] wax=\[([^\]]*)\]")


def port_rows(path, max_lines=0):
    """max_lines: keep only the first N logged FRAMES (0 = all).

    [U-9141 2026-09-29] The D2 gate's controlled arm. D3_DRIVE_2026-09-28.md §3.5 measured
    that the recipe's race LENGTH is not constant across commits (1083 frames at 56ad3806,
    1623 at 09a73dc6) even though the wall clock is fixed at 50 s, and §3.6 showed a longer
    race repopulates the speed bands the D2 table is made of. Truncating both port ends to
    the same frame count removes that variable.

    Truncation is applied to the RAW frames, before the regime filter and before the spike
    median is computed, so the spike exclusion is also computed on the same span on both
    ends rather than on a median taken over a longer run.

    Frame count == simulated time here: the standalone's chain dt is pinned (frameMs = 50,
    i.e. 1/60 s), measured as a single distinct `linTerm=1.66667e-05` over all 3596 samples
    of verify/d3_force_20260929/cad1/friction_diag.log. So this is a fixed-duration window,
    not just a fixed row count.
    """
    rows = []
    for line in open(path, errors='replace'):
        mm = RE.search(line)
        if not mm: continue
        if max_lines and len(rows) >= max_lines: break
        wf = [float(x) for x in mm[11].split(',')]; wax = [float(x) for x in mm[12].split(',')]
        rows.append(dict(rs=int(mm[1]), steer=float(mm[2]), gnd=float(mm[3]), sp=float(mm[5]), velH=float(mm[6]), bodyH=float(mm[7]), avy=float(mm[9]),
                         fm=[math.hypot(wf[2 * w], wf[2 * w + 1]) for w in range(4)], axH=math.atan2(wax[5], wax[4])))
    medf = [med([r['fm'][w] for r in rows if r['sp'] >= 1500]) for w in range(4)]
    bad = set()
    for k, r in enumerate(rows):
        if any(r['fm'][w] > 3 * medf[w] for w in range(4)):
            for j in range(max(0, k - 30), min(len(rows), k + 31)): bad.add(j)
    out = []
    for k, r in enumerate(rows):
        if k in bad or r['rs'] or r['sp'] < 1500 or r['steer'] < 0.9 or r['gnd'] < 3.5: continue
        out.append(dict(i=k, sp=r['sp'], s_fwd=abs(wrap(r['velH'] - r['bodyH'])), s_ax=abs(wrap(r['velH'] - r['axH'])), off=wrap(r['axH'] - r['bodyH']), avy=r['avy']))
    return out, len(bad)


TBINS = ((0, 60), (60, 120), (120, 180), (180, 240), (240, 360), (360, 540), (540, 900))


def transient(rows, label):
    """[U-9147 2026-09-29] Slip and speed against TIME SINCE THE STEER-HOLD ONSET,
    instead of against speed.

    Why this and not the speed bands. With the 100/105 body-yaw fix in,
    `d(bodyH)/frame` is exact on both sides, and yet `|d(velH)| < |d(bodyH)|` on BOTH --
    i.e. neither car is in steady state inside the scored window, both are still building
    slip. A speed-banded median then mixes two different things: where the car sits on
    the slip curve, and how fast it got there. The band populations already differ (port
    n=227 vs original n=312 at 1500-2000), which is the tell.

    Onset = the first row whose steer passes the per-side threshold. Rows are counted in
    LOGGED FRAMES from there, which is the same unit on both sides (the chain dt is
    pinned at frameMs = 50 in the standalone, and the original's +0x494 steps by exactly
    -50 per frame -- both A6a calls are one 50 ms budget).

    Read it this way: if the port's slip-vs-elapsed curve LIES ON the original's and
    simply has not got as far, the defect is in how fast the car reaches the regime
    (acceleration / RecoverOffMesh), not in the cornering law. If the curves differ at
    the same elapsed frame, the cornering law is still short.
    """
    if not rows:
        print(f"=== TRANSIENT {label}: no rows ===")
        return
    print(f"=== TRANSIENT {label}: {len(rows)} rows from onset ===")
    print(f"  {'frames':<10} {'n':>4} {'slip vs fwd':>12} {'slip vs AXIS':>13} "
          f"{'speed':>9} {'av.y':>7}")
    for lo, hi in TBINS:
        s = [r for r in rows if lo <= r['t'] < hi]
        if len(s) < 5:
            continue
        print(f"  {f'{lo}-{hi}':<10} {len(s):>4} {med([r['s_fwd'] for r in s]):>12.4f} "
              f"{med([r['s_ax'] for r in s]):>13.4f} {med([r['sp'] for r in s]):>9.1f} "
              f"{med([r['avy'] for r in s]):>+7.3f}")


def orig_trans_rows(path):
    """[U-9147] every grounded frame from the steer-hold onset, with `t` = frames since
    onset. NO speed filter -- the point is to see the whole spin-up, including the part
    the 1500 cut-off hides."""
    _, _, frames = m.load_msd(path); f = lambda p, o: struct.unpack_from('<f', p, o)[0]
    idxs = sorted(frames)
    onset = None
    for k, idx in enumerate(idxs):
        if f(frames[idx], 0x1a8) >= 33.0:
            onset = k; break
    if onset is None:
        return []
    out = []
    for k in range(onset, len(idxs)):
        p = frames[idxs[k]]
        if f(p, 0x9e0) < 3.5:
            continue
        b2 = 0x1a4 + 2 * 0xc4
        fwdH = math.atan2(f(p, 0x9dc), f(p, 0x9d4))
        axH = math.atan2(f(p, b2 + 0x84), f(p, b2 + 0x7c))
        velH = math.atan2(f(p, 0x9b8), f(p, 0x9b0))
        out.append(dict(t=k - onset, sp=f(p, 0x9e4), s_fwd=abs(wrap(velH - fwdH)),
                        s_ax=abs(wrap(velH - axH)), avy=f(p, 0x9c0)))
    return out


def port_trans_rows(path):
    """[U-9147] the port side of the same thing."""
    rows = []
    for line in open(path, errors='replace'):
        mm = RE.search(line)
        if not mm:
            continue
        wax = [float(x) for x in mm[12].split(',')]
        rows.append(dict(rs=int(mm[1]), steer=float(mm[2]), gnd=float(mm[3]),
                         sp=float(mm[5]), velH=float(mm[6]), bodyH=float(mm[7]),
                         avy=float(mm[9]), axH=math.atan2(wax[5], wax[4])))
    onset = next((k for k, r in enumerate(rows) if r['steer'] >= 0.9), None)
    if onset is None:
        return []
    out = []
    for k in range(onset, len(rows)):
        r = rows[k]
        if r['rs'] or r['gnd'] < 3.5:
            continue
        out.append(dict(t=k - onset, sp=r['sp'], s_fwd=abs(wrap(r['velH'] - r['bodyH'])),
                        s_ax=abs(wrap(r['velH'] - r['axH'])), avy=r['avy']))
    return out


def reseed_shadow(path, max_lines, drops):
    """[U-9147 2026-09-29] DIAGNOSTIC ONLY -- NOT THE D2 GATE.

    TrackRenderer::RecoverOffMesh (TrackRenderer.cpp:2142-2164) does three things on every
    fire: it halves car_speed_, it sets car_yaw_ to the recovered heading, and it writes
    car_vel_ = (cos ry, sin ry) * sp -- i.e. it puts the velocity EXACTLY along the body
    heading, so the slip angle this file measures is set to ZERO. It also calls
    VehiclePhysics_ResetOrientation, which raises g_bodyBasisReseed and therefore shows up
    as `reseed=1`. port_rows() already drops that ONE frame; the frames after it, where
    slip is climbing back from zero, are all scored.

    This prints the scored medians with an additional N-frame shadow dropped after each
    reseed, for several N. It is a measurement of how much of the slip deficit the
    recoveries account for -- it is NOT a re-score. The pre-registered rule
    (re/analysis/D2_REOPEN_2026-09-29.md section 3) fixes the reducer, and the scored
    numbers stay whatever report() says.

    n and the median speed are printed for every row on purpose: dropping frames moves
    the regime, and a band scored off-regime is not a measurement.
    """
    rows = []
    for line in open(path, errors='replace'):
        mm = RE.search(line)
        if not mm:
            continue
        if max_lines and len(rows) >= max_lines:
            break
        wf = [float(x) for x in mm[11].split(',')]
        wax = [float(x) for x in mm[12].split(',')]
        rows.append(dict(rs=int(mm[1]), steer=float(mm[2]), gnd=float(mm[3]),
                         sp=float(mm[5]), velH=float(mm[6]), bodyH=float(mm[7]),
                         avy=float(mm[9]),
                         fm=[math.hypot(wf[2 * w], wf[2 * w + 1]) for w in range(4)],
                         axH=math.atan2(wax[5], wax[4])))
    medf = [med([r['fm'][w] for r in rows if r['sp'] >= 1500]) for w in range(4)]
    spike = set()
    for k, r in enumerate(rows):
        if any(r['fm'][w] > 3 * medf[w] for w in range(4)):
            for j in range(max(0, k - 30), min(len(rows), k + 31)):
                spike.add(j)
    nres = sum(1 for r in rows if r['rs'])
    print(f"\n=== RESEED SHADOW (DIAGNOSTIC, not the gate) {path} ===")
    print(f"  {nres} reseed frames in {len(rows)} logged frames "
          f"(RecoverOffMesh zeroes slip on every one)")
    print(f"  {'drop':<6} {'1500-2000':>22} {'2000-2600':>22}")
    print(f"  {'':<6} {'n':>5} {'slip':>7} {'speed':>8} {'n':>5} {'slip':>7} {'speed':>8}")
    for d in drops:
        shadow = set()
        for k, r in enumerate(rows):
            if r['rs']:
                for j in range(k, min(len(rows), k + d + 1)):
                    shadow.add(j)
        sel = []
        for k, r in enumerate(rows):
            if k in spike or k in shadow or r['rs'] or r['sp'] < 1500 or r['steer'] < 0.9 or r['gnd'] < 3.5:
                continue
            sel.append(dict(sp=r['sp'], s=abs(wrap(r['velH'] - r['bodyH']))))
        cells = []
        for lo, hi in BANDS:
            b = [x for x in sel if lo <= x['sp'] < hi]
            if len(b) < 8:
                cells.append(f"{len(b):>5} {'-':>7} {'-':>8}")
            else:
                cells.append(f"{len(b):>5} {med([x['s'] for x in b]):>7.4f} "
                             f"{med([x['sp'] for x in b]):>8.1f}")
        print(f"  {d:<6} {cells[0]} {cells[1]}")


def report(rows, label):
    print(f"=== {label}: n={len(rows)}  axis-minus-forward offset median {med([r['off'] for r in rows]):+.4f} rad ===")
    for lo, hi in BANDS:
        s = [r for r in rows if lo <= r['sp'] < hi]
        if len(s) < 8: print(f"  {lo}-{hi}: n={len(s)}"); continue
        print(f"  {lo}-{hi}: n={len(s):>3}  slip vs fwd {med([r['s_fwd'] for r in s]):.4f}  slip vs AXIS {med([r['s_ax'] for r in s]):.4f}  av.y {med([r['avy'] for r in s]):+.3f}")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', default='verify/a8_steer_20260824/orig_steerR.msd')
    ap.add_argument('--port', action='append', default=[])
    ap.add_argument('--max-lines', type=int, default=0,
                    help='[U-9141] keep only the first N logged frames of each --port log '
                         '(0 = all). The D2 controlled arm; see port_rows(). The ORIGINAL '
                         'side is deliberately NOT truncated: its capture length is fixed '
                         'and archived, and it is the reference the D2 row was measured '
                         'against. What varies across commits is the PORT length.')
    ap.add_argument('--transient', action='store_true',
                    help='[U-9147] also print slip/speed against FRAMES SINCE THE '
                         'STEER-HOLD ONSET instead of against speed. See transient().')
    ap.add_argument('--reseed-shadow', action='store_true',
                    help='[U-9147] DIAGNOSTIC, not the gate: how much of the slip deficit '
                         'the RecoverOffMesh recoveries account for. See reseed_shadow().')
    a = ap.parse_args()
    report(orig_rows(a.orig), 'ORIGINAL ' + a.orig)
    for p in a.port:
        rows, nbad = port_rows(p, a.max_lines)
        tag = f' [first {a.max_lines} frames]' if a.max_lines else ''
        report(rows, f'PORT {p}{tag} (spike/reseed-excluded {nbad} rows)')
    for p in a.port:
        if a.reseed_shadow:
            reseed_shadow(p, a.max_lines, [0, 5, 10, 15, 30, 60])
    if a.transient:
        print()
        transient(orig_trans_rows(a.orig), 'ORIGINAL ' + a.orig)
        for p in a.port:
            transient(port_trans_rows(p), f'PORT {p}')


if __name__ == '__main__':
    main()
