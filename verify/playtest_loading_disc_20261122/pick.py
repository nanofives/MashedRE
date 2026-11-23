"""Pick splash frames by disc width and side; compare disc crops against the original's frames.

For each capture: bounding box of the disc (x 470..630, y 30..130 above threshold), plus the
mean saturation of the disc region (0 for a white/grey silhouette, >0 for the coloured texture).
"""
import glob
import os
import sys
from PIL import Image


def stats(path):
    im = Image.open(path).convert('RGB')
    pts = []
    sat = 0.0
    for x in range(470, 630):
        for y in range(40, 122):
            r, g, b = im.getpixel((x, y))
            if r + g + b > 60:
                pts.append((x, y))
                sat += max(r, g, b) - min(r, g, b)
    if not pts:
        return None
    xs = [p[0] for p in pts]
    # splash text present? (copyright block)
    txt = sum(1 for x in range(60, 580, 4) for y in range(170, 290, 4) if sum(im.getpixel((x, y))) > 300)
    return min(xs), max(xs), len(pts), sat / len(pts), txt


for pat in sys.argv[1:]:
    for f in sorted(glob.glob(pat)):
        try:
            s = stats(f)
        except OSError as e:
            s = 'ERR %s' % e
        print(os.path.basename(f), s)
