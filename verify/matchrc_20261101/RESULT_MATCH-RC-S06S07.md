# RESULT MATCH-RC-S06S07, 2026-11-01

**Verdict: GREEN (text-only re-classify leg).** 20 rows touched in `hooks.csv` and `re/analysis/CHANGELOG.md`:
18 rows C3 -> C4 by route `match`, and 2 rows (0x00426cc0, 0x0042fe30) already C4 by the frida route that gain the
match route in `notes`. No source, build or gate run in this step; it applies the route-`match` text that
RESULT_MATCH-S06 and RESULT_MATCH-S07 prepared, after re-checking each leg at this tip (`orch/integration` 134f233cd).

## The three legs, per route (CONFIDENCE.md "C4 routes by byte match")

1. **Byte identity + relocheck, re-run at this tip.** `MSYS_NO_PATHCONV=1 py -3.12 re/tools/match/match.py <src> --bytes --only 0x<rva>`
   (XP DDK cl 13.00.9176 / c2 13.00.9178, `/O2 /GS /Ob1`, C, x87) and `re/tools/match/relocheck.py <src>` on all 20
   sources: `rerun_s06s07.txt` (this folder). All 20 report `BYTE-IDENTICAL (relocations masked)` (whole symbol, 0
   differing unmasked bytes) and relocheck `OK` with `unchecked=0`. 0x0042bb60 scores 95.9% by instruction only
   because Ghidra's size is short (123/118 ins); its whole-symbol bytes (323 B) are identical.
   **Source sha256 differs from the S06 `results.tsv` column for the S06 rows** (e.g. 0x00426090 `9571180a...` in the table,
   `cec6a82a...` on disk): the table hash was taken before the preamble / C++-compat edits that RESULT_MATCH-S06 lists, and
   those edits were re-scored 100%. The `notes` cells carry the sha256 of the file as it is now (the one the exe compiles).
   The S06 RESULT's per-row text quoting the old 16-hex was not used.
2. **The exe uses that source as the body, with an exe caller.** One wrapper per RVA `Match/MatchS06_<va8>.cpp` /
   `Match/MatchS07_<va8>.cpp` (exist at this tip, exe + asi). `exe-callers` and the `WinMain` chain are from the
   linker-map call graph (`verify/matchs06_20261101/exe_reach.tsv`, `verify/matchs07_20261101/exe_reach.tsv`, S07's
   `exe_reach.py`): all 20 rows have direct call sites and `reach_WinMain = yes`. The swap is the only body
   (`lint_rva_bodies.py` NEW 0 per both RESULTs).
3. **Gates unchanged.** Merge-train runs on the merged branches: MATCH-S07 (2026-11-01 19:10) and MATCH-S06
   (2026-11-01 20:49), `re/orchestrator/INTEGRATION_LOG.md`: G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3, (b) 3/3,
   (e) 3/3, D2 3/3, 13 tracks identical, PASS. `gctrl.ref` / `b.ref` in `re/orchestrator/merge_train.json` at this tip are
   the same values. Branch-side runs: RESULT_MATCH-S06 and RESULT_MATCH-S07 "Gates".

## Rows promoted (hooks.csv: confidence C4, `file` = wrapper, `exe_file` cleared, status cell kept as in MATCH-S01-RC)

| rva | name | before | exe sites | chain from WinMain |
|---|---|---|---|---|
| 0x00426090 | GlobalDat0066ce58Get | C3 | 8 | RangeTableMemberTest <- PP_408610 <- PathProg_MaybeReplay <- TrackRenderer::Load |
| 0x00426bb0 | Global66d6d8Get | C3 | 5 | TG_414a70 <- TargetClosest_MaybeReplay <- TrackRenderer::Load |
| 0x00426bc0 | GetDat0066d6e0 | C3 | 1 | SnapNodeToSafeStartLines <- RoundGrid_MaybeReplay <- TrackRenderer::Load |
| 0x00426bd0 | GetTableEntry0066d658 | C3 | 1 | same |
| 0x00426cb0 | SlotIndexToPtr | C3 | 7 | PP_407e20 <- PP_408610 <- PathProg_MaybeReplay <- TrackRenderer::Load |
| 0x00426cf0 | GetDat0066d6e4 | C3 | 1 | PP_408610 <- PathProg_MaybeReplay <- TrackRenderer::Load |
| 0x00426d00 | FrontendArraySlotGet | C3 | 8 | PP_407e20 <- ... |
| 0x0042a9c0 | ModeCodeLookup | C3 | 2 | PromptStripResolve <- PromptStripAppend <- **Nav_DevGoto** <- WinMain |
| 0x0042ad90 | Find42ad90 | C3 | 9 | **Nav_DevGoto** <- WinMain |
| 0x0042b920 | ConstantGetter22 | C3 | 1 | PromptStripResolve <- PromptStripAppend <- Nav_Init |
| 0x00431b00 / 0x00431ae0 / 0x00431af0 | DefaultParam_SetField00/04/08 | C3 | 1 each | ExecuteFrontendBootChain |
| 0x0042c1d0 | RaceStateArrayZero | C3 | 1 | ExecuteFrontendBootChain |
| 0x0042d3e0 | MenuEntryArrayInit | C3 | 2 | ExecuteFrontendBootChain |
| 0x0042bb60 | MenuTeamBalance | C3 | 2 | RenderFrame, and WinMain directly (Team Select) |
| 0x0042fa00 | MenuTeamSelectTick | C3 | 1 | RenderFrame (Team Select) |
| 0x0042f6a0 | GetRaceSubMode | C3 | 3 | RaceEndFlagIfEndMode <- PickupPoolSpawn <- InitPickups |
| 0x00426cc0 | VehicleTable4cPtrGet | C4 (frida) | 4 | PP_407e20 ... : match route added, stays C4 |
| 0x0042fe30 | RaceEndFlagIfEndMode | C4 (frida) | 1 | PickupPoolSpawn <- InitPickups : match route added, stays C4 |

