#!/usr/bin/env python3
"""Emit a read-fleet brief queue for orchestrator candidate buckets.

Two lessons from iteration 9 are baked in here:

1. **Pre-resolve everything the worker would otherwise search for.** In iter9,
   5 of 6 workers hit the 600s timeout doing `glob re/analysis/** ` per RVA plus
   a hooks.csv grep per RVA. Both are cheap locally and expensive for a worker,
   so this script resolves the plate path AND the hooks.csv row up front and
   embeds them in the prompt. The worker then only reads the named plates and
   ARG_TYPES.md.

2. **READY must include harness safety.** iter9's READY criterion was
   plate + arg_type + C2 caller, which passed two MUTATORS into a lane built for
   read-only getters: 0x00403640 clobbers the live BackBuffRaster handle
   DAT_00636b78, and 0x00496ce0 is HardwareExitApplication teardown. The prompt
   below adds a harness_safety column and makes SAFE a precondition of READY.

Usage:
  py -3.12 scripts/orch_make_brief_queue.py <out.json> <bucket_id> [<bucket_id>...]
  py -3.12 scripts/orch_make_brief_queue.py <out.json> --all
"""
import csv
import json
import pathlib
import re
import sys
from collections import defaultdict

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
from orch_rank_gate import library_band          # noqa: E402

ROOT = pathlib.Path(__file__).resolve().parents[1]
BUCKETS = ROOT / "re/orchestrator/candidate_buckets.json"
HOOKS = ROOT / "hooks.csv"
ANALYSIS = ROOT / "re/analysis"

PLATE_RE = re.compile(r"^(?:0x)?([0-9a-fA-F]{8})\.md$")


CONF_RANK = {"C4": 4, "C3": 3, "C2": 2, "C1": 1, "C0": 0}
CONF_RE = re.compile(r"^confidence:\s*(C[0-4])\s*$", re.M)


def plate_rank(path):
    """Rank candidate plates for one RVA: highest frontmatter confidence wins,
    then the larger file.

    Shortest-path was WRONG: an RVA often has both an original C1 plate and a
    later C2 plate in a promote_c2_* directory, and the C2 plate is the one
    that carries the disassembly and the caller. 0x00403640 has exactly this
    shape - promote_c2_perm_piz_callees/00403640.md names boot_app_init_d3's
    plate as its own source_plate.
    """
    try:
        head = (ROOT / path).read_text(encoding="utf-8", errors="replace")[:800]
    except OSError:
        return (-1, 0)
    m = CONF_RE.search(head)
    conf = CONF_RANK.get(m.group(1), -1) if m else -1
    try:
        size = (ROOT / path).stat().st_size
    except OSError:
        size = 0
    return (conf, size)


def index_plates():
    idx = defaultdict(list)
    for p in ANALYSIS.rglob("*.md"):
        m = PLATE_RE.match(p.name)
        if m:
            idx[m.group(1).lower()].append(
                p.relative_to(ROOT).as_posix())
    for v in idx.values():
        v.sort(key=plate_rank, reverse=True)
    return idx


def index_hooks():
    rows = {}
    with HOOKS.open(newline="", encoding="utf-8", errors="replace") as f:
        for r in csv.DictReader(f):
            rva = (r.get("rva") or "").strip().lower()
            if not rva or rva.startswith("#"):
                continue
            rows[rva] = r
    return rows


