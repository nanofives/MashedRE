# RESULT MATCH-RC-S08, 2026-11-02

**Verdict: GREEN (text-only re-classify leg).** Base `orch/integration` e1ce45de5 (MATCH-S08 merged as d642e2ccb).
No source, build or gate run in this step. Re-run of the static tools: `rerun.txt` (this folder).
Inputs: `verify/matchs08_20261101/RESULT_MATCH-S08.md`, `verify/remerge_20261102/RESULT_MATCH-REMERGE.md`.

## Rows (the 4 C4 candidates of RESULT_MATCH-S08)

| rva | name | move | route / reason |
|---|---|---|---|
| 0x004425d0 | FUN_004425d0 (UtilZeroTable8964c0) | C3 -> C4 | `match`, all three legs |
| 0x00448700 | VehicleSeedWritePair | C2 -> C4 | `match`, all three legs (the rubric has no C3 prerequisite on the match route) |
| 0x00442cc0 | AiVehicleFloat4Get | stays C3 | legs (1) and (3) hold, leg (2) not run by the default build |
| 0x00443300 | FUN_00443300 (AiSplineEval_x87) | stays C3 | same as 0x00442cc0 |

## Legs

(1) Byte identity, from `rerun.txt` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87):

| rva | match.py | whole symbol | relocheck | src sha256 |
|---|---|---|---|---|
| 0x004425d0 | 10/8 ins, 80.0% (Ghidra size short) | 45 B BYTE-IDENTICAL | OK checked=3 unchecked=0 | f08b728e...7227 |
| 0x00442cc0 | 7/7 ins, 100.0% | 24 B BYTE-IDENTICAL | OK checked=2 unchecked=0 | da2f5b8b...c8ef |
| 0x00443300 | 112/112 ins, 100.0% | 307 B BYTE-IDENTICAL | OK checked=14 unchecked=0 | bea47eca...1c34 |
| 0x00448700 | 13/13 ins, 100.0% | 43 B BYTE-IDENTICAL | OK checked=4 unchecked=0 | aed03972...1aab |

(2) Body and exe caller. Each `Match/MatchS08_<va8>.cpp` is in `exe_sources.rsp` and `asi_sources.rsp` and includes
`re/match/s08/<va8>.c` (read in this step). The exe caller check read the call sites, not only the S08 `exe_reach.tsv`:

- 0x004425d0: `UtilZeroTable8964c0();` at `D3d9Render/TrackRenderer.cpp:716` in the race setup of `TrackRenderer::Load`,
  no condition. Default build runs it.
- 0x00448700: `TrackRenderer.cpp:5537` (`VehicleSeedWritePair(0, 0)`, case 3 first frame) and `:272`, `:277`
  (`MatchEndConclude`). Default build runs it. Its callee 0x004464c0 is the port's CameraEntryDispatchAll in the exe, so the
  row is not evidence for 0x004464c0.
- 0x00442cc0: the one exe site is `call AiVehicleFloat4Get` at `AiModeSeven_naked.inc:174`, reached only through
  `Ai::ModeSeven_MaybeReplay` (`TrackRenderer.cpp:3324`), which returns at once unless `MASHED_C30_REPLAY` is set
  (`AiTargetClosest.cpp:504`). Default OFF.
- 0x00443300: the 11 exe sites are in `AiLeafTargets_naked.inc` (e.g. `:792`), reached only through
  `Ai::AiLeaf_MaybeReplay` (`TrackRenderer.cpp:3325`), which returns at once unless `MASHED_AILEAF_REPLAY` is set
  (`AiLeafTargets.cpp:471`). Default OFF.

A direct call chain from `WinMain` exists for all four (`exe_reach.tsv`), and RESULT_MATCH-S08 already says that it does
not show the gate scenario executes the body. For the two replay-gated rows the default build never executes the swapped
body, which is the 0x0042e3a0 shape (RESULT_MATCH-RC-FU: caller under `if (false && ...)`, demoted to C3 with the L-68
note). Applied the same way here: L-68 note, no `c4=match`.

(3) Gates unchanged after the swap: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3 = `merge_train.json` refs
(RESULT_MATCH-S08 arm run on exe `66fa7228`, and RESULT_MATCH-REMERGE for the merged S08 head 3e48a2d09). The merged
integration tip was not re-gated here (text-only), the merge train gates on enqueue. For the two replay-gated rows the
gates show only that the swap changed nothing the default build runs.

## Rubric question for the lead

Whether "exe calls the body" (L-68) is met by a call site that only an env-gated replay harness reaches. I applied
"no" (default build does not execute it), consistent with RC-FU's 0x0042e3a0. If the lead rules "yes", 0x00442cc0 and
0x00443300 take C3 -> C4 with the same legs, and the notes already carry the evidence.

## Other swapped S08 rows (not C4, missing leg)

| rows | level | missing leg |
|---|---|---|
| 0x00432ad0, 0x004331a0, 0x00436810, 0x0043aee0, 0x0044b000 (exe + asi) | unchanged | (2) exe-callers=0 |
| 0x00433240, 0x0043c000 and 18 PromoLoop leaves 0x00432260, 0x00432290, 0x00442410, 0x00442420, 0x00442df0, 0x00443080, 0x00443090, 0x004430a0, 0x004430b0, 0x004495d0, 0x00449880, 0x0044c370, 0x0044d6e0, 0x0044dfe0, 0x0044dff0, 0x0044e020, 0x0044e050, 0x0044e070 (asi only) | unchanged | (2) not in the exe. 0x0043c000 is already C4 by the frida route (asi-only swap) |
| 0x004323c0, 0x004325c0, 0x00441820, 0x00441c80, 0x00442600, 0x004464c0, 0x00442c80, 0x00442ce0, 0x00448940, 0x00448dc0 | unchanged | (2) not swapped (NOT-SAFE, reasons in RESULT_MATCH-S08). 0x00442c80 and 0x00442ce0 wait for MATCH-S08-FU |
| 54 byte-identical rows with no port body | unchanged | (2) no body, no caller |

The L-68 notes line for the 24 callerless swapped rows is not applied here (the row names the 4 candidates). A mechanical
follow-up row can add it, as noted in RC-FU.

## Pending

- MATCH-S08-FU (0x00442c80, 0x00442ce0, 0x00413f90 notes and file cells): stage `merge-queued` at the time of writing,
  not applied.

## Tracker transaction

`hooks.csv`: 4 rows (file cells moved to the wrapper TUs, notes). `re/analysis/CHANGELOG.md`: 4 lines. `STUBS.md`,
`DEFERRED.md`, `UNCERTAINTIES.md`: no change, no new U/S rows. Edits were made with the edit tool on the row cells, no script.
