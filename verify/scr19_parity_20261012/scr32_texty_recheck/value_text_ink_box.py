# value_text_ink_box.py <orig.bmp> <sa.bmp> X0 X1 Y0 Y1
# Same method as ../scr32_fix/value_text_ink.py (ink = max(R,G,B) < 48 in a fixed box, IoU by integer shift
# -3..3, top 3), with the box and the files as arguments, plus the ink-row centroid (sa - orig).
import sys
from PIL import Image
import numpy as np
o_p, s_p = sys.argv[1], sys.argv[2]; X0, X1, Y0, Y1 = (int(v) for v in sys.argv[3:7])
def ink(p):
    a = np.asarray(Image.open(p).convert('RGB')).astype(int)
    if a.shape[1] != 640: raise SystemExit(f"{p}: width {a.shape[1]}, expected 640")
    return a[Y0:Y1+1, X0:X1+1].max(axis=2) < 48
o, s = ink(o_p), ink(s_p)
def box(m):
    ys, xs = np.nonzero(m); return (X0+xs.min(), X0+xs.max(), Y0+ys.min(), Y0+ys.max(), int(m.sum()))
print("orig ink x %d..%d y %d..%d n=%d" % box(o)); print("sa   ink x %d..%d y %d..%d n=%d" % box(s))
best = []
for dy in range(-3, 4):
    for dx in range(-3, 4):
        sh = np.roll(np.roll(s, dy, 0), dx, 1)
        best.append((float((o & sh).sum()) / float((o | sh).sum()), dx, dy))
best.sort(reverse=True)
print("IoU by shift (dx,dy) top 3:", [(f"{v:.3f}", dx, dy) for v, dx, dy in best[:3]])
yo = np.nonzero(o)[0].mean(); ys_ = np.nonzero(s)[0].mean()
print(f"ink row centroid sa - orig: {ys_ - yo:+.3f} px")
