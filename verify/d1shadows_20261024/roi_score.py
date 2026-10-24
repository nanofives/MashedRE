"""D1-SHADOWS ROI measures (PREREG_D1-SHADOWS.md): M1 = Pearson r of orig vs standalone grayscale in the road
ROI x 290..459, y 150..299 (640x480); M2 = standalone ROI mean luminance. Also writes side-by-sides.

    py -3.12 roi_score.py <orig_dir> <out_dir> <arm>=<stand_dir> [...] --rows 884 924 959
"""
import argparse
from pathlib import Path

import numpy as np
from PIL import Image

X0, X1, Y0, Y1 = 290, 460, 150, 300


def gray(p):
    a = np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)
    return 0.299 * a[..., 0] + 0.587 * a[..., 1] + 0.114 * a[..., 2]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("orig_dir"); ap.add_argument("out_dir"); ap.add_argument("arms", nargs="+")
    ap.add_argument("--rows", type=int, nargs="+", required=True)
    a = ap.parse_args()
    out = Path(a.out_dir); out.mkdir(parents=True, exist_ok=True)
    arms = [s.split("=", 1) for s in a.arms]
    print(f"ROI x {X0}..{X1 - 1} y {Y0}..{Y1 - 1}")
    print("row   arm      M1_r     M2_stand  M2_orig  meanabs_full")
    for k in a.rows:
        po = Path(a.orig_dir) / f"orig_row_{k:05d}.bmp"
        go = gray(po)
        ro = go[Y0:Y1, X0:X1].ravel()
        imgs = [Image.open(po).convert("RGB")]
        for name, d in arms:
            ps = Path(d) / f"stand_row_{k:05d}.bmp"
            gs = gray(ps)
            rs = gs[Y0:Y1, X0:X1].ravel()
            r = float(np.corrcoef(ro, rs)[0, 1])
            full = float(np.abs(np.asarray(Image.open(po).convert("RGB"), dtype=np.float64)
                                - np.asarray(Image.open(ps).convert("RGB"), dtype=np.float64)).mean())
            print(f"{k:5d} {name:8s} {r:8.4f} {rs.mean():9.2f} {ro.mean():8.2f} {full:10.2f}")
            imgs.append(Image.open(ps).convert("RGB"))
        w, h = imgs[0].size
        sbs = Image.new("RGB", (w * len(imgs), h))
        for i, im in enumerate(imgs):
            sbs.paste(im, (w * i, 0))
        sbs.save(out / f"sbs_row_{k:05d}_{'_'.join(n for n, _ in arms)}.jpg", quality=88)


if __name__ == "__main__":
    main()
