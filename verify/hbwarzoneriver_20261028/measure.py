# HB-WARZONE-RIVER: re-score the HB-TEXANIM-REFS round-2 Warzone water numbers on the committed
# references, with the original's letterbox accounted for. No new capture, no game run.
# usage: py -3.12 measure.py   (reads D:/MashedWT/HB-TEXANIM-REFS_data/{orig2,render2})
# Luma 0.299R+0.587G+0.114B. Mask W = |L(MASK) - L(OFF)| > 6, the HB-TEXANIM-REFS PREREG2 mask.
import numpy as np
from pathlib import Path
from PIL import Image, ImageFilter

D = Path("D:/MashedWT/HB-TEXANIM-REFS_data")
CASES = ("training_p3", "training_p4", "sands_p11", "sands_p8", "warzone_p3", "warzone_p2")
L = lambda a: a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114
rgb = lambda p: np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)
orig = lambda c: rgb(D / "orig2" / c / "orig.bmp")
arm = lambda c, a: rgb(D / "render2" / f"{c}_{a}" / "race1" / "01_grid.bmp")
f1 = lambda v: "/".join(f"{x:.1f}" for x in v)


def black_rows(o):
    r = L(o).mean(1)
    return np.flatnonzero(r < 1)


def grad(a):
    gy = np.zeros_like(a); gy[1:-1] = a[2:] - a[:-2]
    gx = np.zeros_like(a); gx[:, 1:-1] = a[:, 2:] - a[:, :-2]
    return np.hypot(gx, gy)


def warp(img, sx, sy, ox, oy):
    """port image resampled into original pixel coordinates: orig(x,y) <- port((x-ox)/sx, (y-oy)/sy)."""
    ys, xs = np.mgrid[0:480, 0:640]
    px = np.round((xs - ox) / sx).astype(int); py = np.round((ys - oy) / sy).astype(int)
    ok = (px >= 0) & (px < 640) & (py >= 0) & (py < 480)
    return img[np.clip(py, 0, 479), np.clip(px, 0, 639)], ok


def fit(case, seeds):
    """grid search of a 2D scale+offset, gradient correlation over original rows 190..411 (below the HUD panels)."""
    bl = lambda p: grad(np.asarray(Image.open(p).convert("L").filter(ImageFilter.GaussianBlur(2)), dtype=np.float64))
    go = bl(D / "orig2" / case / "orig.bmp"); gq = bl(D / "render2" / f"{case}_OFF" / "race1" / "01_grid.bmp")
    ys, xs = np.mgrid[190:412, 0:640]
    best = (-1.0,)
    for sx0, sy0, ox0, oy0 in seeds:
        for sx in np.arange(sx0 - 0.02, sx0 + 0.021, 0.01):
            for sy in np.arange(sy0 - 0.02, sy0 + 0.021, 0.01):
                for ox in np.arange(ox0 - 4, ox0 + 4.1, 2):
                    for oy in np.arange(oy0 - 4, oy0 + 4.1, 2):
                        px = np.round((xs - ox) / sx).astype(int); py = np.round((ys - oy) / sy).astype(int)
                        ok = (px >= 0) & (px < 640) & (py >= 0) & (py < 480)
                        if ok.mean() < 0.6:
                            continue
                        c = np.corrcoef(go[ys[ok], xs[ok]], gq[py[ok], px[ok]])[0, 1]
                        if c > best[0]:
                            best = (c, sx, sy, ox, oy)
    return best


def main():
    print("## 1. Letterbox rows in the original references (row mean luma < 1)\n")
    print("| case | black rows | first | last | viewport rows |")
    print("|---|---|---|---|---|")
    for c in CASES:
        b = black_rows(orig(c))
        vp = "-" if len(b) == 0 else f"{b[b < 240].max() + 2}..{b[b > 240].min() - 2}"
        print(f"| {c} | {len(b)} | {b[0] if len(b) else '-'} | {b[-1] if len(b) else '-'} | {vp} |")

    print("\n## 2. PREREG2 mask split by original region (no geometry change)\n")
    print("| case | region | n | % of mask | original L | OFF L | ANIM L | OFF - orig |")
    print("|---|---|---|---|---|---|---|---|")
    for c in CASES:
        o, off, an, mk = orig(c), arm(c, "OFF"), arm(c, "ANIM"), arm(c, "MASK")
        lo, lf, la = L(o), L(off), L(an)
        M = np.abs(L(mk) - lf) > 6
        b = black_rows(o)
        lb = np.zeros_like(M); lb[b] = True
        regs = [("all (PREREG2)", M)]
        if len(b):
            regs += [("letterbox rows", M & lb), ("outside letterbox", M & ~lb)]
        for name, mm in regs:
            if mm.sum() == 0:
                continue
            print(f"| {c} | {name} | {mm.sum()} | {mm.sum() / M.sum() * 100:.1f} | {lo[mm].mean():.1f} | "
                  f"{lf[mm].mean():.1f} | {la[mm].mean():.1f} | {lf[mm].mean() - lo[mm].mean():+.1f} |")

    print("\n## 3. Warzone, port resampled onto the original's geometry\n")
    seeds = {"warzone_p2": [(0.81, 0.79, 24, 13)], "warzone_p3": [(0.81, 0.79, 24, 13), (0.67, 0.67, 84, 84)]}
    for c, sd in seeds.items():
        r, sx, sy, ox, oy = fit(c, sd)
        print(f"{c}: best fit sx={sx:.3f} sy={sy:.3f} ox={ox:.0f} oy={oy:.0f}  gradient correlation r={r:.3f}")
        o = orig(c); lo = L(o)
        W = {a: warp(arm(c, a), sx, sy, ox, oy) for a in ("OFF", "ANIM", "MASK")}
        ok = W["OFF"][1]
        M = (np.abs(L(W["MASK"][0]) - L(W["OFF"][0])) > 6) & ok
        b = black_rows(o); M[b] = False
        M[:185, :270] = False   # standings panels (rows 81..184, cols 0..269)
        E = M.copy()
        for dy in range(-3, 4):
            for dx in range(-3, 4):
                E &= np.roll(np.roll(M, dy, 0), dx, 1)
        if r < 0.5:
            print(f"  r < 0.5: alignment not trusted, not scored\n")
            continue
        lf, la = L(W["OFF"][0]), L(W["ANIM"][0])
        print(f"  mask eroded 3 px: n={E.sum()} ({E.mean() * 100:.2f}% of frame)  original L {lo[E].mean():.1f}  "
              f"OFF L {lf[E].mean():.1f}  ANIM L {la[E].mean():.1f}  OFF - orig {lf[E].mean() - lo[E].mean():+.1f}")
        print(f"  RGB original {f1(o[E].mean(0))}  OFF {f1(W['OFF'][0][E].mean(0))}  ANIM {f1(W['ANIM'][0][E].mean(0))}")
        print("  | original rows | n | original L | OFF L | OFF - orig |")
        print("  |---|---|---|---|---|")
        for y0 in range(190, 416, 25):
            bb = E.copy(); bb[:y0] = False; bb[y0 + 25:] = False
            if bb.sum() > 200:
                print(f"  | {y0}-{y0 + 24} | {bb.sum()} | {lo[bb].mean():.1f} | {lf[bb].mean():.1f} | {lf[bb].mean() - lo[bb].mean():+.1f} |")
        print()


if __name__ == "__main__":
    main()
