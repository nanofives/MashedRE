#!/usr/bin/env python3
"""shadow_ab_report.py -- turn shadow_ab.log into per-function verdicts (TT-11).

    py -3.12 re/tools/shadow_ab_report.py [original/shadow_ab.log] [--manifest re/parity/shadow_sites.tsv]
                                          [--tsv out.tsv] [--md out.md]

VERDICTS (one per function seen in the log, plus NO_SAMPLES for armed sites never hit)
  CLEAN       n>0 samples, ndiff==0 on all, PATCHBYTE proof says A/B-IS-REAL
  DIVERGENT   n>0 samples, ndiff>0 on at least one (the port does not reproduce the original)
  UNPROVEN    samples exist but no A/B-IS-REAL proof line -- Uninstall did not change the
              patch byte, so the "original" call may have re-entered our own JMP. A
              vacuous all-OK, NOT evidence (ShadowAB.h "PROOF THE A/B IS REAL").
  SKIPPED     only SKIP:no-hook-index lines -- the hook was never installed (U-9087 E9 guard,
              MASHED_HOOK_ONLY not listing it, or a dual-install refusal). Nothing measured.
  DIVERGENT_FLOAT10  DIVERGENT on a float10 (x87 ST0) return: MSVC long double is 64-bit, so the
              two sides round differently. A lane LIMIT, not a statement about the port.
  NO_SAMPLES  in the manifest as a live site, absent from the log. The scenario never reached
              it; "no samples is NOT evidence" (feedback_evidence_discipline).

A CLEAN row is INPUT to re-classify (cite the log + sample count + proof line). It is not a
C-level by itself: the caller/callee gates and the note still apply.
"""
import argparse
import collections
import csv
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_LOG = ROOT / "original" / "shadow_ab.log"
MANIFEST = ROOT / "re" / "parity" / "shadow_sites.tsv"

# `rva=` is optional: the hand-written B5c sites (RwpIntegrator.cpp b5cLeafLog) omit it and
# write their samples to original/phys_c4_*_selftest.log -- pass those paths as extra logs.
SAMPLE_RE = re.compile(r"^\[(\d+)\] fn=(\S+)(?: rva=([0-9a-f]{8}))? idx=(-?\d+) ndiff=(\d+)(.*)$")
PROOF_RE = re.compile(r"^\[--\] fn=(\S+) PATCHBYTE installed=([0-9a-f]{2}) uninstalled=([0-9a-f]{2}) (\S+)")


def parse(log_paths):
    fns = {}
    lines = []
    for lp in log_paths:
        if Path(lp).exists():
            lines += Path(lp).read_text(encoding="utf-8", errors="replace").splitlines()
    for raw in lines:
        line = raw.strip()
        m = SAMPLE_RE.match(line)
        if m:
            _, fn, rva, idx, nd, detail = m.groups()
            d = fns.setdefault(fn, {"fn": fn, "rva": rva or "", "n": 0, "ndiff": 0, "skip": 0,
                                     "proof": "", "detail": collections.Counter(), "idx": idx})
            if rva and not d["rva"]:
                d["rva"] = rva          # the PATCHBYTE proof line comes first and has no rva=
            if "SKIP:no-hook-index" in detail:
                d["skip"] += 1
                continue
            d["n"] += 1
            if int(nd):
                d["ndiff"] += 1
                d["detail"][detail.strip()] += 1
            continue
        m = PROOF_RE.match(line)
        if m:
            fn, inst, uninst, _verdict = m.groups()
            d = fns.setdefault(fn, {"fn": fn, "rva": "", "n": 0, "ndiff": 0, "skip": 0,
                                     "proof": "", "detail": collections.Counter(), "idx": ""})
            d["proof"] = f"{classify_proof(inst, uninst)} installed={inst} uninstalled={uninst}"
    return fns


def classify_proof(inst, uninst):
    """Decide from the two patch bytes whether the 'original' call really ran original code.

    The header's own label only knows one good shape (installed E9 -> uninstalled not-E9).
    A second shape is also real: the hook was NEVER installed (both bytes equal and not E9),
    which happens when a shadow-wrapped callee is reached through OUR ported caller rather
    than through its own JMP -- the code at the RVA is the untouched original, so calling it
    is a genuine A/B. That is what the 14 'SUSPECT' rows of the first Core23 boot were.
    The one vacuous shape is E9 both sides: Uninstall changed nothing and the call re-entered
    our JMP (or the original is itself an E9 thunk, U-9087 -- indistinguishable here).
    """
    if inst == "e9" and uninst != "e9":
        return "A/B-IS-REAL"
    if inst != "e9" and inst == uninst:
        return "A/B-IS-REAL(hook-not-installed)"
    return "SUSPECT-no-restore"


