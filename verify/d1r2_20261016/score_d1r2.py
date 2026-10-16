"""d1r2 scorer: the gates of PREREG_d1r2.md section 4, one subcommand each.

  src   <trace_v3.csv>                       G-SRC   (original: which matrix the frame holds)
  angle <trace_v3.csv> <driver_out.csv>      G-ANGLE (offline port vs original fields)
  pose  <trace_v3.csv> <cam_dump.csv> [--shift N]   G-POSE (exe replay vs original frame)
  wire  <cam_dump.csv> <librw_dump.csv>      G-WIRE  (published basis == what librw renders)

trace_v3.csv = re/frida/camera_probe.py --frame. Rows are indexed by trace row (the replay
consumes them in order, so the exe dump's `row` column is the join key).
"""
import argparse
import csv
import math
import statistics
import sys

CAM = 0x00897FE0
DEG2RAD_F = 0.017453292384743690   # float bits 0x3c8efa35 (RwMatrixRotate 0x005cd7a8)


def load(path):
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def fl(r, k):
    return float(r[k])


def f32(x):
    import struct
    return struct.unpack("<f", struct.pack("<f", x))[0]


def vec(r, pfx, comp):
    return [fl(r, f"{pfx}{comp}{a}") for a in "xyz"]


def frame(r, pfx):
    return {c: vec(r, pfx + "_", c) for c in ("r", "u", "a", "p")}


def comparable(r):
    """PREREG section 3: standard path only."""
    if int(float(r["camp"])) != CAM or int(float(r["mode"])) != 0:
        return False
    for i in range(4):
        if int(float(r[f"c{i}alive"])) != 1 or int(float(r[f"c{i}dead"])) != 0:
            return False
        if int(float(r[f"c{i}vgate"])) != 0:
            return False
    return True


# ---- RW math in float64 (RwMatrixRotateInner.cpp:11-23 row layout) -----------------------
def rot(axis, deg):
    x, y, z = axis
    n = math.sqrt(x * x + y * y + z * z)
    x, y, z = x / n, y / n, z / n
    a = deg * DEG2RAD_F
    c, s, t = math.cos(a), math.sin(a), 1.0 - math.cos(a)
    return [[c + x * x * t, x * y * t + z * s, x * z * t - y * s],
            [x * y * t - z * s, c + y * y * t, y * z * t + x * s],
            [x * z * t + y * s, y * z * t - x * s, c + z * z * t]]


def mul(m, r):   # row-vector post-concat: M' = M . R
    return [[sum(m[i][k] * r[k][j] for k in range(3)) for j in range(3)] for i in range(3)]


def camera_apply(yaw, pitch, roll, pos):
    """0x00441760: identity, rotate world Y by yaw, own right by pitch, own at by roll."""
    m = [[1.0, 0, 0], [0, 1.0, 0], [0, 0, 1.0]]
    m = mul(m, rot([0.0, 1.0, 0.0], yaw))
    m = mul(m, rot(m[0], pitch))
    m = mul(m, rot(m[2], roll))
    return {"r": m[0], "u": m[1], "a": m[2], "p": list(pos)}


def maxdiff(fa, fb, keys=("r", "u", "a", "p")):
    return max(abs(fa[k][i] - fb[k][i]) for k in keys for i in range(3))


def ang(u, v):
    du = math.sqrt(sum(a * a for a in u)); dv = math.sqrt(sum(a * a for a in v))
    c = sum(a * b for a, b in zip(u, v)) / (du * dv)
    return math.degrees(math.acos(max(-1.0, min(1.0, c))))


def dist(u, v):
    return math.sqrt(sum((a - b) ** 2 for a, b in zip(u, v)))


def pct(xs, q):
    if not xs:
        return float("nan")
    s = sorted(xs)
    return s[min(len(s) - 1, int(round(q * (len(s) - 1))))]


def summary(name, xs, unit=""):
    if not xs:
        return f"{name}: n=0"
    return (f"{name}: n={len(xs)} median={statistics.median(xs):.6g}{unit} "
            f"p95={pct(xs, 0.95):.6g}{unit} max={max(xs):.6g}{unit}")


def verdict(ok):
    return "PASS" if ok else "FAIL"


