import json, math, sys
import numpy as np
f32 = np.float32
fx = json.load(open(sys.argv[1]))
s = [x for x in fx if x.get("hl") and x["hl"]["nq"] > 0]
worst = 0.0
nchk = 0
for a in s:
    c = a["cam"]
    V = [f32(v) for v in c["view"]]
    L = [f32(v) for v in c["ltm"]]
    rvw = [f32(v) for v in c["rvw"]]
    off = [f32(v) for v in c["off"]]
    # derived RW view matrix: inv(LTM) then x' = -0.5 rvw.x x + 0.5 z (+ offsets), y' likewise
    R = np.array(L, dtype=np.float32).reshape(4, 4)
    Ri = R[:3, :3].T
    pi = -(R[3, :3] @ Ri)
    inv = np.zeros((4, 4), dtype=np.float32)
    inv[:3, :3] = Ri
    inv[3, :3] = pi
    D = inv.copy()
    D[:, 0] = f32(-0.5) * rvw[0] * inv[:, 0] + f32(0.5) * inv[:, 2]
    D[:, 1] = f32(-0.5) * rvw[1] * inv[:, 1] + f32(0.5) * inv[:, 2]
    Vm = np.array(V, dtype=np.float32).reshape(4, 4)
    dv = float(np.max(np.abs(D[:, :3] - Vm[:, :3])))
    nf = [f32(v) for v in c["nf"]]
    zb = [f32(v) for v in a["zbuf"]]
    w, h = f32(c["ras"][0]), f32(c["ras"][1])
    qi = 0
    for d in a["hl"]["d"]:
        p = [f32(x) for x in d["f"][:3]]
        size = f32(d["f"][3])
        use = Vm
        cx = f32(f32(p[0] * use[0, 0] + p[1] * use[1, 0]) + f32(p[2] * use[2, 0])) + use[3, 0]
        cy = f32(f32(p[0] * use[0, 1] + p[1] * use[1, 1]) + f32(p[2] * use[2, 1])) + use[3, 1]
        cz = f32(f32(p[0] * use[0, 2] + p[1] * use[1, 2]) + f32(p[2] * use[2, 2])) + use[3, 2]
        if not (nf[0] < cz < nf[1]):
            continue
        r = f32(f32(1.0) / cz)
        sx = f32(f32(r * w) * cx)
        sy = f32(f32(r * h) * cy)
        ip = f32(f32(1.0) / f32(1.6))
        hw = f32(f32(f32(f32(f32(f32(w / h) * w) * ip) * f32(0.8)) * size) * r)
        hh = f32(f32(f32(f32(ip * h) * f32(0.8)) * size) * r)
        if not (-hw < sx < hw + w and -hh < sy < hh + h):
            continue
        x0, x1, y0, y1 = f32(sx - hw), f32(hw + sx), f32(sy - hh), f32(hh + sy)
        x0 = max(x0, f32(0)); y0 = max(y0, f32(0)); x1 = min(x1, w); y1 = min(y1, h)
        z = f32(f32(f32(f32(cz - nf[0]) * f32(nf[1] / cz)) * f32(f32(zb[1] - zb[0]) / f32(nf[1] - nf[0]))) + zb[0])
        if qi >= len(a["hl"]["q"]):
            print("extra quad"); break
        q = a["hl"]["q"][qi]
        err = max(abs(q[0] - x0), abs(q[7] - x1), abs(q[1] - y0), abs(q[15] - y1), abs(q[2] - z), abs(q[3] - r))
        worst = max(worst, err)
        nchk += 1
        qi += 1
    if qi != a["hl"]["nq"]:
        print("row", a["row"], "quad count port", qi, "orig", a["hl"]["nq"])
print("samples", len(s), "quads checked", nchk, "worst abs err", worst, "view-matrix formula max diff", dv)
