#!/usr/bin/env python3
"""Direct witness for U-9699: the camera-man world-matrix yaw in the original's draw record against the
standalone's, per nine-frame row.

    py -3.12 verify/rprops2_20261016/camman_yaw.py <arm> [<arm> ...] [--tex CamMan] [--out file]

Each draw record carries `w` (the D3DTS_WORLD, 16 floats, row-vector convention). A rotation about +Y by
`yaw` degrees stores w[0] = cos, w[2] = sin (e.g. TRAINING 884: 0.99706 / 0.07668 -> 4.398 deg), so
yaw = atan2(w[2], w[0]). Records are paired by nearest translation (w[12..14]); the delta is reported
as the signed difference wrapped to (-180, 180]. Original records: verify/d1r1_20261016/evidence.
"""
import json
import math
import sys
from pathlib import Path

WT = Path(__file__).resolve().parents[2]
D1 = WT / "verify" / "d1r1_20261016" / "evidence"
LOG = WT / "log" / "rprops2"
FRAMES = {"training": (884, 924, 959), "arctic": (1051, 1091, 1126), "city": (761, 801, 836)}


def draws(path, tex):
    d = json.loads(Path(path).read_text())
    out = []
    for e in d["events"]:
        if e.get("e") == "d" and str(e.get("tex", "")).lower().startswith(tex.lower()) and e.get("w"):
            w = e["w"]
            out.append({"pos": (w[12], w[13], w[14]), "yaw": math.degrees(math.atan2(w[2], w[0])),
                        "tri": e.get("tri"), "tex": e.get("tex")})
    return out


def wrap(a):
    while a > 180: a -= 360
    while a <= -180: a += 360
    return a


def main():
    a = sys.argv[1:]
    tex = "CamMan"
    outp = None
    if "--tex" in a:
        i = a.index("--tex"); tex = a[i + 1]; del a[i:i + 2]
    if "--out" in a:
        i = a.index("--out"); outp = Path(a[i + 1]); del a[i:i + 2]
    lines = []
    for arm in a:
        lines.append(f"arm {arm}")
        for trk, ks in FRAMES.items():
            for k in ks:
                o = draws(D1 / f"{trk}_orig_row_{k:05d}.draws.json", tex)
                s = draws(LOG / f"{arm}_{trk}" / "d1r2" / f"stand_row_{k:05d}.draws.json", tex)
                if not o and not s:
                    continue
                for x in o:
                    best = min(s, key=lambda y: sum((p - q) ** 2 for p, q in zip(x["pos"], y["pos"])), default=None)
                    if best is None:
                        lines.append(f"  {trk} {k}: orig {x['tex']} pos=({x['pos'][0]:.3f},{x['pos'][1]:.3f},{x['pos'][2]:.3f}) "
                                     f"yaw={x['yaw']:.3f}  stand: none")
                        continue
                    dpos = math.dist(x["pos"], best["pos"])
                    lines.append(f"  {trk} {k}: orig {x['tex']} pos=({x['pos'][0]:.3f},{x['pos'][1]:.3f},{x['pos'][2]:.3f}) "
                                 f"yaw={x['yaw']:.3f}  stand yaw={best['yaw']:.3f}  d_yaw={wrap(best['yaw'] - x['yaw']):+.3f}  "
                                 f"d_pos={dpos:.4f}  tri {x['tri']}/{best['tri']}")
                lines.append(f"  {trk} {k}: orig draws {len(o)}, stand draws {len(s)}")
    txt = "\n".join(lines)
    print(txt)
    if outp:
        outp.write_text(txt + "\n")


if __name__ == "__main__":
    main()
