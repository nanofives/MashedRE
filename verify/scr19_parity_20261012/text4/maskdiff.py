# maskdiff.py <orig.bmp> <sa_base.bmp> <sa_t4.bmp> -- text4 pixel check restricted to pixels the new draws own
# opaquely, so the time-varying video backdrop and the preview crossfade phase do not enter:
#   preview_borders  the four FUN_0042e8b0 border quads (black, alpha a = 0xff)
#   vehicle_opaque   FUN_004739f0 quad 456,144 152x152, where car1 (TRACKIMAGES.TXD, point filter) has alpha 255
#   pu_frame_opaque  the four 48x48 "Powerupshadow" quads (558-38c, 372), where its alpha is 191 (its maximum,
#                    so 1/4 of the backdrop still shows through), minus the
#                    32x32 icon quads drawn over them (562-38c, 376)
# Textures are tex/car1.png and tex/PowerupShadow.png (txd_peek.py decodes of the TXDs the original reads).
# Prints per region: pixel count, mean abs RGB diff, max channel diff, share of pixels with a channel diff > 16.
import sys
from PIL import Image
from pathlib import Path

HERE = Path(__file__).resolve().parent
o, b, t = (Image.open(p).convert("RGB") for p in sys.argv[1:4])

def rect_pixels(x, y, w, h):
    return [(i, j) for j in range(y, y + h) for i in range(x, x + w)]

def tex_mask(png, x, y, size, amin=255):
    a = Image.open(png).convert("RGBA").split()[3].resize((size, size), Image.NEAREST)
    return [(x + i, y + j) for j in range(size) for i in range(size) if a.getpixel((i, j)) >= amin]

regions = {}
regions["preview_borders"] = sorted(set(rect_pixels(328, 94, 2, 184) + rect_pixels(328, 94, 274, 2) +
                                        rect_pixels(328, 276, 274, 2) + rect_pixels(600, 94, 2, 182)))
regions["vehicle_opaque"] = tex_mask(HERE / "pixel" / "tex" / "car1.png", 456, 144, 152)
icons = set()
for c in range(4):
    icons |= set(rect_pixels(562 - 38 * c, 376, 32, 32))
pf = []
for c in range(4):
    pf += [p for p in tex_mask(HERE / "pixel" / "tex" / "PowerupShadow.png", 558 - 38 * c, 372, 48, 191) if p not in icons]
regions["pu_frame_opaque"] = pf

print(f"{'region':16} {'px':>6} | {'base mean':>9} {'max':>4} {'>16':>7} | {'t4 mean':>8} {'max':>4} {'>16':>7}")
for name, pts in regions.items():
    row = [f"{name:16} {len(pts):6}"]
    for im in (b, t):
        tot = mx = over = 0
        for p in pts:
            a1 = o.getpixel(p); a2 = im.getpixel(p)
            d = [abs(u - v) for u, v in zip(a1, a2)]
            tot += sum(d); mx = max(mx, max(d)); over += max(d) > 16
        row.append(f"{tot / (3 * len(pts)):9.2f} {mx:4} {over / len(pts) * 100:6.2f}%")
    print(" | ".join(row))
