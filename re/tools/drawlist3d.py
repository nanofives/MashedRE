# drawlist3d.py -- the 3D (per-D3D9-draw) mode of re/tools/drawlist_diff.py.
#
# [d1r1 2026-10-16] D1-residue R1 (verify/d1r1_20261016/PREREG_d1r1.md I-5 + amendment 1). Reads
# the per-draw records written by
#   original:   re/frida/camera_probe.py --drawrec   -> orig_row_<K>.draws.json
#   standalone: MASHED_DBG_D3DREC + MASHED_CAM_REPLAY_SHOTS -> stand_row_<K>.draws.json
# Both carry {"side", "row", "events": [...]}, one event per draw {"e":"d", "k":vtable slot, "pt",
# "n", "tex", "tex1", "rs", "tss", "w", "fvf", "vs", "st", "tri", "bb"}. The original's file also
# carries Present ("P") and director ("D") markers and spans two frames, so the frame of row K is
# cut out between the Present before D(K) and the Present after it.
#
# Matching is on the GEOMETRY DRAWN, not on call arguments: RW's D3D9 instancing draws indexed
# triangle strips, librw draws triangle lists with unshared vertices, so primitive and vertex
# counts never agree by construction. A draw's identity is (stage-0 texture, non-degenerate
# triangle count, world-space AABB of the vertices those triangles use).
#
# Rows (every row is a divergence; matched draws are only counted):
#   STATE    same geometry, key render states differ (which ones is listed)
#   TEX1     same geometry, stage-1 texture differs
#   MOVED    same texture and triangle count, AABB elsewhere (pose / animation)
#   SPLIT    a texture group's triangle total agrees but is split into different draws
#   MISSING  triangles of a texture the original draws and the standalone does not
#   EXTRA    triangles of a texture the standalone draws and the original does not
#   ORDER    a blended matched draw's order against other matched draws differs
# MISSING / EXTRA rows say how many of their draws lie wholly OUTSIDE the view frustum of the
# trace row (they rasterise nothing). A --cite JSON list [{"id","kind","tex" (regex),
# "detail"? (regex),"note"}] marks rows CITED. Verdict: GREEN (no rows), CITED (every row cited)
# or RED. Exit 0 / 0 / 1.
import csv
import json
import re
from collections import defaultdict
from pathlib import Path

# rs index order (camera_probe.py RS_LIST, DrawRec.cpp kRs)
RS_NAMES = ["Z", "ZW", "ZF", "AT", "AREF", "AF", "AB", "SRC", "DST", "CULL", "FOG", "LIT"]
# Camera near/far, measured live on the original (verify/d1_lens/RESULT.md: near 0.1, far 360.0)
NEAR, FAR = 0.1, 360.0


def is_draw3d_file(d):
    return isinstance(d, dict) and "events" in d and "side" in d


def cut_frame(doc):
    """-> list of draw events of the frame of doc['row']."""
    ev = doc["events"]
    if doc.get("side") != "orig":
        return [e for e in ev if e.get("e") == "d"]
    row = doc["row"]
    iD = next((i for i, e in enumerate(ev) if e.get("e") == "D" and e.get("row") == row), None)
    if iD is None:
        raise ValueError(f"no director marker for row {row}")
    lo = max((i for i, e in enumerate(ev[:iD]) if e.get("e") == "P"), default=0)
    hi = next((i for i in range(iD + 1, len(ev)) if ev[i].get("e") == "P"), len(ev))
    pre = sum(1 for e in ev[lo + 1:iD] if e.get("e") == "d")
    if pre:
        print(f"note: {pre} draws between the Present before D({row}) and D({row}) "
              f"(I-FRAME): they are kept in the frame")
    return [e for e in ev[lo + 1:hi] if e.get("e") == "d"]


def key_states(rs):
    """The matched render-state tuple with don't-care fields zeroed."""
    s = list(rs[:10])
    if s[0] == 0:          # depth test off: ZFUNC unused
        s[2] = 0
    if s[3] == 0:          # alpha test off: ref/func unused
        s[4] = s[5] = 0
    if s[6] == 0:          # blend off: factors unused
        s[7] = s[8] = 0
    return tuple(s)


def nprims(e):
    return e["n"][-1]


def clean(name):
    # names end at the first NUL / non-printable (an early original-side recorder read the
    # 32-byte RwTexture name field whole; the bytes past the NUL decoded as U+FFFD)
    return re.split(r"[^\x20-\x7e]", name or "-")[0] or "?"


