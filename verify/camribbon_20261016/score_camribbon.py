"""camribbon scorer (PREREG_camribbon.md section 4).

  nodes <A.txt> <B.txt> [--label X]        per-node float32 BIT equality of dir / c0 / c3 between two
                                           nodes files (9 floats per line: dir c0 c3, %.9g)
  course <course.bin> <nodes.txt>          the original's live course block (records at 0xc1c0 + i*0x4c,
                                           dir +0x00, c0 +0x18, c3 +0x3c, count at +0x16240) vs a nodes file
  led <nodes.txt> <ribbon.txt>             list the nodes with no LED elevation (from the exe's ribbon file)
  pose <trace_v3.csv> <cam_dump.csv> [--rows A:B]   published pose vs the original frame (d1r2 G-POSE
                                           metrics) on the comparable racing rows, optionally a row range
"""
import argparse
import math
import statistics
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "d1r2_20261016"))
import score_d1r2 as S  # noqa: E402


def f32(x):
    return struct.unpack("<f", struct.pack("<f", float(x)))[0]


def rd_nodes(p):
    out = []
    for ln in open(p):
        if ln.strip():
            v = [f32(x) for x in ln.split()]
            out.append((v[0:3], v[3:6], v[6:9]))
    return out


def cmp_sets(a, b, label):
    print(f"[{label}] A nodes {len(a)}, B nodes {len(b)}")
    n = min(len(a), len(b))
    names = ("dir", "c0", "c3")
    eq = [0, 0, 0]
    worst = [0.0, 0.0, 0.0]
    firstbad = [[], [], []]
    for i in range(n):
        for g in range(3):
            if a[i][g] == b[i][g]:
                eq[g] += 1
            else:
                if len(firstbad[g]) < 8:
                    firstbad[g].append(i)
            worst[g] = max(worst[g], max(abs(a[i][g][k] - b[i][g][k]) for k in range(3)))
    for g in range(3):
        print(f"  {names[g]}: equal {eq[g]}/{n}  max|d| {worst[g]:.6g}  first differing nodes {firstbad[g]}")
    return eq, n


def c_nodes(args):
    a, b = rd_nodes(args.a), rd_nodes(args.b)
    eq, n = cmp_sets(a, b, args.label)
    return 0 if (len(a) == len(b) and all(e == n for e in eq)) else 1


def c_course(args):
    b = Path(args.course).read_bytes()
    cnt = struct.unpack_from("<i", b, 0x16240)[0]
    o = []
    for i in range(cnt):
        r = struct.unpack_from("<19f", b, 0xC1C0 + i * 0x4C)
        o.append((list(r[0:3]), list(r[6:9]), list(r[15:18])))
    s = rd_nodes(args.nodes)
    eq, n = cmp_sets(o, s, f"original course block ({cnt} nodes) vs {Path(args.nodes).name}")
    return 0 if (cnt == len(s) and all(e == n for e in eq)) else 1


def c_led(args):
    rows = [ln.split() for ln in open(args.ribbon) if ln.strip() and not ln.startswith("#")]
    nol = [int(r[0]) for r in rows if r[1] == "0"]
    print(f"nodes {len(rows)}, with no LED elevation: {len(nol)}  {nol[:40]}")
    return 0


def c_pose(args):
    t = S.load(args.trace)
    dump = S.load(args.dump)
    last = {}
    for r in dump:
        row = int(r["row"])
        if row >= 0:
            last[row] = r
    lo, hi = (0, 10 ** 9)
    if args.rows:
        a, b = args.rows.split(":")
        lo, hi = int(a), int(b)
    pos, at, rt, n_all = [], [], [], 0
    for i, r in enumerate(t):
        if i < lo or i > hi:
            continue
        n_all += 1
        if i not in last or not S.comparable(r) or int(float(r["ba8c"])) != 6:
            continue
        s = last[i]
        m = S.frame(r, "M")
        pos.append(S.dist(m["p"], [S.fl(s, "ex"), S.fl(s, "ey"), S.fl(s, "ez")]))
        at.append(S.ang(m["a"], [S.fl(s, "ax"), S.fl(s, "ay"), S.fl(s, "az")]))
        rt.append(S.ang(m["r"], [S.fl(s, "rx"), S.fl(s, "ry"), S.fl(s, "rz")]))
    print(f"trace rows in range {n_all}, comparable racing rows joined {len(pos)}")
    for nme, xs, u in (("pos", pos, ""), ("at", at, " deg"), ("right", rt, " deg")):
        print(S.summary(nme, xs, u))
    ok = bool(pos) and (statistics.median(pos) <= 0.001 and S.pct(pos, 0.95) <= 0.01
                        and statistics.median(at) <= 0.005 and S.pct(at, 0.95) <= 0.05
                        and statistics.median(rt) <= 0.005 and S.pct(rt, 0.95) <= 0.05)
    print(f"G-POSE-TOLERANCE: {'PASS' if ok else 'FAIL'}")
    return 0 if ok else 1


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    sp = ap.add_subparsers(dest="cmd", required=True)
    s = sp.add_parser("nodes"); s.add_argument("a"); s.add_argument("b"); s.add_argument("--label", default="nodes")
    s.set_defaults(fn=c_nodes)
    s = sp.add_parser("course"); s.add_argument("course"); s.add_argument("nodes"); s.set_defaults(fn=c_course)
    s = sp.add_parser("led"); s.add_argument("nodes"); s.add_argument("ribbon"); s.set_defaults(fn=c_led)
    s = sp.add_parser("pose"); s.add_argument("trace"); s.add_argument("dump"); s.add_argument("--rows", default="")
    s.set_defaults(fn=c_pose)
    a = ap.parse_args()
    return a.fn(a)


if __name__ == "__main__":
    sys.exit(main())
