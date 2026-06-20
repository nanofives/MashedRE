#!/usr/bin/env py -3.12
"""promote_subdivide.py — route the 'unclassified' first-party C2 bucket.

WHY THIS EXISTS
---------------
The existing promotion tooling (promote_frontier.py -> promote_classify.py ->
promote_recognize.py) only addresses the LEAF + recognized-shape slice of C2.
The promote-c3-batch C2_GATE_AUDIT taxonomy flags the dominant residual as
"unclassified | ~1300 | no recognizable blocker in notes; sample to subdivide"
(recon 2026-06-19, re/analysis/C2_PROMOTABILITY_RECON_2026-06-19.md). Those are
NON-LEAF functions (they have callees) whose blocker is structural, not in the
notes -- so a notes/prose classifier can never tier them.

This tool tiers them MECHANICALLY from the binary + hooks.csv, with NO Ghidra and
NO running game (collision-free with a concurrent promotion session). It reuses
promote_frontier.analyze() for the call graph / sizes and the
promote_classify STATE heuristic, and adds the one signal the frontier throws
away: CALLEE CONFIDENCE. That yields the promotion DEPENDENCY ORDER -- a function
blocked only by callees that are themselves promotable is "N hops from clean".

LANES (one primary lane per function, most-blocking first):
  oversize            size > OVERSIZE bytes            -> too big for 1-session reimpl
  indirect-dispatch   has calls but 0 resolved targets -> vtable/jump-table/import; can't gate
  callee-blocked      >=1 direct callee is < C2        -> callee-first (lists the blockers)
  state-runnable-now  callee-clean + reads global/arg  -> booted-game run_diff (now UNBLOCKED)
  synthetic-candidate callee-clean + display-indep      -> synthetic run_diff works today

OUTPUT (re/analysis/recon_c2/, subdivide_* prefix -- never touches the shared
promote_frontier.tsv / promote_classified.tsv):
  subdivide_all.tsv            every clean first-party C2, one row, all signals
  subdivide_<lane>.tsv         per-lane worklists
  subdivide_order.tsv          callee-blocked rows whose blockers are all currently
                               promotable leaves (the "promote these to unblock N parents")

Run:  py -3.12 scripts/promote_subdivide.py
"""
import csv
import os
import re
import sys
from collections import Counter, defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "scripts"))
import promote_frontier as PF
import promote_classify as PC

OUTDIR = os.path.join(ROOT, "re", "analysis", "recon_c2")

OVERSIZE = 2048          # C2_GATE_AUDIT 'oversize' threshold (~2KB; one-fn-per-session)
PROMOTABLE = {"C2", "C3", "C4"}
# notes markers that mean 'CRT/library wearing a non-library subsystem label'
LIBMARK = re.compile(r"fiddb|vs2003|library match|__crt|\bseh\b|heap_init|mtinit|"
                     r"msvcrt|crt-band|lever3", re.I)


def conf_of(hooks, va):
    r = hooks.get(va)
    return (r.get("confidence") or "").strip() if r else "?unmapped"


def state_signals(insns):
    """Reuse promote_classify's STATE test: reads .data global or derefs an arg."""
    ms = [(i.mnemonic, i.op_str) for i in insns]
    reads_global = any(("0x6" in o or "0x7" in o or "0x8" in o or "0x9" in o)
                       and "[" in o for _, o in ms)
    derefs_arg = any("[eax" in o or "[ecx" in o or "[edx" in o for _, o in ms)
    return reads_global, derefs_arg


def arg_hint(insns):
    tags = []
    ops = " ".join(i.op_str for i in insns)
    if "[esp" in ops or "[ebp +" in ops or "[ebp+" in ops:
        tags.append("stack")
    if "[ecx" in ops:
        tags.append("ecx-this")           # __thiscall-shaped
    if "[eax" in ops:
        tags.append("eax-deref")          # implicit-this / arg deref
    if re.search(r"\bst\(?[0-7]\)?\b", ops):
        tags.append("fpu")
    return ",".join(tags) or "-"