# ---- gates ---------------------------------------------------------------------------------
def g_src(a):
    rows = load(a.trace)
    by_state = {}
    for r in rows:
        by_state.setdefault(int(float(r["ba8c"])), 0)
        by_state[int(float(r["ba8c"]))] += 1
    print(f"rows={len(rows)}  by DAT_0063ba8c: {dict(sorted(by_state.items()))}")
    cams = sorted({int(float(r['camp'])) for r in rows})
    print(f"cam pointers: {[hex(c) for c in cams]}")
    race = [(i, r) for i, r in enumerate(rows) if int(float(r["ba8c"])) == 6
            and int(float(r["camp"])) == CAM]
    print(f"racing rows (ba8c==6, cam==0x00897fe0): {len(race)}"
          + (f", trace rows {race[0][0]}..{race[-1][0]}" if race else ""))
    if not race:
        print("G-SRC: FAIL (no racing rows captured)")
        return 1
    fa = sum(1 for _, r in race if int(float(r["flag"])) == 0)
    fr = sum(1 for _, r in race if fl(r, "roll") == 0.0)
    ok_a = fa == len(race) and fr == len(race)
    print(f"(a) flag==0 {fa}/{len(race)}, roll==0 {fr}/{len(race)}: {verdict(ok_a)}")
    d_apply = []
    for _, r in race:
        m = frame(r, "M")
        ap = camera_apply(fl(r, "azim"), fl(r, "elev"), fl(r, "roll"), [fl(r, "px"), fl(r, "py"), fl(r, "pz")])
        d_apply.append(maxdiff(m, ap))
    n_ok = sum(1 for d in d_apply if d <= 1e-4)
    ok_b = n_ok >= 0.99 * len(race)
    print(f"(b) modelling == Camera::Apply(+0x38,+0x34,+0x3c,+0x40) within 1e-4: {n_ok}/{len(race)}  "
          f"{summary('maxdiff', d_apply)}: {verdict(ok_b)}")
    d_ltm = []
    for (i, r), (j, r2) in zip(race, race[1:]):
        if j != i + 1:
            continue
        d_ltm.append(maxdiff(frame(r2, "L"), frame(r, "M")))
    n_l = sum(1 for d in d_ltm if d <= 1e-6)
    ok_c = d_ltm and n_l >= 0.99 * len(d_ltm)
    print(f"(c) LTM(r+1) == modelling(r) within 1e-6: {n_l}/{len(d_ltm)}  {summary('maxdiff', d_ltm)}: "
          f"{verdict(bool(ok_c))}")
    vw = [(fl(r, "rvwx"), fl(r, "rvwy")) for _, r in race]
    n_vw = sum(1 for x, y in vw if abs(x - 0.6) <= 1e-4 and abs(y - 0.45) <= 1e-4)
    ok_d = n_vw == len(vw)
    vset = sorted(set(vw))[:4]
    print(f"(d) RwCamera viewWindow == (0.6,0.45): {n_vw}/{len(vw)}  values {vset}: {verdict(ok_d)}")
    # report only: hold-flag rows
    held = [r for r in rows if int(float(r["flag"])) != 0 and int(float(r["camp"])) == CAM]
    if held:
        de = [maxdiff(frame(r, "M"), frame(r, "E")) for r in held]
        print(f"report: flag!=0 rows {len(held)}, by ba8c "
              f"{sorted({int(float(r['ba8c'])) for r in held})}, modelling vs entry0 {summary('maxdiff', de)}; "
              f"e0c0 values {sorted({round(fl(r, 'e0c0'), 6) for r in held})[:4]}")
    other = [r for r in rows if int(float(r["flag"])) == 0 and int(float(r["ba8c"])) != 6
             and int(float(r["camp"])) == CAM]
    if other:
        do = [maxdiff(frame(r, "M"), camera_apply(fl(r, "azim"), fl(r, "elev"), fl(r, "roll"),
                                                 [fl(r, "px"), fl(r, "py"), fl(r, "pz")])) for r in other]
        print(f"report: flag==0 rows outside ba8c 6: {len(other)} (ba8c "
              f"{sorted({int(float(r['ba8c'])) for r in other})}), modelling vs Apply {summary('maxdiff', do)}")
    ok = ok_a and ok_b and bool(ok_c) and ok_d
    print(f"G-SRC: {verdict(ok)}")
    return 0 if ok else 1


def wrap180(d):
    return (d + 180.0) % 360.0 - 180.0


