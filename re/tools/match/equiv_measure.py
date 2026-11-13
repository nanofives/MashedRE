"""Run equiv.py over every REFUSED row of re/analysis/match_ledger.tsv that has a source (EQUIV-CHECK measurement).

    py -3.12 re/tools/match/equiv_measure.py --out verify/equivcheck_20261111/measure.tsv [--jobs 6] [--relax-icall]

One subprocess per source file (timeout 240 s), so a pathological function cannot hang the run. Prints one line
per file as it finishes: [N/M] <file> <verdict counts>.
"""
import argparse
import collections
import concurrent.futures as cf
import json
import pathlib
import subprocess
import sys

HERE = pathlib.Path(__file__).resolve().parent
ROOT = HERE.parents[2]
LEDGER = ROOT / "re" / "analysis" / "match_ledger.tsv"


TOOL = HERE / "equiv.py"


def run(args):
    src, rvas, relax = args
    cmd = [sys.executable, str(TOOL), str(ROOT / src), "--only", ",".join(f"0x{r:08x}" for r in rvas),
           "--json"] + (["--relax-icall"] if relax else [])
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=240, cwd=str(ROOT))
    except subprocess.TimeoutExpired:
        return src, [{"addr": x, "verdict": "NOT-PROVEN", "kind": "timeout", "detail": "240 s"} for x in rvas]
    try:
        i = r.stdout.index("[")
        res = json.loads(r.stdout[i:])
    except ValueError:
        msg = (r.stdout + r.stderr).strip().splitlines()
        msg = msg[-1] if msg else "no output"
        kind = "compile-failed" if "compile failed" in r.stdout + r.stderr else "tool-error"
        return src, [{"addr": x, "verdict": "NOT-PROVEN", "kind": kind, "detail": msg[:200]} for x in rvas]
    got = {x["addr"] for x in res}
    for x in rvas:
        if x not in got:
            res.append({"addr": x, "verdict": "NOT-PROVEN", "kind": "no-annotation", "detail": "not in source"})
    return src, res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", required=True)
    ap.add_argument("--jobs", type=int, default=6)
    ap.add_argument("--relax-icall", action="store_true")
    ap.add_argument("--limit", type=int, default=0)
    ap.add_argument("--tool", default="", help="a frozen copy of equiv.py in this directory (for long runs)")
    ap.add_argument("--proven-in", default="", help="only the rows proven (EQUIV / IF) in this earlier TSV")
    ap.add_argument("--rows-from", default="", help="exactly the rows (rva + source) of this earlier TSV, so a re-run "
                                                    "compares like with like when the ledger has moved since")
    a = ap.parse_args()
    global TOOL
    if a.tool:
        TOOL = HERE / a.tool
    rows = [ln.split("\t") for ln in LEDGER.read_text().splitlines()[1:] if ln.strip()]
    meta = {}
    by_src = collections.defaultdict(list)
    for r in rows:
        if r[14] != "REFUSED" or not r[1] or r[1] == "-":
            continue
        addr = int(r[0], 16)
        meta[addr] = r
        by_src[r[1]].append(addr)
    if a.rows_from:
        by_src = collections.defaultdict(list)
        for ln in pathlib.Path(a.rows_from).read_text().splitlines()[1:]:
            f = ln.split("\t")
            by_src[f[1]].append(int(f[0], 16))
    if a.proven_in:
        keep = {int(ln.split("\t")[0], 16) for ln in pathlib.Path(a.proven_in).read_text().splitlines()[1:]
                if ln.split("\t")[7] != "NOT-PROVEN"}
        by_src = {k: [x for x in v if x in keep] for k, v in by_src.items()}
        by_src = {k: v for k, v in by_src.items() if v}
    items = sorted(by_src.items())
    if a.limit:
        items = items[:a.limit]
    out = []
    with cf.ThreadPoolExecutor(a.jobs) as ex:
        futs = [ex.submit(run, (src, rvas, a.relax_icall)) for src, rvas in items]
        for i, f in enumerate(cf.as_completed(futs)):
            src, res = f.result()
            c = collections.Counter(x["verdict"] for x in res)
            print(f"[{i + 1}/{len(items)}] {src} " + " ".join(f"{k}={v}" for k, v in sorted(c.items())), flush=True)
            for x in res:
                m = meta.get(x["addr"])
                out.append((x, src, m))
    lines = ["rva\tsource\tbest_pct\tbytes_bad\tbytes_n\tsubsystem\thooks_conf\tverdict\tclasses\tregs\tcallers_all_ignore\tkind\tdetail\trules\tlocal_callees\thalts"]
    for x, src, m in sorted(out, key=lambda t: t[0]["addr"]):
        lines.append("\t".join([
            f"0x{x['addr']:08x}", src, m[3] if m else "", m[4] if m else "", m[5] if m else "", m[13] if m else "",
            m[12] if m else "", x["verdict"], ",".join(x.get("classes", [])), ",".join(x.get("regs", [])),
            str(x.get("callers_all_ignore", "")), x.get("kind", ""), str(x.get("detail", "")).replace("\t", " ")[:300],
            ",".join(x.get("rules", [])), ",".join(x.get("local_callees", [])),
            " | ".join(x.get("halts", [])).replace("\t", " ")[:600]]))
    pathlib.Path(a.out).parent.mkdir(parents=True, exist_ok=True)
    pathlib.Path(a.out).write_text("\n".join(lines) + "\n")
    c = collections.Counter(x["verdict"] for x, _, _ in out)
    print("TOTAL", dict(c), "->", a.out)


if __name__ == "__main__":
    main()
