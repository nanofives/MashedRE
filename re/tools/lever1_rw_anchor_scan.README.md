# Lever-1: RW String-Anchor Scan

## What it does

`lever1_rw_anchor_scan.py` scans `original/MASHED.exe` for null-terminated
strings in `.rdata` and `.data` that match RenderWare 3.x API-name patterns
(`Rw*`, `Rt*`, `Rp*`, `Rwp*`, `rwID_*`, etc.) and uses them as naming
anchors for the functions that reference them.

Two evidence patterns produce actionable proposals:

**STATUS / PREFIX** — The function directly logs its own name, e.g.
`"RtFSManagerRegister successful"`.  The emitting function IS (or directly
wraps) the named API.  These are the most reliable anchors.

**CALLING-callsite** — The engine-init orchestrator logs `"Calling X"` before
calling X.  A forward binary scan from the push site finds the CALL target
that follows the log call; that target is the X function.

FILE-pattern strings (`@@(#)$Id: //Physics/Rwp37Active/src/...`) are extracted
but yield 0 actionable proposals in MASHED.exe because those strings are
embedded in static data structures accessed via pointer arithmetic, not via
direct `push imm32` in code.

## Running

```powershell
# From repo or worktree root:
py -3.12 re/tools/lever1_rw_anchor_scan.py
```

Output is written to `re/analysis/plans/lever1_rw_proposals.tsv`.

## Results (2026-05-23 run)

| Metric | Count |
|---|---|
| RW-related strings found in binary | 60 |
| Strings with detectable .text refs | 5 |
| Emitter functions (hold the log string) | 2 |
| Callee inferences (CALLING-callsite) | 3 |
| **Total proposals** | **8** |
| High-confidence proposals | 4 |
| Low-confidence proposals | 4 |

### High-confidence proposals

| function_rva | current_name | proposed_name | evidence |
|---|---|---|---|
| 0x004624c0 | FUN_004624c0 | RtFSManagerRegister | STATUS ("RtFSManagerRegister successful") |
| 0x004c2fb0 | FUN_004c2fb0 | RwEngineStart | CALLING-callsite (forward from "Calling RwEngineStart") |
| 0x004c30b0 | FUN_004c30b0 | RwEngineOpen | CALLING-callsite (forward from "Calling RwEngineOpen") |
| 0x004c32b0 | FUN_004c32b0 | RwEngineInit | CALLING-callsite (forward from "Calling RwEngineInit") |

All four were cross-verified via Ghidra MCP `function_at` and match
existing hooks.csv analysis notes (C2/C3 render subsystem).

### Low-confidence proposals

`FUN_00493710` (the engine init orchestrator) is proposed for four names
because it logs all four "Calling X" strings before delegating — it is none
of them specifically.  These rows are informational; the function already has
C3 analysis in hooks.csv.

## Caveats and false positives

**Expected yield was over-estimated.**  The Lever-1 skill description projected
80–150 proposals.  The actual yield is 8.  MASHED.exe contains only 5 RW API
log strings (vs. hundreds in a full Criterion SDK release): Criterion shipped
the game without the standard RW debug/verbose log layer, retaining only the
init-path instrumentation.

**FILE-pattern strings are not useful here.**  The `@@(#)$Id:` RCS strings for
the Rwp physics middleware (`RwpMgr.c`, `RwpDyn.c`, etc.) appear in `.rdata`
but are accessed through static data-structure pointers and floating-point
constant references (the first 4 bytes of the ID string happen to be valid
float constants used by the physics solver).  They yield 0 push/mov refs in
`.text` and very few Ghidra xrefs (all are `fmul` instructions using the
string content as a float, not as a string).

**CALLING-callsite for RtFSManagerOpen is unreliable.**  The engine-init
sequence calls several helper and status functions between logging "Calling
RtFSManagerOpen" and actually opening the FS manager.  The forward-call
heuristic cannot reliably identify the correct callee; all candidates were
excluded via the `CALLING_SITE_HELPERS` list after Ghidra + hooks.csv
cross-check.  The actual `RtFSManagerOpen` implementation in MASHED.exe
remains unlocated by this approach.

**`0x5c9d00 (GetRaceEndTrigger)` is a confirmed false positive.**  It is a
2-byte `XOR EAX,EAX; RET` stub that appears as a CALL target near the
"Calling RwEngineInit" site because of how the engine-init function lays out
its error-handling path.  It is excluded via `CALLING_SITE_HELPERS`.

## Writeback workflow (future session, OUT OF SCOPE for this run)

1. Review `lever1_rw_proposals.tsv` — focus on `confidence_hint=high` rows.
2. For each accepted rename: open a Ghidra master session (not a pool clone)
   and call `mcp__ghidra__function_rename` to apply the name.
3. Update `hooks.csv` via `re-classify` to reflect the new canonical name.
4. The low-confidence rows for `FUN_00493710` do NOT need renaming — the
   function already has C3 status and a descriptive analysis note.

Note: Rows 3–6 (FUN_00493710 as orchestrator) are documentation artifacts,
not rename proposals — the function calls all four APIs and cannot be named
after any one of them.
