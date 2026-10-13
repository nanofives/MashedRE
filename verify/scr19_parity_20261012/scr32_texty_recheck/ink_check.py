# ink_check.py <orig_frame.json> <orig.bmp> <sa_pre.bmp> <sa_post.bmp>
# Second witness, independent of the camera projection: compares the original's backbuffer ink with the
# standalone's, per original text call. It does NOT compare ink against a layout law (that is the biased use,
# memory ink-centroid-is-a-biased-witness-for-glyph-y); both sides rasterize the same FGDC20 atlas cells.
# Region = the call's projected glyph box (recheck_frame_probe vertices through the camera) padded 4 px.
# Ink = pixels within 60 (max channel distance) of the call's colour arg (raw[3], ARGB; alpha < 0x40 skipped).
# Calls whose region overlaps another visible call's region with the same colour are flagged (shadow overlap).
# Per call and build: ink box, best integer-shift IoU (dx, dy) over -4..4, ink-centroid delta (sa - orig).
import json, sys
import numpy as np
from PIL import Image
F = json.load(open(sys.argv[1]))["frame"]; cam = F["cam"]; W, H = cam["ras_w"], cam["ras_h"]
img = {k: np.asarray(Image.open(p).convert("RGB")).astype(int) for k, p in
       (("orig", sys.argv[2]), ("pre", sys.argv[3]), ("post", sys.argv[4]))}
for k, a in img.items():
    if a.shape[:2] != (480, 640): raise SystemExit(f"{k}: shape {a.shape}, expected 480x640")
def mul(p, m):
    x, y, z = p
    return (x*m[0]+y*m[4]+z*m[8]+m[12], x*m[1]+y*m[5]+z*m[9]+m[13], x*m[2]+y*m[6]+z*m[10]+m[14])
boxes = {}
for s in F["submits"]:
    for v in s["verts"]:
        p = mul(mul(v[:3], s["ltm"]), cam["view"]); x, y = p[0]/p[2]*W, p[1]/p[2]*H
        b = boxes.setdefault(s["call"], [1e9, 1e9, -1e9, -1e9])
        b[0] = min(b[0], x); b[1] = min(b[1], y); b[2] = max(b[2], x); b[3] = max(b[3], y)
vis = {}
for ci, b in boxes.items():
    c = F["calls"][ci]; col = int(c["raw"][3], 16)
    if (col >> 24) < 0x40: continue
    vis[ci] = (max(0, int(b[0]) - 4), max(0, int(b[1]) - 4), min(639, int(b[2]) + 5), min(479, int(b[3]) + 5),
               ((col >> 16) & 255, (col >> 8) & 255, col & 255))
def mask(a, r):
    x0, y0, x1, y1, rgb = r
    return np.abs(a[y0:y1+1, x0:x1+1] - np.array(rgb)).max(axis=2) < 60
def stats(m, r):
    ys, xs = np.nonzero(m)
    if len(xs) == 0: return None
    return (r[0]+xs.min(), r[0]+xs.max(), r[1]+ys.min(), r[1]+ys.max(), len(xs), r[0]+xs.mean(), r[1]+ys.mean())
print("call ret        id     colour   region            overlap | orig box / n | pre: box  bestIoU(dx,dy)  dcent(x,y) | post: same")
for ci, r in sorted(vis.items()):
    c = F["calls"][ci]
    ov = [cj for cj, q in vis.items() if cj != ci and q[4] == r[4] and not (q[2] < r[0] or q[0] > r[2] or q[3] < r[1] or q[1] > r[3])]
    mo = mask(img["orig"], r); so = stats(mo, r)
    if so is None or so[4] < 20: continue
    line = f"{ci:3d} {c['ret']} 0x{int(c['raw'][0],16):<4x} {c['raw'][3]} ({r[0]},{r[1]})-({r[2]},{r[3]}) {'OVL' if ov else '   '} | {so[0]}..{so[1]} {so[2]}..{so[3]} n{so[4]:4d}"
    for k in ("pre", "post"):
        ms = mask(img[k], r); ss = stats(ms, r)
        if ss is None: line += f" | {k}: no ink"; continue
        best = max(((float((mo & np.roll(np.roll(ms, dy, 0), dx, 1)).sum()) / max(1.0, float((mo | np.roll(np.roll(ms, dy, 0), dx, 1)).sum())), dx, dy)
                    for dy in range(-4, 5) for dx in range(-4, 5)))
        # best shift (dx,dy) is the shift APPLIED to the standalone that best aligns it, so the standalone's
        # offset relative to the original is (-dx, -dy).
        line += (f" | {k}: {ss[0]}..{ss[1]} {ss[2]}..{ss[3]} n{ss[4]:4d} IoU {best[0]:.3f} off({-best[1]:+d},{-best[2]:+d})"
                 f" dcent({ss[5]-so[5]:+.2f},{ss[6]-so[6]:+.2f})")
    print(line)
