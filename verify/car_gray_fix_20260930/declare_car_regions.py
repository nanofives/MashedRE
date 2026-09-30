# G4a car-region declaration, run on the PRE-fix arm BEFORE the post-fix binary
# exists (CAR_GRAY_FIX_ACCEPTANCE_2026-09-30.md, rule G4a).
#
# Procedure, exactly as pre-registered: the bounding box of each connected
# component of the PRE-fix `GREY u PAINT` mask (the two classes
# gray_frac.py defines), dilated by 4 px.
#
# Why this is a sharp rule and not a judgement call: dropping the untextured
# hull/locator atomics can only UNCOVER pixels that the pre-fix car silhouette
# already covered, so every pixel the fix is allowed to change lies inside that
# silhouette. A differing pixel outside it is by construction not attributable
# to the fix.
#
# --min drops components below N px (single-pixel speckle on a compressed
# terrain edge is not a car); the dropped components are PRINTED, not hidden,
# so the reader can see what was excluded.
#
# Usage: py -3.12 .../declare_car_regions.py <img> [--min N] [--dilate N]
import argparse
import sys
from collections import deque

from PIL import Image

sys.path.insert(0, str(__import__("pathlib").Path(__file__).resolve().parent))
from gray_frac import is_grey, is_paint          # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("img")
    ap.add_argument("--min", type=int, default=40)
    ap.add_argument("--dilate", type=int, default=4)
    ap.add_argument("--sat", type=int, default=14)
    ap.add_argument("--lo", type=int, default=30)
    ap.add_argument("--hi", type=int, default=130)
    a = ap.parse_args()
    im = Image.open(a.img).convert("RGB")
    W, H = im.size
    px = im.load()
    mask = [[False] * W for _ in range(H)]
    for y in range(H):
        for x in range(W):
            p = px[x, y]
            mask[y][x] = is_grey(p, a.sat, a.lo, a.hi) or is_paint(p)
    seen = [[False] * W for _ in range(H)]
    comps = []
    for y in range(H):
        for x in range(W):
            if not mask[y][x] or seen[y][x]:
                continue
            q = deque([(x, y)])
            seen[y][x] = True
            x0 = x1 = x
            y0 = y1 = y
            n = 0
            while q:
                cx, cy = q.popleft()
                n += 1
                x0, x1 = min(x0, cx), max(x1, cx)
                y0, y1 = min(y0, cy), max(y1, cy)
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nx, ny = cx + dx, cy + dy
                    if 0 <= nx < W and 0 <= ny < H and mask[ny][nx] \
                            and not seen[ny][nx]:
                        seen[ny][nx] = True
                        q.append((nx, ny))
            comps.append((n, x0, y0, x1, y1))
    comps.sort(reverse=True)
    kept, drop = [], []
    for n, x0, y0, x1, y1 in comps:
        d = a.dilate
        box = (max(0, x0 - d), max(0, y0 - d),
               min(W, x1 + 1 + d), min(H, y1 + 1 + d))
        (kept if n >= a.min else drop).append((n, box))
    print(f"{a.img}  {W}x{H}  components={len(comps)} "
          f"kept>={a.min}: {len(kept)}")
    for n, b in kept:
        print(f"   --car {b[0]},{b[1]},{b[2]},{b[3]}    n={n}")
    if drop:
        tot = sum(n for n, _ in drop)
        print(f"   dropped {len(drop)} component(s) below {a.min} px, "
              f"{tot} px total: "
              + ", ".join(f"{n}@{b[0]},{b[1]}" for n, b in drop[:12])
              + (" ..." if len(drop) > 12 else ""))
    return 0


if __name__ == "__main__":
    sys.exit(main())
