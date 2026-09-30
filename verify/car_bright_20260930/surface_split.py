# Pose-controlled comparison of the car's LIT-SURFACE SPLIT.
#
# Both sides light the body as clamp(amb + sun*max(0, N.L)) * texel, with the
# SAME ambient (0.5 on TRAINING, measured on both sides) and the SAME sun colour
# (1.0). So a body pixel is at the "ambient floor" when N.L <= 0 and above it
# when the panel faces the light. The defect is a LIGHT DIRECTION error, so the
# right statistic is not a mean brightness (which depends on framing) but the
# FRACTION of the car's visible paint sitting on the ambient floor.
#
# floor  : ratio <= 0.58  (ambient 0.5 plus R5G6B5 quantisation and texture noise)
# sunlit : ratio >= 0.85
#
# Usage: py -3.12 verify/car_bright_20260930/surface_split.py <img> <x0,y0,x1,y1> <label>
import sys
from PIL import Image

TEXEL_R = 236.0
FLOOR, SUN = 0.58, 0.85


def split(path, box, label):
    im = Image.open(path).convert("RGB").crop(box)
    body = [p for p in im.getdata()
            if p[0] >= 32 and p[0] > p[1] * 1.6 and p[0] > p[2] * 1.6
            and p[1] > 4 and p[2] > 4]          # p[1]/p[2] > 4 drops pure-red HUD
    n = len(body)
    if n == 0:
        print(f"{label}: n_body=0"); return
    r = [p[0] / TEXEL_R for p in body]
    nf = sum(1 for v in r if v <= FLOOR)
    ns = sum(1 for v in r if v >= SUN)
    nm = n - nf - ns
    print(f"{label}")
    print(f"   n_body={n}  mean_ratio={sum(r)/n:.4f}")
    print(f"   at ambient floor (<= {FLOOR}) : {nf:6d}  {100.0*nf/n:5.1f}%")
    print(f"   mid                            : {nm:6d}  {100.0*nm/n:5.1f}%")
    print(f"   sunlit          (>= {SUN}) : {ns:6d}  {100.0*ns/n:5.1f}%")


if __name__ == "__main__":
    split(sys.argv[1], tuple(int(v) for v in sys.argv[2].split(",")), sys.argv[3])
