#!/usr/bin/env python3
"""area_residue.py -- turn a subsystem label into an ordered candidate queue.

The per-area recurring loop needs one thing nothing in the tree produced before:
given an area (canonical hooks.csv subsystem), emit the residue -- the rows below
the confidence bar -- ordered cheapest-win-first, each tagged with the THREE
coverage facts the ledger tracks separately:

  documented   -- confidence >= bar (this residue is everything BELOW the bar)
  implemented  -- file column points at a real mashedmod/src/mashed_re/**.cpp
  linked       -- that .cpp is in build.bat (exe target) or asi_sources.rsp

A C3 row can be pure documentation (file points at re/analysis/**.md); S-DoD needs
LINKED, so the readiness order surfaces rows already implemented+linked first --
those are the shortest hop to a real, default-path-reached function.

No Ghidra, no MCP: pure read of hooks.csv + the two build manifests. Runs on account2.

Usage:
  py -3.12 re/tools/area_residue.py --subsystem render
  py -3.12 re/tools/area_residue.py --subsystem render --file-prefix Camera/
  py -3.12 re/tools/area_residue.py --subsystem render --bar C3 --out queue.tsv --json
"""
import argparse
import csv
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
HOOKS = ROOT / "hooks.csv"
BUILD_BAT = ROOT / "mashedmod" / "build.bat"
ASI_RSP = ROOT / "mashedmod" / "asi_sources.rsp"
SRC_DIR = "mashedmod/src/mashed_re/"

CONF_ORDER = {"C0": 0, "C1": 1, "C2": 2, "C3": 3, "C4": 4}


def conf_rank(raw: str) -> int:
    """Accept 'C2' or bare '2'; unknown -> -1 (sorts as most-unknown)."""
    s = (raw or "").strip().upper()
    if s in CONF_ORDER:
        return CONF_ORDER[s]
    if s.isdigit():
        return int(s)
    m = re.match(r"C?(\d)", s)
    return int(m.group(1)) if m else -1


def norm(p: str) -> str:
    return (p or "").replace("\\", "/").strip().strip('"').lower()


def load_linked() -> set:
    """Basename set of every .cpp referenced by either build manifest."""
    linked = set()
    for f in (BUILD_BAT, ASI_RSP):
        if not f.exists():
            continue
        text = f.read_text(encoding="utf-8", errors="replace")
        for m in re.finditer(r'([^\s"]+\.cpp)', text):
            # skip REM/commented lines in build.bat
            line = m.string[m.string.rfind("\n", 0, m.start()) + 1: m.start()]
            if re.match(r"\s*(rem\b|::)", line, re.IGNORECASE):
                continue
            linked.add(norm(m.group(1)).rsplit("/", 1)[-1])
    return linked


def classify_file(fcol: str) -> str:
    f = norm(fcol)
    if not f:
        return "empty"
    if f.endswith(".cpp") and SRC_DIR in f + "/" or (f.endswith(".cpp") and "mashed_re/" in f):
        return "cpp"
    if f.endswith(".md"):
        return "doc"
    if f.endswith(".cpp"):
        return "cpp"
    return "other"


