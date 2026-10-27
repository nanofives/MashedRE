# HB-TEXANIM-REFS metric (PREREG.md sections 4-5). Luma 0.299R+0.587G+0.114B.
# usage: py -3.12 measure.py <render_dir> [<refs_dir>] [--def]
#   --def also checks rule "default render == pre-flip ANIM" on each track's s8 (arm DEF).
import subprocess, sys
from pathlib import Path
import numpy as np
from PIL import Image

W = Path(__file__).resolve().parents[2]
REFS = Path("D:/MashedWT/HB-TEXANIM-REFS_data/orig")
TRACKS = ("training", "sands", "warzone")
SETTLES = (8, 14, 20)
MIN_MASK = 0.5   # % of frame (PREREG section 4)


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


def imgdiff_mean(a, b):
    r = subprocess.run([sys.executable, str(W / "re/tools/imgdiff.py"), str(a), str(b)],
                       capture_output=True, text=True)
    for ln in r.stdout.splitlines():
        if "mean" in ln.lower():
            return ln.strip()
    return (r.stdout.strip().splitlines() or ["?"])[0]


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    rd = Path(args[0])
    refs = Path(args[1]) if len(args) > 1 else REFS
    check_def = "--def" in sys.argv
    shot = lambda case, arm: rd / f"{case}_{arm}" / "race1" / "01_grid.bmp"
    f = lambda v: "/".join(f"{x:.1f}" for x in v)

    print("| vantage | track | % frame W | original L | OFF L | ANIM L | dOFF | dANIM | scored | "
          "toward | orig RGB | OFF RGB | ANIM RGB | R2 outside % |")
    print("|---|---|---|---|---|---|---|---|---|---|---|---|---|---|")
    verdict = {t: [] for t in TRACKS}
    r2_all, notes = True, []
    for t in TRACKS:
        for s in SETTLES:
            case = f"{t}_s{s}"
            ref = refs / case / "orig.bmp"
            trk = (refs / case / "orig_track.txt")
            if not ref.exists() or not shot(case, "OFF").exists() or not shot(case, "ANIM").exists():
                print(f"| {case} | {t} | missing | | | | | | no | | | | | |")
                continue
            tname = trk.read_text().splitlines()[0].strip() if trk.exists() else "?"
            if tname.upper() != t.upper():
                print(f"| {case} | {tname} (track mismatch) | | | | | | | no | | | | | |")
                continue
            o, off, anim = rgb(ref), rgb(shot(case, "OFF")), rgb(shot(case, "ANIM"))
            lo, loff, lanim = luma(o), luma(off), luma(anim)
            m = np.abs(lanim - loff) > 6
            pct = m.mean() * 100
            if pct < MIN_MASK:
                print(f"| {case} | {t} | {pct:.2f} | | | | | | no | | | | | |")
                continue
            a, b, c = lo[m].mean(), loff[m].mean(), lanim[m].mean()
            doff, danim = abs(b - a), abs(c - a)
            outside = (np.abs(anim - off).max(-1) > 0) & ~dilate(m, 3)
            r2 = outside.mean() * 100
            r2_all &= r2 <= 0.5
            tw = "yes" if danim < doff else ("tie" if danim == doff else "NO")
            verdict[t].append(bool(danim < doff) if danim != doff else None)
            print(f"| {case} | {t} | {pct:.2f} | {a:.1f} | {b:.1f} | {c:.1f} | {doff:.1f} | {danim:.1f} | yes | "
                  f"{tw} | {f(o[m].mean(0))} | {f(off[m].mean(0))} | {f(anim[m].mean(0))} | {r2:.3f} |")
            notes.append((case, ref, shot(case, "OFF"), shot(case, "ANIM")))
    print()
    print("R0 OFF vs OFF2 (s8):")
    r0_all = True
    for t in TRACKS:
        p1, p2 = shot(f"{t}_s8", "OFF"), shot(f"{t}_s8", "OFF2")
        if p1.exists() and p2.exists():
            same = bool((rgb(p1) == rgb(p2)).all())
            r0_all &= same
            print(f"  {t}: bit-identical={same}")
        else:
            r0_all = False
            print(f"  {t}: missing")
    print()
    print("R1 per track:")
    flips = []
    for t in TRACKS:
        v = verdict[t]
        if not v:
            st = "NOT SCORED"
        elif any(x is False for x in v):
            st = "AWAY"
        elif all(x is True for x in v):
            st = "TOWARD"
        else:
            st = "TIE (not TOWARD)"
        flips.append(st == "TOWARD")
        print(f"  {t}: {st} ({len(v)} scored vantage(s))")
    print(f"R2 all scored vantages <= 0.5% outside: {r2_all}")
    print(f"FLIP allowed (R0 and R2 and all TOWARD): {r0_all and r2_all and all(flips)}")
    print()
    print("imgdiff global (reported, not gating):")
    for case, ref, poff, panim in notes:
        print(f"  {case} orig vs OFF : {imgdiff_mean(ref, poff)}")
        print(f"  {case} orig vs ANIM: {imgdiff_mean(ref, panim)}")
    print()
    for t in TRACKS:
        d = rd / f"{t}_s8_ANIM" / "mashed_re.log"
        hits = [l.strip() for l in d.read_text(errors="replace").splitlines() if "TEXANIM" in l] if d.exists() else []
        print(f"{t}_s8_ANIM log TEXANIM: {hits or 'none'}")
    if check_def:
        print()
        print("Post-flip: DEF (no knob env) vs pre-flip ANIM on s8:")
        for t in TRACKS:
            p1, p2 = shot(f"{t}_s8", "ANIM"), shot(f"{t}_s8", "DEF")
            ok = p1.exists() and p2.exists() and bool((rgb(p1) == rgb(p2)).all())
            print(f"  {t}: bit-identical={ok}")


if __name__ == "__main__":
    main()
