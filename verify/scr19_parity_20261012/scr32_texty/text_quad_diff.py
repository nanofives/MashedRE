# Glyph-quad diff: original (text_frame_probe.py json, projected) vs standalone (drawstream tex 9 quads).
# Match: same atlas cell (u_min, v_min within 1e-3), nearest top-left within 40 px. Reports per original
# text call the glyph count, matched count and the median dx / dy (standalone - original) of x_min / y_min,
# and cell height delta. Usage: py -3.12 text_quad_diff.py <orig_frame.json> <sa_drawstream.json>
import json, struct, sys, statistics as st
o = json.load(open(sys.argv[1]))["frame"]; d = json.load(open(sys.argv[2]))
cam = o["cam"]; W, H = cam["ras_w"], cam["ras_h"]
def mul(p, m):
    x, y, z = p
    return (x*m[0] + y*m[4] + z*m[8] + m[12], x*m[1] + y*m[5] + z*m[9] + m[13], x*m[2] + y*m[6] + z*m[10] + m[14])
og = []
for s in o["submits"]:
    pts = []
    for v in s["verts"]:
        p = mul(mul(v[:3], s["ltm"]), cam["view"]); pts.append((p[0]/p[2]*W, p[1]/p[2]*H, v[3], v[4]))
    for q in range(0, len(pts) - 3, 4):
        g = pts[q:q+4]
        og.append(dict(call=s["call"], x0=min(p[0] for p in g), y0=min(p[1] for p in g), y1=max(p[1] for p in g),
                       u=min(p[2] for p in g), v=min(p[3] for p in g)))
k = sorted(d)[0]; sg = []
for rec in d[k]:
    if not rec.get("s") or rec["s"][0] != 9: continue
    b = bytes.fromhex(rec["v"])
    vs = [struct.unpack_from("<4fI2f", b, j*28) for j in range(len(b)//28)]
    sg.append(dict(x0=min(v[0] for v in vs), y0=min(v[1] for v in vs), y1=max(v[1] for v in vs),
                   u=min(v[5] for v in vs), v=min(v[6] for v in vs), used=False))
per = {}
for g in og:
    best = None
    for s in sg:
        if s["used"] or abs(s["u"]-g["u"]) > 1e-3 or abs(s["v"]-g["v"]) > 1e-3: continue
        dist = abs(s["x0"]-g["x0"]) + abs(s["y0"]-g["y0"])
        if dist < 40 and (best is None or dist < best[0]): best = (dist, s)
    r = per.setdefault(g["call"], dict(n=0, m=0, dx=[], dy=[], dh=[]))
    r["n"] += 1
    if best:
        s = best[1]; s["used"] = True; r["m"] += 1
        r["dx"].append(s["x0"]-g["x0"]); r["dy"].append(s["y0"]-g["y0"]); r["dh"].append((s["y1"]-s["y0"])-(g["y1"]-g["y0"]))
alldy = []
print(f"orig glyphs {len(og)}  sa glyphs {len(sg)}  sa unmatched {sum(not s['used'] for s in sg)}")
for ci in sorted(per):
    c = o["calls"][ci] if ci >= 0 else {}; r = per[ci]
    med = lambda a: f"{st.median(a):+.4f}" if a else "   n/a"
    print(f"call {ci:2d} fn {c.get('fn','?')} ret {c.get('ret','?')} id 0x{c.get('id',0):x} x {c.get('x',0):.1f} "
          f"y {c.get('y',0):.1f} s {c.get('scale',0):.3f} a {c.get('align','?')}: glyphs {r['n']} matched {r['m']} "
          f"dx {med(r['dx'])} dy {med(r['dy'])} dh {med(r['dh'])}")
    alldy += r["dy"]
if alldy:
    print(f"ALL matched {len(alldy)}: dy median {st.median(alldy):+.4f} min {min(alldy):+.4f} max {max(alldy):+.4f} "
          f"mean|dy| {sum(abs(v) for v in alldy)/len(alldy):.4f}")
