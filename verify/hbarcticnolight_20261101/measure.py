# HB-ARCTIC-NOLIGHT-CAP metric. Luma 0.299R+0.587G+0.114B. Water mask M = |L_SEA - L_OFF| > 6 on the
# HB-ARCTIC-OPENSEA standalone pair at the same pinned pose (<maskdir>/render/<pose>_{SEA,OFF}); the
# SEA knob that made that pair was deleted by HB-ARCTIC-FOLDRM, so the mask is reused, not remade.
# The pose basis of this step's capture must equal OPENSEA's (checked and printed).
# Scores per pose: original (this capture) vs DEF and vs NL4 over M, the 4.5 bar of
# verify/hbarctic_20261027/RESULT_HB-ARCTIC.md section 3, whole-frame mean |dL|, DEF vs NL4 and DEF vs
# DEF2 pixel counts, and the OPENSEA original over M (capture-to-capture repeatability).
# usage: py -3.12 measure.py <datadir> <maskdir> <pose> [...]
import sys
from pathlib import Path
import numpy as np
from PIL import Image

BAR = 4.5


def luma(p):
    a = np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def main():
    d, md = Path(sys.argv[1]), Path(sys.argv[2])
    shot = lambda root, p, a: root / "render" / f"{p}_{a}" / "race1" / "01_grid.bmp"
    print("| pose | basis = OPENSEA | water mask % | original | OPENSEA original | DEF | NL4 | d DEF | d NL4 | bar 4.5 DEF / NL4 | frame mean abs dL DEF / NL4 | DEF vs NL4 px differ % | DEF vs DEF2 px differ % |")
    print("|---|---|---|---|---|---|---|---|---|---|---|---|---|")
    for p in sys.argv[3:]:
        same = (d / "orig" / p / "orig_cambasis.txt").read_text().strip() == (md / "orig" / p / "orig_cambasis.txt").read_text().strip()
        o, o_old = luma(d / "orig" / p / "a.bmp"), luma(md / "orig" / p / "a.bmp")
        a, n = luma(shot(d, p, "DEF")), luma(shot(d, p, "NL4"))
        m = np.abs(luma(shot(md, p, "SEA")) - luma(shot(md, p, "OFF"))) > 6
        lo, lold, la, ln = o[m].mean(), o_old[m].mean(), a[m].mean(), n[m].mean()
        da, dn = abs(la - lo), abs(ln - lo)
        an = (np.abs(a - n) > 0).mean() * 100
        a2 = f"{(np.abs(a - luma(shot(d, p, 'DEF2'))) > 0).mean() * 100:.3f}" if shot(d, p, "DEF2").exists() else "-"
        print(f"| {p} | {'yes' if same else 'NO'} | {100 * m.mean():.2f} | {lo:.1f} | {lold:.1f} | {la:.1f} | {ln:.1f} | "
              f"{da:.1f} | {dn:.1f} | {'PASS' if da <= BAR else 'FAIL'} / {'PASS' if dn <= BAR else 'FAIL'} | "
              f"{np.abs(o - a).mean():.1f} / {np.abs(o - n).mean():.1f} | {an:.3f} | {a2} |")


if __name__ == "__main__":
    main()