def verdict(d):
    if d["n"] == 0:
        return "SKIPPED" if d["skip"] else "NO_SAMPLES"
    if not d["proof"].startswith("A/B-IS-REAL"):
        return "UNPROVEN"
    return "DIVERGENT" if d["ndiff"] else "CLEAN"


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("log", nargs="*", default=[str(DEFAULT_LOG)],
                    help="one or more logs (shadow_ab.log, phys_c4_*_selftest.log)")
    ap.add_argument("--manifest", default=str(MANIFEST), help="shadow_sites.tsv; adds NO_SAMPLES rows")
    ap.add_argument("--only", default="", help="comma list of RVAs to report on (else all)")
    ap.add_argument("--tsv", default="", help="write rows here (rva name verdict n ndiff proof detail)")
    ap.add_argument("--md", default="", help="write a markdown evidence table here")
    ap.add_argument("--quiet", action="store_true")
    args = ap.parse_args()

    fns = parse(args.log)
    by_rva = {d["rva"]: d for d in fns.values() if d["rva"]}

    only = {x.strip().lower().replace("0x", "").zfill(8) for x in args.only.split(",") if x.strip()}
    rows = []
    seen = set()
    for d in fns.values():
        if only and d["rva"] not in only:
            continue
        seen.add(d["rva"])
        det = "; ".join(f"{k} x{v}" for k, v in d["detail"].most_common(3))
        rows.append({"rva": d["rva"], "name": d["fn"], "verdict": verdict(d), "n": d["n"],
                     "ndiff": d["ndiff"], "proof": d["proof"], "detail": det})
    mp = Path(args.manifest)
    if mp.exists():
        kinds = {}
        with open(mp, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f, delimiter="	"):
                kinds[r["rva"]] = r.get("kind", "")
        for row in rows:
            # x87 80-bit return compared through a 64-bit long double: not a bit-identity lane
            if row["verdict"] == "DIVERGENT" and kinds.get(row["rva"]) == "ret_float10":
                row["verdict"] = "DIVERGENT_FLOAT10"
        with open(mp, newline="", encoding="utf-8") as f:
            for r in csv.DictReader(f, delimiter="\t"):
                if r.get("status", "").startswith(("GENERATED", "ALREADY")) and r["rva"] not in seen:
                    if only and r["rva"] not in only:
                        continue
                    rows.append({"rva": r["rva"], "name": r["name"], "verdict": "NO_SAMPLES",
                                 "n": 0, "ndiff": 0, "proof": "", "detail": ""})
    rows.sort(key=lambda r: (r["verdict"], r["rva"]))

    tally = collections.Counter(r["verdict"] for r in rows)
    if not args.quiet:
        print(f"{'+'.join(Path(l).name for l in args.log)}: " + "  ".join(f"{k}={v}" for k, v in sorted(tally.items())))
        for r in rows:
            print(f"  {r['verdict']:<10} {r['rva']}  {r['name']:<34} n={r['n']:<4} ndiff={r['ndiff']:<4} "
                  f"{r['proof']}  {r['detail']}")
    fields = ["rva", "name", "verdict", "n", "ndiff", "proof", "detail"]
    if args.tsv:
        with open(args.tsv, "w", newline="", encoding="utf-8") as f:
            w = csv.DictWriter(f, fieldnames=fields, delimiter="\t")
            w.writeheader()
            w.writerows(rows)
    if args.md:
        with open(args.md, "w", encoding="utf-8") as f:
            f.write(f"| rva | name | verdict | samples | divergent | proof | detail |\n|---|---|---|---|---|---|---|\n")
            for r in rows:
                f.write(f"| 0x{r['rva']} | {r['name']} | {r['verdict']} | {r['n']} | {r['ndiff']} | "
                        f"{r['proof']} | {r['detail']} |\n")
    return 0 if tally.get("DIVERGENT", 0) == 0 else 2


if __name__ == "__main__":
    sys.exit(main())