def norm(e, i):
    tex, tex1 = clean(e.get("tex")), clean(e.get("tex1", "-"))
    return {
        "i": i, "k": e["k"], "pt": e["pt"], "np": nprims(e), "tri": e.get("tri"),
        "bb": e.get("bb"), "screen": e.get("screen", 0),
        "tex": tex, "texl": tex.lower(), "tex1": tex1, "tex1l": tex1.lower(),
        "rs": e["rs"], "ks": key_states(e["rs"]), "fog": e["rs"][10], "tss": e.get("tss"),
        "w": e.get("w"), "vs": e.get("vs"), "src": e.get("src", "d3d"),
    }


def states_str(ks):
    return " ".join(f"{n}={v}" for n, v in zip(RS_NAMES, ks))


def bb_close(a, b, tol):
    if a is None or b is None:
        return False
    ext = max(max(a[3] - a[0], a[4] - a[1], a[5] - a[2]), 1.0)
    t = tol + 1e-4 * ext
    return all(abs(x - y) <= t for x, y in zip(a, b))


def bb_dist(a, b):
    if a is None or b is None:
        return 1e9
    return max(abs(x - y) for x, y in zip(a, b))


class Frustum:
    def __init__(self, trace_csv, row):
        rows = list(csv.DictReader(open(trace_csv)))
        r = rows[row]
        g = lambda k: float(r[k])
        self.right = [g("M_rx"), g("M_ry"), g("M_rz")]
        self.up = [g("M_ux"), g("M_uy"), g("M_uz")]
        self.at = [g("M_ax"), g("M_ay"), g("M_az")]
        self.pos = [g("M_px"), g("M_py"), g("M_pz")]
        self.vx, self.vy = g("rvwx"), g("rvwy")

    def outside(self, bb):
        """True when the AABB lies wholly outside one frustum plane (rasterises nothing)."""
        if bb is None:
            return False
        cs = []
        for x in (bb[0], bb[3]):
            for y in (bb[1], bb[4]):
                for z in (bb[2], bb[5]):
                    d = [x - self.pos[0], y - self.pos[1], z - self.pos[2]]
                    cs.append((sum(a * b for a, b in zip(d, self.right)),
                               sum(a * b for a, b in zip(d, self.up)),
                               sum(a * b for a, b in zip(d, self.at))))
        tests = [lambda c: c[2] < NEAR, lambda c: c[2] > FAR,
                 lambda c: c[0] > self.vx * c[2], lambda c: c[0] < -self.vx * c[2],
                 lambda c: c[1] > self.vy * c[2], lambda c: c[1] < -self.vy * c[2]]
        return any(all(t(c) for c in cs) for t in tests)


