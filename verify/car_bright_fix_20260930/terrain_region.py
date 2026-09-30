# A4 "against the original": pose-tolerant terrain REGION statistic.
#
# A pixel diff against the original is not usable here -- the original-side
# reference frame (orig_train.bmp, race_draw_burst --settle 4.0 --mode-sel 1) is
# a different camera at a different roll, and CAR_BRIGHTNESS_2026-09-30.md's own
# addendum measured that comparison at mean abs 57.83 dominated by window/pose,
# not shading. So the cross-side channel is a REGION AGGREGATE over a box that
# is road/terrain on the side being measured: mean and median RGB, which is what
# race_terrain_ambient_20260830.md's "band aggregate" is.
#
# Usage: py -3.12 .../terrain_region.py <label>=<img>:<x0,y0,x1,y1> [...]
import sys
from PIL import Image


def main():
    for spec in sys.argv[1:]:
        label, _, rest = spec.partition("=")
        path, _, bs = rest.rpartition(":")
        box = tuple(int(v) for v in bs.split(","))
        im = Image.open(path).convert("RGB").crop(box)
        px = list(im.getdata())
        n = len(px)
        mr = sum(p[0] for p in px) / n
        mg = sum(p[1] for p in px) / n
        mb = sum(p[2] for p in px) / n
        med = [sorted(p[c] for p in px)[n // 2] for c in range(3)]
        print(f"{label:<22} box={bs:<20} n={n:6d} "
              f"mean=({mr:7.3f},{mg:7.3f},{mb:7.3f}) "
              f"median=({med[0]:3d},{med[1]:3d},{med[2]:3d}) "
              f"luma={0.299*mr+0.587*mg+0.114*mb:7.3f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
