# POST-HOC (not in PREREG): luma over the PRE-FIX fold mask |geomON - geomOFF| > 6 from
# verify/arctic_ref/sea_search/<pose>/, i.e. the region the 2026-08-31 "sea too dark" row
# was measured on, compared against today's OFF arm. usage: posthoc_oldmask.py <render_dir>
import sys
from pathlib import Path
import numpy as np
from PIL import Image
W = Path(__file__).resolve().parents[2]
L = lambda p: (lambda a: a[..., 0] * .299 + a[..., 1] * .587 + a[..., 2] * .114)(
    np.asarray(Image.open(p).convert("RGB"), dtype=np.float64))
rd = Path(sys.argv[1])
print("| pose | old mask % | original | pre-fix folded (dir geomOFF) | pre-fix unfolded (dir geomON) | today OFF | today dOFF | frame mean |orig-today| |")
print("|---|---|---|---|---|---|---|---|")
for s in ("s8", "s14"):
    b = W / f"verify/arctic_ref/sea_search/{s}"
    o, po, ps = L(b / "a.bmp"), L(b / "geomOFF/race1/01_grid.bmp"), L(b / "geomON/race1/01_grid.bmp")
    t = L(rd / f"arctic_{s}_OFF/race1/01_grid.bmp")
    m = np.abs(ps - po) > 6
    print(f"| arctic_{s} | {100 * m.mean():.2f} | {o[m].mean():.1f} | {po[m].mean():.1f} | {ps[m].mean():.1f} | "
          f"{t[m].mean():.1f} | {abs(t[m].mean() - o[m].mean()):.1f} | {np.abs(o - t).mean():.1f} |")
