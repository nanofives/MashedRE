# regiondiff.py <orig.bmp> <sa_base.bmp> <sa_t4.bmp> <tag> -- text4 pixel check of the new s18/s24 draws.
# Crops each region (640x480 virtual = device here) from all three images, writes the crops as PNG
# (<tag>_<region>_{orig,base,t4}.png, for re/tools/imgdiff.py), and prints per region the mean abs RGB
# difference and the share of pixels whose max channel diff exceeds 16, for base and t4 against the original.
import sys
from PIL import Image, ImageChops

REGIONS = {
    "preview_frame": (326, 92, 604, 280),   # FUN_0042e8b0: 330,96 270x180 + borders 328..602, 94..278
    "panel_box":     (32, 300, 608, 396),   # 0x00436991: 32,300 576x96 (cars/vs inside are unchanged textures)
    "powerup_row":   (440, 368, 610, 424),  # frames 558-38c,372 48x48 + icons
    "vehicle":       (456, 144, 608, 296),  # 0x0043bcf3: 456,144 152x152
    "band_right":    (384, 0, 640, 64),     # FUN_00473ee0 band at slide-128 (base 384, original 928)
}

def stats(a, b):
    d = ImageChops.difference(a, b)
    px = list(d.getdata())
    n = len(px)
    mean = sum(sum(p) for p in px) / (3.0 * n)
    over = sum(1 for p in px if max(p) > 16) / n * 100.0
    return mean, over

o, b, t = (Image.open(p).convert("RGB") for p in sys.argv[1:4])
tag = sys.argv[4]
print(f"{'region':14} {'base mean':>9} {'base >16':>9} {'t4 mean':>9} {'t4 >16':>9}")
for name, box in REGIONS.items():
    co, cb, ct = o.crop(box), b.crop(box), t.crop(box)
    for im, side in ((co, "orig"), (cb, "base"), (ct, "t4")):
        im.save(f"{tag}_{name}_{side}.png")
    mb, ob = stats(co, cb)
    mt, ot = stats(co, ct)
    print(f"{name:14} {mb:9.2f} {ob:8.2f}% {mt:9.2f} {ot:8.2f}%")