def g_angle(a):
    t = load(a.trace)
    d = {int(r["row"]): r for r in load(a.driver)}
    eye, yaw, pitch, aim = [], [], [], []
    for i, r in enumerate(t):
        if i not in d or not comparable(r):
            continue
        p = d[i]
        eye.append(dist([fl(r, "px"), fl(r, "py"), fl(r, "pz")], [fl(p, "epx"), fl(p, "epy"), fl(p, "epz")]))
        yaw.append(abs(wrap180(fl(r, "azim") - fl(p, "pyaw"))))
        pitch.append(abs(wrap180(fl(r, "elev") - fl(p, "ppitch"))))
        aim.append(ang([fl(r, "dx"), fl(r, "dy"), fl(r, "dz")], [fl(p, "pax"), fl(p, "pay"), fl(p, "paz")]))
    for n, xs, u in (("eye", eye, ""), ("yaw", yaw, " deg"), ("pitch", pitch, " deg"), ("aim", aim, " deg")):
        print(summary(n, xs, u))
    ok = (eye and statistics.median(eye) <= 0.001 and pct(eye, 0.95) <= 0.01
          and statistics.median(yaw) <= 0.005 and pct(yaw, 0.95) <= 0.05
          and statistics.median(pitch) <= 0.005 and pct(pitch, 0.95) <= 0.05
          and statistics.median(aim) <= 0.005)
    print(f"G-ANGLE: {verdict(bool(ok))}")
    return 0 if ok else 1


def g_pose(a):
    t = load(a.trace)
    dump = load(a.dump)
    last = {}
    for r in dump:       # the LAST published frame per consumed row
        row = int(r["row"])
        if row >= 0:
            last[row] = r
    pos, at, rt, vw, src = [], [], [], [], {}
    for i, r in enumerate(t):
        j = i + a.shift
        if j not in last or not comparable(r) or int(float(r["ba8c"])) != 6:
            continue
        s = last[j]
        src[int(s["src"])] = src.get(int(s["src"]), 0) + 1
        m = frame(r, "M")
        pos.append(dist(m["p"], [fl(s, "ex"), fl(s, "ey"), fl(s, "ez")]))
        at.append(ang(m["a"], [fl(s, "ax"), fl(s, "ay"), fl(s, "az")]))
        rt.append(ang(m["r"], [fl(s, "rx"), fl(s, "ry"), fl(s, "rz")]))
        # compare as float32 BITS: the exe prints %.9g, Frida prints 17 digits of the same float
        vw.append(abs(f32(fl(s, "vw58")) - f32(fl(r, "vw58"))))
    print(f"shift={a.shift}  joined racing rows={len(pos)}  exe src counts {src}")
    for n, xs, u in (("pos", pos, ""), ("at", at, " deg"), ("right", rt, " deg"), ("vw58", vw, "")):
        print(summary(n, xs, u))
    ok = (pos and statistics.median(pos) <= 0.001 and pct(pos, 0.95) <= 0.01
          and statistics.median(at) <= 0.005 and pct(at, 0.95) <= 0.05
          and statistics.median(rt) <= 0.005 and pct(rt, 0.95) <= 0.05
          and max(vw) == 0.0 and set(src) == {1})
    print(f"G-POSE: {verdict(bool(ok))}")
    return 0 if ok else 1


def g_wire(a):
    pub = {int(r["seq"]): r for r in load(a.dump)}
    lib = load(a.librw)
    n = bad = 0
    srcs = {}
    worst = 0.0
    for r in lib:
        s = int(r["seq"])
        if s not in pub:
            continue
        p = pub[s]
        if int(p["valid"]) != 1:
            continue
        srcs[int(p["src"])] = srcs.get(int(p["src"]), 0) + 1
        n += 1
        pairs = [("rx", "rx"), ("ry", "ry"), ("rz", "rz"), ("ux", "ux"), ("uy", "uy"), ("uz", "uz"),
                 ("ax", "ax"), ("ay", "ay"), ("az", "az"), ("px", "ex"), ("py", "ey"), ("pz", "ez")]
        d = max(abs(fl(r, k1) - fl(p, k2)) for k1, k2 in pairs)
        worst = max(worst, d)
        if d != 0.0:
            bad += 1
    print(f"librw rows joined with a valid published basis: {n}, src counts {srcs}, "
          f"not bit-equal {bad}, worst |d| {worst:.3g}")
    ok = n > 0 and bad == 0
    print(f"G-WIRE: {verdict(ok)}")
    return 0 if ok else 1


