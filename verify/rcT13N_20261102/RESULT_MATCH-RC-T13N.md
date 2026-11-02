# RESULT MATCH-RC-T13N + MATCH-RC-T01G + MATCH-RC-NAVDEV (text-only), 2026-11-02

**Verdict: GREEN.** One branch, `orch/MATCH-RC-T13N`, off `orch/integration` 035dc5bff. Text-only: `hooks.csv` (8 rows) and
`re/analysis/CHANGELOG.md` (8 lines). No source, build or game run. Tracker edits follow the re-classify skill's
documented operation (row update plus CHANGELOG line below `<!-- ENTRIES -->`, newest first); `hooks.csv` keeps CRLF and a
8-line diff, the CHANGELOG diff is +8 lines.

## Gate shared by every C4 below (`re/CONFIDENCE.md`, "C4 routes by byte match")

- (1) match.py 100% byte-identical with XP DDK `cl 13.00.9176 / c2 13.00.9178 /O2 /GS /Ob1`, as C, x87, and relocheck OK:
  `verify/match_naked_t13_20261101/match_rerun.txt` (re-run at the merged head, T13 and NAKED sources) and the T01 / T08 RESULTs.
- (2) the exe compiles that source as the RVA's one body (rva-lint NEW=0, one body per RVA, `Match/` TU; float TUs in
  `mashedmod/x87_tus.txt`: `MatchS01` and `MatchPilot_0046c5f0`; the other bodies have no float).
- (3) gates unchanged: merge train 20261102_004044 (`re/orchestrator/INTEGRATION_LOG.md`, 2026-11-02 01:00) merged
  MATCH-TOPDOWN-T01 (carries T01/T02/T08/T09/T12) and MATCH-0040E370-NAKED (carries T13) and reproduced
  G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3 = the `merge_train.json` refs, plus (b) 3/3, (e) 3/3, D2 3/3, 13 tracks.
  The T13 branch also ran its own full gates on exe `f137a9a0` (`RESULT_MATCH-NAKED-T13-MERGE.md`, equal). T01's own
  H_DEF arms were capped at 900 s (byte-identical prefixes, B equal); its C4 lines were conditional on the train's
  full-hash run, and the train's run is the one cited here.
- L-68 callerless rule: every exe-callers count below is from the merge RESULTs' `exe_callsites.txt` (T13, merged build)
  or the T01 / T08 RESULT call-site columns (this branch's build).

## Transactions, one per RVA

| rva | name | level | exe-callers | evidence |
|---|---|---|---|---|
| 0x0040e370 | IsCarSlotActive | C4 -> C4, **no edit** | 8 | already re-classified by NAKED (commit 23703c125, in `orch/integration`); the merge RESULT confirms 8 callers on the merged build |
| 0x00407a20 | GetAiLapCounter | C4 -> C4, route `match` added | 1 (AiPreTickRubberBand+0x37) | src sha256 `61139c86...a0178`, 100.0% 4/4 17 B, relocheck OK 1/0; file/exe_file moved to `Match/MatchS01_00407a20.cpp` |
| 0x00407cd0 | FUN_00407cd0 (PP_407cd0) | C2 -> C4 | 4 (PP_407e20) | `8c1e867e...e4f48`, 73/73 157 B, relocheck OK 1/0; naked PP_407cd0 gone from the map |
| 0x0040e350 | GetRenderSubMode | C3 -> C4 | 2 (TG_SubMode, Ai::ModeSeven) | `b27a00ee...018b8`, 2/2 6 B, relocheck OK 1/0 |
| 0x0046c5f0 | FUN_0046c5f0 (TriangleFaceNormal) | C2 -> C4 | 1 (VehicleWheelForceIntegrate+0xe5c) | `bc95ce9f...cd65`, 71/71 210 B, relocheck OK checked=4 unchecked=0; x87 TU |
| 0x00407a60 | FUN_00407a60 | C3 -> C4 | 2 (PathProg_RaceInit tail-jmp, inlined copy in PathProg_MaybeReplay) | `a5fe8c5a...95169`, 12/12 40 B, relocheck OK 2/0; supersedes the S01-RC exe-callers=0 hold |
| 0x00417730 | VehicleRaceAngleGet | C3 -> C4 | 2 (AiPreTickRubberBand) | `850bb793...910cd`, 3/3 12 B, relocheck OK 0/1 (`_DAT_0089a880`, resolved by address in S03-RC); supersedes the S03-RC exe-callers=0 hold |

Sha256 values are of the git blob (`git show HEAD:<path> | sha256sum`). The 00407a60 value differs from the S01-RC note
(`bc8c765f...`), which hashed the CRLF checkout (same effect the merge RESULT records for T13's prefixes).

## NAVDEV (L-84, answered (a) by Mariano 2026-11-02)

0x0042a9c0 and 0x0042ad90 stay C4. A dev-only exe path (`Frontend::Nav_DevGoto <- WinMain`) counts as an exe caller under
L-68. Recorded in both rows' `hooks.csv` notes and in two CHANGELOG lines (`C4->C4`). No level change, no revert of the
RC-S06S07 edits.

## Every other swapped RVA, and its missing leg

None missing for the seven swaps these merges carried (the table above is all of them: NAKED 1, T13 4, T01 1, T08 1).
Rows the same RESULTs list as `left` carry no swap in these merges, so they have no C4 leg to complete here:
T13 lists 12 left rows, T01 49, T08 8 (reason per row in each RESULT, mostly "no exe call site: every original caller has no
exe body"). Specific cases with a level already set, untouched here:

- 0x00414180: already swapped and called (1 exe caller), held at C3 by open stub S-3669 (T01 RESULT). Not a missing match leg.
- 0x0041d820, 0x0040e340: already C4 (T01 RESULT).
- 0x00414060: exe port is a partial body, a swap would be a behaviour change (T01 RESULT). Not swapped.

## Not done on purpose

No source touched, no `-Full`, no merge or enqueue, no change to the 12 + 49 + 8 left rows.