Each `notes` cell says which callees and globals no gate exercises (CONFIDENCE.md, "How the Write-set leg relates").
Flags the lead may overrule (the rows are still promoted, by the letter of L-68):
- **0x0042a9c0, 0x0042ad90**: reach `WinMain` only through `Frontend::Nav_DevGoto`, the dev goto path; a default run does
  not execute it. Noted in each row. To hold them at C3 instead, revert those two row edits.
- **0x0042bb60, 0x0042fa00**: reached through Team Select (nav screen 16), which none of the merge-train gates runs; noted.
- 0x00431ae0/af0/b00 were demoted C4 -> C3 on 2026-10-26 (WSH-NOEVID, no committed diff) and reach C4 again here on the
  match route, which needs no diff.

## Rows not promoted, and the missing leg

| set | rows | missing leg |
|---|---|---|
| S06 swapped, no exe caller reaching `WinMain` | 94 (104 swapped minus the 10 above; of these 9 are already C4 by frida: 0x00426c10, 0x00426c30, 0x00426c70, 0x00427780, 0x004295a0, 0x0042a920, 0x0042aad0, 0x0042aae0, 0x0042ae10) | (2) exe-callers=0 or callers only in callerless bodies (L-68); list in `verify/matchs06_20261101/swaps.tsv` and `exe_reach.tsv` |
| S07 swapped, no exe caller | 79 (45 exe+asi, 34 asi-only; 17 already C4 by another route) | (2), same; `verify/matchs07_20261101/swaps.tsv`, `exe_reach.tsv` |
| S06 identical, not swapped | 16 + 2 NOT-SAFE (0x004274e0 behaviour change, 0x004282a0 CROSS-TARGET) | (2) not the body; 0x004274e0 needs a knob + PREREG or a decision row |
| S07 identical, not swapped | 25 (RESULT_MATCH-S07 table: old port body diverges, `...For(port state)` forms, register convention, callee without port body, pair file, 0x0042e3a0 / 0x0042fb70 not assessed) | (2) not the body |
| below 100% | 6 (S06) + 14 (S07) | (1) not byte-identical |

The 173 callerless swapped rows keep their level. **Their `notes` were not given the L-68 line**
(`match evidence: identical, swapped, exe-callers=0; C4 when a caller lands (L-68 ruling)`), because this step covers
the rows that qualify; both RESULTs list them in `swaps.tsv`. A follow-up mechanical row can add the line.

## Tracker transaction

- `hooks.csv`: 20 rows (`confidence`, `file`, `exe_file`, `notes`). The CSV round trip dropped redundant quotes on
  row 0x0041d820; that row was restored byte-for-byte, so `git diff` shows 20 changed rows.
- `re/analysis/CHANGELOG.md`: 20 lines under `<!-- ENTRIES -->`.
- `STUBS.md`, `UNCERTAINTIES.md`, `DEFERRED.md`: no change. No new U-NNNN or S-NNNN. The match route carries no stubs in
  these bodies (one-instruction to 56-instruction leaves and walkers), and no `[UNCERTAIN]` marker was touched.
- Script: `apply_rc.py` (scratch, not committed) performed the row edits; output is the diff on this branch.

## Files

`verify/matchrc_20261101/rerun_s06s07.txt` (match.py + relocheck output and source sha256 for all 20 sources).
