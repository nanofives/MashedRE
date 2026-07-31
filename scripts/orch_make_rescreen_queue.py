#!/usr/bin/env python3
"""Re-screen NEEDS_GHIDRA rows against rubric clauses added since they were judged.

A NEEDS_GHIDRA verdict means "shape, callers, or safety could not be determined
from the plate". Some of those rows were not really undeterminable — they were
blocked by a rule that has since changed. Two clauses landed after the gate
screen ran:

  * identified-callee (2026-07-30) — a NAMED library callee satisfies the
    callee-half even though it stays C1 by library-skip.
  * indirect-dispatch (2026-07-31) — where the ONLY callee is an indirect call
    off a runtime data table, the callee-half is vacuous, as for a leaf.

The second one in particular is likely to move rows: "calls something I cannot
identify" was a natural reason to answer NEEDS_GHIDRA, and for a `call dword
ptr [reg]` dispatcher that is now a PASS rather than a blocker. Re-asking is
cheap (off-quota, plate reads only) compared with opening Ghidra on 30 rows.

The output is deliberately narrow: this asks ONLY whether a clause resolves the
blocker, not for a fresh full screen.

Usage:
  py -3.12 scripts/orch_make_rescreen_queue.py <out.json> [--per-unit N]
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

{n} functions were previously screened NEEDS_GHIDRA — meaning their shape, callers, or safety could not be settled from the plate alone. Since then TWO rubric clauses were added. Your ONLY job is to decide whether either clause resolves the blocker, so the row can proceed without a Ghidra session.

Read ONLY the plate paths in the table below. Do NOT glob, do NOT grep, do NOT open any other file.

{table}

THE TWO CLAUSES:

  (A) IDENTIFIED-CALLEE. The "one callee at C2+" requirement is also satisfied by an *identified* callee — one with a recovered real name OR a documented library role, including named third-party APIs (e.g. RpClumpDestroy, _strlen, RwFreeList*). Such callees stay C1 by library-skip policy, but they are understood context, not unknown territory. It is NOT satisfied by an anonymous FUN_xxxxxxxx with no recovered name or role.

  (B) INDIRECT-DISPATCH. Where a function's ONLY callee is an indirect call whose target comes from a runtime data table or a function-pointer field — `call dword ptr [reg]`, a vtable slot, a callback field — the callee requirement is VACUOUS, as it is for a leaf with no callees at all. Rationale: the call graph contains no unknown FUNCTION, only a datum; which function runs is a property of the caller's data at run time. This does NOT apply if the function ALSO calls an anonymous callee directly — those still block.

For EACH row report:
1. blocker - what the plate says was actually undeterminable: CALLEE_UNKNOWN, SHAPE_UNKNOWN, SAFETY_UNKNOWN, CALLERS_UNKNOWN, or NO_PLATE. Quote the plate line that shows it.
2. callee_inventory - every callee the plate names, and for each: DIRECT_NAMED (give the name), DIRECT_ANONYMOUS (FUN_xxxxxxxx), or INDIRECT (through a vtable slot / table entry / fn-ptr field — say which). If the plate names no callees at all, say NONE.
3. clause_applies - A, B, BOTH, or NEITHER, with the plate line that decides it.
4. residual_blocker - if a clause resolves the CALLEE question, is anything else still undeterminable (shape, safety, callers)? A row can be unblocked on callees and still stuck on safety.

Then output:

(A) TSV, one row per RVA, columns exactly:
rva, hooks_csv_name, blocker, callee_inventory, clause_applies, residual_blocker, verdict

verdict is exactly one of:
  UNBLOCKED_BY_CLAUSE - a clause resolves the blocker AND nothing else is undeterminable. This row can be screened for safety/shape normally now.
  CLAUSE_PARTIAL      - a clause resolves the CALLEE question but a residual blocker remains. Name the residual.
  STILL_NEEDS_GHIDRA  - neither clause helps; the blocker is genuinely shape/safety/callers.

(B) The UNBLOCKED_BY_CLAUSE rows, each with the one-line reason and which clause did it.
(C) Tally per verdict, and separately: how many rows had at least one INDIRECT callee (clause B is the one expected to move rows).

Be concise. The tables are the deliverable; skip narrative preamble."""


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    out = pathlib.Path(argv[1])
    per = int(argv[argv.index("--per-unit") + 1]) if "--per-unit" in argv else 6

    with RANKED.open(newline="", encoding="utf-8") as f:
        rows = [r for r in csv.DictReader(f, delimiter="\t")
                if r["brief_verdict"] == "NEEDS_GHIDRA"]
    # SAFE first — a row unblocked on callees is only useful if it is also
    # callable, and SAFE rows are the ones that can go straight to authoring.
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
        units.append({"id": "rescreen_r%d" % (i // per + 1), "model": "sonnet",
                      "prompt": PROMPT.format(n=len(chunk),
                                              table="\n".join(lines))})

    out.write_text(json.dumps({
        "description": ("Clause re-screen - %d NEEDS_GHIDRA rows in %d units. "
                        "Asks ONLY whether the identified-callee or "
                        "indirect-dispatch clause resolves the blocker, so the "
                        "row can proceed without a Ghidra session."
                        % (len(rows), len(units))),
        "defaults": {"repo": "Mashed", "model": "sonnet", "timeoutSec": 900},
        "units": units,
    }, indent=2) + "\n", encoding="utf-8")

    print("rows: %d  units: %d" % (len(rows), len(units)))
    print("wrote %s" % out)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
