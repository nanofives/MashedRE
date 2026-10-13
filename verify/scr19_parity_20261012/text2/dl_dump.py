# dl_dump.py <dl.json> [frame_key] -- list original draw-list quads (menu_draw_burst format):
# per record x0,y0,x1,y1 (vertex bbox), colour of vertex 0, u/v bbox, and the return chain.
import json, struct, sys
j = json.load(open(sys.argv[1]))
key = sys.argv[2] if len(sys.argv) > 2 else sorted(j)[0]
for i, rec in enumerate(j[key]):
    b = bytes.fromhex(rec["v"]); n = len(b) // 28
    vs = [struct.unpack_from("<4fI2f", b, 28 * k) for k in range(n)]
    xs = [v[0] for v in vs]; ys = [v[1] for v in vs]; us = [v[5] for v in vs]; vv = [v[6] for v in vs]
    print(f"{i:3d} x {min(xs):8.3f}..{max(xs):8.3f} y {min(ys):8.3f}..{max(ys):8.3f} "
          f"col {vs[0][4]:08x} u {min(us):.4f}..{max(us):.4f} v {min(vv):.4f}..{max(vv):.4f} n{n} r {' '.join(rec['r'][:3])}")