def diff3d(da, db, tol_bb=0.01, frustum=None):
    fa, fb = cut_frame(da), cut_frame(db)
    empty = (sum(1 for e in fa if nprims(e) == 0) + da.get("empty", 0),
             sum(1 for e in fb if nprims(e) == 0) + db.get("empty", 0))
    A = [norm(e, i) for i, e in enumerate(e for e in fa if nprims(e) > 0)]
    B = [norm(e, i) for i, e in enumerate(e for e in fb if nprims(e) > 0)]
    nogeo = (sum(1 for d in A if d["tri"] is None), sum(1 for d in B if d["tri"] is None))
    rows = []
    off = (lambda d: frustum is not None and not d["screen"] and frustum.outside(d["bb"]))

    # level 1: same texture, same triangle count, AABB within tolerance
    ga, gb = defaultdict(list), defaultdict(list)
    for d in A: ga[(d["texl"], d["tri"], d["screen"])].append(d["i"])
    for d in B: gb[(d["texl"], d["tri"], d["screen"])].append(d["i"])
    pairs, la, lb = [], [], []
    for s in set(ga) | set(gb):
        xa, yb = ga.get(s, []), gb.get(s, [])
        cand = sorted((bb_dist(A[x]["bb"], B[y]["bb"]), x, y) for x in xa for y in yb
                      if bb_close(A[x]["bb"], B[y]["bb"], tol_bb))
        ua, ub = set(), set()
        for _, x, y in cand:
            if x in ua or y in ub:
                continue
            ua.add(x); ub.add(y); pairs.append((x, y))
        la += [x for x in xa if x not in ua]
        lb += [y for y in yb if y not in ub]
    # STATE / TEX1 rows on matched geometry
    st, t1 = defaultdict(list), defaultdict(list)
    for x, y in pairs:
        if A[x]["ks"] != B[y]["ks"]:
            diffs = tuple(n for n, u, v in zip(RS_NAMES, A[x]["ks"], B[y]["ks"]) if u != v)
            st[(A[x]["tex"], diffs, A[x]["ks"], B[y]["ks"])].append((x, y))
        if A[x]["tex1l"] != B[y]["tex1l"]:
            t1[(A[x]["tex"], A[x]["tex1"], B[y]["tex1"])].append((x, y))
    for (tex, diffs, ka, kb), lst in st.items():
        rows.append({"kind": "STATE", "tex": tex, "n": len(lst),
                     "detail": f"{len(lst)} matched draws differ in {','.join(diffs)}: "
                               f"A {states_str(ka)} | B {states_str(kb)}",
                     "a_idx": [p[0] for p in lst], "b_idx": [p[1] for p in lst]})
    for (tex, a1, b1), lst in t1.items():
        rows.append({"kind": "TEX1", "tex": tex, "n": len(lst),
                     "detail": f"{len(lst)} matched draws, stage-1 texture A {a1} | B {b1}",
                     "a_idx": [p[0] for p in lst], "b_idx": [p[1] for p in lst]})
    # level 2: same texture and triangle count, AABB elsewhere -> MOVED
    ga2, gb2 = defaultdict(list), defaultdict(list)
    for x in la: ga2[(A[x]["texl"], A[x]["tri"])].append(x)
    for y in lb: gb2[(B[y]["texl"], B[y]["tri"])].append(y)
    mv = defaultdict(list)
    ua, ub = set(), set()
    for s in set(ga2) & set(gb2):
        cand = sorted((bb_dist(A[x]["bb"], B[y]["bb"]), x, y) for x in ga2[s] for y in gb2[s])
        for d, x, y in cand:
            if x in ua or y in ub:
                continue
            ua.add(x); ub.add(y); mv[A[x]["tex"]].append((x, y, d))
    for tex, lst in mv.items():
        x, y, d = lst[0]
        rows.append({"kind": "MOVED", "tex": tex, "n": len(lst),
                     "detail": f"{len(lst)} draws, same texture and triangle count, AABB moved by up "
                               f"to {max(p[2] for p in lst):.3g}; first A {fmtbb(A[x]['bb'])} "
                               f"B {fmtbb(B[y]['bb'])}",
                     "a_idx": [p[0] for p in lst], "b_idx": [p[1] for p in lst]})
    la = [x for x in la if x not in ua]
    lb = [y for y in lb if y not in ub]
    # level 3: per texture, leftover triangles
    ta, tb = defaultdict(list), defaultdict(list)
    for x in la: ta[A[x]["texl"]].append(x)
    for y in lb: tb[B[y]["texl"]].append(y)
    for t in sorted(set(ta) | set(tb)):
        xa, yb = ta.get(t, []), tb.get(t, [])
        sa = sum(A[x]["tri"] or 0 for x in xa); sb = sum(B[y]["tri"] or 0 for y in yb)
        tex = (A[xa[0]] if xa else B[yb[0]])["tex"]
        if xa and yb and sa == sb:
            rows.append({"kind": "SPLIT", "tex": tex, "n": len(xa) + len(yb),
                         "detail": f"{sa} triangles, A {len(xa)} draws / B {len(yb)} draws",
                         "a_idx": xa, "b_idx": yb})
            continue
        if xa:
            o = sum(1 for x in xa if off(A[x]))
            rows.append({"kind": "MISSING", "tex": tex, "n": len(xa), "offscreen": o,
                         "detail": f"A-only {len(xa)} draws {sa} tri ({o} offscreen); "
                                   f"first {desc(A[xa[0]])}",
                         "a_idx": xa, "b_idx": []})
        if yb:
            o = sum(1 for y in yb if off(B[y]))
            rows.append({"kind": "EXTRA", "tex": tex, "n": len(yb), "offscreen": o,
                         "detail": f"B-only {len(yb)} draws {sb} tri ({o} offscreen); "
                                   f"first {desc(B[yb[0]])}",
                         "a_idx": [], "b_idx": yb})
    # ORDER: blended matched draws whose order against other matched draws flips
    mp = sorted(pairs)
    inv = defaultdict(lambda: [0, defaultdict(int)])
    for x, y in mp:
        if A[x]["ks"][6] != 1:
            continue
        for x2, y2 in mp:
            if x2 != x and (x2 < x) != (y2 < y):
                r = inv[A[x]["tex"]]
                r[0] += 1
                r[1][A[x2]["tex"]] += 1
    for tex, (n, partners) in inv.items():
        top = sorted(partners.items(), key=lambda t: -t[1])[:4]
        rows.append({"kind": "ORDER", "tex": tex, "n": n,
                     "detail": f"{n} order inversions vs matched draws, mostly: "
                               + ", ".join(f"{t}x{c}" for t, c in top),
                     "a_idx": [], "b_idx": []})
    info = {"empty_dropped": empty, "no_geometry": nogeo}
    fog = defaultdict(int)
    for x, y in pairs:
        if A[x]["fog"] != B[y]["fog"]:
            fog[(A[x]["fog"], B[y]["fog"])] += 1
    info["fog_differs"] = {f"A{a}/B{b}": n for (a, b), n in fog.items()}
    return A, B, pairs, rows, info


