"""Per-function class and route table for the T2-path functions with no C4 body.

Same logic as log/orch/audit_20261121/census_join2.py (T2 = race-load or race-tick
phases of the T2-RACETICK-CENSUS, level != C4) plus the class split of xtab.py,
written as one tracked table (re/census/t2_route_table.tsv).

Usage: py -3.12 re/census/t2_route_table.py [--root <repo root>] [--out <tsv>]
Default root: the repo root containing this script.
"""
import argparse
import collections
import csv
import os

ROUTES = {
    "game code": "replay inside the original on ref1, then provisional bind",
    "library (Lua/qhull/CRT)": "library build, outside the port count (L-67/L-70/L-97)",
    "RenderWare core": "L-124 route (matched RW core on d3d9)",
    "RW-Physics": "replay inside the original, then PORT-RWP-WIRE",
    "no hooks.csv row": "discovery first (function + C1 row), then by class",
}


def cls(s):
    s = s.lower()
    if "lua" in s or "qhull" in s or "crt" in s or "libpng" in s or "zlib" in s:
        return "library (Lua/qhull/CRT)"
    if "renderware-physics" in s:
        return "RW-Physics"
    if "renderware" in s:
        return "RenderWare core"
    if s == "":
        return "no hooks.csv row"
    return "game code"


def lvl(s):
    return s.split()[0].split(":")[0]


def main():
    ap = argparse.ArgumentParser()
    here = os.path.dirname(os.path.abspath(__file__))
    ap.add_argument("--root", default=os.path.abspath(os.path.join(here, "..", "..")))
    ap.add_argument("--out", default=os.path.join(here, "t2_route_table.tsv"))
    a = ap.parse_args()

    def rows(path):
        with open(os.path.join(a.root, path), encoding="utf-8", errors="replace", newline="") as f:
            return list(csv.DictReader((ln for ln in f if not ln.startswith("#")), delimiter="\t"))

    census = rows("verify/census_20261116/census_T2-RACETICK-CENSUS.tsv")
    ledger = {r["rva"].lower(): r for r in rows("re/analysis/match_ledger.tsv")}
    prov = {r["rva"].lower(): r for r in rows("re/match/PROVISIONAL.tsv")}
    insitu = {r["rva"].lower(): r for r in rows("verify/provinsitu_20261111/PROV_VERDICTS.tsv")}

    T2 = {"race-load", "race-tick"}
    agg = {}
    for r in census:
        k = r["rva"].lower()
        v = agg.setdefault(k, dict(t2calls=0, status=r["status"], binding=set(), phases=set(),
                                   name=r["name"], sub=r["subsystem"]))
        v["phases"].add(r["phase"])
        v["binding"].add(r["binding"])
        if r["phase"] in T2:
            v["t2calls"] += int(r["calls"] or 0)
    non = {k: v for k, v in agg.items() if v["phases"] & T2 and lvl(v["status"]) != "C4"}

    out = []
    for k, v in non.items():
        led = ledger.get(k, {})
        pct = ""
        src = ""
        for d in (led, prov.get(k, {})):
            try:
                pct = float(d["best_pct"])
                src = d.get("source", "")
                break
            except (KeyError, ValueError):
                pass
        c = cls(v["sub"])
        out.append((k, c, ROUTES[c], v["t2calls"], lvl(v["status"]), src, pct, v["name"], v["sub"],
                    ",".join(sorted(v["binding"])), int(k in prov), insitu.get(k, {}).get("verdict", "")))
    out.sort(key=lambda r: -r[3])
    with open(a.out, "w", encoding="utf-8", newline="") as f:
        f.write("rva\tclass\troute\tcalls\thooks_level\tsource\tbest_pct\tname\tsubsystem\tbinding\tin_provisional\tinsitu_verdict\n")
        for r in out:
            f.write("\t".join("" if x is None else str(x) for x in r) + "\n")
    n = collections.Counter(r[1] for r in out)
    tot = sum(r[3] for r in out) or 1
    calls = collections.Counter()
    for r in out:
        calls[r[1]] += r[3]
    print("total", len(out))
    for c in ROUTES:
        print(f"{c:26s} {n[c]:4d} {100*calls[c]/tot:5.1f}% of calls")
    print("wrote", a.out)


if __name__ == "__main__":
    main()
