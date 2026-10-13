# Standalone glyph quads (tex 9) from a MASHED_DBG_DRAWSTREAM json: first frame key, vertex = x,y,z,rhw,col,u,v.
# Usage: py -3.12 sa_text_quads.py <drawstream.json> [x0 x1 y0 y1]   (box filter on the quad's top-left)
import json, struct, sys
d = json.load(open(sys.argv[1]))
box = [float(v) for v in sys.argv[2:6]] if len(sys.argv) >= 6 else None
k = sorted(d)[0]
for i, rec in enumerate(d[k]):
    if not rec.get("s") or rec["s"][0] != 9: continue
    b = bytes.fromhex(rec["v"]); n = len(b) // 28
    vs = [struct.unpack_from("<4fI2f", b, j * 28) for j in range(n)]
    xs = [v[0] for v in vs]; ys = [v[1] for v in vs]
    if box and not (box[0] <= min(xs) <= box[1] and box[2] <= min(ys) <= box[3]): continue
    print(f"{k}[{i}] x {min(xs):.4f}..{max(xs):.4f} y {min(ys):.4f}..{max(ys):.4f} uv0 ({vs[0][5]:.4f},{vs[0][6]:.4f})")
