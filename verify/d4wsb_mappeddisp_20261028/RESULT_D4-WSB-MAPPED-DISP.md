# RESULT D4-WSB-MAPPED-DISP: RenderWare-Physics 147 mapped + 14 new rows

Verdict: **GREEN (disposition filed, tracker writes not made)**. All 161 C2 rows of class `mapped` (147) and `new` (14) in
`third-party-library[RenderWare-Physics-3.7]` have a disposition. Input: `verify/wsbcensus_20261026/wsb_census.tsv`
(D4-WSB-CENSUS, 3/3 unanimous). Per-row table: `verify/d4wsb_mappeddisp_20261028/disposition.tsv` (SHA-256 `43fc37893230b6f20ab15232063df6261a065063eccbae495a9728a267cf9ba7`).
Text-only: no build, no game run, no source or tracker edit, no promotion. Gates: not run (no code change).

## Disposition counts

| disposition | id | rows |
|---|---|---|
| PORT STEP (census EXEC, 91 rows) | D4-WSB-PORT-BROAD-1 | 17 |
| | D4-WSB-PORT-BROAD-2 | 18 |
| | D4-WSB-PORT-BVH | 25 |
| | D4-WSB-PORT-SOLV | 18 |
| | D4-WSB-PORT-NARROW-TAIL | 13 (NARROW 4 + TAIL 9) |
| DEFERRED group (census NOT-EXEC, 70 rows) | D4-WSB-DEF-BROAD | 26 |
| | D4-WSB-DEF-BVH | 19 |
| | D4-WSB-DEF-SOLV | 14 |
| | D4-WSB-DEF-NARROW | 9 |
| | D4-WSB-DEF-TAIL | 2 |

Totals: mapped 78 EXEC + 69 NOT-EXEC, new 13 EXEC + 1 NOT-EXEC = 91 + 70 = 161. Chunks are address order inside a band
(BROAD split in two because it has 35 EXEC rows). The ids are proposals: creating ledger rows is the lead's call.

## What "mapped" means here, and what a port step is

Every row's hooks.csv entry is C2 with a generated plate (`re/analysis/c2gen/third-party-library_RenderWare-Physics-3.7/<rva>.md`,
C1->C2 2026-10-11). `mapped` carries no reimplementation, so the default build does not run a ported body for it. A PORT STEP is:
author the body at `mashedmod/src/mashed_re/`, `RH_ScopedInstall`, default-OFF knob + PREREG per WORKER_RULES, then re-classify
C2 to C3 only with a Frida A/B. That last leg needs an original-side run and is **BLOCKED this run** (MASHED.exe does not boot:
no audio endpoint, AV at 0x005baf48). Nothing here was ported or promoted.

## NOT-EXEC rows: why a DEFERRED group, not a dispatch

- The census scenario is one track, one mode (10), no player input, no race end, no pause (census RESULT, "Scope of the evidence").
  NOT-EXEC means not reached there, not dead.
- Cross-check against the 2026-10-11 census (`re/coverage/reach.tsv`, same method): 0 of the 70 NOT-EXEC rows were reached there
  either (all four reach columns 0, 0/3 runs). Two censuses agree. One EXEC row (1 of 91) was not reached on 10-11, so
  the two scenarios are not identical.
- I did not mark any of the 70 as dead and did not write an explicit dispatch row for them: no function-level evidence of a
  dispatcher gap exists yet, and a "dispatch" disposition would need a decomp read of the caller chain (Ghidra, not available
  on this account or run).

Proposed DEFERRED rows, one per band (ids `D4-WSB-DEF-*` above, to be numbered D-NNNN on insert):
- **Why deferred:** NOT-EXEC in 3/3 runs of the canonical census scenario and in the 2026-10-11 census, C2 plate only, no
  reimplementation, so the default build cannot execute them faithfully but the original does not reach them in the measured scenario either.
- **Re-pickup when:** a wider census (player-driven race, tracks other than 0, modes other than 10, race end, pause) reaches the row, in which case
  it moves to the matching PORT STEP, or a static caller walk proves it has no caller in the executed image (then close as not applicable).
- The widened census is the concrete next measurement: it needs an original-side run, **BLOCKED** this run.

## Tracker writes: not made, and why

`re-classify` documents two DEFERRED operations only: `deferred_close.py close|rewrite`. It has **no add-row operation**, and
WORKER_RULES forbids ad-hoc scripts or hand edits for trackers. So the five DEFERRED rows above are filed here as text, not inserted.
No C-level moves, so there is no hooks.csv, STUBS.md or UNCERTAINTIES.md change either. Open for the lead: either add the five
rows through a skill-sanctioned path, or accept this RESULT as the disposition record. No user decision needed.

## Files

- `verify/d4wsb_mappeddisp_20261028/disposition.tsv`: 161 rows, `rva, band, status, census, phases, disposition, id`.
- Input: `verify/wsbcensus_20261026/wsb_census.tsv` (committed by D4-WSB-CENSUS), `re/coverage/reach.tsv` (2026-10-11).
