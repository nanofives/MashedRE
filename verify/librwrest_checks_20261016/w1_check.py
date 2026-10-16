"""W1 wheel / steer / body-lean law check (PREREG_librwrest_checks.md sections 1 and 2).

  py -3.12 w1_check.py prep    <carparts.json> <replay_in.txt> <records.json>
  py -3.12 w1_check.py compare <carparts.json> <records.json> <replay_out.txt> [-o report.txt]

prep   pairs every snapshot with the previous consecutive row and writes the replay input the
       standalone's MASHED_CARPARTS_LAW reads (one line: row car units a0..a3 spin0..3 steer0..3
       va0..2 vb0..2) plus the list of included / excluded (row, car) records.
compare reads the standalone's poses and checks, per included record,
  W1-a  |CarPartsSpinStep(prev angle, units, spin) - recorded angle| <= 1e-4 deg (all 4 wheels)
  W1-b  wheel, hub and part 0x13 frame matrices vs the recorded modelling matrices, max |d| <= 1e-4
        over the 12 elements; only the LOD set rec+0x298 selects (amendment A3).
Part codes in the probe JSON are decimal keys: parts 0..3, 0x21..0x24 = 33..36, 0x13 = 19.
"""
import json
import struct
import sys
from pathlib import Path

TOL_ANG = 1e-4
TOL_MAT = 1e-4


def f32(x):
    return struct.unpack("<f", struct.pack("<f", x))[0]


def load(path):
    return json.loads(Path(path).read_text())


def rows_of(cp):
    return [r for r in cp["rows"] if "s" in r]


def build_records(cp):
    """Return (included, excluded, stats). included: dicts with the replay inputs."""
    rows = rows_of(cp)
    by_row = {r["row"]: r for r in rows}
    inc, exc = [], []
    ff4_ok = ff4_bad = ff4_na = 0
    for r in rows:
        prev = by_row.get(r["row"] - 1)
        nxt = by_row.get(r["row"] + 1)
        if nxt is not None and nxt["ff4"] >= r["ff4"]:
            if nxt["ff4"] - r["ff4"] == r["s"]["units"]:
                ff4_ok += 1
            else:
                ff4_bad += 1
        else:
            ff4_na += 1
        if prev is None:
            for ci in range(4):
                exc.append({"row": r["row"], "car": ci, "why": "first row of window (A5)"})
            continue
        for ci, c in enumerate(r["s"]["cars"]):
            pc = prev["s"]["cars"][ci]
            # A7: the probe snapshot is taken after FUN_00420870 ran, so rec+0x298 at row n is the
            # value AFTER this frame's update; FUN_0041f3d0 (first in that call) used the value
            # the previous row recorded
            lod_eff = pc["lod"]
            if lod_eff not in (0, 1):
                exc.append({"row": r["row"], "car": ci, "why": f"rec+0x298 at row n-1 = {lod_eff} (A3/A7)"})
                continue
            if c["al"] != 1 and c["ang"] == pc["ang"]:
                exc.append({"row": r["row"], "car": ci, "why": f"veh+4 = {c['al']} and angles unchanged (A4)"})
                continue
            inc.append({"row": r["row"], "car": ci, "lod": lod_eff, "units": r["s"]["units"],
                        "prev_ang": pc["ang"], "ang": c["ang"], "spin": c["spin"],
                        "steer": c["steer"], "va": c["va"], "vb": c["vb"]})
    return inc, exc, {"ff4_ok": ff4_ok, "ff4_bad": ff4_bad, "ff4_na": ff4_na, "rows": len(rows)}


def cmd_prep(cp_path, in_path, rec_path):
    cp = load(cp_path)
    inc, exc, st = build_records(cp)
    with open(in_path, "w") as f:
        for r in inc:
            f.write(" ".join(str(x) for x in (
                [r["row"], r["car"], r["units"]] + [repr(v) for v in r["prev_ang"]] +
                r["spin"] + [repr(v) for v in r["steer"]] + [repr(v) for v in r["va"]] +
                [repr(v) for v in r["vb"]])) + "\n")
    Path(rec_path).write_text(json.dumps({"included": [{"row": r["row"], "car": r["car"]} for r in inc],
                                          "excluded": exc, "stats": st}))
    print(f"prep: {len(inc)} included, {len(exc)} excluded, rows {st['rows']}, "
          f"units==ff4(n+1)-ff4(n): ok {st['ff4_ok']} bad {st['ff4_bad']} n/a {st['ff4_na']}")