def imgdiff_mean(a_path, b_path):
    """re/tools/imgdiff.py's 'all' mean abs diff (B resampled to A's size)."""
    import re
    import subprocess
    from pathlib import Path
    tool = Path(__file__).resolve().parents[2] / "re" / "tools" / "imgdiff.py"
    out = subprocess.run([sys.executable, str(tool), str(a_path), str(b_path)],
                         capture_output=True, text=True).stdout
    m = re.search(r"\(all ([0-9.]+)\)", out)
    over = re.search(r"pixels over threshold \d+: \d+ \(([0-9.]+)%\)", out)
    return (float(m.group(1)) if m else float("nan"), float(over.group(1)) if over else float("nan"))


def g_frame(a):
    from pathlib import Path
    from PIL import Image
    od, sd = Path(a.orig_dir), Path(a.stand_dir)
    ok_all = True
    for k in a.rows:
        o = od / f"orig_row_{k:05d}.bmp"
        res = {}
        for off in (0, -a.ctl, a.ctl):
            s = sd / f"stand_row_{k + off:05d}.bmp"
            res[off] = imgdiff_mean(o, s) if (o.exists() and s.exists()) else (float("nan"), float("nan"))
        ok = res[0][0] < res[-a.ctl][0] and res[0][0] < res[a.ctl][0]
        ok_all &= ok
        print(f"K={k}: mean-abs(orig@K, stand@K)={res[0][0]:.2f} ({res[0][1]:.1f}% over 16)  "
              f"stand@K-{a.ctl}={res[-a.ctl][0]:.2f}  stand@K+{a.ctl}={res[a.ctl][0]:.2f}: {verdict(ok)}")
        if a.sbs and o.exists() and (sd / f"stand_row_{k:05d}.bmp").exists():
            io = Image.open(o).convert("RGB")
            ist = Image.open(sd / f"stand_row_{k:05d}.bmp").convert("RGB").resize(io.size)
            sbs = Image.new("RGB", (io.width * 2, io.height))
            sbs.paste(io, (0, 0)); sbs.paste(ist, (io.width, 0))
            p = Path(a.sbs) / f"sbs_row_{k:05d}.png"
            sbs.save(p)
            print(f"   side-by-side (original | standalone) -> {p}")
    print(f"G-FRAME: {verdict(ok_all)}")
    return 0 if ok_all else 1


def nodes_out(a):
    import json
    st = json.load(open(a.static))
    with open(a.out, "w") as f:
        for n in st["nodes"]:
            f.write(" ".join(f"{v:.9g}" for v in (*n["dir"], *n["c0"], *n["c3"])) + "\n")
    print(f"{len(st['nodes'])} nodes (node_count {st['node_count']}) -> {a.out}")
    return 0


def nodecmp(a):
    def rd(p):
        return [[float(x) for x in ln.split()] for ln in open(p) if ln.strip()]
    o, s = rd(a.orig), rd(a.stand)
    print(f"original nodes {len(o)}, standalone nodes {len(s)}")
    if len(o) != len(s):
        print("node COUNT differs")
    worst = [0.0] * 3
    for no, ns in zip(o, s):
        for g in range(3):
            worst[g] = max(worst[g], max(abs(no[3 * g + k] - ns[3 * g + k]) for k in range(3)))
    print(f"max |d| dir {worst[0]:.3g}  c0 {worst[1]:.3g}  c3 {worst[2]:.3g}")
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sp = ap.add_subparsers(dest="cmd", required=True)
    s = sp.add_parser("nodes"); s.add_argument("static"); s.add_argument("out"); s.set_defaults(fn=nodes_out)
    s = sp.add_parser("nodecmp"); s.add_argument("orig"); s.add_argument("stand"); s.set_defaults(fn=nodecmp)
    s = sp.add_parser("src"); s.add_argument("trace"); s.set_defaults(fn=g_src)
    s = sp.add_parser("angle"); s.add_argument("trace"); s.add_argument("driver"); s.set_defaults(fn=g_angle)
    s = sp.add_parser("pose"); s.add_argument("trace"); s.add_argument("dump")
    s.add_argument("--shift", type=int, default=0); s.set_defaults(fn=g_pose)
    s = sp.add_parser("wire"); s.add_argument("dump"); s.add_argument("librw"); s.set_defaults(fn=g_wire)
    s = sp.add_parser("frame"); s.add_argument("orig_dir"); s.add_argument("stand_dir")
    s.add_argument("rows", type=int, nargs="+"); s.add_argument("--ctl", type=int, default=60)
    s.add_argument("--sbs", default=""); s.set_defaults(fn=g_frame)
    a = ap.parse_args()
    return a.fn(a)


if __name__ == "__main__":
    sys.exit(main())