def fmtbb(b):
    return "-" if b is None else "[" + ",".join(f"{v:.2f}" for v in b) + "]"


def desc(d):
    return (f"k{d['k']} pt{d['pt']} tri={d['tri']} tex={d['tex']} tex1={d['tex1']} "
            f"bb={fmtbb(d['bb'])} src={d['src']} {states_str(d['ks'])}")


def apply_cites(rows, cites):
    for r in rows:
        for c in cites:
            if c.get("kind", "*") not in ("*", r["kind"]):
                continue
            if not re.search(c.get("tex", ".*"), r["tex"], re.I):
                continue
            if "detail" in c and not re.search(c["detail"], r["detail"], re.I):
                continue
            # "all_offscreen": the row's every draw lies wholly outside the view frustum
            if c.get("all_offscreen") and r.get("offscreen") != r.get("n"):
                continue
            if "src" in c and not all(s == c["src"] for s in r.get("srcs", [])):
                continue
            r["cite"] = c["id"]
            break


def run(path_a, path_b, args):
    da = json.loads(Path(path_a).read_text())
    db = json.loads(Path(path_b).read_text())
    fr = Frustum(args.trace, da["row"]) if getattr(args, "trace", None) else None
    A, B, pairs, rows, info = diff3d(da, db, getattr(args, "tol_bb", 0.01), fr)
    cites = json.loads(Path(args.cite).read_text()) if args.cite else []
    apply_cites(rows, cites)
    order = {"MISSING": 0, "EXTRA": 1, "STATE": 2, "TEX1": 3, "MOVED": 4, "SPLIT": 5, "ORDER": 6}
    rows.sort(key=lambda r: (order[r["kind"]], r["tex"].lower()))
    ta = sum(d["tri"] or 0 for d in A); tb = sum(d["tri"] or 0 for d in B)
    mta = sum(A[x]["tri"] or 0 for x, _ in pairs)
    print(f"== 3D {da.get('side')} row {da.get('row')} ({len(A)} draws, {ta} tri) vs "
          f"{db.get('side')} row {db.get('row')} ({len(B)} draws, {tb} tri)")
    print(f"   matched {len(pairs)} draws ({mta} tri); rows {len(rows)} "
          f"({sum(1 for r in rows if 'cite' in r)} cited); info {info}")
    for r in rows[:args.max_rows]:
        tag = f"CITED {r['cite']}" if "cite" in r else "RED"
        print(f"  {r['kind']:<8} {tag:<10} {r['tex']:<22} {r['detail']}")
    if len(rows) > args.max_rows:
        print(f"  ... {len(rows) - args.max_rows} more rows (--max-rows)")
    if args.json_out:
        Path(args.json_out).write_text(json.dumps(
            {"a": path_a, "b": path_b, "matched": len(pairs), "na": len(A), "nb": len(B),
             "tri_a": ta, "tri_b": tb, "rows": rows, "info": info}, indent=1, default=str))
        print(f"-> {args.json_out}")
    red = [r for r in rows if "cite" not in r]
    verdict = "GREEN" if not rows else ("CITED" if not red else "RED")
    print(f"VERDICT: {verdict} (matched={len(pairs)} rows={len(rows)} red={len(red)})")
    return 0 if not red else 1
