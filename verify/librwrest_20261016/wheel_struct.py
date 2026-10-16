"""librwrest W2 (structure), from EXISTING original captures: for every car draw in the d1r1
original draw records (orig_row_<K>.draws.json, D3DTS_WORLD per draw) and the trace row K's car
root LTM (Q{i}_*), T = W * Q^-1 (RW row-vector convention) is the draw's frame chain below the
clump root. FUN_0041f3d0 @ 0x0041f3d0 poses the wheel atomic's frame (REPLACE, rotation about X)
and its parent's (REPLACE, rotation about Y, position restored), and the DFF puts those parents
(ADVANTAGE0 F3..F10) under F1 at the root. So a wheel draw must give T.pos = a hub frame's streamed
position and T.rot = Rx(a) * Ry(b) (wheel first, then hub): T.rot[0][1] == 0 and the right vector
has no y. The other order (Ry * Rx) has right.y = sin(a) sin(b) != 0 whenever both angles are.

    py -3.12 wheel_struct.py <trace.csv> <orig_dir> K1,K2,... [out.txt]
"""
import csv
import json
import math
import sys
from pathlib import Path

HUBS = {  # ADVANTAGE0.DFF frame -> streamed position (dffframes dump, this session)
    3: (0.1803, 0.0905, -0.3500), 4: (-0.1803, 0.0905, -0.3500), 5: (-0.1682, 0.0905, 0.2750),
    6: (0.1682, 0.0905, 0.2750), 7: (-0.1713, 0.0905, 0.2750), 8: (0.1714, 0.0905, 0.2750),
    9: (-0.1806, 0.0905, -0.3500), 10: (0.1806, 0.0905, -0.3500)}
M12 = [f"{a}{b}" for a in ("r", "u", "a", "p") for b in ("x", "y", "z")]


def inv_aff(m):  # rows right, up, at (3x3) + pos
    R = [m[0:3], m[3:6], m[6:9]]
    det = (R[0][0] * (R[1][1] * R[2][2] - R[1][2] * R[2][1]) - R[0][1] * (R[1][0] * R[2][2] - R[1][2] * R[2][0])
           + R[0][2] * (R[1][0] * R[2][1] - R[1][1] * R[2][0]))
    I = [[0] * 3 for _ in range(3)]
    for r in range(3):
        for c in range(3):
            a = [[R[i][j] for j in range(3) if j != r] for i in range(3) if i != c]
            I[r][c] = ((-1) ** (r + c)) * (a[0][0] * a[1][1] - a[0][1] * a[1][0]) / det
    p = m[9:12]
    ip = [-(p[0] * I[0][c] + p[1] * I[1][c] + p[2] * I[2][c]) for c in range(3)]
    return [I[0][0], I[0][1], I[0][2], I[1][0], I[1][1], I[1][2], I[2][0], I[2][1], I[2][2]] + ip


def mul(a, b):  # a then b, row vectors
    o = [0.0] * 12
    for r in range(3):
        for c in range(3):
            o[r * 3 + c] = sum(a[r * 3 + k] * b[k * 3 + c] for k in range(3))
    for c in range(3):
        o[9 + c] = a[9] * b[c] + a[10] * b[3 + c] + a[11] * b[6 + c] + b[9 + c]
    return o


def main():
    trace, od, shots = sys.argv[1], Path(sys.argv[2]), [int(s) for s in sys.argv[3].split(",")]
    rows = list(csv.DictReader(open(trace)))
    out = []
    worst_pos, worst_ry, n = 0.0, 0.0, 0
    for K in shots:
        ev = json.loads((od / f"orig_row_{K:05d}.draws.json").read_text())["events"]
        draws = [e for e in ev if e.get("e") == "d" and str(e.get("tex", "")).lower().startswith("advantage")]
        r = rows[K]
        Q = [[float(r[f"Q{i}_{k}"]) for k in M12] for i in range(4)]
        Qi = [inv_aff(q) for q in Q]
        for d in draws:
            w = d["w"]
            W = [w[0], w[1], w[2], w[4], w[5], w[6], w[8], w[9], w[10], w[12], w[13], w[14]]
            best = None
            for i in range(4):
                T = mul(W, Qi[i])
                for f, hp in HUBS.items():
                    dp = max(abs(T[9 + j] - hp[j]) for j in range(3))
                    if best is None or dp < best[0]:
                        best = (dp, i, f, T)
            dp, i, f, T = best
            if dp > 2e-3:
                continue   # not a wheel draw (body subtree / other part)
            n += 1
            ry = abs(T[1])            # right.y: 0 for Rx * Ry
            a = math.degrees(math.atan2(T[5], T[4]))   # spin about X from the up row (y, z)
            b = math.degrees(math.atan2(-T[2], T[0]))  # steer about Y from the right row (x, z)
            worst_pos, worst_ry = max(worst_pos, dp), max(worst_ry, ry)
            out.append(f"K={K} car{i} hubF{f} tri={d.get('tri')} |dpos|={dp:.2e} right.y={T[1]:+.2e} "
                       f"spin~{a:+8.2f} steer~{b:+7.2f}")
    out.append(f"wheel draws: {n}; max |T.pos - hub pos| = {worst_pos:.2e}; max |right.y| = {worst_ry:.2e}")
    s = "\n".join(out)
    print(s)
    if len(sys.argv) > 4:
        Path(sys.argv[4]).write_text(s + "\n")


if __name__ == "__main__":
    main()
