# imgdiff.py — numeric pixel-diff backstop for parity verification.
#
# The draw-list differ (drawlist_diff.py) is the primary composition verifier;
# this tool covers what a draw list cannot encode: texture DECODE correctness,
# font rasterization quality, gradient interpolation — anything where the
# geometry is right but the pixels are not. It replaces "eyeball the two
# screenshots" with numbers and a heatmap.
#
# Typical inputs: an original-side window screenshot (640x480) vs the
# standalone's MASHED_DBG_BBDUMP backbuffer dump (800x600,
# verify/dbg_backbuffer.bmp — the trusted channel; window captures of the
# standalone are unreliable on this machine, see frontend_feedback tracker
# 2026-06-12 verification note). B is resampled to A's size first.
#
# Usage:
#   py -3.12 re/tools/imgdiff.py original.png re_backbuffer.bmp
#       [--out heatmap.png] [--grid 8x6] [--threshold 16] [--fail-mean N]
#
# Output: global per-channel mean abs diff, % pixels over --threshold, a
# per-region grid of mean diffs (locates WHERE divergence concentrates), and
# an amplified abs-diff heatmap image. Exit 1 only if --fail-mean is given
# and exceeded; otherwise informational (exit 0).
import argparse
import sys
from pathlib import Path

from PIL import Image, ImageChops


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("file_a", help="reference image (original)")
    ap.add_argument("file_b", help="image under test (standalone)")
    ap.add_argument("--out", default=None, help="write amplified diff heatmap here")
    ap.add_argument("--grid", default="8x6", help="region grid, COLSxROWS")
    ap.add_argument("--threshold", type=int, default=16,
                    help="per-pixel max-channel diff counted as 'different'")
    ap.add_argument("--fail-mean", type=float, default=None,
                    help="exit 1 if global mean abs diff exceeds this")
    args = ap.parse_args()

    a = Image.open(args.file_a).convert("RGB")
    b = Image.open(args.file_b).convert("RGB")
    if b.size != a.size:
        print(f"resampling B {b.size} -> A {a.size}")
        b = b.resize(a.size, Image.BILINEAR)

    diff = ImageChops.difference(a, b)
    px = diff.load()
    w, h = diff.size

    sums = [0, 0, 0]
    over = 0
    for y in range(h):
        for x in range(w):
            r, g, bl = px[x, y]
            sums[0] += r; sums[1] += g; sums[2] += bl
            if max(r, g, bl) > args.threshold:
                over += 1
    n = w * h
    means = [s / n for s in sums]
    mean_all = sum(means) / 3
    print(f"size {w}x{h}  mean abs diff R={means[0]:.2f} G={means[1]:.2f} "
          f"B={means[2]:.2f} (all {mean_all:.2f})")
    print(f"pixels over threshold {args.threshold}: {over} "
          f"({100.0 * over / n:.2f}%)")

    cols, rows = (int(v) for v in args.grid.lower().split("x"))
    print(f"region grid {cols}x{rows} (mean abs diff per cell):")
    cw, ch = w / cols, h / rows
    for ry in range(rows):
        cells = []
        for rx in range(cols):
            s = cnt = 0
            for y in range(int(ry * ch), int((ry + 1) * ch)):
                for x in range(int(rx * cw), int((rx + 1) * cw)):
                    r, g, bl = px[x, y]
                    s += r + g + bl
                    cnt += 3
            cells.append(s / cnt if cnt else 0.0)
        print("  " + " ".join(f"{c:6.1f}" for c in cells))

    if args.out:
        amplified = diff.point(lambda v: min(255, v * 4))
        amplified.save(args.out)
        print(f"-> {args.out}")

    if args.fail_mean is not None and mean_all > args.fail_mean:
        print(f"FAIL: mean {mean_all:.2f} > {args.fail_mean}")
        return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
