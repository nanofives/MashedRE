#!/usr/bin/env python3
"""n, MEDIAN SPEED and MEDIAN FRAME INDEX for the three scored D2 metrics.

STANDING TOOL. D2 attempt 14 computed this in a scratchpad one-off that is now gone,
so attempt 15 had to rebuild it; this file exists so that does not happen a third time.

WHY IT EXISTS. A speed-banded cross-side table is not matched on TIME. If the two arms
traverse a band at different points in their run, any time-ramping quantity shows a
reproducible cross-side deficit that is pure artefact (memory
`band-on-speed-compares-different-moments`; D2_REOPEN_2026-09-29.md section 26.10 turned
that into a standing guard). Every scored row therefore has to be printed with its
population's median frame index next to the value.

It does NOT recompute the scored values -- a8_slip_axis.py and a8_momentum.py remain
authoritative for those. It reuses THEIR row builders and THEIR filters, so the
populations reported here are the populations those tools score, by construction.

Usage:
  py -3.12 re/tools/statediff/a8_medframe.py --orig <orig>.msd --port <dir>/motion_diag.log
        [--max-lines 1080] [--orig-release 886] [--port-release 1]

`d` is the frame index relative to the release frame (the first frame the car is
commanded to move), which is the only index comparable across the two arms.
"""
import argparse, math, os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import a8_momentum as m      # noqa: E402
import a8_slip_axis as sa    # noqa: E402


def med(v):
    v = sorted(v)
    return v[len(v) // 2] if v else float('nan')


def report(label, rows, rel):
    if not rows:
        print(f"  {label:<34} n={0:4d}  median speed {'nan':>9}  "
              f"median frame {'nan':>6} (median d {'nan':>6})")
        return
    sp = med([r['sp'] for r in rows])
    fr = med([r['i'] for r in rows])
    print(f"  {label:<34} n={len(rows):4d}  median speed {sp:9.2f}  "
          f"median frame {fr:6.0f} (median d {fr - rel:6.0f})")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--orig', required=True)
    ap.add_argument('--port', required=True)
    ap.add_argument('--max-lines', type=int, default=1080)
    ap.add_argument('--orig-release', type=int, default=886)
    ap.add_argument('--port-release', type=int, default=1)
    ap.add_argument('--orig-steer-min', type=float, default=33.0)
    ap.add_argument('--port-steer-min', type=float, default=0.9)
    ap.add_argument('--drive-min-speed', type=float, default=500.0)
    a = ap.parse_args()

    print("=== SCORED METRICS with n, median speed AND median frame index ===")

    # --- the two slip bands: a8_slip_axis's own rows and its own BANDS ----------
    orows = sa.orig_rows(a.orig)
    prows, _ = sa.port_rows(a.port, a.max_lines)
    for lo, hi in sa.BANDS:
        report(f"ORIG slip {lo}-{hi}", [r for r in orows if lo <= r['sp'] < hi], a.orig_release)
        report(f"PORT slip {lo}-{hi}", [r for r in prows if lo <= r['sp'] < hi], a.port_release)

    # --- driving-median: a8_momentum's own rows and its own driving filter ------
    # a8_momentum.py:236 selects horiz >= min_speed AND steer >= steer_min, then takes
    # the median of horiz. Mirrored here on the same row builders.
    om = m.samples_original(a.orig)
    pm = m.samples_port(a.port, a.max_lines)
    od = [dict(i=r['idx'], sp=r['horiz']) for r in om
          if r['horiz'] >= a.drive_min_speed and r['steer'] >= a.orig_steer_min]
    # the port side indexes by LINE number, not frame ordinal -> renumber in order
    pd = [dict(i=k, sp=r['horiz']) for k, r in enumerate(pm)
          if r['horiz'] >= a.drive_min_speed and r['steer'] >= a.port_steer_min]
    report(f"ORIG driving-median (horiz>={a.drive_min_speed:.0f})", od, a.orig_release)
    report(f"PORT driving-median (horiz>={a.drive_min_speed:.0f})", pd, a.port_release)

    print()
    print(f"  R_orig={a.orig_release}  R_port={a.port_release}")


if __name__ == '__main__':
    main()
