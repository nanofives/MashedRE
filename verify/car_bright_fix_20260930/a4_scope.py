# A4 (CAR_BRIGHTNESS_2026-09-30.md) regression guard, scoped.
#
# imgdiff.py --grid 8x6 answers "which 80x80 cells moved". A4's claim is
# stronger than that: NO non-car surface may change. So this script diffs the
# pose-identical pre-fix / post-fix pair pixel-by-pixel and reports, per
# capture:
#
#   n_diff        pixels whose max-channel abs diff exceeds --thr (default 0:
#                 ANY change at all counts -- the poses are bit-identical under
#                 MASHED_DETERMINISTIC=1, so there is no jitter floor to absorb)
#   bbox          the bounding box of every differing pixel
#   components    connected clusters of differing pixels, largest first, with
#                 each cluster's bbox and pixel count -- a car is one compact
#                 cluster, a terrain-wide regression is not
#   outside       differing pixels OUTSIDE the union of the --car boxes, which
#                 is the actual A4 number: it must be 0 for terrain/sea/sky.
#
# Usage:
#   py -3.12 verify/car_bright_fix_20260930/a4_scope.py <pre.bmp> <post.bmp>
#       [--thr N] [--car x0,y0,x1,y1 ...] [--box name=x0,y0,x1,y1 ...]
import argparse
import sys
from collections import deque

from PIL import Image


def parse_box(s):
    return tuple(int(v) for v in s.split(","))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("pre")
    ap.add_argument("post")
    ap.add_argument("--thr", type=int, default=0)
    ap.add_argument("--car", action="append", default=[])
    ap.add_argument("--box", action="append", default=[])
    ap.add_argument("--maxcomp", type=int, default=6)
    a = ap.parse_args()

    A = Image.open(a.pre).convert("RGB")
    B = Image.open(a.post).convert("RGB")
    if A.size != B.size:
        print(f"SIZE MISMATCH {A.size} vs {B.size}")
        return 2
    W, H = A.size
    pa, pb = A.load(), B.load()
    mask = bytearray(W * H)
    n = 0
    x0 = y0 = 10**9
    x1 = y1 = -1
    for y in range(H):
        for x in range(W):
            ca, cb = pa[x, y], pb[x, y]
            d = max(abs(ca[0] - cb[0]), abs(ca[1] - cb[1]), abs(ca[2] - cb[2]))
            if d > a.thr:
                mask[y * W + x] = 1
                n += 1
                if x < x0: x0 = x
                if x > x1: x1 = x
                if y < y0: y0 = y
                if y > y1: y1 = y
    print(f"{a.pre}\n  vs {a.post}")
    print(f"  size={W}x{H}  thr={a.thr}  n_diff={n} ({100.0*n/(W*H):.2f}% of frame)")
    if n == 0:
        print("  IDENTICAL")
        return 0
    print(f"  bbox=({x0},{y0},{x1},{y1})")

    # connected components (4-neighbour) over the differing pixels
    seen = bytearray(W * H)
    comps = []
    for sy in range(H):
        for sx in range(W):
            i0 = sy * W + sx
            if not mask[i0] or seen[i0]:
                continue
            q = deque([(sx, sy)])
            seen[i0] = 1
            cx0 = cx1 = sx
            cy0 = cy1 = sy
            cnt = 0
            while q:
                x, y = q.popleft()
                cnt += 1
                if x < cx0: cx0 = x
                if x > cx1: cx1 = x
                if y < cy0: cy0 = y
                if y > cy1: cy1 = y
                for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)):
                    nx, ny = x + dx, y + dy
                    if 0 <= nx < W and 0 <= ny < H:
                        j = ny * W + nx
                        if mask[j] and not seen[j]:
                            seen[j] = 1
                            q.append((nx, ny))
            comps.append((cnt, (cx0, cy0, cx1, cy1)))
    comps.sort(reverse=True)
    print(f"  components={len(comps)} (largest {min(a.maxcomp, len(comps))} shown)")
    for cnt, bb in comps[:a.maxcomp]:
        print(f"    n={cnt:6d} bbox={bb}")

    cars = [parse_box(c) for c in a.car]
    if cars:
        out = 0
        for y in range(H):
            for x in range(W):
                if not mask[y * W + x]:
                    continue
                if not any(bx0 <= x < bx1 and by0 <= y < by1
                           for bx0, by0, bx1, by1 in cars):
                    out += 1
        print(f"  differing pixels OUTSIDE the {len(cars)} car box(es): {out}")

    for spec in a.box:
        name, _, bs = spec.partition("=")
        bx0, by0, bx1, by1 = parse_box(bs)
        tot = dif = 0
        sa = sb = 0
        for y in range(by0, min(by1, H)):
            for x in range(bx0, min(bx1, W)):
                tot += 1
                ca, cb = pa[x, y], pb[x, y]
                sa += sum(ca)
                sb += sum(cb)
                if mask[y * W + x]:
                    dif += 1
        print(f"  box {name:<16} {bs:<20} n={tot:6d} differing={dif:6d} "
              f"({100.0*dif/max(tot,1):5.2f}%)  mean_pre={sa/max(tot,1)/3:.3f} "
              f"mean_post={sb/max(tot,1)/3:.3f}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
