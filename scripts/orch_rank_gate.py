#!/usr/bin/env python3
"""Rank the caller-gate pool SAFE-first, then cheapest — not size-first.

WHY THIS IS A SEPARATE SCRIPT (and not a sort key in orch_make_brief_queue.py):
harness_safety does not exist in caller_gate_144.tsv. The gate TSV carries only
rva/name/conf/size/callers/verdict; safety is produced downstream, by the
read-fleet brief, as a judgement over the plate body. So the bucket-cutting
script at orch_make_brief_queue.py:192 CANNOT rank by safety — at that point
nothing knows it yet. Ranking can only happen after the briefs come back, which
is what this script does.

The size-ascending order the cutter uses has a specific pathology, called out in
its own docstring and confirmed twice: tiny leaf functions that share one
exit-path caller are exactly what a shutdown sequence looks like, so
cheapest-first preferentially surfaces TEARDOWN families. iter11 cycle1 lost a
whole bucket to it (all 6 of state_boot_b1_s6 came back MUTATOR_LANE, 5 of them
13-byte singleton releases under HardwareExitApplication). This script is the
correction: authorable rows first, teardown last.

Reads every read-fleet gate brief (the fenced TSV each one emits), joins
harness_safety + verdict back onto the gate TSV, and writes gate_ranked.tsv.

Usage:
  py -3.12 scripts/orch_rank_gate.py [out.tsv]
"""
import csv
import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]
GATE = ROOT / "re/orchestrator/caller_gate_144.tsv"
RUNS = ROOT / "re/orchestrator/read_fleet/runs"

# A brief's TSV rows start with the RVA and are tab- or pipe-separated depending
# on how the worker rendered them; accept both rather than fight the formatting.
ROW_RE = re.compile(r"^\|?\s*(0x00[0-9a-fA-F]{6})\s*[|\t]")

# Verdict ordering: what can be authored now, ..., what needs a different lane.
VERDICT_RANK = {
    "READY": 0,
    "NEEDS_NEW_HANDLER": 1,
    "NEEDS_GHIDRA": 2,
    "MUTATOR_LANE": 3,
    "NO_PLATE": 4,
}
# Safety ordering: SAFE is authorable into the synthetic lane; everything else
# routes to snapshot/restore. DESTROYS_DEVICE is last — a synthetic call there
# kills the game window.
SAFETY_RANK = {
    "SAFE": 0,
    "CALLS_UNKNOWN": 1,
    "WRITES_GLOBAL": 2,
    "TEARDOWN": 3,
    "DESTROYS_DEVICE": 4,
}


def split_row(line):
    line = line.strip()
    if line.startswith("|"):
        cells = [c.strip() for c in line.strip("|").split("|")]
    else:
        cells = [c.strip() for c in line.split("\t")]
    return [c for c in cells if c != ""]


def scrape_briefs():
    """rva -> {safety, verdict, brief}. Later runs win on conflict."""
    out = {}
    for md in sorted(RUNS.glob("*/gate_b*.md")):
        for line in md.read_text(encoding="utf-8", errors="replace").splitlines():
            if not ROW_RE.match(line):
                continue
            cells = split_row(line)
            rva = cells[0].lower()
            # Workers often qualify the safety cell in place — "WRITES_GLOBAL
            # (DAT_007dc578)", "TEARDOWN - caller is HardwareExitApplication".
            # An exact-equality match silently dropped 11 of 90 rows on the
            # first pass, so match the leading token instead.
            safety = next((k for c in cells for k in SAFETY_RANK
                           if c.upper().startswith(k)), "")
            verdict = next((c for c in cells if c in VERDICT_RANK), "")
            if not verdict:
                continue
            out[rva] = {"safety": safety, "verdict": verdict,
                        "brief": md.relative_to(ROOT).as_posix()}
    return out


def main(argv):
    out_path = pathlib.Path(argv[1]) if len(argv) > 1 else \
        ROOT / "re/orchestrator/gate_ranked.tsv"
    briefs = scrape_briefs()

    with GATE.open(newline="", encoding="utf-8") as f:
        gate = [r for r in csv.DictReader(f, delimiter="\t")
                if r["verdict"] == "GATE_PASS"]

    rows = []
    for g in gate:
        b = briefs.get(g["rva"].lower(), {})
        rows.append({
            "rva": g["rva"], "name": g["name"], "conf": g["conf"],
            "size": int(g["size"] or 9999), "best_caller": g["best_caller"],
            "harness_safety": b.get("safety", "UNBRIEFED"),
            "brief_verdict": b.get("verdict", "UNBRIEFED"),
            "brief": b.get("brief", ""),
        })

    # SAFE-then-size, with unbriefed rows sorted after everything judged.
    rows.sort(key=lambda r: (
        SAFETY_RANK.get(r["harness_safety"], 9),
        VERDICT_RANK.get(r["brief_verdict"], 9),
        r["size"],
    ))

    cols = ["rva", "name", "conf", "size", "best_caller",
            "harness_safety", "brief_verdict", "brief"]
    with out_path.open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=cols, delimiter="\t")
        w.writeheader()
        w.writerows(rows)

    tally, safety = {}, {}
    for r in rows:
        tally[r["brief_verdict"]] = tally.get(r["brief_verdict"], 0) + 1
        safety[r["harness_safety"]] = safety.get(r["harness_safety"], 0) + 1
    print("briefs scraped: %d rows over %d files"
          % (len(briefs), len(set(b['brief'] for b in briefs.values()))))
    print("gate rows ranked: %d" % len(rows))
    print("verdict: " + "  ".join("%s=%d" % kv for kv in sorted(tally.items())))
    print("safety : " + "  ".join("%s=%d" % kv for kv in sorted(safety.items())))
    print("\ntop of the SAFE-first ranking:")
    for r in rows[:12]:
        print("  %s  %-34s %4d B  %-14s %s"
              % (r["rva"], r["name"][:34], r["size"],
                 r["harness_safety"], r["brief_verdict"]))
    print("\nwrote %s" % out_path)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
