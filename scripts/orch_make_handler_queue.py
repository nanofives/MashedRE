#!/usr/bin/env python3
"""Emit a read-fleet queue that DESIGNS the missing arg_type handlers.

Different question from orch_make_brief_queue.py. That script asks "is this row
safe and does an existing handler fit?" — and 22 of 102 gate-passing rows came
back NEEDS_NEW_HANDLER, i.e. safe enough but with no handler that matches their
shape. Those rows are stuck until somebody works out what the handler would have
to do.

iter12 showed that is cheap: str_arg_int_get took one cycle to write and
promoted its row outright (0x004d8770). NEEDS_NEW_HANDLER is now the second
largest bucket, so the bottleneck is the DESIGN work, which is pure reading —
exactly what belongs off-quota.

The deliverable per row is a handler SPEC precise enough to implement without
re-reading the plate: what to seed, what to observe, and the per-seed expected
values that make it non-degenerate. Rows that need the SAME handler must be
grouped, because one handler unlocking six rows is the whole point.

Usage:
  py -3.12 scripts/orch_make_handler_queue.py <out.json> [--per-unit N]
"""
import csv
import json
import pathlib
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from orch_make_brief_queue import index_plates, index_hooks  # noqa: E402

ROOT = pathlib.Path(__file__).resolve().parents[1]
RANKED = ROOT / "re/orchestrator/gate_ranked.tsv"

PROMPT = """READ-ONLY. NO-GUESSING: report only what a file literally shows, cite the file for every claim, never infer intent, and do not write any file. If a fact is not literally in a file you were given, the answer is UNKNOWN.

You are designing Frida A/B **seed handlers** for {n} functions that a previous screening pass marked NEEDS_NEW_HANDLER: they are safe enough to call, but no existing arg_type in re/frida/ARG_TYPES.md matches their argument shape.

Read ONLY the plate paths in the table below, plus re/frida/ARG_TYPES.md. Do NOT glob, do NOT grep hooks.csv, do NOT open diff_template.js (it is 232 KB and you do not need it — ARG_TYPES.md is its generated index).

{table}

BACKGROUND — how a handler works and how it fails:
The harness calls the function synthetically with seeded arguments inside a LIVE running game, once against the original and once against our port, then compares the returns. A handler is the code that turns one test vector into actual arguments.

Two failure modes matter more than anything else, and your spec must rule out BOTH:
  (a) TERMINATION / BOUNDS — if the callee walks memory until a terminator (a string, a list, a sentinel-ended array), a handler that seeds raw bytes without writing that terminator makes the callee run off the end of the allocation and crash. A real example: 0x004d8770 needed a NUL-terminated string seed; the generic pointer handler had no terminator.
  (b) DEGENERACY — if every seed produces the SAME return, the run is bit-identical and proves nothing. A real example: 0x005aeed0 takes a HANDLE, every synthetic int is an invalid handle, so every seed returns the same value. Such a row is NOT fixable with a new handler and must be reported as SCENARIO_ONLY.

For EACH row report:
1. arg_shape — each parameter, and whether the plate literally describes it being DEREFERENCED and to what depth. A parameter the plate calls "int" while also describing a dereference is a KNOWN recurring defect in this project — flag it explicitly.
2. seed_recipe — exactly what the handler must construct: buffer size, field-by-field contents, and any terminator/sentinel the callee needs. Cite the plate line that proves each requirement.
3. observable — what the comparison should read: the return value, or a named written field, and how to normalise it (e.g. pointer returns must become an offset from the seed buffer, because the two sides get different addresses).
4. non_degeneracy — 3+ concrete seeds AND the distinct value each should produce. If you cannot produce distinct expected values from the plate alone, say NEEDS_GHIDRA. If distinct values are impossible in principle because the input must be a live OS/engine object (handle, device, socket, file), say SCENARIO_ONLY — that is a real and useful answer.
5. handler_name — a proposed snake_case arg_type name, and whether an EXISTING name in ARG_TYPES.md is close enough to extend instead of adding a new one (cite the line).

Then output:

(A) TSV, one row per RVA, columns exactly:
rva, hooks_csv_name, arg_shape, deref_defect, seed_recipe, observable, non_degeneracy, handler_name, verdict

verdict is exactly one of:
  SPEC_COMPLETE  - seed recipe, observable, and 3+ distinct expected values all determined from the plate.
  SCENARIO_ONLY  - cannot be made non-degenerate synthetically; needs a live object from a scenario attach.
  NEEDS_GHIDRA   - shape or expected values not determinable from the plate.

(B) GROUPING — the most important section. Cluster the rows by the handler they need. For each cluster: the proposed handler name, which RVAs it would unlock, and the ONE seed recipe that serves all of them. A handler that unlocks 4 rows is worth far more than 4 bespoke handlers, so look hard for shared shapes.

(C) Tally per verdict.

Be concise. The tables are the deliverable; skip narrative preamble."""


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    out = pathlib.Path(argv[1])
    per = int(argv[argv.index("--per-unit") + 1]) if "--per-unit" in argv else 6

    with RANKED.open(newline="", encoding="utf-8") as f:
        rows = [r for r in csv.DictReader(f, delimiter="\t")
                if r["brief_verdict"] == "NEEDS_NEW_HANDLER"]
    # SAFE first: those are the rows that can actually be authored once the
    # handler exists. A CALLS_UNKNOWN row still needs a safety answer after this.
    rows.sort(key=lambda r: (r["harness_safety"] != "SAFE", int(r["size"])))

    plates, hooks = index_plates(), index_hooks()
    units = []
    for i in range(0, len(rows), per):
        chunk = rows[i:i + per]
        lines = ["| # | RVA | plate path (read this file) | hooks.csv name |"
                 " size | safety |",
                 "|---|---|---|---|---|---|"]
        for j, r in enumerate(chunk, 1):
            key = r["rva"][2:].lower()
            plate = plates.get(key, [None])[0] or "NO_PLATE_FOUND"
            lines.append("| %d | %s | %s | %s | %s | %s |"
                         % (j, r["rva"], plate,
                            hooks.get(key, {}).get("name", "?"),
                            r["size"], r["harness_safety"]))
        units.append({"id": "handler_h%d" % (i // per + 1), "model": "sonnet",
                      "prompt": PROMPT.format(n=len(chunk),
                                              table="\n".join(lines))})

    out.write_text(json.dumps({
        "description": ("Handler-design queue - %d NEEDS_NEW_HANDLER rows in %d "
                        "units. Deliverable is a seed-recipe SPEC per row plus a "
                        "GROUPING of rows that share one handler."
                        % (len(rows), len(units))),
        "defaults": {"repo": "Mashed", "model": "sonnet", "timeoutSec": 900},
        "units": units,
    }, indent=2) + "\n", encoding="utf-8")

    print("rows: %d  units: %d" % (len(rows), len(units)))
    for u in units:
        print("  %-12s prompt %d chars" % (u["id"], len(u["prompt"])))
    print("wrote %s" % out)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
