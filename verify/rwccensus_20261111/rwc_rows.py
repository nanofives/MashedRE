"""RWC-CENSUS step 1: the C1 evidence for the RW-band functions RW-SIZING found with no census row.

  py -3.12 verify/rwccensus_20261111/rwc_rows.py            # writes NEWROWS.tsv
  py -3.12 verify/rwccensus_20261111/rwc_rows.py --score    # after the census: writes STATIC_SCORED.tsv + summary

Static, read-only. Same graph as verify/rwsizing_20261111/rw_census.py (direct E8 edges + address-taken reloc
edges, origtop_size.py), same band rule, same executed set (re/coverage/reach.tsv of 2026-10-11, read from git
at the RW-SIZING base so a refreshed reach.tsv does not move the input set).

NEWROWS.tsv: one row per function that RW-SIZING lists as `uncensused` + `static_below=y` + no hooks.csv row (218).
For each: the subsystem by band (physics band -> RenderWare-Physics-3.7, else renderware), and a BFS caller chain
from an executed RW-band function down to it, each edge marked d (direct call) or r (address taken). That chain is
the C1 evidence (re/CONFIDENCE.md C0->C1 (c): a caller chain that anchors it to a known subsystem entry point).

--score: joins the refreshed re/coverage/reach.tsv onto all 220 static-only rows (218 + the 2 with a row).
"""
from __future__ import annotations

import collections
import csv
import io
import pathlib
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[1]
sys.path.insert(0, str(ROOT / "verify" / "rwsizing_20261111"))
import rw_census as RC  # noqa: E402

BASE_REACH_REV = "778b8b91d"   # orch/integration when this row started; reach.tsv there = the 2026-10-11 census


def base_reach():
    txt = subprocess.run(["git", "-C", str(ROOT), "show", f"{BASE_REACH_REV}:re/coverage/reach.tsv"],
                         capture_output=True, text=True, check=True).stdout
    rt = {}
    for r in csv.DictReader(io.StringIO(txt), delimiter="\t"):
        a = int(r["rva"], 16)
        if a < 0x00400000:
            a += 0x00400000
        rt[a] = (r["reached_menu"] == "1", r["reached_race_load"] == "1", r["reached_race_run"] == "1")
    return rt


def targets():
    with open(ROOT / "verify" / "rwsizing_20261111" / "RW_FUNCS.tsv", newline="") as fh:
        rows = [r for r in csv.DictReader(fh, delimiter="\t")
                if r["phase"] == "uncensused" and r["static_below"] == "y"]
    return rows


def graph():
    O = RC.O
    ent, bounds = O.load_bounds()
    fset = set(ent)
    H = RC.P.load_hooks()
    edges = collections.defaultdict(dict)    # a -> {callee: kind}
    for kind, src in (("r", O.reloc_edges(RC.EXE, ent, bounds)[0]), ("d", O.direct_edges())):
        for a, cs in src.items():
            for c in cs:
                if c in fset and c != a:
                    edges[a][c] = kind          # direct wins when both
    rw = {a: RC.band(a, H.get(a)) for a in ent}
    rw = {a: b for a, b in rw.items() if b}
    return ent, H, edges, rw


def main(argv):
    ent, H, edges, rw = graph()
    rt = base_reach()
    executed = {a for a in rw if a in rt and any(rt[a])}
    parent = {a: None for a in executed}
    q = collections.deque(sorted(executed))
    while q:
        a = q.popleft()
        for c, k in sorted(edges.get(a, {}).items()):
            if c in rw and c not in parent:
                parent[c] = (a, k)
                q.append(c)

    T = targets()
    if "--score" in argv:
        return score(T, rt)
    out = []
    for r in T:
        a = int(r["rva"], 16)
        if r["conf"] != "NOROW":
            continue
        chain, cur = [], a
        while parent[cur] is not None:
            p, k = parent[cur]
            chain.append((p, k))
            cur = p
        root = cur
        ph = "+".join(n for n, f in zip(("menu", "race_load", "race_run"), rt[root]) if f)
        hops = " ".join(f"0x{p:08x}-{k}->" for p, k in reversed(chain)) + f" 0x{a:08x}"
        sub = ("third-party-library[RenderWare-Physics-3.7]" if r["band"] == "physics"
               else "third-party-library[renderware]")
        out.append(dict(rva=f"0x{a:08x}", subsystem=sub, band=r["band"], component=r["component"],
                        size=r["size"], root=f"0x{root:08x}", root_phase=ph, depth=len(chain),
                        first_edge=chain[0][1], chain=hops))
    with open(HERE / "NEWROWS.tsv", "w", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=list(out[0]), delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(out)
    print(f"NEWROWS.tsv: {len(out)} rows;", collections.Counter(o["subsystem"] for o in out),
          "depth", collections.Counter(o["depth"] for o in out), "edge", collections.Counter(o["first_edge"] for o in out))
    return 0


def score(T, _rt_base):
    new = {}
    with open(ROOT / "re" / "coverage" / "reach.tsv", newline="") as fh:
        for r in csv.DictReader(fh, delimiter="\t"):
            a = int(r["rva"], 16)
            a = a + 0x00400000 if a < 0x00400000 else a
            new[a] = r
    out = []
    for r in T:
        a = int(r["rva"], 16)
        n = new.get(a)
        if n is None:
            ph, rm, rr = "NO-ROW", "-", "-"
        else:
            ph = "+".join(k for k in ("menu", "race_load", "race_run") if n[f"reached_{k}"] == "1") or "not-run"
            rm, rr = n["runs_menu"], n["runs_race"]
        out.append(dict(rva=r["rva"], band=r["band"], component=r["component"], size=r["size"], phase=ph,
                        runs_menu=rm, runs_race=rr))
    with open(HERE / "STATIC_SCORED.tsv", "w", newline="") as fh:
        w = csv.DictWriter(fh, fieldnames=list(out[0]), delimiter="\t", lineterminator="\n")
        w.writeheader()
        w.writerows(out)
    ex = [o for o in out if o["phase"] not in ("not-run", "NO-ROW")]
    print(f"static-only rows: {len(out)}  executed: {len(ex)} ({sum(int(o['size']) for o in ex)} B)  "
          f"not-run: {sum(o['phase'] == 'not-run' for o in out)}  no reach row: {sum(o['phase'] == 'NO-ROW' for o in out)}")
    print("by phase:", collections.Counter(o["phase"] for o in out))
    print("executed by component:", collections.Counter(o["component"] for o in ex))
    print("executed by band:", collections.Counter(o["band"] for o in ex))
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
