# Project the original's captured Im3D glyph vertices (text_y_probe.py json) to screen pixels.
# RW perspective camera: p = local * ltm (RwMatrix rows right/up/at/pos), p' = p * view (cam+0x20),
# u = p'.x / p'.z, v = p'.y / p'.z, pixel = (u * ras_w, v * ras_h). Camera frame LTM is folded into ltm
# by FUN_00552e40 (its tail: FUN_004c4600(&0x00912b98, &0x00912b18, FUN_004c0ed0(camera frame))).
import json, sys
d = json.load(open(sys.argv[1]))["call"]
print("args", d["args"])
def mul(p, m):
    x, y, z = p
    return (x*m[0] + y*m[4] + z*m[8] + m[12], x*m[1] + y*m[5] + z*m[9] + m[13], x*m[2] + y*m[6] + z*m[10] + m[14])
for s in d["submits"]:
    c = s["cam"]; W, H = c["ras_w"], c["ras_h"]
    pts = []
    for v in s["verts"]:
        p = mul(mul(v[:3], s["ltm"]), c["view"])
        pts.append((p[0] / p[2] * W, p[1] / p[2] * H))
    for q in range(0, len(pts), 4):
        g = pts[q:q+4]; xs = [p[0] for p in g]; ys = [p[1] for p in g]
        print(f"glyph {q//4}: x {min(xs):.4f}..{max(xs):.4f} y {min(ys):.4f}..{max(ys):.4f} "
              f"uv0 ({s['verts'][q][3]:.4f},{s['verts'][q][4]:.4f})")
