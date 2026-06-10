#!/usr/bin/env python3
"""
gen_c3_batch_aa.py — emit c3_batch_aa.txt: 6 Sonnet 4.6 C2->C3 promotion
session blocks over the signature-viable subset of the c3_batch_z clean pool.

Selection (on top of c3_filter_v4's c3_batch_z_passed.tsv = 100 candidates):
  keep only decompiled signatures with 0..4 args (harness-expressible);
  drop void(void) (no synthetic-diff target), 5+arg (harness max 4),
  and notes with no determinable `ret FUN_xxx(args)` line.
Reports the dropped buckets as a deferred section.
"""
from __future__ import annotations
import re
from pathlib import Path

ROOT = Path(r"C:\Users\maria\Desktop\Proyectos\Mashed")
PASSED = ROOT / "re" / "analysis" / "plans" / "c3_batch_z_passed.tsv"
OUT = ROOT / "c3_batch_aa.txt"
BATCH_ID = "aa"
N_SESSIONS = 6
ANCHOR = "BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E"

SIG_RE = re.compile(
    r'\b(void|undefined\d?|int|uint|float|char|short|bool|double)\s*(\*?)\s*'
    r'(FUN_[0-9a-fA-F]{8})\s*\(([^)]*)\)'
)

rows = [l.rstrip("\n").split("\t") for l in open(PASSED, encoding="utf-8")][1:]

viable = []   # (rva, name, sub, note, nargs, sig)
deferred = {"void(void)": [], "5+arg": [], "no-sig": []}
for r in rows:
    rva, name, sub, argt, note = r[0], r[1], r[2], r[3], r[4]
    p = ROOT / note
    body = p.read_text(encoding="utf-8", errors="replace") if p.exists() else ""
    m = SIG_RE.search(body)
    if not m:
        deferred["no-sig"].append((rva, sub, note))
        continue
    ret, star, fn, args = m.groups()
    args = args.strip()
    if args in ("", "void"):
        if ret == "void":
            deferred["void(void)"].append((rva, sub, note))
            continue
        n = 0
    else:
        n = len([a for a in args.split(",") if a.strip()])
    if n > 4:
        deferred["5+arg"].append((rva, sub, note))
        continue
    sig = f"{ret}{'*' if star else ''} {fn}({args})"
    viable.append((rva, name, sub, note, n, sig))

# sort by complexity (nargs) then rva; round-robin into sessions for balance
viable.sort(key=lambda x: (x[4], x[0]))
sessions = [[] for _ in range(N_SESSIONS)]
for i, c in enumerate(viable):
    sessions[i % N_SESSIONS].append(c)

HEADER = f"""\
================================================================================
c3_batch_{BATCH_ID}.txt  —  C2->C3 promotion fanout  (generated 2026-05-29)
================================================================================
Shape:   STANDARD — {N_SESSIONS} sessions x ~{len(viable)//N_SESSIONS}-{(len(viable)+N_SESSIONS-1)//N_SESSIONS} = {len(viable)} candidates
Model:   Sonnet 4.6 (claude-sonnet-4-6) per session
Source:  re/analysis/plans/c3_batch_z_passed.tsv (100 clean-pool candidates)
Subset:  signature-viable only (0..4 decompiled args). Dropped at gen time:
           void(void)  : {len(deferred['void(void)'])}  (no synthetic-diff target)
           5+arg       : {len(deferred['5+arg'])}  (harness max 4-arg)
           no-sig-found: {len(deferred['no-sig'])}  (signature undetermined in note)

!! YIELD CAVEAT (promote-c3-batch skill calibration): frontend C3 yield was
   ~0% on the c3_batch_h baseline; these notes are freshly C2-matured (inline-
   UNCERTAIN + shape-only passes of 2026-05-29) but UNPROVEN for C3. Treat this
   as a measured pilot: if sessions land <~30% promotions, the rest is
   C2-readiness work (arg_type classification), not C3 work. All 33 carry
   arg_type=unknown in their notes — each worker MUST determine the real
   signature, verify it against re/frida/diff_template.js, and STOP+queue
   (NOT invent an arg_type) if unsupported. Live-state renderers that read the
   D3D device should be refused, not forced.

HOW TO RUN: open {N_SESSIONS} Claude Code sessions; paste session block <i> into
session <i>; run each. Then from main run the `frida-sweep` skill to merge +
integration-diff. A batch not swept is NOT landed.
"""

