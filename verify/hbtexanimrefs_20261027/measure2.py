# HB-TEXANIM-REFS PREREG2 metric (sections 2, 5). Luma 0.299R+0.587G+0.114B, mask W = |L(MASK) - L(OFF)| > 6.
# usage: py -3.12 measure2.py <render2_dir> [--def]
import json, subprocess, sys
from pathlib import Path
import numpy as np
from PIL import Image

W = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
REFS2 = Path("D:/MashedWT/HB-TEXANIM-REFS_data/orig2")
TRACKS = ("training", "sands", "warzone")


def rgb(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)


def luma(a):
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def dilate(m, r):
    out = m.copy()
    for dy in range(-r, r + 1):
        for dx in range(-r, r + 1):
            out |= np.roll(np.roll(m, dy, 0), dx, 1)
    return out


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    rd = Path(args[0])
    poses = list(json.loads((HERE / "poses_r2.json").read_text()))
    shot = lambda case, arm: rd / f"{case}_{arm}" / "race1" / "01_grid.bmp"
    f = lambda v: "/".join(f"{x:.1f}" for x in v)
    print("| pose | % frame W | original L | OFF L | ANIM L | dOFF | dANIM | toward | orig RGB | OFF RGB | ANIM RGB "
          "| MAE OFF | MAE ANIM | R2 outside % |")
    print("|---|---|---|---|---|---|---|---|---|---|---|---|---|---|")
    res = {t: [] for t in TRACKS}
    r2_all = True
    for case in poses:
        t = case.split("_")[0]
        ref = REFS2 / case / "orig.bmp"
        tk = REFS2 / case / "orig_track.txt"
        need = [ref, shot(case, "OFF"), shot(case, "ANIM"), shot(case, "MASK")]
        if not all(p.exists() for p in need):
            print(f"| {case} | missing | | | | | | | | | | | | |")
            res[t].append(None)
            continue
        tname = tk.read_text().splitlines()[0].strip() if tk.exists() else "?"
        if tname.upper() != t.upper():
            print(f"| {case} | track mismatch {tname} | | | | | | | | | | | | |")
            res[t].append(None)
            continue
        o, off, anim, msk = (rgb(p) for p in need)
        lo, loff, lanim = luma(o), luma(off), luma(anim)
        m = np.abs(luma(msk) - loff) > 6
        a, b, c = lo[m].mean(), loff[m].mean(), lanim[m].mean()
        doff, danim = abs(b - a), abs(c - a)
        mae_off, mae_anim = np.abs(lo - loff)[m].mean(), np.abs(lo - lanim)[m].mean()
        outside = (np.abs(anim - off).max(-1) > 0) & ~dilate(m, 3)
        r2 = outside.mean() * 100
        r2_all &= r2 <= 0.5
        tw = bool(danim < doff)
        res[t].append("T" if danim < doff else ("A" if danim > doff else "="))
        print(f"| {case} | {m.mean() * 100:.2f} | {a:.1f} | {b:.1f} | {c:.1f} | {doff:.1f} | {danim:.1f} | "
              f"{'yes' if tw else 'NO'} | {f(o[m].mean(0))} | {f(off[m].mean(0))} | {f(anim[m].mean(0))} | "
              f"{mae_off:.1f} | {mae_anim:.1f} | {r2:.3f} |")
    print()
    print("R0 OFF vs OFF2 (first pose per track):")
    r0_all = True
    for t in TRACKS:
        first = next(c for c in poses if c.startswith(t + "_"))
        p1, p2 = shot(first, "OFF"), shot(first, "OFF2")
        same = p1.exists() and p2.exists() and bool((rgb(p1) == rgb(p2)).all())
        r0_all &= same
        print(f"  {first}: bit-identical={same}")
    print()
    print("R1 per track:")
    allt = True
    for t in TRACKS:
        v = res[t]
        if v and all(x == "T" for x in v):
            st = "TOWARD"
        elif any(x == "A" for x in v):
            st = "AWAY"
        else:
            st = "MIXED"
        allt &= st == "TOWARD"
        print(f"  {t}: {st} {v}")
    print(f"R2 all poses <= 0.5% outside: {r2_all}")
    print(f"FLIP allowed (R0 and R2 and all TOWARD): {r0_all and r2_all and allt}")
    print()
    for case in poses:
        lg = rd / f"{case}_ANIM" / "mashed_re.log"
        hits = [l.strip() for l in lg.read_text(errors="replace").splitlines() if "TEXANIM" in l] if lg.exists() else []
        print(f"{case}_ANIM log TEXANIM: {hits or 'none'}")
    if "--def" in sys.argv:
        print()
        print("Post-flip: DEF (no knob env) vs pre-flip ANIM, first pose per track:")
        for t in TRACKS:
            first = next(c for c in poses if c.startswith(t + "_"))
            p1, p2 = shot(first, "ANIM"), shot(first, "DEF")
            ok = p1.exists() and p2.exists() and bool((rgb(p1) == rgb(p2)).all())
            print(f"  {first}: bit-identical={ok}")


if __name__ == "__main__":
    main()
