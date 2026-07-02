# WS-E vehicle lighting acceptance — heading-matched original-vs-standalone
# comparison (2026-07-02, r6/ws-e-vehicle-lighting).
#
# Inputs (this directory):
#   orig_grid.png / orig_donut_*.png + shots.json   original MASHED.exe, Arctic
#       TimeTrial solo, Stallion (VEHICLES\STALLION.PIZ confirmed via CreateFile
#       log), d3d9-shim backbuffer dumps (re/frida/capture_relight_parity.py).
#   sa_relit/*.png    standalone race demo, MASHED_RPLIGHT on (default),
#       Stallion (MASHED_CAR_SEL=12), laps objective, headings logged by
#       RELIGHT_CAP lines (exe_main cap()).
#   sa_legacy/*.png   same, MASHED_RPLIGHT=0 (legacy load-time model-space bake).
#
# Matched pairs (headings, radians — atan2(fwd_z, fwd_x) both sides):
#   GRID : orig_grid      1.57011  <->  01_grid      1.54978   (d=0.020)
#   A    : orig_donut_01  0.03502  <->  01_turned_a  0.05274   (d=0.018)
#   B    : orig_donut_24 -0.17080  <->  01_turned_b -0.16959   (d=0.001)
#
# Metric: mean RGB over the car-PAINT pixels inside a per-composition bounding
# box. Paint mask = red-dominant pixel (R>40, R>=1.15*G, R>=1.15*B) — Stallion
# livery 0 is red; the mask keys on the painted shell and rejects road/snow/HUD.
# Box rects were chosen per composition class (rear-chase vs side view) so the
# mask never touches the red track-side signs/barriers.
#
# Full-frame imgdiff heatmaps (re/tools/imgdiff.py) accompany each pair for
# localization; full-frame means are NOT the acceptance number (HUD + camera
# pose differ by design — residuals in README.md).
import json, subprocess, sys
from pathlib import Path
from PIL import Image

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[2]          # worktree root (orig_parity -> ws_e_lighting -> verify -> root)
IMGDIFF = ROOT / "re" / "tools" / "imgdiff.py"

BOX_REAR = (200, 280, 460, 480)   # rear-chase compositions (orig grid, sa all)
BOX_SIDE = (110, 280, 570, 470)   # original side-view compositions (donut_01/24)

# Roof band — the one painted panel visible in EVERY composition here, with a
# fixed world-space normal (up), so its lit value must be heading-INDEPENDENT
# and equal across orig/relit/legacy if light acquisition is right.
ROOF = {
    "orig_grid.png":     (250, 288, 420, 322),
    "orig_donut_01.png": (250, 295, 440, 335),
    "orig_donut_24.png": (250, 295, 440, 335),
    "sa":                (250, 290, 380, 330),
}

def paint_mean(path, box):
    im = Image.open(path).convert("RGB").crop(box)
    px = im.load()
    n = 0; s = [0, 0, 0]
    for y in range(im.height):
        for x in range(im.width):
            r, g, b = px[x, y]
            if r > 40 and r >= g * 1.15 and r >= b * 1.15:
                s[0] += r; s[1] += g; s[2] += b; n += 1
    if n == 0: return (0.0, 0.0, 0.0, 0)
    return (s[0] / n, s[1] / n, s[2] / n, n)

PAIRS = [
    ("GRID", "orig_grid.png",     BOX_REAR, "01_grid.png",     BOX_REAR),
    ("A",    "orig_donut_01.png", BOX_SIDE, "01_turned_a.png", BOX_REAR),
    ("B",    "orig_donut_24.png", BOX_SIDE, "01_turned_b.png", BOX_REAR),
]

def run_imgdiff(a, b, out):
    r = subprocess.run([sys.executable, str(IMGDIFF), str(a), str(b),
                        "--out", str(out)], capture_output=True, text=True)
    first = r.stdout.splitlines()[:2]
    return " | ".join(first)

def main():
    print("== car-paint region means (R,G,B, n-pixels) ==")
    rows = {}
    for tag, of, obox, sf, sbox in PAIRS:
        o = paint_mean(HERE / of, obox)
        rl = paint_mean(HERE / "sa_relit" / sf, sbox)
        lg = paint_mean(HERE / "sa_legacy" / sf, sbox)
        rows[tag] = (o, rl, lg)
        print(f"[{tag}] orig   {of:20s} R={o[0]:6.1f} G={o[1]:6.1f} B={o[2]:6.1f} n={o[3]}")
        print(f"[{tag}] relit  {sf:20s} R={rl[0]:6.1f} G={rl[1]:6.1f} B={rl[2]:6.1f} n={rl[3]}")
        print(f"[{tag}] legacy {sf:20s} R={lg[0]:6.1f} G={lg[1]:6.1f} B={lg[2]:6.1f} n={lg[3]}")
        dr = abs(o[0]-rl[0]); dl = abs(o[0]-lg[0])
        print(f"[{tag}] |orig-relit| R={dr:5.1f}   |orig-legacy| R={dl:5.1f}")
    print()
    print("== original heading response (paint R mean) ==")
    g, a, b = rows["GRID"][0], rows["A"][0], rows["B"][0]
    print(f"  orig: h=+1.570 R={g[0]:.1f} | h=+0.035 R={a[0]:.1f} | h=-0.171 R={b[0]:.1f}")
    for side in ("relit", "legacy"):
        i = 1 if side == "relit" else 2
        g2, a2, b2 = rows["GRID"][i], rows["A"][i], rows["B"][i]
        print(f"  {side:6s}: h=+1.550 R={g2[0]:.1f} | h=+0.053 R={a2[0]:.1f} | h=-0.170 R={b2[0]:.1f}")
    print()
    print("== roof band (fixed up-normal; heading-independent by law) ==")
    for tag, of, _, sf, _ in PAIRS:
        o = paint_mean(HERE / of, ROOF[of])
        rl = paint_mean(HERE / "sa_relit" / sf, ROOF["sa"])
        lg = paint_mean(HERE / "sa_legacy" / sf, ROOF["sa"])
        print(f"  [{tag}] orig R={o[0]:6.1f} (n={o[3]})  relit R={rl[0]:6.1f} (n={rl[3]})  "
              f"legacy R={lg[0]:6.1f} (n={lg[3]})")
    print()
    print("== full-frame imgdiff (informational; heatmaps written) ==")
    for tag, of, _, sf, _ in PAIRS:
        for side in ("relit", "legacy"):
            out = HERE / f"diff_{tag.lower()}_orig_vs_{side}.png"
            line = run_imgdiff(HERE / of, HERE / f"sa_{side}" / sf, out)
            print(f"  {tag} orig vs {side}: {line}")
    print()
    print("== standalone toggle (relit vs legacy, identical framing) ==")
    for tag, _, _, sf, _ in PAIRS:
        out = HERE / f"diff_{tag.lower()}_toggle.png"
        line = run_imgdiff(HERE / "sa_relit" / sf, HERE / "sa_legacy" / sf, out)
        print(f"  {tag} relit vs legacy: {line}")

if __name__ == "__main__":
    main()