PROMPT_HEAD = """READ-ONLY. NO-GUESSING: report only what a file literally shows, cite the file for every claim, never infer intent, and do not write any file. If a fact is not literally in a file you were given, the answer is UNKNOWN.

You are pre-screening {n} C2 {sub}-subsystem functions for C2->C3 Frida promotion.

Everything you need has been resolved for you - do NOT glob, do NOT search re/analysis, do NOT grep hooks.csv. Read ONLY the plate paths listed below plus re/frida/ARG_TYPES.md. That is {n}+1 file reads total; anything more means you are doing work that was already done.

{table}

For EACH row, read its plate (frontmatter + mechanical description + any body/disassembly section; skip long appendices) and report:

1. param_shape - param count, and for each param whether the plate literally describes it being DEREFERENCED. A param a plate calls "int" while also describing a dereference is a known recurring defect in this project: flag it.
2. ret_type - as the plate literally states. If the plate says void, also report whether the plate describes any caller testing the result; if it does not say, write RETURN_UNVERIFIED. (A void-declared function whose callers branch on EAX was a real multi-week defect here.)
3. arg_type_match - the ONE existing arg_type from re/frida/ARG_TYPES.md whose shape matches, cited by line, or NEEDS_NEW_HANDLER. Never invent a name absent from that file.
4. caller_rvas / caller_confidences - only callers the plate literally names, with the confidence given in the table above; else CALLERS_NEEDS_GHIDRA.
5. **harness_safety** - THE CRITICAL COLUMN. The verification harness calls the function synthetically, with seeded arguments, inside a LIVE running game. Judge from the plate body only, and answer with exactly one of:
   - SAFE - reads only; writes nothing outside its own return value.
   - WRITES_GLOBAL - stores to any DAT_/global address. Name the global. (A synthetic call overwrites live engine state.)
   - TEARDOWN - frees/destroys/releases resources, or its named caller is an exit/shutdown path.
   - DESTROYS_DEVICE - touches window, D3D device, or display (e.g. DestroyWindow, Release on a device).
   - CALLS_UNKNOWN - its only effect is calling a callee the plate does not characterise, so the blast radius cannot be judged.
   Quote the plate line that decides it. When torn between SAFE and anything else, do NOT pick SAFE.

Then output:

(A) TSV, one row per RVA, columns exactly:
rva, plate_found, hooks_csv_name, confidence, param_shape, deref_ptr, ret_type, arg_type_match, arg_type_evidence, caller_rvas, caller_confidences, harness_safety, safety_evidence, verdict

verdict is exactly one of:
  READY            - plate found AND an existing arg_type matches AND >=1 caller is C2+ AND harness_safety is SAFE.
  MUTATOR_LANE     - would otherwise be READY, but harness_safety is WRITES_GLOBAL / TEARDOWN / DESTROYS_DEVICE. These are NOT rejects - they route to the snapshot/restore A/B mutator lane instead of the synthetic getter lane.
  NEEDS_NEW_HANDLER
  NEEDS_GHIDRA     - shape, callers, or safety not determinable from the given files.
  NO_PLATE

(B) The READY rows, cheapest-first by plate body size, one clause each.
(C) The MUTATOR_LANE rows, each with the specific global/resource a synthetic call would damage.
(D) Tally per verdict, plus any RVA where the plate frontmatter and the hooks.csv confidence in the table above DISAGREE - report those as a tracker conflict, do not pick a winner.

Be concise. The tables are the deliverable; skip narrative preamble."""


GATE_NOTE = """
The caller column above is AUTHORITATIVE - it was resolved locally in Ghidra
(getReferencesTo, so call sites inside un-wrapped blocks are included). Do NOT
answer CALLERS_NEEDS_GHIDRA for any row in this batch: every row listed here
already has a confirmed C2-or-better caller. Copy the caller and its confidence
straight from the table into your caller_rvas / caller_confidences columns.

That means the ONLY things standing between a row and READY are its shape, its
arg_type match, and harness_safety. Judge those carefully - especially safety.
"""


def cited_plate(row):
    """The plate hooks.csv itself points at, if it is an analysis note on disk.

    iter23 near-miss, and the reason this exists: 0x0052df40 has TWO plates.
    bucket_util_0052daf0_00582680/0x0052df40.md describes a tidy surface memcpy
    and carries no library tag; re/analysis/bucket_00516bb0/_BUCKET_HALT.md
    declares the whole 0x00516bb0..0x0052df40 bucket statically-linked
    third-party and says "do NOT re-issue this bucket". plate_rank() picked the
    first (it scores frontmatter confidence, then size, and a HALT report has
    neither) so the screen, the handoff, and the next run's directive all
    inherited a clean-looking row that was never a port target.

    hooks.csv's `file` column is the row's own citation, so it wins. For rows
    already implemented `file` is a .cpp path, hence the re/analysis/ guard.
    """
    f = (row.get("file") or "").strip().replace("\\", "/")
    if not f.startswith("re/analysis/"):
        return None
    return f if (ROOT / f).exists() else None


def screen_bucket(rvas, hooks):
    """Split RVAs into (keep, dropped) — dropped are never port targets.

    Library-band membership is preflight's cheapest disqualifier, but preflight
    runs at AUTHORING time, long after a worker has already been paid to screen
    the row. Applying it here means a library RVA never reaches a worker at all.
    """
    keep, dropped = [], []
    for rva in rvas:
        row = hooks.get(rva[2:].lower(), {})
        band = library_band(rva)
        cited = cited_plate(row)
        conf = (row.get("confidence") or "").strip().upper()
        if band:
            dropped.append((rva, "library band '%s'" % band))
        elif cited and cited.endswith("_BUCKET_HALT.md"):
            dropped.append((rva, "hooks.csv cites a bucket HALT: %s" % cited))
        elif conf in ("C3", "C4"):
            # This lane exists to move C2 -> C3. A row that is already there is
            # finished work. iter22 paid to screen 0x00407550 at "C2" when it had
            # been promoted to C3 that same day; a smoke test of this screen then
            # caught 0x004b6b00 and 0x004cbb50 queued at C3 too. The buckets are
            # a static list, so they go stale the moment anything is promoted —
            # read the live confidence instead of trusting the bucket.
            dropped.append((rva, "already %s in hooks.csv - nothing to promote"
                            % conf))
        else:
            keep.append(rva)
    return keep, dropped


