# HB-FOREST-WATER metric (PREREG.md sections 4-5). Luma 0.299R+0.587G+0.114B.
# usage: py -3.12 measure.py <render_dir> [<hbarctic_render_dir>]
import subprocess, sys
from pathlib import Path
import numpy as np
from PIL import Image

W = Path(__file__).resolve().parents[2]
REF = W / "verify/water_refs/forest_s8/orig.bmp"
HBA = Path("D:/MashedWT/HB-ARCTIC_data/render")


def rgb(p):
    return np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)


def luma(a):
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


def grad(lum):
    g = np.zeros_like(lum)
    g[:, :-1] += np.abs(np.diff(lum, axis=1))
    g[:-1, :] += np.abs(np.diff(lum, axis=0))
    return g


def dilate(m, r):
    out = m.copy()
    for dy in range(-r, r + 1):
        for dx in range(-r, r + 1):
            out |= np.roll(np.roll(m, dy, 0), dx, 1)
    return out


def imgdiff_mean(a, b):
    r = subprocess.run([sys.executable, str(W / "re/tools/imgdiff.py"), str(a), str(b)],
                       capture_output=True, text=True)
    return r.stdout.strip().splitlines()


def main():
    rd = Path(sys.argv[1])
    hba = Path(sys.argv[2]) if len(sys.argv) > 2 else HBA
    shot = lambda arm: rd / f"forest_s8_{arm}" / "race1" / "01_grid.bmp"
    o, off, off2, anim = rgb(REF), rgb(shot("OFF")), rgb(shot("OFF2")), rgb(shot("ANIM"))
    lo, loff, lanim = luma(o), luma(off), luma(anim)

    r0 = bool((off == off2).all())
    print(f"R0 OFF vs OFF2 bit-identical: {r0} (pixels differing "
          f"{(np.abs(off - off2).max(-1) > 0).mean() * 100:.3f}%)")

    hsea = luma(rgb(hba / "forest_s8_SEA/race1/01_grid.bmp"))
    hoff_rgb = rgb(hba / "forest_s8_OFF/race1/01_grid.bmp")
    w1 = np.abs(hsea - luma(hoff_rgb)) > 6
    w2 = np.abs(lanim - loff) > 6
    print(f"HB-ARCTIC OFF vs this OFF bit-identical: {bool((hoff_rgb == off).all())}")
    print()
    print("| mask | % frame | original L | OFF L | ANIM L | dOFF | dANIM | orig RGB | OFF RGB | ANIM RGB | grad orig | grad OFF | grad ANIM |")
    print("|---|---|---|---|---|---|---|---|---|---|---|---|---|")
    go, goff, ganim = grad(lo), grad(loff), grad(lanim)
    rows = {}
    for name, m in (("W1", w1), ("W2", w2)):
        a, b, c = lo[m].mean(), loff[m].mean(), lanim[m].mean()
        ca, cb, cc = o[m].mean(0), off[m].mean(0), anim[m].mean(0)
        rows[name] = (a, b, c, ca, cc, go[m].mean(), ganim[m].mean())
        f = lambda v: "/".join(f"{x:.1f}" for x in v)
        print(f"| {name} | {m.mean() * 100:.2f} | {a:.1f} | {b:.1f} | {c:.1f} | {abs(b - a):.1f} | "
              f"{abs(c - a):.1f} | {f(ca)} | {f(cb)} | {f(cc)} | {go[m].mean():.2f} | "
              f"{goff[m].mean():.2f} | {ganim[m].mean():.2f} |")
    print()
    a, b, c, ca, cc, gro, gra = rows["W1"]
    r1 = abs(c - a) <= 8 and abs(c - a) < abs(b - a)
    r2 = bool((np.abs(cc - ca) <= 8).all())
    ratio = gra / gro if gro > 0 else float("inf")
    r3 = 0.5 <= ratio <= 2.0
    outside = ~dilate(w1, 3)
    diff_out = (np.abs(anim - off).max(-1) > 0) & outside
    r4 = diff_out.mean() * 100 <= 0.5
    print(f"R1 |dANIM|={abs(c - a):.1f} <= 8 and < |dOFF|={abs(b - a):.1f}: {r1}")
    print(f"R2 per-channel |ANIM-orig| = {np.abs(cc - ca).round(1).tolist()} all <= 8: {r2}")
    print(f"R3 gradient ratio ANIM/orig = {ratio:.2f} in [0.5, 2]: {r3}")
    print(f"R4 ANIM != OFF outside W1 (dilated 3 px) = {diff_out.mean() * 100:.3f}% <= 0.5%: {r4}")
    print()
    print("imgdiff orig vs OFF:")
    for ln in imgdiff_mean(REF, shot("OFF")):
        print("  " + ln)
    print("imgdiff orig vs ANIM:")
    for ln in imgdiff_mean(REF, shot("ANIM")):
        print("  " + ln)
    print()
    for arm in ("OFF", "ANIM"):
        p = rd / f"forest_s8_{arm}" / "mashed_re.log"
        if p.exists():
            hits = [l.strip() for l in p.read_text(errors="replace").splitlines() if "TEXANIM" in l]
            print(f"forest_s8_{arm} log TEXANIM: {hits or 'none'}")
    for case in ("sands", "training", "warzone"):
        d = rd / f"{case}_ANIM"
        lg, lr = d / "mashed_re.log", d / "librw_race.txt"
        t = [l.strip() for l in lg.read_text(errors="replace").splitlines() if "TEXANIM" in l] if lg.exists() else []
        r = [l.strip() for l in lr.read_text(errors="replace").splitlines() if "[hbforest]" in l] if lr.exists() else []
        prov = (d / "PROVENANCE.txt").read_text().splitlines()[:4] if (d / "PROVENANCE.txt").exists() else []
        print(f"{case}_ANIM: shot={(d / 'race1/01_grid.bmp').exists()} {prov[2:4]} log={t} librw={r}")


if __name__ == "__main__":
    main()
