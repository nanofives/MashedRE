"""DDKOBJ-ADDR-REGISTERED: compare this branch's ddkobj build/check against the saved head check (1379467a6).

    py -3.12 verify/ddkobj_addrreg_20261127/compare_head.py <out dir of this build> [head dir]

Prints the summary deltas, the status of each of the 75 in-scope units (RACE-LOAD-LUA-2 section 2 b/c/d), every
DIFFERENT rva that is new against the head, and every not-in-map rva that is new.
"""
import csv
import json
import pathlib
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
out = pathlib.Path(sys.argv[1])
head = pathlib.Path(sys.argv[2]) if len(sys.argv) > 2 else ROOT.parent.parent / "log" / "orch" / "run56_headcheck"
if not head.exists():
    head = pathlib.Path(r"C:\Users\maria\Desktop\Proyectos\Mashed\log\orch\run56_headcheck")

REFUSED15 = "0047a1b0 0047a540 0047a580 0047a5b0 0047a5e0 0047a6b0 0047a6f0 0047a880 0047aa50 0047aaa0 0047aad0 " \
            "0047acd0 0047ad00 0047b230 0047b250".split()
CALLBACKS50 = ("0047a180 0047a1e0 0047a280 0047a320 0047a3a0 0047a440 0047a4a0 0047a610 0047a720 0047a750 0047a790 "
               "0047a7d0 0047a7f0 0047a810 0047a8b0 0047a950 0047a9b0 0047aa20 0047ab30 0047abd0 0047ac80 004917c0 "
               "00491860 00491900 00491a10 0047ad40 0047ade0 0047ae80 0047aef0 0047af50 0047afa0 0047b000 0047b0a0 "
               "0047b110 0047b160 0047b1b0 0047b200 0047b280 0047b350 0047b3c0 0047b420 0047b480 0047b500 0047b5b0 "
               "0047b630 0047b6b0 0047b720 0047b7d0 004919b0 00491a70").split()
CALLEES10 = "00405780 00471430 00471450 00471490 00471560 00490e50 00490fb0 00490ff0 00491010 00491030".split()
SCOPE = {"0x" + r: g for g, lst in (("refused15", REFUSED15), ("callback50", CALLBACKS50), ("callee10", CALLEES10))
         for r in lst}
assert len(SCOPE) == 75, len(SCOPE)


def load(d):
    s = json.loads((d / "ddkobj_summary.json").read_text())
    rows = {r["rva"]: r for r in csv.DictReader(open(d / "BYTECHECK.tsv", encoding="utf-8"), delimiter="\t")}
    b = json.loads((d / "ddkobj_build.json").read_text())
    st = {}
    for u in b["units"]:
        for r in u["rvas"]:
            st[r] = u["status"]
    return s, rows, st


hs, hrows, hst = load(head)
ns, nrows, nst = load(out)
for k in ("identical", "different", "not_in_map", "functions", "units", "units_linked"):
    print(f"{k:14s} head {hs[k]:5d}  branch {ns[k]:5d}  delta {ns[k] - hs[k]:+d}")
print("refusals       head", hs["refusals"], " branch", ns["refusals"])
print("dual body      head", len(hs["dual_body_rvas"]), " branch", len(ns["dual_body_rvas"]))
print("fn-addr refs ok", ns.get("fn_addr_refs_ok"), " bad", ns.get("fn_addr_refs_bad"))
tally = {}
print("\nin-scope 75:")
for r, g in sorted(SCOPE.items()):
    res = nrows.get(r, {}).get("result", "absent:" + nst.get(r, "no unit"))
    tally.setdefault((g, res), 0)
    tally[(g, res)] += 1
    if res != "IDENTICAL":
        print(f"  {r} {g} {res} (head {hrows.get(r, {}).get('result', hst.get(r))})")
for (g, res), n in sorted(tally.items()):
    print(f"  {g:10s} {res:12s} {n}")
nd = sorted(set(ns["different_rvas"]) - set(hs["different_rvas"]))
gone = sorted(set(hs["different_rvas"]) - set(ns["different_rvas"]))
print("\nnew DIFFERENT:", nd or "none", " no longer DIFFERENT:", gone or "none")
for r in nd:
    x = nrows[r]
    print("  ", r, x["src"], "first_diff", x["first_diff"], "diff_bytes", x["diff_bytes"], "in scope:", SCOPE.get(r, "no"))
nn = sorted(set(ns["not_in_map_rvas"]) - set(hs["not_in_map_rvas"]))
print("new not-in-map:", nn or "none")
print("in-scope still not-in-map:", sorted(r for r in ns["not_in_map_rvas"] if r in SCOPE) or "none")
print("prov kept VS2022 fns in linked units: head", hs["provisional_functions_vs2022_in_linked_units"],
      " branch", ns["provisional_functions_vs2022_in_linked_units"])
