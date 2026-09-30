# Car-box HULL-GREY vs PAINT pixel counter, for CAR_GRAY_CHASSIS acceptance G2.
#
# The defect (re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md M2/M3) is that four
# untextured car-sized hull atomics with material colour (102,102,102) and
# 16-24 one-triangle locators are drawn over the painted body. Their pixels are
# ACHROMATIC: the hull material has R==G==B==102, and every lighting term in
# both renderers is a per-channel scale by the same lit-vertex colour (white
# sun, grey ambient), so a hull pixel stays achromatic whatever the light.
# Its luma therefore lands in 102 x [ambient .. 1.0] = [51 .. 102] pre- or
# post-brightness-fix.  The locator spikes are the same material, seen edge-on,
# so they land darker.
#
# GREY selector : max(R,G,B) - min(R,G,B) <= --sat (default 14)
#                 and luma in [--lo, --hi] (default 30..130)
# PAINT selector: the one verify/car_bright_20260930/body_ratio.py uses --
#                 R >= 32 and R > 1.6*G and R > 1.6*B -- so the two counts are
#                 directly comparable with the brightness child's numbers.
#
# Reported per box: n_box, n_grey, n_paint, grey_frac = n_grey/(n_grey+n_paint),
# and the dominant colour of each class with its own n. grey_frac is the G2
# statistic: it is a RATIO of two car-surface classes, so it does not depend on
# how much terrain the box happens to include, which is what makes it
# comparable across two sides whose cameras differ.
#
# Usage: py -3.12 .../gray_frac.py <label>=<img>:<x0,y0,x1,y1> [...] [--sat N]
#                                  [--lo N] [--hi N] [--mask out.png]
import argparse
import collections
import sys

from PIL import Image

TEXEL = (236.0, 52.0, 60.0)   # Advantage paint swatch, CAR_GRAY_CHASSIS M5


def luma(p):
    return 0.299 * p[0] + 0.587 * p[1] + 0.114 * p[2]


def is_grey(p, sat, lo, hi):
    return (max(p) - min(p)) <= sat and lo <= luma(p) <= hi


def is_paint(p):
    return p[0] >= 32 and p[0] > p[1] * 1.6 and p[0] > p[2] * 1.6


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("specs", nargs="+")
    ap.add_argument("--sat", type=int, default=14)
    ap.add_argument("--lo", type=int, default=30)
    ap.add_argument("--hi", type=int, default=130)
    ap.add_argument("--mask")
    a = ap.parse_args()
    for spec in a.specs:
        label, _, rest = spec.partition("=")
        path, _, bs = rest.rpartition(":")
        box = tuple(int(v) for v in bs.split(","))
        im = Image.open(path).convert("RGB").crop(box)
        px = list(im.getdata())
        grey = [p for p in px if is_grey(p, a.sat, a.lo, a.hi)]
        paint = [p for p in px if is_paint(p)]
        ng, np_ = len(grey), len(paint)
        den = ng + np_
        frac = (ng / den) if den else float("nan")
        gd = collections.Counter(grey).most_common(1)
        pd = collections.Counter(paint).most_common(1)
        print(f"{label:<26} box={bs:<20} n_box={len(px):6d} "
              f"n_grey={ng:6d} n_paint={np_:6d} grey_frac={frac:.4f}")
        if gd:
            c, k = gd[0]
            print(f"   grey dominant  = {c} n={k} luma={luma(c):.1f}")
        if pd:
            c, k = pd[0]
            print(f"   paint dominant = {c} n={k} "
                  f"ratio/texel=({c[0]/TEXEL[0]:.4f},{c[1]/TEXEL[1]:.4f},"
                  f"{c[2]/TEXEL[2]:.4f})")
        if a.mask:
            m = Image.new("RGB", im.size, (0, 0, 0))
            m.putdata([(255, 0, 255) if is_grey(p, a.sat, a.lo, a.hi)
                       else ((0, 255, 0) if is_paint(p) else (0, 0, 0))
                       for p in px])
            m.save(a.mask)
            print(f"   mask -> {a.mask}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
