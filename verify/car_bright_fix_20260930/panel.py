# Side-by-side crop panel for the A2/A4 before/after evidence.
#
# Usage: py -3.12 .../panel.py <out.png> <scale> <label>=<img>:<x0,y0,x1,y1> [...]
import sys
from PIL import Image, ImageDraw


def main():
    out = sys.argv[1]
    scale = int(sys.argv[2])
    crops, labels = [], []
    for spec in sys.argv[3:]:
        label, _, rest = spec.partition("=")
        path, _, bs = rest.rpartition(":")
        box = tuple(int(v) for v in bs.split(","))
        c = Image.open(path).convert("RGB").crop(box)
        c = c.resize((c.width * scale, c.height * scale), Image.NEAREST)
        crops.append(c)
        labels.append(label)
    pad, bar = 8, 18
    W = sum(c.width for c in crops) + pad * (len(crops) + 1)
    H = max(c.height for c in crops) + pad * 2 + bar
    im = Image.new("RGB", (W, H), (24, 24, 24))
    d = ImageDraw.Draw(im)
    x = pad
    for c, lb in zip(crops, labels):
        im.paste(c, (x, pad + bar))
        d.text((x, pad // 2), lb, fill=(235, 235, 235))
        x += c.width + pad
    im.save(out)
    print(f"-> {out} {im.size}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
