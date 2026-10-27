# HB-ARCTIC-OPENSEA metric, same as verify/hbarctic_20261027/measure.py (section 2) and
# posthoc_oldmask.py (section 3). Luma 0.299R+0.587G+0.114B. Water mask M = |L_SEA - L_OFF| > 6
# on the standalone pair at the transplanted pose. Reports original vs default (OFF) mean luma
# over M, |d|, the 4.5 bar from RESULT_HB-ARCTIC.md section 3, and whole-frame mean |L_orig - L_OFF|.
# usage: py -3.12 measure.py <datadir> <pose> [...]
import sys
from pathlib import Path
import numpy as np
from PIL import Image

BAR = 4.5


def luma(p):
    a = np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def main():
    d = Path(sys.argv[1])
    shot = lambda p, a: d / "render" / f"{p}_{a}" / "race1" / "01_grid.bmp"
    print("| pose | water mask % | original | default (OFF) | SEA fold | d default | bar 4.5 | frame mean abs dL |")
    print("|---|---|---|---|---|---|---|---|")
    for p in sys.argv[2:]:
        o, a, b = luma(d / "orig" / p / "a.bmp"), luma(shot(p, "OFF")), luma(shot(p, "SEA"))
        m = np.abs(b - a) > 6
        fr = np.abs(o - a).mean()
        if not m.any():
            print(f"| {p} | 0.00 | | | | | | {fr:.1f} |")
            continue
        lo, la, lb = o[m].mean(), a[m].mean(), b[m].mean()
        dd = abs(la - lo)
        print(f"| {p} | {100 * m.mean():.2f} | {lo:.1f} | {la:.1f} | {lb:.1f} | {dd:.1f} | "
              f"{'PASS' if dd <= BAR else 'FAIL'} | {fr:.1f} |")
        if shot(p, "OFF2").exists():
            a2 = luma(shot(p, "OFF2"))
            print(f"|   control {p} OFF vs OFF2 | frame mean abs dL {np.abs(a - a2).mean():.3f} | "
                  f"pixels differing {(np.abs(a - a2) > 0).mean() * 100:.3f}% | | | | | |")


if __name__ == "__main__":
    main()