def main():
    a = PF.analyze()
    hooks = a["hooks"]
    size_of, has_call = a["size_of"], a["has_call"]
    direct_callees, callers = a["direct_callees"], a["callers"]
    code, tva = a["code"], a["text_va"]

    # universe: clean first-party C2 in .text (PF library bands already excluded
    # via is_first_party; additionally drop notes-flagged CRT/library remnants).
    universe = []
    for va, row in hooks.items():
        if (row.get("confidence") or "").strip() != "C2":
            continue
        if not PF.is_first_party(row):
            continue
        if va not in size_of:
            continue
        if LIBMARK.search(row.get("notes") or ""):
            continue
        universe.append(va)
    universe.sort()

    # a callee/parent is "currently a promotable leaf" if it is a first-party
    # C2 leaf within the frontier size window (these are the cheapest to land,
    # so a parent blocked only by them is one cheap hop from clean).
    def is_cheap_leaf(va):
        r = hooks.get(va)
        if not r or (r.get("confidence") or "").strip() != "C2":
            return False
        if not PF.is_first_party(r):
            return False
        return (not has_call.get(va, True)) and PF.MIN_BODY <= size_of.get(va, 9999) <= PF.MAX_BODY

    rows = []
    for va in universe:
        sz = size_of.get(va, 0)
        callees = direct_callees.get(va, set())
        blocking = sorted(c for c in callees if conf_of(hooks, c) not in PROMOTABLE)
        ncall_c2 = sum(1 for c in callers.get(va, ())
                       if conf_of(hooks, c) in PROMOTABLE)
        insns = PC.decode_body(code, tva, va, sz)
        reads_global, derefs_arg = state_signals(insns)
        hint = arg_hint(insns)

        # ---- lane (priority: most-blocking first) ----
        if sz > OVERSIZE:
            lane = "oversize"
        elif has_call.get(va, False) and not callees:
            lane = "indirect-dispatch"
        elif blocking:
            lane = "callee-blocked"
        elif reads_global or derefs_arg:
            lane = "state-runnable-now"
        else:
            lane = "synthetic-candidate"

        # is this callee-blocked row one cheap hop from clean?
        one_hop = (lane == "callee-blocked"
                   and all(is_cheap_leaf(c) for c in blocking))

        blk = ";".join(f"{c:08x}:{conf_of(hooks, c) or '-'}" for c in blocking)
        rows.append(dict(
            va=va, sub=(hooks[va].get("subsystem") or ""),
            name=(hooks[va].get("name") or ""), lane=lane, size=sz,
            n_callees=len(callees), n_blocking=len(blocking), blocking=blk,
            arg_hint=hint, n_callers_c2plus=ncall_c2, one_hop=one_hop,
            notes=(hooks[va].get("notes") or "").replace("\t", " ").replace("\n", " ")[:160],
        ))

    os.makedirs(OUTDIR, exist_ok=True)
    cols = ["va", "sub", "name", "lane", "size", "n_callees", "n_blocking",
            "blocking", "arg_hint", "n_callers_c2plus", "notes"]

    def write_tsv(path, subset):
        with open(path, "w", encoding="utf-8", newline="") as f:
            w = csv.writer(f, delimiter="\t")
            w.writerow(["rva"] + cols[1:])
            for r in subset:
                w.writerow([f"{r['va']:08x}"] + [r[c] for c in cols[1:]])

    write_tsv(os.path.join(OUTDIR, "subdivide_all.tsv"), rows)
    by_lane = defaultdict(list)
    for r in rows:
        by_lane[r["lane"]].append(r)
    for lane, subset in by_lane.items():
        # sort actionable lanes by smallest first (cheapest reimpl), blocked by fewest blockers
        subset.sort(key=lambda r: (r["n_blocking"], r["size"]))
        write_tsv(os.path.join(OUTDIR, f"subdivide_{lane}.tsv"), subset)

    one_hop_rows = [r for r in rows if r["one_hop"]]
    one_hop_rows.sort(key=lambda r: (r["n_blocking"], r["size"]))
    write_tsv(os.path.join(OUTDIR, "subdivide_order.tsv"), one_hop_rows)

    # blockers ranked by how many parents they unblock (promote these first)
    blocker_unblocks = Counter()
    for r in rows:
        if r["lane"] == "callee-blocked":
            for tok in r["blocking"].split(";"):
                if tok:
                    blocker_unblocks[tok.split(":")[0]] += 1

    # ---------- summary ----------
    print(f"clean first-party C2 universe: {len(rows)}")
    print(f"output dir: {OUTDIR}\n")
    print("LANE DISTRIBUTION:")
    for lane, subset in sorted(by_lane.items(), key=lambda kv: -len(kv[1])):
        print(f"  {len(subset):5d}  {lane}")
    print()
    act = [r for r in rows if r["lane"] in ("synthetic-candidate", "state-runnable-now")]
    print(f"DIRECTLY ACTIONABLE NOW (callee-clean): {len(act)}")
    print(f"  synthetic-candidate (run_diff today)        : "
          f"{sum(1 for r in rows if r['lane']=='synthetic-candidate')}")
    print(f"  state-runnable-now  (booted run_diff, open) : "
          f"{sum(1 for r in rows if r['lane']=='state-runnable-now')}")
    print(f"\nCALLEE-BLOCKED one-hop-from-clean (subdivide_order.tsv): {len(one_hop_rows)}")
    print("  -> promoting their blockers (cheap leaves) unblocks these parents.\n")
    print("TOP 15 BLOCKERS by #parents-unblocked (promote-first targets):")
    for rva, n in blocker_unblocks.most_common(15):
        c = conf_of(hooks, int(rva, 16))
        cheap = "leaf" if is_cheap_leaf(int(rva, 16)) else "non-leaf/oos"
        print(f"  {rva}  unblocks {n:3d} parents  (now {c or '-'}, {cheap})")

    # subsystem breakdown of the two actionable lanes
    print("\nactionable lanes by subsystem:")
    sub_ct = Counter(r["sub"] for r in act)
    for s, n in sub_ct.most_common():
        print(f"  {n:4d}  {s}")


if __name__ == "__main__":
    sys.exit(main())
