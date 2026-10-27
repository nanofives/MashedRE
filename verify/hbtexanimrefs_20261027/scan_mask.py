# PREREG2 pose pick (standalone only, no original pixels read): water mask % per scan pose,
# mask = |L(MASK) - L(OFF)| > 6. usage: py -3.12 scan_mask.py <scan_render_dir>
import sys
from pathlib import Path
import numpy as np
from PIL import Image


def luma(p):
    a = np.asarray(Image.open(p).convert("RGB"), dtype=np.float64)
    return a[..., 0] * 0.299 + a[..., 1] * 0.587 + a[..., 2] * 0.114


rd = Path(sys.argv[1])
for d in sorted(rd.glob("*_OFF")):
    case = d.name[:-4]
    off, msk = d / "race1/01_grid.bmp", rd / f"{case}_MASK" / "race1/01_grid.bmp"
    if not (off.exists() and msk.exists()):
        print(f"{case}: missing")
        continue
    m = np.abs(luma(msk) - luma(off)) > 6
    lg = rd / f"{case}_MASK" / "mashed_re.log"
    ok = "ok" if lg.exists() and "textures=1 " in lg.read_text(errors="replace") else "?"
    print(f"{case}: mask {m.mean() * 100:.2f}% masktex={ok}")
