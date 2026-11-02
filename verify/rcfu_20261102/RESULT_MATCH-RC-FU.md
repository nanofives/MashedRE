# RESULT MATCH-RC-FU, 2026-11-02

**Verdict: GREEN (text-only re-classify leg).** Base `orch/integration` 864ca99af (MATCH-S06-FU and MATCH-S07-FU merged).
No source, build or gate run in this step. Re-run output: `rerun.txt` (this folder).

## Rows

| rva | name | move | route / reason |
|---|---|---|---|
| 0x004282a0 | MenuMenusBA | C3 -> C4 | `match`, all three legs below |
| 0x0042e3a0 | MenuChromeShellA | C4 -> C3 | frida-route C4 measured a body with a wrong constant; exe-callers=0 (L-68) |
| 0x004274e0 | LocalizationFileLoad | stays C2 | exe-callers=0, .asi only; notes + U-0869 closed |

## 0x004282a0 (C3 -> C4, route match)

1. Byte identity: `match.py re/match/s06/004282a0.c --bytes --only 0x004282a0` (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`,
   C, x87): 100.0% insn 26/26, 122 B whole symbol BYTE-IDENTICAL. `relocheck.py`: OK checked=7 unchecked=2
   (`@__security_check_cookie@4`, `___security_cookie`). Source sha256 `b3dea86c...5362` (the 16-hex in S06's `results.tsv` is from before the preamble edits).
2. Body: `Match/MatchS06_004282a0.cpp` is the one body in exe_sources.rsp and asi_sources.rsp (`MenuMeasure.cpp` deleted). Exe callers: 3 direct sites,
   `MenuMenusBA <- MeasureMenuMessage <- RenderFrame <- WinMain` (`verify/matchs06fu_20261102/exe_reach.tsv`). Exe-side callees are port stand-ins, so the row is not evidence for 0x00427780 / 0x004277a0 / 0x005554d0.
3. Gates unchanged after the swap: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3 = `merge_train.json` refs (RESULT_MATCH-S06-FU arm 3; measure log 19/19 bit-identical, 9/9 equal to the original probe).
   The merged S06-FU + S07-FU tip was not re-gated here (text-only); the merge train gates on enqueue.
Write-set leg: not asked on the match route (CONFIDENCE.md). Statically the body stores only to its own frame (`short text[512]`), so it would be empty anyway.

## 0x0042e3a0 (C4 -> C3)

The premise in the row title holds. RESULT_MATCH-S07-FU: the old body drew the second amber tick at y = 65.5, the original draws 67.0 (`0x42860000` at 0x0042e52f).
The matched source is byte-identical (`rerun.txt`: 138/138 ins, 487 B, relocheck OK checked=17 unchecked=0, sha256 `25a46fa6...8272`) and is the one body
(`Match/MatchS07_0042e3a0.cpp`, exe + asi), but the exe's only call is under `if (false && g_b16_chrome_ready)` (`exe_main.cpp:5083`), exe-callers=0.
So it is neither a faithful frida C4 (the evidence covered the old body) nor a match C4 (L-68). Demoted to C3 with the L-68 note.
Rubric question for the lead (S07-FU flagged it): whether a frida-route C4 survives a body swap. Applied here: no, because the evidence no longer describes the shipped body.

## 0x004274e0 and U-0869

Stays C2 (`.asi` only, exe-callers=0). `re/match/s06/004274e0.c` sha256 `18b3d516...394b`: 100.0% 38/38 ins, 148 B BYTE-IDENTICAL, relocheck OK checked=12 unchecked=0
(re-run in `rerun.txt`). The relocheck line resolves `s_USA_dat_005cd5ac`: the original's case-5 arm pushes 0x005cd5ac. That answers the
"no data xref" gap of **U-0869** and it is closed with `uncertainty_resolve.py`. Not answered: whether USA.dat exists in a shipped PIZ (the port opens it only behind `MASHED_LOC_USA_DAT`, default OFF).

## Other RVAs in the two RESULTs

Every other swapped RVA is at C3 or C2 today and none qualifies for C4:

| rva | level | missing leg |
|---|---|---|
| 0x0042b960 CarSlotInit1P | C3 | (2) exe-callers=0 |
| 0x0042ef40 VehicleUnlockFlagGet | C3 | (2) exe-callers=0 |
| 0x00431d00 CarSelectReset | C2 | (2) exe-callers=0 |
| 0x00431b80 (in-file callee) | C2 | (2) exe-callers=0 |
| 0x0042fb70 MenuSpriteDispatchB | C3 | (2) exe-callers=0 |
| 0x0042d290 MenusLapTimeFmt | C3 | (2) not in the exe (.asi only) |
| 0x0042b9e0 CarSlotAssign | C3 | (3) exe default skips FUN_0040e480 behind knob MASHED_CARSLOT_ASSIGN_SLOTSTATE (OFF) |
| 0x0042f7b0 FrontendCursorUpdate | C3 | (2) the default exe calls FrontendCursorUpdatePreSwap; matched body only with MASHED_ABILITY_CURSOR_MATCHED (OFF) |
| 0x00430b90 ProgressBarSetA | C3 | (1)/(2) not swapped, matched text needs a port-only line |

The L-68 notes line for the seven callerless S07-FU rows (and the two knob rows' note) is not applied here (this step covers the three RVAs in the row). A mechanical follow-up row can add it.

## Tracker transaction

`hooks.csv`: 3 rows. `re/analysis/CHANGELOG.md`: 2 lines (0x004282a0, 0x0042e3a0) plus the U-0869 line from the helper. `STUBS.md`, `DEFERRED.md`: no change. No new U/S rows.
Edits were made with the edit tool on the row cells, no script.
