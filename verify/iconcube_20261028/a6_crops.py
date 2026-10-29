r"""D5-ICONCUBE-GLOW arm A6: where the knob-ON frame differs from the default one, and crops of it.

    py -3.12 verify\iconcube_20261028\a6_crops.py <on.bmp> <off.bmp> <out_dir> [--orig <orig.bmp>] [--tag NAME]

Diffs the two standalone frames of one replayed row (run13.py, same pose), groups the differing pixels into
connected boxes (8-neighbour, boxes closer than 6 px merged), prints each box, and writes <tag>_on.png /
<tag>_off.png / <tag>_orig.png crops of the union box (padded 24 px, scaled x3) for the RESULT. The original's
BMP is a frame of the same row from the on-disk capture, cropped at the same rectangle: a reading aid, not a
pixel comparison (the pool state is not matched).
"""
import argparse
import sys
from pathlib import Path

import numpy as np
from PIL import Image


def boxes(mask):
    h, w = mask.shape
    seen = np.zeros_like(mask, dtype=bool)
    out = []
    ys, xs = np.nonzero(mask)
    for y0, x0 in zip(ys, xs):
        if seen[y0, x0]:
            continue
        st = [(y0, x0)]; seen[y0, x0] = True
        x1 = x2 = x0; y1 = y2 = y0; n = 0
        while st:
            y, x = st.pop(); n += 1
            x1, x2, y1, y2 = min(x1, x), max(x2, x), min(y1, y), max(y2, y)
            for dy in (-1, 0, 1):
                for dx in (-1, 0, 1):
                    yy, xx = y + dy, x + dx
                    if 0 <= yy < h and 0 <= xx < w and mask[yy, xx] and not seen[yy, xx]:
                        seen[yy, xx] = True; st.append((yy, xx))
        out.append([x1, y1, x2, y2, n])
    merged = True
    while merged:
        merged = False
        for i in range(len(out)):
            for j in range(i + 1, len(out)):
                a, b = out[i], out[j]
                if a[0] - 6 <= b[2] and b[0] - 6 <= a[2] and a[1] - 6 <= b[3] and b[1] - 6 <= a[3]:
                    out[i] = [min(a[0], b[0]), min(a[1], b[1]), max(a[2], b[2]), max(a[3], b[3]), a[4] + b[4]]
                    del out[j]; merged = True; break
            if merged:
                break
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("on"); ap.add_argument("off"); ap.add_argument("out")
    ap.add_argument("--orig"); ap.add_argument("--tag", default="row")
    a = ap.parse_args()
    on = np.asarray(Image.open(a.on).convert("RGB")).astype(int)
    off = np.asarray(Image.open(a.off).convert("RGB")).astype(int)
    if on.shape != off.shape:
        print(f"size mismatch {on.shape} vs {off.shape}"); return 1
    mask = np.abs(on - off).max(axis=2) > 0
    n = int(mask.sum())
    print(f"{a.tag}: {on.shape[1]}x{on.shape[0]}  differing pixels {n}")
    if n == 0:
        return 0
    bs = boxes(mask)
    for b in sorted(bs, key=lambda b: -b[4]):
        print(f"  box x {b[0]}..{b[2]} y {b[1]}..{b[3]}  pixels {b[4]}")
    x1 = max(0, min(b[0] for b in bs) - 24); y1 = max(0, min(b[1] for b in bs) - 24)
    x2 = min(on.shape[1], max(b[2] for b in bs) + 25); y2 = min(on.shape[0], max(b[3] for b in bs) + 25)
    print(f"  crop x {x1}..{x2} y {y1}..{y2}")
    out = Path(a.out); out.mkdir(parents=True, exist_ok=True)
    srcs = [("on", a.on), ("off", a.off)] + ([("orig", a.orig)] if a.orig else [])
    for nm, p in srcs:
        im = Image.open(p).convert("RGB").crop((x1, y1, x2, y2))
        im = im.resize((im.width * 3, im.height * 3), Image.NEAREST)
        im.save(out / f"{a.tag}_{nm}.png", optimize=True)
    return 0


if __name__ == "__main__":
    sys.exit(main())
