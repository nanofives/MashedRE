#!/usr/bin/env python3
"""Emit a read-fleet queue that screens MUTATOR_LANE rows for A/B viability.

The snapshot/restore A/B lane snapshots a write surface, runs the PORT, rolls
state back, then RE-EXECUTES THE ORIGINAL and byte-compares. That last step is
the trap: the original runs later in wall-clock time than the port did, so any
write whose value comes from a non-deterministic source — a timer, an RNG, a
frame counter, an allocator address — differs between the two captures no matter
how correct the port is. The lane REDs, and the RED says nothing.

That is not hypothetical. iter12 picked 0x00495110 as the pilot on shape alone:
11 bytes, void, zero params, exactly one write surface, and the strongest caller
gate in the pool (C4). It writes a timer read, so it can never pass. The
expensive part of this lane is the hand-authored driver plus the plate-cited
write-surface window list, and spending that on a row that can only RED is the
failure this screen exists to prevent.

So the question here is NOT "is this row safe" (already answered — they are all
mutators) but "is its write surface DETERMINISTIC given pre-call state, and can
the surface be enumerated from cited evidence".

Usage:
  py -3.12 scripts/orch_make_mutator_queue.py <out.json> [--per-unit N]
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

You are screening {n} functions that a previous pass classified as MUTATOR_LANE — they write state, so they cannot be tested by calling them with seeded arguments. They route instead to a snapshot/restore A/B lane. Your job is to decide which of them that lane can actually verify.

Read ONLY the plate paths in the table below. Do NOT glob, do NOT grep, do NOT open any other file.

{table}

HOW THE LANE WORKS, and why most rows fail it:
  snapshot the write surface -> run the PORT on live pre-call state -> capture
  -> roll the state back -> RE-EXECUTE THE ORIGINAL -> capture -> byte-compare.
The original therefore executes LATER IN TIME than the port. Three consequences, and every row must be judged against all three:

  1. NON-DETERMINISM KILLS IT. If any written value derives from a timer/QPC, an RNG, a frame or tick counter, an allocation address, or an uninitialised read, the two captures differ for reasons unrelated to the port. A real example: 0x00495110 stores the result of a timer read, so it can never pass, despite being otherwise the best-shaped row in the pool.
  2. THE SURFACE MUST BE ENUMERABLE. Every address the function writes must be nameable from the plate. "It writes through param_1" is NOT enumerable unless the plate gives the offsets. An un-enumerable surface means the rollback is incomplete and leaves engine state inconsistent afterwards.
  3. IT MUST FIRE MORE THAN ONCE. The lane compares over N natural calls. A function whose only caller is a shutdown/exit path fires once per session, which is not a sample. Judge this from the callers the plate names.

For EACH row report:
1. write_surface - every address or field the plate literally says it writes, with the plate line for each. If the plate does not enumerate them, say NOT_ENUMERABLE.
2. value_source - for each written value, where it comes from: a function ARGUMENT, a computation on pre-call state, or a NON-DETERMINISTIC source (name which: timer/RNG/counter/allocation/uninitialised). Quote the deciding plate line.
3. call_frequency - from the callers the plate names: PER_FRAME, PER_EVENT, ONCE_AT_INIT, ONCE_AT_SHUTDOWN, or UNKNOWN.
4. rollback_risk - anything that makes the state un-restorable: it frees or destroys a resource, closes a handle, releases a COM object, or hands a pointer to something outside the snapshot. Such a row cannot be rolled back at all, only observed.

Then output:

(A) TSV, one row per RVA, columns exactly:
rva, hooks_csv_name, write_surface, value_source, call_frequency, rollback_risk, verdict

verdict is exactly one of:
  AB_READY       - surface fully enumerable, every value deterministic given pre-call state, fires more than once, and rollback is safe.
  AB_NONDET      - a written value comes from a non-deterministic source. Name the source. This row can NEVER pass the lane; it is a permanent reject, not a deferral.
  AB_UNENUMERABLE- the plate does not pin down every written address.
  AB_ONESHOT     - only fires on an init or shutdown path, so N natural calls is 1.
  AB_IRREVERSIBLE- frees/destroys/releases something, so the rollback cannot restore it.

(B) The AB_READY rows RANKED, cheapest write surface first (fewest distinct addresses), each with the exact window list a driver would need: every address plus its byte width, each citing a plate line.

(C) The AB_NONDET rows, each naming the specific non-deterministic source. This list is valuable — it is what stops a future run from re-picking these.

(D) Tally per verdict.

Be concise. The tables are the deliverable; skip narrative preamble."""


def main(argv):
    if len(argv) < 2:
        print(__doc__)
        return 2
    out = pathlib.Path(argv[1])
    per = int(argv[argv.index("--per-unit") + 1]) if "--per-unit" in argv else 6

    with RANKED.open(newline="", encoding="utf-8") as f:
        rows = [r for r in csv.DictReader(f, delimiter="\t")
                if r["brief_verdict"] == "MUTATOR_LANE"]
    # WRITES_GLOBAL first: teardown rows are very likely AB_ONESHOT /
    # AB_IRREVERSIBLE anyway, so put the plausible ones in the early units.
    order = {"WRITES_GLOBAL": 0, "CALLS_UNKNOWN": 1, "TEARDOWN": 2,
             "DESTROYS_DEVICE": 3}
    rows.sort(key=lambda r: (order.get(r["harness_safety"], 9), int(r["size"])))

    plates, hooks = index_plates(), index_hooks()
    units = []
    for i in range(0, len(rows), per):
        chunk = rows[i:i + per]
        lines = ["| # | RVA | plate path (read this file) | hooks.csv name |"
                 " size | safety | best caller |",
                 "|---|---|---|---|---|---|---|"]
        for j, r in enumerate(chunk, 1):
            key = r["rva"][2:].lower()
            plate = plates.get(key, [None])[0] or "NO_PLATE_FOUND"
            lines.append("| %d | %s | %s | %s | %s | %s | %s |"
                         % (j, r["rva"], plate,
                            hooks.get(key, {}).get("name", "?"),
                            r["size"], r["harness_safety"],
                            r["best_caller"]))
        units.append({"id": "mut_m%d" % (i // per + 1), "model": "sonnet",
                      "prompt": PROMPT.format(n=len(chunk),
                                              table="\n".join(lines))})

    out.write_text(json.dumps({
        "description": ("Mutator A/B viability screen - %d MUTATOR_LANE rows in "
                        "%d units. Deliverable is a per-row verdict on whether "
                        "the snapshot/restore lane can verify it at all, plus a "
                        "cited write-surface window list for the ones it can."
                        % (len(rows), len(units))),
        "defaults": {"repo": "Mashed", "model": "sonnet", "timeoutSec": 900},
        "units": units,
    }, indent=2) + "\n", encoding="utf-8")

    print("rows: %d  units: %d" % (len(rows), len(units)))
    print("wrote %s" % out)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
