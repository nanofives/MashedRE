# uvcheck.py <capture.json> [--rotate 0x42e65a] -- text4: the preview-frame UV law against captured vertices.
# For every 135x180 preview half-quad at (330|465, 96) in each frame, print its per-vertex (u, v) and solve the
# FUN_00474890 default/mode-1..5 law (exe_main.cpp PreviewPairQuad) for an integer p:
#   left half  u0 .. u1*0.5, right half u1*0.5 .. u1, v vt .. vb.
# Reports which modes reproduce the captured UVs exactly (float32) and the p they need.
import json, struct, sys

def f32(x): return struct.unpack("<f", struct.pack("<f", x))[0]

def law(p, mode):
    k = f32(f32(p) * f32(1.0 / 2048.0))
    if mode == 1: vt = f32(0.125 - k); vb = f32(1 - vt); u0 = vt; u1 = vb
    elif mode == 2: vt = 0.0625; vb = 0.9375; u0 = f32(0.125 - k); u1 = f32(1 - k)
    elif mode == 3:
        j = f32(0.125 - k); vt = 0.0625; vb = 0.9375; u0 = f32(0.125 - j); u1 = f32(1 - j)
    elif mode == 4:
        j = f32(0.125 - k); vt = f32(0.125 - j); vb = f32(1 - j); u0 = 0.0625; u1 = 0.9375
    elif mode == 5: vt = f32(0.125 - k); vb = f32(1 - k); u0 = 0.0625; u1 = 0.9375
    else: vt = k; vb = f32(1 - k); u0 = k; u1 = vb
    uh = f32(u1 * 0.5)
    return (u0, uh, vt, vb), (uh, f32(uh + uh), vt, vb)

def quads(path):
    d = json.load(open(path))
    for lab, recs in d.items():
        out = []
        for r in recs:
            raw = bytes.fromhex(r["v"])
            vs = [struct.unpack_from("<ffffIff", raw, i * 0x1c) for i in range(len(raw) // 0x1c)]
            xs = [v[0] for v in vs]; ys = [v[1] for v in vs]
            x, y, w, h = min(xs), min(ys), max(xs) - min(xs), max(ys) - min(ys)
            if abs(w - 135) < 0.01 and abs(h - 180) < 0.01 and abs(y - 96) < 0.01:
                us = [v[5] for v in vs]; vv = [v[6] for v in vs]
                out.append((x, vs[0][4], (min(us), max(us), min(vv), max(vv)), r.get("s")))
        yield lab, out

for lab, qs in quads(sys.argv[1]):
    print(f"== {lab}: {len(qs)} preview half-quads")
    # pair left/right halves in stream order
    for i in range(0, len(qs) - 1, 2):
        L, R = qs[i], qs[i + 1]
        print(f"  L x={L[0]:.2f} col={L[1]:08x} uv={tuple(round(v, 6) for v in L[2])} s={L[3]}")
        print(f"  R x={R[0]:.2f} col={R[1]:08x} uv={tuple(round(v, 6) for v in R[2])} s={R[3]}")
        hits = []
        for mode in range(6):
            for p in range(-256, 300):
                a, b = law(p, mode)
                if all(abs(f32(x) - f32(y)) == 0 for x, y in zip(a, L[2])) and \
                   all(abs(f32(x) - f32(y)) == 0 for x, y in zip(b, R[2])):
                    hits.append((mode, p))
        print(f"    law fits (mode, p): {hits[:6]}{' ...' if len(hits) > 6 else ''}")
    break
