import csv, json, math, sys
import numpy as np
f32 = np.float32
fx = json.load(open(sys.argv[1]))
rows = list(csv.DictReader(open(sys.argv[2])))
s = [x for x in fx if x.get("hl") and x["hl"]["nq"] > 0]
a = s[len(s) // 2]
c = a["cam"]
M = [f32(v) for v in c["mod"]]
cam_at = M[8:11]
cam_pos = M[12:15]
row = rows[a["row"] - 1] if a["row"] - 1 < len(rows) else None
print("sample row", a["row"], "trace rows", len(rows))


def length(v):
    return f32(math.sqrt(float(f32(f32(v[2] * v[2]) + f32(v[1] * v[1])) + f32(v[0] * v[0]))))


def dotc(p, q):
    v = f32(f32(p[2] * q[2]) + f32(p[0] * q[0])) + f32(p[1] * q[1])
    return f32(max(-1.0, min(1.0, v)))


recs = []
for ci, car in enumerate(a["hl"]["cars"]):
    # fade from the trace car position (c{i}x..)
    if row is None:
        continue
    for rr in (rows[a["row"] - 2], rows[a["row"] - 1], rows[min(a["row"], len(rows) - 1)]):
        cp = [f32(float(rr[f"c{ci}x"])), f32(float(rr[f"c{ci}y"])), f32(float(rr[f"c{ci}z"]))]
        d = [f32(cp[k] - cam_pos[k]) for k in range(3)]
        fd = f32(f32(length(d) * f32(0.5)) - f32(1.0))
        fade = f32(-0.05) if fd > f32(-0.05) else (f32(-1.0) if fd < f32(-1.0) else fd)
        print(" car", ci, "fade from trace row", rr["t"][:6] if "t" in rr else "", float(fade))
    if not car["hlflag"]:
        continue
    for part, col, neg, scale in ((25, (255, 255, 255), False, 1.0), (26, (255, 255, 255), False, 1.0),
                                  (28, (255, 255, 255), False, 1.0), (27, (255, 255, 255), False, 1.0),
                                  (55, (255, 0, 0), True, 0.25), (56, (255, 0, 0), True, 0.25)):
        P = car["parts"].get(str(part))
        if P is None:
            continue
        P = [f32(v) for v in P]
        pat = [-P[8], -P[9], -P[10]] if neg else P[8:11]
        pp = P[12:15]
        d = [f32(pp[k] - cam_pos[k]) for k in range(3)]
        dist = length(d)
        recs.append((ci, part, pp, d, dist, pat, col, scale))
for d in a["hl"]["d"][:24]:
    print(" orig rec", [round(x, 4) for x in d["f"][:4]], hex(d["c"]))
print("---- port (fade solved per record from the orig record):")
for (ci, part, pp, d, dist, pat, col, scale), od in zip(recs, a["hl"]["d"]):
    o = od["f"]
    fade_est = [(o[k] - pp[k]) / d[k] if abs(d[k]) > 1e-4 else float('nan') for k in range(3)]
    v = dotc(pat, cam_at)
    v = v if v <= 0 else f32(0)
    alpha = int(f32(f32(f32(v * v) * f32(255.0)) * f32(scale)))
    size = f32(f32(dist * f32(0.025)) + f32(0.05))
    print(f" car{ci} part{part}: size {float(size):.5f} vs {o[3]:.5f}; fade_est {[round(x,5) for x in fade_est]}; alpha {alpha} vs {(od['c']>>24)&0xff}")
