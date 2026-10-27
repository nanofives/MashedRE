# HB-ARCTIC-SKY scorer (PREREG.md metrics). Band = rows 0..79 minus the countdown box (x 200..440,
# rows 0..59); sub-band = rows 20..69, same cut; rest = rows 80..479, mean abs luma diff arm vs DEF.
# Original frames: HB-ARCTIC-OPENSEA_data/orig/<pose>/a.bmp. Standalone: <data>/render/<pose>_<arm>/race1/01_grid.bmp.
#
# usage: py -3.12 measure.py <datadir>
import sys
from pathlib import Path
import numpy as np
from PIL import Image

ORIG_DATA = Path("D:/MashedWT/HB-ARCTIC-OPENSEA_data")
POSES = ["e1", "w1", "w2", "n1"]
ARMS = ["DEF", "K1", "KA", "KL"]
W = np.array([0.299, 0.587, 0.114])


def luma(p):
    return np.asarray(Image.open(p).convert("RGB")).astype(float) @ W


def cut(r0, r1):
    m = np.zeros((480, 640), bool)
    m[r0:r1 + 1] = True
    m[0:60, 200:441] = False
    return m


BAND, SUB = cut(0, 79), cut(20, 69)
REST = np.zeros((480, 640), bool)
REST[80:] = True


def main():
    data = Path(sys.argv[1])
    out = []
    d1 = data / "render" / "e1_DEF" / "race1" / "01_grid.bmp"
    d2 = data / "render" / "e1_DEF2" / "race1" / "01_grid.bmp"
    same = d1.read_bytes() == d2.read_bytes()
    out.append(f"control e1 DEF vs DEF2 byte-identical: {same}")
    out.append("")
    out.append("| pose | arm | band | d band | sub-band | d sub | rest vs DEF |")
    out.append("|---|---|---|---|---|---|---|")
    for pose in POSES:
        o = luma(ORIG_DATA / "orig" / pose / "a.bmp")
        out.append(f"| {pose} | original | {o[BAND].mean():.1f} | | {o[SUB].mean():.1f} | | |")
        ref = luma(data / "render" / f"{pose}_DEF" / "race1" / "01_grid.bmp")
        for arm in ARMS:
            a = luma(data / "render" / f"{pose}_{arm}" / "race1" / "01_grid.bmp")
            out.append(f"| {pose} | {arm} | {a[BAND].mean():.1f} | {abs(a[BAND].mean() - o[BAND].mean()):.1f} | "
                       f"{a[SUB].mean():.1f} | {abs(a[SUB].mean() - o[SUB].mean()):.1f} | "
                       f"{np.abs(a[REST] - ref[REST]).mean():.3f} |")
    print("\n".join(out))


if __name__ == "__main__":
    main()
