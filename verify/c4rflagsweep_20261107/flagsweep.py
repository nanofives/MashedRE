"""C4R-C2-FLAGSWEEP: recompile every re/match source holding a C2/C3 row that is not byte-identical in
re/analysis/match_ledger.tsv under a flag grid, with the recorded compiler (XP DDK cl 13.00.9176 / c2 13.00.9178),
and record per (source, flag set, RVA) the match.py bytes verdict plus relocheck (2026-11-07).

  py -3.12 verify/c4rflagsweep_20261107/flagsweep.py run [--jobs 8] [--limit N]
  py -3.12 verify/c4rflagsweep_20261107/flagsweep.py report

run    appends to sweep.jsonl beside this file (committed gzipped as sweep.jsonl.gz; report reads either) (resumable: finished (source, flags) pairs are skipped).
report writes hits.tsv (every target row that reaches the match gate under some flag set) and summary.json.

Grid: O {/O1,/O2} x Ob {/Ob0,/Ob1,/Ob2} x G {(default = /GB),/G5,/G6} x {,/Oy-} x {,/GS} = 72 sets, plus each
source's recorded flags with /GF appended. match.py always adds /Gy, so /Gy is not an axis.
Gate (same as c4_reclass.gate_match): bytes_bad == 0 and (insn acc == 1 or n_mine > n_orig) and relocheck OK.
"""
import argparse
import csv
import itertools
import json
import pathlib
import sys
from concurrent.futures import ProcessPoolExecutor, as_completed

HERE = pathlib.Path(__file__).resolve().parent
W = HERE.parents[1]
MATCHDIR = W / "re/tools/match"
LEDGER = W / "re/analysis/match_ledger.tsv"
OUT = HERE / "sweep.jsonl"

GRID = [" ".join(x for x in combo if x) for combo in itertools.product(
    ["/O1", "/O2"], ["/Ob0", "/Ob1", "/Ob2"], ["", "/G5", "/G6"], ["", "/Oy-"], ["", "/GS"])]


def ledger():
    with open(LEDGER, encoding="utf-8") as f:
        return list(csv.DictReader(f, delimiter="\t"))


def targets():
    rows = [r for r in ledger() if r["hooks_conf"] in ("C2", "C3") and r["verdict"] == "REFUSED"]
    by_src = {}
    for r in rows:
        by_src.setdefault(r["source"], []).append(r["rva"])
    return rows, by_src


def norm_flags(s):
    return " ".join(s.split())


def work(src, flags):
    sys.path.insert(0, str(MATCHDIR))
    import match, relocheck  # noqa: E402
    p = W / src
    res = {"src": src, "flags": flags, "fns": {}}
    try:
        for r in match.compare(p, flags, quiet=True, bytes_mode=True):
            res["fns"][f"0x{r['addr']:08x}"] = {"acc": round(r["acc"], 5), "bad": len(r["bytes_bad"]),
                                                "n": r["bytes_n"], "nm": r["n_mine"], "no": r["n_orig"]}
        if any(v["bad"] == 0 for v in res["fns"].values()):
            relocheck.match.file_flags = lambda _t, _f=flags: _f  # relocheck reads the FLAGS line; override it
            for r in relocheck.check(p):
                d = res["fns"].setdefault(f"0x{r[0]:08x}", {})
                d["reloc"] = r[1]
    except BaseException as e:  # compile error under this flag set
        res["error"] = (str(e).splitlines() or [type(e).__name__])[-1][:200]
    return res


def gate(d):
    return bool(d) and d.get("bad", 1) == 0 and (d.get("acc") == 1 or d.get("nm", 0) > d.get("no", 0)) \
        and d.get("reloc") == "OK"


def run(a):
    rows, by_src = targets()
    sys.path.insert(0, str(MATCHDIR))
    import match  # noqa: E402
    done = set()
    if OUT.exists():
        for ln in OUT.read_text(encoding="utf-8").splitlines():
            j = json.loads(ln)
            done.add((j["src"], j["flags"]))
    tasks = []
    for src in sorted(by_src):
        rec = norm_flags(match.file_flags((W / src).read_text()) or match.DEFAULT_FLAGS)
        for fl in GRID + [rec, rec + " /GF"]:
            if (src, fl) not in done and (src, fl) not in tasks:
                tasks.append((src, fl))
    if a.limit:
        tasks = tasks[:a.limit]
    print(f"targets: {len(rows)} rows in {len(by_src)} sources; {len(tasks)} compiles to run "
          f"({len(done)} already done)", flush=True)
    n = len(tasks)
    with ProcessPoolExecutor(max_workers=a.jobs) as ex, open(OUT, "a", encoding="utf-8") as fo:
        futs = {ex.submit(work, s, f): (s, f) for s, f in tasks}
        for i, fu in enumerate(as_completed(futs), 1):
            r = fu.result()
            fo.write(json.dumps(r) + "\n")
            fo.flush()
            st = "ERR" if r.get("error") else ("HIT" if any(gate(r["fns"].get(v)) for v in by_src[r["src"]]) else "ok")
            print(f"[{i}/{n}] {r['src']} {r['flags']} {st}", flush=True)


