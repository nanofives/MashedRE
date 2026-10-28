# HB-WARZONE-RIVER-CAP: score the Warzone river on NON-letterboxed original captures (race state 3)
# against the HB-TEXANIM-REFS round-2 port frames for the same pinned pose.
# usage: py -3.12 measure_cap.py
# Reads D:/MashedWT/HB-WARZONE-RIVER-CAP_data/orig/<case>_s<settle>/orig.bmp (new originals) and
#       D:/MashedWT/HB-TEXANIM-REFS_data/{orig2,render2} (old letterboxed originals, port arms).
# Luma 0.299R+0.587G+0.114B. Mask W = |L(MASK) - L(OFF)| > 6, the HB-TEXANIM-REFS PREREG2 mask,
# unchanged. Bands = original rows in 25-row steps over the 3 px eroded mask.
import numpy as np
from pathlib import Path
from PIL import Image, ImageFilter

N = Path("D:/MashedWT/HB-WARZONE-RIVER-CAP_data/orig")
D = Path("D:/MashedWT/HB-TEXANIM-REFS_data")
CASES = {"warzone_p2": (1, 2, 3, 4), "warzone_p3": (2, 4)}
L = lambda a: a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114
rgb = lambda p: np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)
new = lambda c, s: rgb(N / f"{c}_s{s}" / "orig.bmp")
arm = lambda c, a: rgb(D / "render2" / f"{c}_{a}" / "race1" / "01_grid.bmp")


def black_rows(o):
    return np.flatnonzero(L(o).mean(1) < 1)


def grad_of(img):
    a = np.asarray(Image.fromarray(np.uint8(np.clip(L(img), 0, 255))).filter(ImageFilter.GaussianBlur(2)),
                   dtype=np.float64)
    gy = np.zeros_like(a); gy[1:-1] = a[2:] - a[:-2]
    gx = np.zeros_like(a); gx[:, 1:-1] = a[:, 2:] - a[:, :-2]
    return np.hypot(gx, gy)


def fit(o, q):
    """2D scale+offset grid search around identity, gradient correlation over the full frame."""
    go, gq = grad_of(o), grad_of(q)
    ys, xs = np.mgrid[0:480, 0:640]
    best = (-1.0,)
    for s in (0.98, 0.99, 1.0, 1.01, 1.02):
        for ox in range(-4, 5, 2):
            for oy in range(-4, 5, 2):
                px = np.round((xs - ox) / s).astype(int); py = np.round((ys - oy) / s).astype(int)
                ok = (px >= 0) & (px < 640) & (py >= 0) & (py < 480)
                c = np.corrcoef(go[ys[ok], xs[ok]], gq[py[ok], px[ok]])[0, 1]
                if c > best[0]:
                    best = (c, s, ox, oy)
    r0 = np.corrcoef(go.ravel(), gq.ravel())[0, 1]
    return r0, best


def erode(M, k=3):
    E = M.copy()
    for dy in range(-k, k + 1):
        for dx in range(-k, k + 1):
            E &= np.roll(np.roll(M, dy, 0), dx, 1)
    return E


def main():
    print("## 1. Letterbox rows and alignment, new originals vs port OFF\n")
    print("| capture | black rows | r at identity | best r | best scale, ox, oy |")
    print("|---|---|---|---|---|")
    for c, ss in CASES.items():
        off = arm(c, "OFF")
        for s in ss:
            o = new(c, s)
            r0, (r, sc, ox, oy) = fit(o, off)
            print(f"| {c}_s{s} | {len(black_rows(o))} | {r0:.3f} | {r:.3f} | {sc:.2f}, {ox}, {oy} |")

    print("\n## 2. PREREG2 mask (unchanged), full frame, new originals\n")
    print("| case | original | n | original L | OFF L | ANIM L | OFF - orig | ANIM - orig |")
    print("|---|---|---|---|---|---|---|---|")
    for c, ss in CASES.items():
        off, an, mk = arm(c, "OFF"), arm(c, "ANIM"), arm(c, "MASK")
        lf, la = L(off), L(an)
        M = np.abs(L(mk) - lf) > 6
        lo_old = L(rgb(D / "orig2" / c / "orig.bmp"))
        print(f"| {c} | old orig2 (letterboxed, PREREG2) | {M.sum()} | {lo_old[M].mean():.1f} | {lf[M].mean():.1f} | "
              f"{la[M].mean():.1f} | {lf[M].mean() - lo_old[M].mean():+.1f} | {la[M].mean() - lo_old[M].mean():+.1f} |")
        for s in ss:
            lo = L(new(c, s))
            print(f"| {c} | new s{s} | {M.sum()} | {lo[M].mean():.1f} | {lf[M].mean():.1f} | {la[M].mean():.1f} | "
                  f"{lf[M].mean() - lo[M].mean():+.1f} | {la[M].mean() - lo[M].mean():+.1f} |")

    print("\n## 3. Row bands over the 3 px eroded mask (same projection on both sides)\n")
    for c, ss in CASES.items():
        off, an, mk = arm(c, "OFF"), arm(c, "ANIM"), arm(c, "MASK")
        lf, la = L(off), L(an)
        E = erode(np.abs(L(mk) - lf) > 6)
        los = {s: L(new(c, s)) for s in ss}
        rows = np.flatnonzero(E.any(1))
        print(f"{c}: eroded mask n={E.sum()} ({E.mean() * 100:.2f}% of frame), rows {rows.min()}..{rows.max()}\n")
        hdr = " | ".join(f"orig s{s}" for s in ss)
        print(f"| rows | n | {hdr} | orig mean | orig spread | OFF L | ANIM L | OFF - orig | ANIM - orig |")
        print("|---" * (8 + len(ss)) + "|")
        y0 = (rows.min() // 25) * 25
        while y0 <= rows.max():
            bb = E.copy(); bb[:y0] = False; bb[y0 + 25:] = False
            if bb.sum() > 200:
                v = [los[s][bb].mean() for s in ss]
                om = float(np.mean(v))
                print(f"| {y0}-{y0 + 24} | {bb.sum()} | " + " | ".join(f"{x:.1f}" for x in v)
                      + f" | {om:.1f} | {max(v) - min(v):.1f} | {lf[bb].mean():.1f} | {la[bb].mean():.1f} | "
                        f"{lf[bb].mean() - om:+.1f} | {la[bb].mean() - om:+.1f} |")
            y0 += 25
        v = [los[s][E].mean() for s in ss]
        om = float(np.mean(v))
        print(f"| all | {E.sum()} | " + " | ".join(f"{x:.1f}" for x in v)
              + f" | {om:.1f} | {max(v) - min(v):.1f} | {lf[E].mean():.1f} | {la[E].mean():.1f} | "
                f"{lf[E].mean() - om:+.1f} | {la[E].mean() - om:+.1f} |\n")
        o = new(c, ss[0])
        print(f"  RGB under eroded mask: orig s{ss[0]} {'/'.join(f'{x:.1f}' for x in o[E].mean(0))}  "
              f"OFF {'/'.join(f'{x:.1f}' for x in off[E].mean(0))}  ANIM {'/'.join(f'{x:.1f}' for x in an[E].mean(0))}\n")


if __name__ == "__main__":
    main()
