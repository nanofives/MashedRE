"""DDKOBJ-STATIC-GLOBALS: compare this branch's ddkobj build/check against a saved check (default: RACE-PIZ-DEVICE's
object build, the same base 461a34f27), and list the 35 static-global sources with their verdict.

    py -3.12 verify/ddkobj_staticglobals_20261127/compare_base.py <out dir of this build> <base dir> [<base dir 2> ...]

Prints, for each base: summary deltas, every rva whose BYTECHECK result changed, every unit whose status changed. Then the
35-source table: unit status, static_globals bound (name -> __ddkabs_<addr>, reference count), each function's result
and abs_data_refs_exact (now vs base).
"""
import csv
import json
import pathlib
import re
import sys

HERE = pathlib.Path(__file__).resolve().parent
WT = HERE.parents[1]
ST = re.compile(r'^static\s+(?:const\s+|volatile\s+)*[A-Za-z_][\w\s\*]*?\b(DAT_[0-9a-fA-F]{8})\s*(\[[^\]]*\])?\s*(=[^;]*)?;', re.M)


def load(d):
    d = pathlib.Path(d)
    s = json.loads((d / "ddkobj_summary.json").read_text())
    rows = {}
    for r in csv.DictReader(open(d / "BYTECHECK.tsv", encoding="utf-8"), delimiter="\t"):
        rows[(r["rva"], r.get("src", ""))] = r
    b = json.loads((d / "ddkobj_build.json").read_text())
    units = {(u["src"], u["wrapper"]): u for u in b["units"]}
    return s, rows, units


out = pathlib.Path(sys.argv[1])
bases = [pathlib.Path(x) for x in sys.argv[2:]]
S, R, U = load(out)
KEYS = ("identical", "functions", "different", "not_in_map", "units_linked", "units", "refusals",
        "provisional_units_skipped", "fn_addr_refs_ok")
for bd in bases:
    s, r, u = load(bd)
    print(f"== against {bd.as_posix()}")
    for k in KEYS:
        print(f"  {k}: base {s.get(k)}  now {S.get(k)}")
    print(f"  dual body: base {len(s.get('dual_body_rvas', []))} now {len(S.get('dual_body_rvas', []))}")
    print(f"  provisional fns in linked units: base {s.get('provisional_functions_vs2022_in_linked_units')} "
          f"now {S.get('provisional_functions_vs2022_in_linked_units')}")
    ch = []
    for k in sorted(set(R) | set(r)):
        a, b = r.get(k, {}).get("result", "absent"), R.get(k, {}).get("result", "absent")
        ea, eb = r.get(k, {}).get("abs_data_refs_exact", ""), R.get(k, {}).get("abs_data_refs_exact", "")
        if a != b:
            ch.append(f"  RESULT {k[0]} {k[1]}: {a} -> {b}")
    nd = sorted(set(S["different_rvas"]) - set(s["different_rvas"]))
    gone = sorted(set(s["different_rvas"]) - set(S["different_rvas"]))
    print(f"  newly DIFFERENT: {nd or 'none'}   no longer DIFFERENT: {gone or 'none'}")
    print(f"  new not-in-map: {sorted(set(S['not_in_map_rvas']) - set(s['not_in_map_rvas'])) or 'none'}")
    print(f"  per-function result changes: {len(ch)}")
    for x in ch:
        print(x)
    uc = [(k, u.get(k, {}).get("status", "absent"), U.get(k, {}).get("status", "absent")) for k in sorted(set(u) | set(U))
          if u.get(k, {}).get("status", "absent") != U.get(k, {}).get("status", "absent")]
    print(f"  unit status changes: {len(uc)}")
    for k, a, b in uc:
        print(f"  UNIT {k[0]} ({k[1]}): {a} -> {b}")

print("== the static-global sources (C view file-static DAT_<addr>)")
by_src = {}
for (src, w), x in U.items():
    by_src.setdefault(src, []).append(x)
b0 = load(bases[0])[1] if bases else {}
n = 0
for p in sorted((WT / "re" / "match").rglob("*.c")):
    t = p.read_text(encoding="utf-8", errors="replace")
    names = sorted({m.group(1) for m in ST.finditer(t)})
    if not names:
        continue
    n += 1
    rel = p.relative_to(WT).as_posix()
    us = by_src.get(rel, [])
    if not us:
        print(f"{rel}\t{','.join(names)}\tno exe wrapper includes it (not a unit)")
        continue
    for x in us:
        sg = "; ".join(f"{g['name']}->{g['ddk']} refs {g['refs']}" for g in x.get("static_globals", [])) or "-"
        fr = []
        for (rva, src), row in sorted(R.items()):
            if src == rel:
                bb = b0.get((rva, src), {})
                fr.append(f"{rva} {row['result']} abs {row.get('abs_data_refs_exact')} "
                          f"(base {bb.get('result', 'absent')} abs {bb.get('abs_data_refs_exact', '')})")
        print(f"{rel}\t{','.join(names)}\t{x['wrapper']}\t{x['status']}\t{x.get('evidence', '')}\t{sg}\t{' ; '.join(fr)}")
print(f"sources: {n}")
