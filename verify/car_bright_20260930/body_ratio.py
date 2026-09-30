# Measure car-body paint brightness as a RATIO to the source texel.
#
# The `Advantage` TXD paint swatch decodes to (236,52,60)
# (re/analysis/CAR_GRAY_CHASSIS_2026-09-29.md M5, crop_txd_swatches.png -- this
# script re-measures it if given --texel-from). Both sides render that texel
# modulated by the lit vertex colour, so `pixel / texel` IS the lighting term.
#
# Selector: "red paint" = R dominant over G and B by >=1.6x and R>=32. On the
# original the backbuffer is R5G6B5, so values are quantised (R,B to /8, G to
# /4); the ratio tolerance in CAR_BRIGHTNESS_2026-09-30.md P3 accounts for it.
#
# Usage:
#   py -3.12 verify/car_bright_20260930/body_ratio.py <img> [x0,y0,x1,y1] [label]
import sys, collections
from PIL import Image

TEXEL = (236.0, 52.0, 60.0)


def measure(path, box=None, label=None):
    im = Image.open(path).convert("RGB")
    W, H = im.size
    if box:
        im = im.crop(box)
    px = list(im.getdata())
    body = [p for p in px if p[0] >= 32 and p[0] > p[1] * 1.6 and p[0] > p[2] * 1.6]
    lbl = label or path
    if not body:
        print(f"{lbl}: img={W}x{H} box={box} n_body=0  NO PAINT PIXELS")
        return
    n = len(body)
    cnt = collections.Counter(body)
    dom, domn = cnt.most_common(1)[0]
    mr = sum(p[0] for p in body) / n
    mg = sum(p[1] for p in body) / n
    mb = sum(p[2] for p in body) / n
    rs = sorted(p[0] for p in body)
    p50, p90, p99 = rs[n // 2], rs[int(.90 * n)], rs[min(n - 1, int(.99 * n))]
    print(f"{lbl}: img={W}x{H} box={box} n_body={n}")
    print(f"   dominant   = {dom}  (n={domn}, {100.0*domn/n:.1f}% of body)")
    print(f"      ratio/texel = ({dom[0]/TEXEL[0]:.4f}, {dom[1]/TEXEL[1]:.4f}, "
          f"{dom[2]/TEXEL[2]:.4f})")
    print(f"   mean       = ({mr:.1f},{mg:.1f},{mb:.1f})")
    print(f"      ratio/texel = ({mr/TEXEL[0]:.4f}, {mg/TEXEL[1]:.4f}, "
          f"{mb/TEXEL[2]:.4f})")
    print(f"   R p50={p50} ({p50/TEXEL[0]:.4f})  p90={p90} ({p90/TEXEL[0]:.4f})  "
          f"p99={p99} ({p99/TEXEL[0]:.4f})")
    print("   top6: " + ", ".join(
        f"{c}->{c[0]/TEXEL[0]:.3f} x{k}" for c, k in cnt.most_common(6)))


if __name__ == "__main__":
    path = sys.argv[1]
    box = None
    label = None
    if len(sys.argv) > 2 and "," in sys.argv[2]:
        box = tuple(int(v) for v in sys.argv[2].split(","))
    if len(sys.argv) > 3:
        label = sys.argv[3]
    measure(path, box, label)
