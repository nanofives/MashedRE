"""RWC-FRAME-JITTER-STRICT: recover the clock DAT_007f1010 behind the checker jitter of 0x00473ee0 from a draw capture.

re/match/s11/00473ee0.c: per quad (j, ii, jj), o = ((ii&1)*21 + (jj*5+45)*8, ii*21 [+416 if j]), w2 = DAT_007f1010 +
DAT_007f1010, corner offsets sin(k + w2) * 4 (x) and sin(k' + w2) * 2 (y), then x * width * 0.0015625, y * height *
0.0020833334 (FUN_0042b8b0 / FUN_0042b8c0, 640 x 480 here). The capture holds the 4 vertices of each quad.

Usage: py -3.12 recover_t.py <capture.json> [<label> ...] [--rsite 0x474464 | --rsym RwCore_00473ee0]
Prints, per frame, the coarse w2 (least squares), then every float32 T in [0, 1432) whose float32 model reproduces all
jitter vertex coordinates bit for bit (model A: float32 arithmetic, sin in double), with the match count of the best.
"""
import json
import math
import struct
import sys

import numpy as np

f32 = np.float32


def quads(frame, site):
    out = []
    for d in frame:
        r = d.get("r", [])
        if not r or not any(site(x) for x in r[:1]):
            continue
        b = bytes.fromhex(d["v"])
        vs = []
        for i in range(4):
            x, y, z, rhw, col, u, v = struct.unpack_from("<ffffIff", b, i * 28)
            vs.append((x, y, col))
        out.append(vs)
    return out


def layout():
    """(j, ii, jj) in draw order, with o.x, o.y."""
    L = []
    for j in range(2):
        for ii in range(3):
            for jj in range(7):
                ox = (ii & 1) * 21 + (jj * 5 + 45) * 8
                oy = ii * 21 + (416 if j else 0)
                L.append((j, ii, jj, ox, oy))
    return L


def model(T, j, ii, jj, ox, oy):
    """float32 model of one quad's 4 corners (x, y), screen 640 x 480."""
    T = f32(T)
    w2 = f32(T + T)
    o_x, o_y = f32(ox), f32(oy)
    px = [o_x, f32(o_x + f32(21)), o_x, f32(o_x + f32(21))]
    py = [o_y, o_y, f32(o_y + f32(21)), f32(o_y + f32(21))]
    kx = [ii + jj, ii + jj + 1, ii + jj + 1, ii + jj + 2]
    ky = [ii * 2 + jj, ii * 2 + jj + 1, ii * 2 + jj + 2, ii * 2 + jj + 3]
    for c in range(4):
        px[c] = f32(px[c] + f32(math.sin(float(f32(f32(kx[c]) + w2))) * 4.0))
        py[c] = f32(py[c] + f32(math.sin(float(f32(f32(ky[c]) + w2))) * 2.0))
    if j == 0:
        if ii == 2:
            py[3] = f32(64)
            py[2] = f32(64)
    elif ii == 0:
        py[1] = f32(416)
        py[0] = f32(416)
    sx, sy = f32(0.0015625), f32(0.0020833334)
    return [(f32(f32(f32(640) * px[c]) * sx), f32(f32(f32(480) * py[c]) * sy)) for c in range(4)]


def score(T, Q, L):
    n = 0
    tot = 0
    for q, (j, ii, jj, ox, oy) in zip(Q, L):
        m = model(T, j, ii, jj, ox, oy)
        for c in range(4):
            tot += 2
            n += (f32(q[c][0]) == m[c][0]) + (f32(q[c][1]) == m[c][1])
    return n, tot


def coarse_w2(Q, L):
    # quad (0,0,0): x0 = 360 + 4 sin(w2); quad (0,0,1)? use corner 1: 381 + 4 sin(1 + w2)
    best = None
    for s in range(0, 62832):
        w = s * 1e-4
        e = 0.0
        for q, (j, ii, jj, ox, oy) in list(zip(Q, L))[:7]:
            e += (q[0][0] - (ox + 4 * math.sin(ii + jj + w))) ** 2
            e += (q[1][0] - (ox + 21 + 4 * math.sin(ii + jj + 1 + w))) ** 2
        if best is None or e < best[0]:
            best = (e, w)
    return best[1]


def nextf(x, k):
    b = struct.unpack("<I", struct.pack("<f", x))[0] + k
    return struct.unpack("<f", struct.pack("<I", b))[0]


def main():
    args = sys.argv[1:]
    rsite = "0x474464"
    rsym = None
    if "--rsite" in args:
        i = args.index("--rsite"); rsite = args[i + 1]; del args[i:i + 2]
    if "--rsym" in args:
        i = args.index("--rsym"); rsym = args[i + 1]; del args[i:i + 2]
    mp = None
    if "--offs" in args:   # comma list of exe-relative return offsets that identify the jitter draw
        i = args.index("--offs"); mp = set(args[i + 1].split(",")); del args[i:i + 2]
    cap = json.load(open(args[0]))
    labels = args[1:] or list(cap.keys())
    site = (lambda x: x in mp) if mp else (lambda x: x == rsite)
    L = layout()
    for lab in labels:
        Q = quads(cap[lab], site)
        if len(Q) != 42:
            print(f"{lab}: {len(Q)} jitter quads (expected 42), skipped")
            continue
        w = coarse_w2(Q, L)
        hits = []
        bestT = None
        for n in range(0, 912):
            T0 = (w + 2 * math.pi * n) / 2
            if T0 >= 1432:
                break
            T0 = float(f32(T0))
            # coordinates resolve T to ~1e-5 relative to sin; scan the ulps within +-3e-4 of T0
            ulp = abs(nextf(T0, 1) - T0) or 1e-7
            span = int(3e-4 / ulp) + 2
            # first a cheap check on quad 0 corner 0 to prune
            for k in range(-span, span + 1):
                T = nextf(T0, k)
                m = model(T, *L[0])
                if f32(Q[0][0][0]) != m[0][0] or f32(Q[0][1][0]) != m[1][0]:
                    continue
                s, tot = score(T, Q, L)
                if bestT is None or s > bestT[0]:
                    bestT = (s, T)
                if s == tot:
                    hits.append(T)
        tb = struct.unpack("<I", struct.pack("<f", bestT[1]))[0] if bestT else 0
        print(f"{lab}: coarse w2 mod 2pi {w:.4f}; exact T count {len(hits)}; best {bestT} (0x{tb:08x}) of {2*4*42}")
        for T in hits[:20]:
            print(f"   T {T!r} 0x{struct.unpack('<I', struct.pack('<f', T))[0]:08x}")


if __name__ == "__main__":
    main()
