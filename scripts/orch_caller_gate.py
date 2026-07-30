#!/usr/bin/env python3
"""Resolve the C2+ caller-gate for the orchestrator's candidate buckets.

Input:  re/orchestrator/caller_gate_144.json  (written by a ghidra_eval sweep;
        per-RVA: is_entry, size, callers[] = containing-function entry points,
        orphans[] = call sites NOT inside any defined Function).
Output: re/orchestrator/caller_gate_144.tsv + a printed tally.

Why orphans matter: callback-only code often never becomes a Ghidra Function
object, so function_callers reports "no callers" for a function that is in fact
called. The sweep therefore used getReferencesTo and recorded such sites
separately. An orphan-only RVA is NOT ungated - it needs its containing block
plated before the gate can be judged (this is exactly how 0x004f8660/0x004f8690
were unblocked in orch-iter4).
"""
import csv
import json
import pathlib
import sys
from collections import Counter

ROOT = pathlib.Path(__file__).resolve().parents[1]
SWEEP = ROOT / "re/orchestrator/caller_gate_144.json"
HOOKS = ROOT / "hooks.csv"
OUT = ROOT / "re/orchestrator/caller_gate_144.tsv"

RANK = {"C0": 0, "C1": 1, "C2": 2, "C3": 3, "C4": 4}


def load_conf():
    lvl = {}
    with HOOKS.open(newline="", encoding="utf-8", errors="replace") as f:
        for r in csv.DictReader(f):
            rva = (r.get("rva") or "").strip().lower()
            if not rva or rva.startswith("#"):
                continue
            try:
                va = int(rva, 16)
            except ValueError:
                continue
            if va < 0x400000:
                va += 0x400000
            lvl["%08x" % va] = ((r.get("confidence") or "").strip(),
                                (r.get("name") or "").strip())
    return lvl


def main():
    sweep = json.loads(SWEEP.read_text(encoding="utf-8"))
    conf = load_conf()
    rows, tally = [], Counter()

    for rva, rec in sweep.items():
        key = rva[2:].lower()
        my_conf, my_name = conf.get(key, ("ABSENT", "?"))

        best, best_rank, seen = "", -1, []
        for c in rec["callers"]:
            cc, cn = conf.get(c.lower(), ("ABSENT", "?"))
            seen.append("%s:%s" % (c, cc))
            if RANK.get(cc, -1) > best_rank:
                best_rank, best = RANK.get(cc, -1), "0x%s(%s)" % (c, cc)

        if not rec["is_entry"]:
            verdict = "NOT_A_FUNCTION"
        elif best_rank >= 2:
            verdict = "GATE_PASS"
        elif rec["callers"]:
            verdict = "GATE_FAIL_CALLERS_C1"
        elif rec["orphans"]:
            verdict = "ORPHAN_BLOCK_CALLERS"
        else:
            verdict = "NO_CALLERS_FOUND"
        tally[verdict] += 1

        rows.append({
            "rva": rva, "name": my_name, "conf": my_conf,
            "size": rec["size"] if rec["size"] is not None else "",
            "n_callers": len(rec["callers"]),
            "n_orphan_sites": len(rec["orphans"]),
            "best_caller": best or "-",
            "all_callers": ";".join(seen) or "-",
            "orphan_sites": ";".join(rec["orphans"][:4]) or "-",
            "verdict": verdict,
        })

    rows.sort(key=lambda r: (r["verdict"] != "GATE_PASS",
                             r["size"] if isinstance(r["size"], int) else 9999))
    cols = list(rows[0].keys())
    with OUT.open("w", newline="", encoding="utf-8") as f:
        w = csv.DictWriter(f, fieldnames=cols, delimiter="\t")
        w.writeheader()
        w.writerows(rows)

    print("%-24s %s" % ("VERDICT", "COUNT"))
    for k, v in tally.most_common():
        print("%-24s %d" % (k, v))
    print("\nGATE_PASS rows (cheapest first):")
    for r in rows:
        if r["verdict"] == "GATE_PASS":
            print("  %s  %-34s %3s B  caller %s"
                  % (r["rva"], r["name"][:34], r["size"], r["best_caller"]))
    print("\nwrote %s" % OUT.relative_to(ROOT))
    return 0


if __name__ == "__main__":
    sys.exit(main())