SESSION_TMPL = """\

================================================================================
Session {n} — c3-batch-{bid}-s{n}  (C2->C3, {k}-function batch)
================================================================================
Model:       Sonnet 4.6 (claude-sonnet-4-6) — hard cap 8 RVAs (this session: {k})
Worktree:    .worktrees/c3-batch-{bid}-s{n}/  (branch c3/batch-{bid}-s{n})
Skills:      hook-author, diff-original, re-classify
Frida pool:  shared via scripts/frida_pool.sh (no pre-allocation)
Batch id:    c3-batch-{bid}

# Setup (run first)
  git worktree add .worktrees/c3-batch-{bid}-s{n} -b c3/batch-{bid}-s{n}
  cd .worktrees/c3-batch-{bid}-s{n}
  # ALL paths below are relative to this worktree; never edit main directly.

# Mission
Promote the {k} C2 functions below to C3 by authoring a small reimpl in
mashed_re_dev.asi, running run_diff_parallel.py for bit-identical synthetic
A/B verification, and applying re-classify transactionally. ONE commit at the
end. Do NOT merge to main (the user runs frida-sweep).

# Candidates (read each analysis note END-TO-END before authoring)
| RVA        | sig (decompiled)                          | analysis note |
|------------|-------------------------------------------|---------------|
{rows}

# Pre-flight
1. Anchor: `sha256sum original/MASHED.exe` == {anchor}
2. Baseline build on clean worktree: `cmd /c mashedmod\\build.bat` must succeed
   BEFORE adding any hook (catches merge breakage inherited from main).
3. For EACH candidate, before authoring, confirm in the note:
   - depth-1 callees all C2+ (filter already checked, but re-confirm);
   - the REAL signature + its arg_type EXISTS in re/frida/diff_template.js
     (grep it). The note says arg_type=unknown — YOU must determine it.
   - NO live-state reads (D3D device vtable `DAT_007d3ff8`, file I/O, COM).
     A renderer that draws via the live device is NOT synthetic-diffable —
     refuse it and queue a one-line note, do not force it.
   If the signature's arg_type is NOT already in diff_template.js: STOP for
   that candidate, append a PROMOTION_QUEUE note requesting the harness
   extension, and move on. Do NOT invent an arg_type (integration will RED).

# Per-function workflow (author all, commit ONCE)
1. File: mashedmod/src/mashed_re/{cluster}/BatchAA_s{n}.cpp (one cpp this session).
   - Comment-cite the RVA + the no-guessing source lines from the note.
   - extern "C" __declspec(dllexport) <ret> __cdecl <Name>(<args>) {{ ... }}
   - RH_ScopedInstall(<Name>, 0x<rva>);
2. build.bat: append the cpp once. hooks_registry.py: one entry per hook with
   the verified arg_type + >=10 test vectors (cover 0/MAX/sign-bit/edge).
3. Build: `cmd /c mashedmod\\build.bat > log\\build_{bid}_s{n}.txt 2>&1`. Halt on fail.

# Verify (ONCE, after all hooks authored)
  scripts/frida_pool.sh cleanup
  py -3.12 re/frida/run_diff_parallel.py <hook1> <hook2> ...
  Expect all GREEN / 100% bit-identical. Debug only the RED ones.

# Promote (re-classify ONCE with all promoted RVAs) — refuses w/o evidence.
# Commit ONCE: `c3: frontend/hud batch {bid} s{n} — <K> hooks, evidence cited`
# Append ONE row to re/PROMOTION_QUEUE.md:
#   2026-05-29  c3-batch-{bid}-s{n}  rvas=<csv>  branch=c3/batch-{bid}-s{n}  evidence=log/diff_*.csv  note=<one-line>

# STOP-AND-ASK if: anchor mismatch; clean-worktree baseline build fails;
# a note describes a function very different from hooks.csv; >2 candidates need
# caller drift-promote; an [UNCERTAIN] body marker not yet in UNCERTAINTIES.md.
"""

def cluster_for(cands):
    subs = {c[2] for c in cands}
    return "Hud" if subs == {"hud"} else "Frontend"

blocks = []
for idx, cands in enumerate(sessions, 1):
    rows_md = "\n".join(
        f"| 0x{rva:8} | {sig:41} | {note} |"
        for (rva, name, sub, note, n, sig) in cands
    )
    blocks.append(SESSION_TMPL.format(
        n=idx, bid=BATCH_ID, k=len(cands), rows=rows_md,
        anchor=ANCHOR, cluster=cluster_for(cands),
    ))

DEFERRED = ["\n" + "=" * 80, "DEFERRED (not in this batch — reason per bucket)", "=" * 80]
for bucket, items in deferred.items():
    DEFERRED.append(f"\n## {bucket}  ({len(items)})")
    for rva, sub, note in items:
        DEFERRED.append(f"  0x{rva}  {sub:9}  {note}")

OUT.write_text(HEADER + "".join(blocks) + "\n".join(DEFERRED) + "\n", encoding="utf-8")
print(f"WROTE {OUT}")
print(f"  sessions: {N_SESSIONS}  viable: {len(viable)}  "
      f"per-session: {[len(s) for s in sessions]}")
print(f"  deferred: void(void)={len(deferred['void(void)'])} "
      f"5+arg={len(deferred['5+arg'])} no-sig={len(deferred['no-sig'])}")
