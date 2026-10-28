# L-42 flip re-score on the flipped DEFAULT build (no env = knob ON, NO = MASHED_NO_TEXANIM=1).
# Same metric as measure2.py. Mask: pose cases |MASK - NO| > 6, forest_s8 |DEF - NO| > 6 (HB-FOREST-WATER W2).
# Also checks DEF == round-2 pre-flip ANIM where both exist (expected different only by the merged integration code).
# usage: py -3.12 measure_flip.py <render3_dir> [<render2_dir>]
import json, sys
from pathlib import Path
import numpy as np
from PIL import Image

W = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
REFS2 = Path("D:/MashedWT/HB-TEXANIM-REFS_data/orig2")


def rgb(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)


def luma(a):
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def main():
    rd = Path(sys.argv[1])
    r2 = Path(sys.argv[2]) if len(sys.argv) > 2 else None
    shot = lambda d, case, arm: d / f"{case}_{arm}" / "race1" / "01_grid.bmp"
    cases = [("forest_s8", W / "verify/water_refs/forest_s8/orig.bmp")] + \
            [(c, REFS2 / c / "orig.bmp") for c in json.loads((HERE / "poses_r2.json").read_text())]
    print("| case | % frame W | original L | OFF (NO_TEXANIM) L | ON (default) L | gap OFF | gap ON | toward |")
    print("|---|---|---|---|---|---|---|---|")
    for case, ref in cases:
        p_def, p_no = shot(rd, case, "DEF"), shot(rd, case, "NO")
        p_m = p_def if case == "forest_s8" else shot(rd, case, "MASK")
        if not all(p.exists() for p in (ref, p_def, p_no, p_m)):
            print(f"| {case} | missing | | | | | | |")
            continue
        o, d, n = luma(rgb(ref)), luma(rgb(p_def)), luma(rgb(p_no))
        m = np.abs(luma(rgb(p_m)) - n) > 6
        a, off, on = o[m].mean(), n[m].mean(), d[m].mean()
        print(f"| {case} | {m.mean() * 100:.2f} | {a:.1f} | {off:.1f} | {on:.1f} | {abs(off - a):.1f} | "
              f"{abs(on - a):.1f} | {'yes' if abs(on - a) < abs(off - a) else 'NO'} |")
    if r2:
        print()
        print("DEF (flipped build) vs round-2 ANIM (pre-flip, pre-merge exe 9f2afe92), bit-identical:")
        for case, _ in cases[1:]:
            a, b = shot(rd, case, "DEF"), shot(r2, case, "ANIM")
            if a.exists() and b.exists():
                x, y = rgb(a), rgb(b)
                print(f"  {case}: {bool((x == y).all())} (pixels differing {(np.abs(x - y).max(-1) > 0).mean() * 100:.3f}%)")


if __name__ == "__main__":
    main()
