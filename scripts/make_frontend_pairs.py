# Build original-vs-standalone side-by-side comparison PNGs for the frontend
# faithfulness review. For each screen id, loads verify/orig_screens/sN.bmp and
# verify/std_screens/sN.bmp, normalizes both to a common height (content is laid
# out in normalized screen coords on both sides, so aspect-correct scaling makes
# them comparable despite 640x480 vs 800x600 backbuffers), and writes a labeled
# left|right composite to verify/cmp/sN.png.
import sys
from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parent.parent
ORIG = ROOT / "verify" / "orig_screens"
STD  = ROOT / "verify" / "std_screens"
OUT  = ROOT / "verify" / "cmp"
H = 480  # common height

def load(p):
    if not p.exists():
        return None
    im = Image.open(p).convert("RGB")
    w = int(im.width * H / im.height)
    return im.resize((w, H), Image.LANCZOS)

def main():
    screens = [int(a) for a in sys.argv[1:]] or \
              [1,2,3,4,6,7,8,15,16,18,19,24,29,30,31,32,33]
    OUT.mkdir(parents=True, exist_ok=True)
    for s in screens:
        o = load(ORIG / f"s{s}.bmp")
        d = load(STD / f"s{s}.bmp")
        gap = 12
        ow = o.width if o else 320
        dw = d.width if d else 320
        canvas = Image.new("RGB", (ow + dw + gap, H + 28), (24, 24, 24))
        if o: canvas.paste(o, (0, 28))
        if d: canvas.paste(d, (ow + gap, 28))
        dr = ImageDraw.Draw(canvas)
        dr.text((4, 8), f"ORIGINAL  s{s}", fill=(0, 255, 120))
        dr.text((ow + gap + 4, 8), f"STANDALONE  s{s}", fill=(120, 200, 255))
        canvas.save(OUT / f"s{s}.png")
        print(f"s{s}: orig={'y' if o else 'MISSING'} std={'y' if d else 'MISSING'}")
    print("->", OUT)

if __name__ == "__main__":
    main()
