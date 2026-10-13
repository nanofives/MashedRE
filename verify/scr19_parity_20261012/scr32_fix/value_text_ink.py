# Value-text ink check for the scr32 toggle row (tex 9 is outside the draw-list diff).
# Ink = max(R,G,B) < 48 inside x 374..410, y 225..255 (between the arrows at 356+16 and 411).
import sys
from PIL import Image
import numpy as np
X0, X1, Y0, Y1 = 374, 410, 225, 255
def ink(p):
    a = np.asarray(Image.open(p).convert('RGB')).astype(int)
    if a.shape[1] != 640: raise SystemExit(f"{p}: width {a.shape[1]}, expected 640")
    return a[Y0:Y1+1, X0:X1+1].max(axis=2) < 48
o, s = ink('orig_scr32_bb.bmp'), ink('sa_scr32_bb.bmp')
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