def build_prompt(bucket, plates, hooks, gate=None):
    has_gate = gate is not None
    hdr = ("| # | RVA | plate path (read this file) | hooks.csv name | conf |"
           " best caller (CONFIRMED C2+) |" if has_gate else
           "| # | RVA | plate path (read this file) | hooks.csv name | conf |")
    sep = "|---|---|---|---|---|---|" if has_gate else "|---|---|---|---|---|"
    lines = [hdr, sep]
    kept, dropped = screen_bucket(bucket["rvas"], hooks)
    for rva, why in dropped:
        print("  DROPPED %s from %s: %s" % (rva, bucket.get("id", "?"), why),
              file=sys.stderr)
    if not kept:
        raise SystemExit(
            "bucket %s has no screenable RVA left after the library/HALT screen"
            % bucket.get("id", "?"))
    for i, rva in enumerate(kept, 1):
        key = rva[2:].lower()
        row = hooks.get(key, {})
        # hooks.csv's own citation wins over the size/confidence ranking.
        plate = (cited_plate(row) or plates.get(key, [None])[0]
                 or "NO_PLATE_FOUND")
        cells = [str(i), rva, plate, row.get("name", "?"),
                 row.get("confidence", "?")]
        if has_gate:
            cells.append(gate.get(rva, {}).get("best_caller", "-"))
        lines.append("| %s |" % " | ".join(cells))
    body = PROMPT_HEAD.format(n=len(kept), sub=bucket["subsystem"],
                              table="\n".join(lines))
    return body + (GATE_NOTE if has_gate else "")


def load_gate():
    p = ROOT / "re/orchestrator/caller_gate_144.tsv"
    if not p.exists():
        return None
    with p.open(newline="", encoding="utf-8") as f:
        return {r["rva"]: r for r in csv.DictReader(f, delimiter="\t")
                if r["verdict"] == "GATE_PASS"}


def main(argv):
    if len(argv) < 3:
        print(__doc__)
        return 2
    out = pathlib.Path(argv[1])
    want = argv[2:]

    data = json.loads(BUCKETS.read_text(encoding="utf-8"))
    plates, hooks = index_plates(), index_hooks()
    gate = load_gate()

    if want and want[0] == "--gate":
        # Gate-driven mode: cut fresh 6-RVA units from GATE_PASS rows only,
        # skipping any RVA named after --skip. Ranked SAFE-agnostic by size,
        # but note iter11: cheapest-first surfaces teardown families, so the
        # safety column in the brief is what actually decides routing.
        if not gate:
            print("no caller_gate_144.tsv - run the Ghidra sweep first")
            return 1
        # --start N numbers the emitted units from gate_bN, so a later cut of
        # the same TSV does not reuse ids the previous run already spent (iter11
        # burned gate_b1..b8; iter12 continues at gate_b9).
        # --skip must come LAST: everything after it is read as RVAs.
        start = int(want[want.index("--start") + 1]) if "--start" in want else 1
        skip = set(want[want.index("--skip") + 1:]) if "--skip" in want else set()
        rows = [r for r in gate.values() if r["rva"] not in skip]
        rows.sort(key=lambda r: int(r["size"] or 9999))
        sel = []
        for i in range(0, len(rows), 6):
            chunk = rows[i:i + 6]
            if len(chunk) < 2:
                break
            sel.append({"id": "gate_b%d" % (i // 6 + start),
                        "subsystem": "mixed (gate-passing)",
                        "rvas": [r["rva"] for r in chunk]})
    else:
        sel = data["buckets"] if want == ["--all"] else [
            b for b in data["buckets"] if b["id"] in want]
        missing = set(want) - {b["id"] for b in sel} - {"--all"}
        if missing:
            print("unknown bucket ids: %s" % ", ".join(sorted(missing)))
            return 1

    units, no_plate = [], 0
    for b in sel:
        no_plate += sum(1 for r in b["rvas"]
                        if r[2:].lower() not in plates)
        units.append({"id": b["id"], "model": "sonnet",
                      "prompt": build_prompt(b, plates, hooks, gate)})

    out.write_text(json.dumps({
        "description": ("Orchestrator brief queue - %d buckets of <=%d RVAs. "
                        "Plate paths and hooks.csv rows PRE-RESOLVED locally "
                        "(iter9: globbing cost 5 of 6 workers their 600s "
                        "budget). READY now requires harness_safety==SAFE."
                        % (len(units), max(len(b["rvas"]) for b in sel))),
        "defaults": {"repo": "Mashed", "model": "sonnet", "timeoutSec": 900},
        "units": units,
    }, indent=2) + "\n", encoding="utf-8")

    print("plate index: %d RVAs" % len(plates))
    for b in sel:
        found = sum(1 for r in b["rvas"] if r[2:].lower() in plates)
        print("  %-22s %2d rvas, %2d plates resolved, prompt %d chars"
              % (b["id"], len(b["rvas"]), found,
                 len(build_prompt(b, plates, hooks))))
    if no_plate:
        print("WARNING: %d RVAs have no plate -> worker will report NO_PLATE"
              % no_plate)
    print("wrote %s" % out)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