def report(a):
    rows, by_src = targets()
    sys.path.insert(0, str(MATCHDIR))
    import match  # noqa: E402
    res = {}
    import gzip
    src = OUT.read_text(encoding="utf-8") if OUT.exists() else gzip.open(str(OUT) + ".gz", "rt", encoding="utf-8").read()
    for ln in src.splitlines():
        j = json.loads(ln)
        res[(j["src"], j["flags"])] = j
    rec = {s: norm_flags(match.file_flags((W / s).read_text()) or match.DEFAULT_FLAGS) for s in by_src}
    hits = []
    for r in rows:
        s, v = r["source"], r["rva"]
        ok = sorted(fl for (ss, fl), j in res.items() if ss == s and gate(j["fns"].get(v)))
        if ok:
            # the other functions of the same source must not lose the gate under the new flags
            recj = res.get((s, rec[s]), {"fns": {}})
            others_ok = {fl: all(gate(res[(s, fl)]["fns"].get(o)) for o, d in recj["fns"].items()
                                 if o != v and gate(d)) for fl in ok}
            hits.append({"rva": v, "source": s, "conf": r["hooks_conf"], "subsystem": r["subsystem"],
                         "recorded": rec[s], "recorded_pct": r["best_pct"], "n_ok": len(ok), "flags_ok": ok,
                         "flags_ok_safe": [fl for fl in ok if others_ok[fl]]})
    with open(HERE / "hits.tsv", "w", encoding="utf-8", newline="") as f:
        f.write("rva\tsource\tconf\tsubsystem\trecorded_flags\trecorded_pct\tn_flagsets_ok\tflagsets_ok\t"
                "flagsets_ok_without_dropping_a_sibling\n")
        for h in hits:
            f.write(f"{h['rva']}\t{h['source']}\t{h['conf']}\t{h['subsystem']}\t{h['recorded']}\t{h['recorded_pct']}\t"
                    f"{h['n_ok']}\t{' | '.join(h['flags_ok'])}\t{' | '.join(h['flags_ok_safe'])}\n")
    errs = sum(1 for j in res.values() if j.get("error"))
    # best pct per row over the grid (how close the remaining rows get)
    best = {}
    for r in rows:
        s, v = r["source"], r["rva"]
        b = max((j["fns"].get(v, {}).get("acc", 0) for (ss, fl), j in res.items() if ss == s), default=0)
        best[v] = (float(r["best_pct"]), 100 * b)
    improved = sum(1 for o, b in best.values() if b > o + 0.05)
    summ = {"target_rows": len(rows), "sources": len(by_src), "compiles": len(res), "compile_errors": errs,
            "rows_reaching_gate": len(hits), "rows_reaching_gate_sibling_safe": sum(1 for h in hits if h["flags_ok_safe"]),
            "rows_insn_pct_improved_over_recorded": improved}
    (HERE / "summary.json").write_text(json.dumps(summ, indent=1))
    with open(HERE / "best_pct.tsv", "w", encoding="utf-8", newline="") as f:
        f.write("rva\trecorded_pct\tbest_grid_pct\n")
        for v, (o, b) in sorted(best.items()):
            f.write(f"{v}\t{o:.1f}\t{b:.1f}\n")
    # per 64 KB address range: which grid flag set scores best on average (per-object flags would show here)
    acc = {}  # rva -> {flags: acc}
    for r in rows:
        s, v = r["source"], r["rva"]
        acc[v] = {fl: j["fns"].get(v, {}).get("acc", 0) for (ss, fl), j in res.items() if ss == s and fl in GRID}
    ranges = {}
    for r in rows:
        if len(acc[r["rva"]]) == len(GRID):
            ranges.setdefault(int(r["rva"], 16) >> 16, []).append(r)
    with open(HERE / "ranges.tsv", "w", encoding="utf-8", newline="") as f:
        f.write("range\trows\tbest_grid_flags\tbest_mean_pct\trecorded_mean_pct\trows_where_a_grid_set_beats_recorded\t"
                "rows_whose_best_is_the_range_best\n")
        for k in sorted(ranges):
            rr = ranges[k]
            mean = {fl: sum(acc[r["rva"]][fl] for r in rr) / len(rr) for fl in GRID}
            # ties (e.g. /Ob0 = /Ob1 when nothing inlines) go to the set most rows of the range record
            nrec = {fl: sum(1 for r in rr if set(rec[r["source"]].split()) == set(fl.split())) for fl in GRID}
            bfl = max(GRID, key=lambda fl: (round(mean[fl], 9), nrec[fl]))
            recm = sum(res.get((r["source"], rec[r["source"]]), {"fns": {}})["fns"].get(r["rva"], {}).get("acc", 0)
                       for r in rr) / len(rr)
            beats = sum(1 for r in rr if max(acc[r["rva"]].values()) > best[r["rva"]][0] / 100 + 0.0005)
            same = sum(1 for r in rr if acc[r["rva"]][bfl] >= max(acc[r["rva"]].values()) - 1e-9)
            f.write(f"0x{k:04x}xxxx\t{len(rr)}\t{bfl}\t{100 * mean[bfl]:.1f}\t{100 * recm:.1f}\t{beats}\t{same}\n")
    print(json.dumps(summ, indent=1))


if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("cmd", choices=["run", "report"])
    ap.add_argument("--jobs", type=int, default=8)
    ap.add_argument("--limit", type=int, default=0)
    a = ap.parse_args()
    (run if a.cmd == "run" else report)(a)
