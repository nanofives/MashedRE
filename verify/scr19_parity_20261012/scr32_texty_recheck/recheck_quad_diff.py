# recheck_quad_diff.py <orig_frame.json> <sa_pre.json> <sa_post.json> [--json out.json]
# Original glyph quads: recheck_frame_probe.py vertices through the camera (p = local*ltm, p' = p*view,
# pixel = (p'.x/p'.z*W, p'.y/p'.z*H)); orig_law_check.py shows this equals the camera-free byte law to 2e-5 px.
# Standalone glyph quads: drawstream records with s[0] == 9 (the menu font texture), first frame key,
# vertex <4fI2f> = x, y, z, rhw, colour, u, v.
# Matching (per standalone build): candidate pairs = same atlas cell (u_min and v_min within 1e-3) and
# |dx0| + |dy0| < 40 px; assigned globally nearest-first, each quad used once.
# Per original text call: glyphs, matched, median dx (x_min), dy (y_min), dh (height), dw (width),
# standalone minus original, for pre and post.
import json, struct, sys, statistics as st
def load_orig(path):
    F = json.load(open(path))["frame"]; cam = F["cam"]; W, H = cam["ras_w"], cam["ras_h"]
    def mul(p, m):
        x, y, z = p
        return (x*m[0]+y*m[4]+z*m[8]+m[12], x*m[1]+y*m[5]+z*m[9]+m[13], x*m[2]+y*m[6]+z*m[10]+m[14])
    g = []
    for s in F["submits"]:
        pts = []
        for v in s["verts"]:
            p = mul(mul(v[:3], s["ltm"]), cam["view"]); pts.append((p[0]/p[2]*W, p[1]/p[2]*H, v[3], v[4]))
        for q in range(0, len(pts) - 3, 4):
            gg = pts[q:q+4]
            g.append(dict(call=s["call"], x0=min(p[0] for p in gg), x1=max(p[0] for p in gg),
                          y0=min(p[1] for p in gg), y1=max(p[1] for p in gg),
                          u=min(p[2] for p in gg), v=min(p[3] for p in gg)))
    return F, g
def load_sa(path):
    d = json.load(open(path)); k = sorted(d)[0]; out = []
    for rec in d[k]:
        if not rec.get("s") or rec["s"][0] != 9: continue
        b = bytes.fromhex(rec["v"]); vs = [struct.unpack_from("<4fI2f", b, j*28) for j in range(len(b)//28)]
        out.append(dict(x0=min(v[0] for v in vs), x1=max(v[0] for v in vs), y0=min(v[1] for v in vs),
                        y1=max(v[1] for v in vs), u=min(v[5] for v in vs), v=min(v[6] for v in vs)))
    return out
def match(og, sg):
    pairs = []
    for i, g in enumerate(og):
        for j, s in enumerate(sg):
            if abs(s["u"]-g["u"]) > 1e-3 or abs(s["v"]-g["v"]) > 1e-3: continue
            dist = abs(s["x0"]-g["x0"]) + abs(s["y0"]-g["y0"])
            if dist < 40: pairs.append((dist, i, j))
    pairs.sort(); ui, uj, m = set(), set(), {}
    for dist, i, j in pairs:
        if i in ui or j in uj: continue
        ui.add(i); uj.add(j); m[i] = j
    return m
F, og = load_orig(sys.argv[1]); pre = load_sa(sys.argv[2]); post = load_sa(sys.argv[3])
mp, mq = match(og, pre), match(og, post)
med = lambda a: st.median(a) if a else None
rows = []
for ci in sorted(set(g["call"] for g in og)):
    idx = [i for i, g in enumerate(og) if g["call"] == ci]
    c = F["calls"][ci] if ci >= 0 else {}
    pen = F["pens"][c["pen"]] if c.get("pen") is not None else {}
    r = dict(call=ci, fn=c.get("fn"), ret=c.get("ret"), id=int(c["raw"][0], 16) if c else None,
             x=pen.get("x"), y=pen.get("y"), scale=pen.get("scale"), align=pen.get("align"), glyphs=len(idx))
    for tag, m, sg in (("pre", mp, pre), ("post", mq, post)):
        ms = [(og[i], sg[m[i]]) for i in idx if i in m]
        r[tag] = dict(matched=len(ms), dx=med([s["x0"]-g["x0"] for g, s in ms]), dy=med([s["y0"]-g["y0"] for g, s in ms]),
                      dh=med([(s["y1"]-s["y0"])-(g["y1"]-g["y0"]) for g, s in ms]),
                      dw=med([(s["x1"]-s["x0"])-(g["x1"]-g["x0"]) for g, s in ms]),
                      ady=[abs(s["y0"]-g["y0"]) for g, s in ms])
    rows.append(r)
f = lambda v: "   n/a  " if v is None else f"{v:+8.4f}"
print(f"orig glyphs {len(og)}  sa pre {len(pre)} (matched {len(mp)})  sa post {len(post)} (matched {len(mq)})")
print("call fn     ret        id     x      y      s     al  n | pre: m  dx       dy       dh      | post: m  dx       dy       dh       dw      | |dy| grows")
for r in rows:
    a, b = r["pre"], r["post"]
    grows = (a["dy"] is not None and b["dy"] is not None and abs(b["dy"]) > abs(a["dy"]) + 1e-3)
    print(f"{r['call']:3d} {r['fn']} {r['ret']} 0x{r['id']:<4x} {r['x'] or 0:6.1f} {r['y'] or 0:6.1f} {r['scale'] or 0:.3f} {r['align']} {r['glyphs']:3d} |"
          f" {a['matched']:3d} {f(a['dx'])} {f(a['dy'])} {f(a['dh'])} | {b['matched']:3d} {f(b['dx'])} {f(b['dy'])} {f(b['dh'])} {f(b['dw'])} | {'GROWS' if grows else ''}")
for tag in ("pre", "post"):
    allady = [v for r in rows for v in r[tag]["ady"]]
    if allady: print(f"{tag}: matched {len(allady)} mean|dy| {sum(allady)/len(allady):.4f} max|dy| {max(allady):.4f}")
if "--json" in sys.argv:
    for r in rows:
        for t in ("pre", "post"): r[t].pop("ady")
    json.dump(rows, open(sys.argv[sys.argv.index("--json")+1], "w"), indent=1)
