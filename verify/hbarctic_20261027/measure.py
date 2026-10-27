# HB-ARCTIC metric (PREREG.md section 3). Luma 0.299R+0.587G+0.114B.
# Water mask M = |L_SEA - L_OFF| > 6. Prints one table row per case plus the R0 control.
# usage: py -3.12 measure.py <render_dir>
import sys
from pathlib import Path
import numpy as np
from PIL import Image

W = Path(__file__).resolve().parents[2]
REF = {
    "arctic_s8":  W / "verify/arctic_ref/sea_search/s8/a.bmp",
    "arctic_s14": W / "verify/arctic_ref/sea_search/s14/a.bmp",
    "superg_s14": W / "verify/water_refs/superg_s14/orig.bmp",
    "forest_s8":  W / "verify/water_refs/forest_s8/orig.bmp",
    "forest_s14": W / "verify/water_refs/forest_s14/orig.bmp",
}
SCORED = ("arctic_s8", "arctic_s14", "superg_s14")


def luma(p):
    a = np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def fogline(d):
    p = d / "librw_race.txt"
    if not p.exists():
        return "no librw_race.txt"
    out = [ln.strip() for ln in p.read_text(errors="replace").splitlines()
           if "P7 fogcolor" in ln or "P7 fogconst" in ln]
    return " | ".join(out[:2]) or "no P7 fog lines"


def main():
    rd = Path(sys.argv[1])
    shot = lambda c, a: rd / f"{c}_{a}" / "race1" / "01_grid.bmp"
    print("| pose | mask % | original | OFF | SEA | dOFF | dSEA | scored |")
    print("|---|---|---|---|---|---|---|---|")
    res = {}
    for c, ref in REF.items():
        if not (shot(c, "OFF").exists() and shot(c, "SEA").exists() and ref.exists()):
            print(f"| {c} | missing | | | | | | |")
            continue
        o, a, b = luma(ref), luma(shot(c, "OFF")), luma(shot(c, "SEA"))
        m = np.abs(b - a) > 6
        pct = 100.0 * m.mean()
        if m.sum() == 0:
            print(f"| {c} | 0.00 | | | | | | {'yes' if c in SCORED else 'no'} |")
            continue
        lo, la, lb = o[m].mean(), a[m].mean(), b[m].mean()
        res[c] = (pct, lo, la, lb)
        print(f"| {c} | {pct:.2f} | {lo:.1f} | {la:.1f} | {lb:.1f} | {abs(la - lo):.1f} | "
              f"{abs(lb - lo):.1f} | {'yes' if c in SCORED else 'no'} |")
    print()
    if shot("arctic_s8", "OFF2").exists() and shot("arctic_s8", "OFF").exists():
        a, a2 = luma(shot("arctic_s8", "OFF")), luma(shot("arctic_s8", "OFF2"))
        b = luma(shot("arctic_s8", "SEA"))
        m = np.abs(b - a) > 6
        print(f"R0 control arctic_s8 OFF vs OFF2: frame mean|dL|={np.abs(a - a2).mean():.3f} "
              f"mask mean|dL|={np.abs(a - a2)[m].mean():.3f} "
              f"pixels differing={(np.abs(a - a2) > 0).mean() * 100:.3f}%")
    print()
    for c in REF:
        for arm in ("OFF", "SEA"):
            d = rd / f"{c}_{arm}"
            if d.exists():
                print(f"{c}_{arm}: {fogline(d)}")


if __name__ == "__main__":
    main()
