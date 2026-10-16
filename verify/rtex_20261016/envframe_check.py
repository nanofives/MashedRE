"""Check the env frame (car record +0x27c) against FUN_00420870 @ 0x00420870:
RwFrameRotate(frame, axis, rec+0x280, rwCOMBINEREPLACE) => modelling = pure rotation of
rec+0x280 degrees; LTM == modelling when the frame has no parent.

    py -3.12 envframe_check.py <trace.csv.matfx.json>
"""
import json
import math
import sys

for sn in json.load(open(sys.argv[1])):
    if "s" not in sn:
        continue
    for i, c in enumerate(sn["s"]["cars"]):
        m, l = c.get("env_m"), c.get("env_ltm")
        if not m:
            continue
        R = [m[0:3], m[3:6], m[6:9]]
        tr = R[0][0] + R[1][1] + R[2][2]
        ang = math.degrees(math.acos(max(-1.0, min(1.0, (tr - 1) / 2))))
        # rotation axis from the antisymmetric part (row-vector RW matrix: v' = v R)
        ax = [R[1][2] - R[2][1], R[2][0] - R[0][2], R[0][1] - R[1][0]]
        n = math.sqrt(sum(a * a for a in ax)) or 1.0
        same = max(abs(a - b) for a, b in zip(m, l)) if l else None
        want = c["f280"] % 360.0
        print(f"row {sn['row']} car{i} f280 {c['f280']:.3f} -> mod360 {want:.3f}  angle(R) {ang:.3f} "
              f"(or {360 - ang:.3f})  axis {[round(a / n, 4) for a in ax]}  pos {[round(v, 4) for v in m[9:12]]}  "
              f"|LTM-M| {same}")
