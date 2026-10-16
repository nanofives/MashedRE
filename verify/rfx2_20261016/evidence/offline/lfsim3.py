"""Offline: the small / big lensflare batches' AABB from the witness light list, a base camera row and a
render camera row of the d1r1 trace, against the original's recorded AABB."""
import csv, json, math, sys
import numpy as np
f32 = np.float32
wit = json.load(open(sys.argv[1]))
rows = list(csv.DictReader(open(sys.argv[2])))
orig = json.load(open(sys.argv[3]))
K = int(sys.argv[4])
base_row = int(sys.argv[5]) if len(sys.argv) > 5 else 0
L = [x for x in wit if x.get("lf")][-1]["lf"]


def mat(r, p):
    return [f32(float(r[f"{p}_{k}"])) for k in ("rx", "ry", "rz", "ux", "uy", "uz", "ax", "ay", "az", "px", "py", "pz")]


def norm_len(v):
    l = math.sqrt(float(v[0]) ** 2 + float(v[1]) ** 2 + float(v[2]) ** 2)
    return f32(l), [f32(v[k] / l) for k in range(3)]


B = mat(rows[base_row], "M")
base = [f32(f32(B[6 + k] * f32(3.0)) + B[9 + k]) for k in range(3)]
C = mat(rows[K], "L")
R = [-C[0], -C[1], -C[2]]
U = [C[3], C[4], C[5]]
small, big = [], []
for e in L:
    n = e["n"]
    k = f32(e["k"])
    if e["n"] == 2 and abs(e["pos"][0] + 33.025) < 1e-3 and abs(e["pos"][2] + 42.1) < 1e-3: k = f32(float(__import__("os").environ["FIXK"]))
    l = f32(1.0) * k
    if l < 0:
        l = f32(0)
    p = [f32(x) for x in e["pos"]]
    d = [f32(p[q] - base[q]) for q in range(3)]
    ln, dn = norm_len(d)
    P = [f32(f32(ln * dn[q]) + base[q]) for q in range(3)]
    sz = [f32(x) for x in e["sz"]]
    for i in range(n):
        if i == 4:
            sx = f32(f32(f32(l * f32(10)) + f32(2)) * sz[4] * l)
            sy = f32(f32(l * f32(0.5)) * sz[4] * l)
        else:
            s = f32(f32(l + f32(1)) * sz[i] * l)
            sx = sy = s
        small.append((P, sx, sy))
    # big: mirrored y, 16 particles
    pm = [p[0], -p[1], p[2]]
    d = [f32(pm[q] - base[q]) for q in range(3)]
    ln, dn = norm_len(d)
    s = f32(f32(l + f32(0.5)) * sz[0] * l)
    for j in range(16):
        Q = [f32(f32(ln * dn[0]) + base[0]), f32(f32(f32(ln * dn[1]) + base[1]) - f32(f32(j) * l * f32(0.5))),
             f32(f32(ln * dn[2]) + base[2])]
        big.append((Q, s, s))


def aabb(ps):
    mn = [1e30] * 3
    mx = [-1e30] * 3
    for P, sx, sy in ps:
        hx, hy = sx * 0.5, sy * 0.5
        for a, b in ((-1, -1), (1, -1), (1, 1), (-1, 1)):
            v = [P[q] + a * hx * R[q] + b * hy * U[q] for q in range(3)]
            for q in range(3):
                mn[q] = min(mn[q], v[q]); mx[q] = max(mx[q], v[q])
    return [round(float(x), 4) for x in mn + mx]


print("port small", len(small), aabb(small))
print("port big  ", len(big), aabb(big))
for e in orig["events"]:
    if e.get("e") == "d" and e.get("tex") == "lensflare-fs8":
        print("orig", e["tri"] // 2, [round(x, 4) for x in e["bb"]])

