#!/usr/bin/env python3
"""R-PIX: mean absolute pixel difference (0-255, all channels) between the original's frame K and the
standalone's frame K for each arm, on the nine frames. Same metric family as re/tools/imgdiff.py.

    py -3.12 score_pix.py <arm> [<arm> ...] [--out file]

Original BMPs: main checkout log/d1r1/orig_<track>/orig_row_<K>.bmp (read only).
Standalone BMPs: log/rprops/<arm>_<track>/d1r2/stand_row_<K>.bmp.
"""
import sys
from pathlib import Path

from PIL import Image, ImageChops, ImageStat

WT = Path(__file__).resolve().parents[2]
MAIN = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
FRAMES = {"training": (884, 924, 959), "arctic": (1051, 1091, 1126), "city": (761, 801, 836)}


def mad(a, b):
    ia, ib = Image.open(a).convert("RGB"), Image.open(b).convert("RGB")
    if ib.size != ia.size:
        ib = ib.resize(ia.size, Image.BILINEAR)
    d = ImageChops.difference(ia, ib)
    return sum(ImageStat.Stat(d).mean) / 3.0


def main():
    args = [a for a in sys.argv[1:] if not a.startswith("--")]
    outp = None
    if "--out" in sys.argv:
        outp = Path(sys.argv[sys.argv.index("--out") + 1])
        args = [a for a in args if a != str(outp)]
    lines = [f"{'frame':<14}" + "".join(f"{a:>10}" for a in args)]
    for trk, ks in FRAMES.items():
        for k in ks:
            o = MAIN / "log" / "d1r1" / f"orig_{trk}" / f"orig_row_{k:05d}.bmp"
            row = f"{trk + ' ' + str(k):<14}"
            for arm in args:
                s = WT / "log" / "rprops" / f"{arm}_{trk}" / "d1r2" / f"stand_row_{k:05d}.bmp"
                row += f"{mad(o, s):>10.3f}" if s.exists() and o.exists() else f"{'-':>10}"
            lines.append(row)
    txt = "\n".join(lines)
    print(txt)
    if outp:
        outp.write_text(txt + "\n")


if __name__ == "__main__":
    main()