def parse_out(path):
    ang, mats, errs = {}, {}, []
    for ln in Path(path).read_text().splitlines():
        t = ln.split()
        if not t:
            continue
        if t[0] == "A":
            ang[(int(t[1]), int(t[2]))] = [float(x) for x in t[3:7]]
        elif t[0] in ("W", "H"):
            mats[(t[0], int(t[1]), int(t[2]), int(t[3]))] = [float(x) for x in t[4:16]]
        elif t[0] == "B":
            mats[("B", int(t[1]), int(t[2]), 0)] = [float(x) for x in t[3:15]]
        elif t[0] == "ERR":
            errs.append(ln)
    return ang, mats, errs


def maxd(a, b):
    return max(abs(x - y) for x, y in zip(a, b))


def cmd_compare(cp_path, rec_path, out_path, rpt):
    cp = load(cp_path)
    inc, exc, st = build_records(cp)
    ang, mats, errs = parse_out(out_path)
    lines = []
    n_a = n_a_fail = n_a_exact = 0
    n_m = n_m_fail = n_m_missing = 0
    worst_a = (0.0, None)
    worst_m = {"W": (0.0, None), "H": (0.0, None), "B": (0.0, None)}
    first_fail = None
    for r in inc:
        key = (r["row"], r["car"])
        pa = ang.get(key)
        if pa is None:
            n_m_missing += 1
            if first_fail is None:
                first_fail = f"row {r['row']} car {r['car']}: no replay output"
            continue
        rows = {x["row"]: x for x in rows_of(cp)}
        cs = rows[r["row"]]["s"]["cars"][r["car"]]
        for w in range(4):
            d = abs(f32(pa[w]) - f32(r["ang"][w]))   # both are float32 in the original / port
            n_a += 1
            if d == 0.0:
                n_a_exact += 1
            if d > worst_a[0]:
                worst_a = (d, (r["row"], r["car"], w, pa[w], r["ang"][w]))
            if d > TOL_ANG:
                n_a_fail += 1
                if first_fail is None:
                    first_fail = (f"W1-a row {r['row']} car {r['car']} wheel {w}: port {pa[w]!r} "
                                  f"recorded {r['ang'][w]!r} (prev {r['prev_ang'][w]!r} units {r['units']} "
                                  f"spin {r['spin'][w]})")
        sel = [(w, w if r["lod"] == 0 else 4 + w, w if r["lod"] == 0 else 0x21 + w) for w in range(4)]
        for w, k, part in sel:
            rec = cs["p"].get(str(part))
            for kind, field in (("W", "m"), ("H", "pm")):
                m = mats.get((kind, r["row"], r["car"], k))
                n_m += 1
                if m is None or rec is None or rec.get(field) is None:
                    n_m_missing += 1
                    n_m_fail += 1
                    if first_fail is None:
                        first_fail = (f"W1-b {kind} row {r['row']} car {r['car']} k {k} part {part}: "
                                      f"port {'present' if m else 'MISSING'}, recorded "
                                      f"{'present' if rec and rec.get(field) else 'MISSING'}")
                    continue
                d = maxd(m, rec[field])
                if d > worst_m[kind][0]:
                    worst_m[kind] = (d, (r["row"], r["car"], k, part))
                if d > TOL_MAT:
                    n_m_fail += 1
                    if first_fail is None:
                        first_fail = (f"W1-b {kind} row {r['row']} car {r['car']} k {k} part {part}: "
                                      f"max|d| {d:.3g} port {m} recorded {rec[field]}")
        rec = cs["p"].get("19")
        m = mats.get(("B", r["row"], r["car"], 0))
        n_m += 1
        if m is None or rec is None:
            n_m_missing += 1
            n_m_fail += 1
            if first_fail is None:
                first_fail = f"W1-b B row {r['row']} car {r['car']}: missing"
        else:
            d = maxd(m, rec["m"])
            if d > worst_m["B"][0]:
                worst_m["B"] = (d, (r["row"], r["car"], 19))
            if d > TOL_MAT:
                n_m_fail += 1
                if first_fail is None:
                    first_fail = (f"W1-b B row {r['row']} car {r['car']}: max|d| {d:.3g} port {m} "
                                  f"recorded {rec['m']}")
    lines.append(f"carparts {cp_path}")
    lines.append(f"  rows {st['rows']}, records included {len(inc)}, excluded {len(exc)}")
    why = {}
    for e in exc:
        why[e["why"]] = why.get(e["why"], 0) + 1
    for k, v in sorted(why.items()):
        lines.append(f"    excluded: {v} x {k}")
    percar = {}
    for r in inc:
        percar[(r["car"], r["lod"])] = percar.get((r["car"], r["lod"]), 0) + 1
    lines.append("  included records per (car, lod set): " + ", ".join(
        f"car {c} set {l}: {n}" for (c, l), n in sorted(percar.items())))
    # input coverage: a law check on near-zero inputs would pass trivially
    st_max = max((abs(v) for r in inc for v in r["steer"]), default=0.0)
    st_n = sum(1 for r in inc for v in r["steer"] if abs(v) > 0.5)
    lean_z = max((abs((r["vb"][0] + r["va"][0]) * 40.0) for r in inc), default=0.0)
    lean_x = max((abs((r["vb"][2] + r["va"][2]) * -100.0) for r in inc), default=0.0)
    spin_max = max((abs(v) for r in inc for v in r["spin"]), default=0)
    ang_span = max((abs(r["ang"][w] - r["prev_ang"][w]) for r in inc for w in range(4)), default=0.0)
    lodh = {}
    rows_by = {x["row"]: x for x in rows_of(cp)}
    for row_n, x in rows_by.items():
        pv = rows_by.get(row_n - 1)
        if pv:
            for ci in range(4):
                lodh[pv["s"]["cars"][ci]["lod"]] = lodh.get(pv["s"]["cars"][ci]["lod"], 0) + 1
    lines.append(f"  input coverage over included records: steer max |{st_max:.3f}| deg ({st_n} wheel-steps over 0.5 deg), "
                 f"body-lean Z max {lean_z:.3f} deg, X max {lean_x:.3f} deg, spin max |{spin_max}|, "
                 f"largest per-row angle step {ang_span:.3f} deg")
    lines.append("  rec+0x298 at row n-1 over all (row, car): " + ", ".join(f"{k}: {v}" for k, v in sorted(lodh.items())))
    lines.append(f"  units == ff4(n+1)-ff4(n): ok {st['ff4_ok']}, bad {st['ff4_bad']}, n/a {st['ff4_na']}")
    lines.append(f"  replay ERR lines: {len(errs)}")
    lines.append(f"  W1-a angle: {n_a} wheel steps, {n_a_fail} over {TOL_ANG} deg, {n_a_exact} bit-identical, "
                 f"worst {worst_a[0]:.3g} deg at {worst_a[1]}")
    lines.append(f"  W1-b matrices: {n_m} frames compared, {n_m_fail} failing (missing {n_m_missing}); worst max|d| "
                 f"wheel {worst_m['W'][0]:.3g} {worst_m['W'][1]}, hub {worst_m['H'][0]:.3g} {worst_m['H'][1]}, "
                 f"body {worst_m['B'][0]:.3g} {worst_m['B'][1]}")
    ok = (len(inc) > 0 and n_a_fail == 0 and n_m_fail == 0 and not errs)
    lines.append(f"  W1 verdict: {'PASS' if ok else 'FAIL'}" + (f"; first failure: {first_fail}" if first_fail else ""))
    txt = "\n".join(lines)
    print(txt)
    if rpt:
        Path(rpt).write_text(txt + "\n")
    return 0 if ok else 1


def main():
    a = sys.argv[1:]
    if not a or a[0] not in ("prep", "compare"):
        print(__doc__)
        return 2
    rpt = None
    if "-o" in a:
        k = a.index("-o")
        rpt = a[k + 1]
        del a[k:k + 2]
    if a[0] == "prep":
        cmd_prep(a[1], a[2], a[3])
        return 0
    return cmd_compare(a[1], a[2], a[3], rpt)


if __name__ == "__main__":
    sys.exit(main())