def rel_src(fcol: str) -> str:
    f = norm(fcol)
    i = f.find("mashed_re/")
    return f[i + len("mashed_re/"):] if i >= 0 else f


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--subsystem", required=True, help="canonical hooks.csv subsystem")
    ap.add_argument("--bar", default="C3", help="confidence bar; residue = rows strictly below it (default C3)")
    ap.add_argument("--file-prefix", default="", help="restrict to rows whose file (relative to mashed_re/) starts with this, e.g. Camera/")
    ap.add_argument("--name-match", default="", help="restrict to rows whose name matches this regex (case-insensitive), e.g. 'cam|view' -- use for slices (cameras, modes) that have no subsystem label")
    ap.add_argument("--out", default="", help="write TSV queue here (default: stdout only prints summary)")
    ap.add_argument("--json", action="store_true", help="also emit a JSON blob to stdout")
    args = ap.parse_args()

    bar = conf_rank(args.bar)
    prefix = args.file_prefix.replace("\\", "/").lower()
    name_re = re.compile(args.name_match, re.IGNORECASE) if args.name_match else None
    linked = load_linked()

    if not HOOKS.exists():
        print(f"FATAL: {HOOKS} not found", file=sys.stderr)
        return 2

    rows = []
    with HOOKS.open(encoding="utf-8", errors="replace", newline="") as fh:
        reader = csv.DictReader(r for r in fh if not r.lstrip().startswith("#"))
        for row in reader:
            if (row.get("subsystem") or "").strip() != args.subsystem:
                continue
            cr = conf_rank(row.get("confidence", ""))
            if cr >= bar:
                continue  # already at/above the bar -> not residue
            fcls = classify_file(row.get("file", ""))
            rel = rel_src(row.get("file", ""))
            if prefix and not rel.startswith(prefix):
                continue
            if name_re and not name_re.search(row.get("name", "")):
                continue
            base = rel.rsplit("/", 1)[-1]
            is_linked = fcls == "cpp" and base in linked
            has_scen = bool((row.get("scenario") or "").strip())
            fd = (row.get("frida_diff") or "").strip()
            fdl = fd.lower()
            allvals = ",".join(str(v) for v in row.values())
            # frida_diff / notes tell us how close a C2 row really is:
            #   BLOCKED*        -> diff known-unreachable; sink below everything.
            #   notes DEMOTED   -> was promoted then reverted (needs harness work, NOT cheap).
            #                      Learned the hard way: an artifact path (log/*.csv) can be a
            #                      DEGENERATE all-True diff for a demoted row -- do NOT trust it.
            #   frida == green* -> a real passing verdict, unpromoted -> the TRUE cheapest win.
            blocked = fdl.startswith("blocked")
            demoted = "DEMOTED" in allvals
            has_evidence = fdl.startswith("green")   # only a literal green verdict, not an artifact path
            readiness = (
                (2 if is_linked else 1 if fcls == "cpp" else 0)
                + (1 if has_scen else 0)
                + (3 if has_evidence else 0)   # verified-but-unpromoted -> closest to C3
                + (cr / 10.0)                  # C2 before C1 before C0 within a tier
            )
            if demoted:
                readiness -= 50.0             # reverted: real work remains, not a cheap win
            if blocked:
                readiness = -100.0            # known-blocked: sink below everything
            rows.append({
                "rva": (row.get("rva") or "").strip(),
                "name": (row.get("name") or "").strip(),
                "confidence": (row.get("confidence") or "").strip(),
                "file_class": fcls,
                "file": rel,
                "linked": "yes" if is_linked else "no",
                "scenario": "yes" if has_scen else "no",
                "frida": ("BLOCKED" if blocked else "DEMOTED" if demoted else "green" if has_evidence else fd[:14] if fd else ""),
                "readiness": round(readiness, 3),
            })

    rows.sort(key=lambda r: (-r["readiness"], r["rva"]))

    # summary
    n = len(rows)
    n_cpp = sum(1 for r in rows if r["file_class"] == "cpp")
    n_linked = sum(1 for r in rows if r["linked"] == "yes")
    n_scen = sum(1 for r in rows if r["scenario"] == "yes")
    n_doc = sum(1 for r in rows if r["file_class"] == "doc")
    n_empty = sum(1 for r in rows if r["file_class"] == "empty")
    n_evid = sum(1 for r in rows if r["frida"] == "green")
    n_blk = sum(1 for r in rows if r["frida"] == "BLOCKED")
    n_dem = sum(1 for r in rows if r["frida"] == "DEMOTED")
    scope = args.subsystem + (f" [{args.file_prefix}]" if args.file_prefix else "")
    print(f"residue({scope}, below {args.bar}): {n} rows")
    print(f"  implemented .cpp: {n_cpp}   linked: {n_linked}   has-scenario: {n_scen}   green-unpromoted: {n_evid}   DEMOTED: {n_dem}   BLOCKED: {n_blk}   doc-only: {n_doc}   no-file: {n_empty}")
    if n:
        print("  top of queue (cheapest-win-first):")
        for r in rows[:8]:
            print(f"    {r['rva']}  {r['confidence']:<3} {r['file_class']:<5} link={r['linked']:<3} scen={r['scenario']:<3} frida={r['frida'] or '-':<10} {r['name']}")

    if args.out:
        outp = Path(args.out)
        with outp.open("w", encoding="utf-8", newline="") as fh:
            w = csv.DictWriter(fh, fieldnames=list(rows[0].keys()) if rows else
                               ["rva", "name", "confidence", "file_class", "file", "linked", "scenario", "readiness"],
                               delimiter="\t")
            w.writeheader()
            w.writerows(rows)
        print(f"  wrote {n} rows -> {outp}")

    if args.json:
        print(json.dumps({"subsystem": args.subsystem, "file_prefix": args.file_prefix,
                          "bar": args.bar, "count": n, "implemented": n_cpp,
                          "linked": n_linked, "rows": rows}, indent=0))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
