# RESULT hand-merge MATCH-0040E370-NAKED + MATCH-TOPDOWN-T13 (one branch), 2026-11-01

**Verdict: GREEN, gates unchanged.** Both red-merge rows sit on one branch, `orch/MATCH-0040E370-NAKED`, rebased by merge onto
`orch/integration` 6d9d01a7f (MATCH-S05 and MATCH-S07 merged). One build, one gate run. Shared branch and sha for both
ledger rows. Row evidence stays in each row's RESULT
(`verify/match0040e370_20261101/RESULT_MATCH-0040E370-NAKED.md`,
`verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-T13.md`); this file covers the merge.

## Merges

1. **8ef97d395** `orch/integration` 6d9d01a7f into NAKED (red-merge 20261101_174537). Conflicts:
   `AiTargetClosest_naked.inc`, `target_p3.json`, `dual_copy_allowlist.txt`, `CHANGELOG.md`. D4-DC-INC-batch3 (on
   integration) bound 0x0040e470 / 0x004c3ac0 / 0x004c3b30 to `CarSlotStateGet` / `RwLutLength_cd` / `RwLutSqrt_cd` and
   dropped `TG_40e470` / `TG_4c3ac0` / `TG_4c3b30`. This row drops `TG_40e370` and binds 0x0040e370 to `IsCarSlotActive`.
   - `target_p3.json` = integration's spec minus the `TG_40e370` func, call `0x0040e370 -> IsCarSlotActive`. Funcs:
     `TG_414a70`, `TG_414300`, `TG_426bb0`.
   - `.inc` regenerated: `py -3.12 re/tools/gen_naked_asm.py <spec> <inc>` from the main checkout. Before that, the
     generator run on integration's spec reproduced integration's committed .inc byte for byte (line endings aside).
     Diff against integration: `TG_40e370` declaration and body gone, `call IsCarSlotActive` at 00414ad5. Nothing else.
   - allowlist = integration's file minus `DUP-IN-TARGET:0040e370`. CHANGELOG: both sides kept.
   - `AiTargetClosest.cpp` header comment: two bodies and one leaf, plus the batch3 bindings.
2. **7d6f1150e** `orch/MATCH-TOPDOWN-T13` 421f5e28c (red-merge 20261101_183121). Conflicts: `AiTargetClosest.cpp`
   (header comment only), `PathProgStep_naked.inc`, `pathprog_p2.json`.
   - `pathprog_p2.json` = integration's spec (batch3 already dropped `PP_408590`, `PP_407b00`, `PP_407b70`,
     `PP_407d70`, `PP_407db0`) minus the `PP_407cd0` func. The call map keeps `0x00407cd0 -> PP_407cd0`, now the matched
     body in `Match/MatchS01_00407cd0.cpp`.
   - `PathProgStep_naked.inc` regenerated the same way (generator reproduced integration's .inc first). Diff against
     integration: the `PP_407cd0` declaration and 157-byte body gone. The 4 `call PP_407cd0` sites stay.
   - T13 had hand-added `extern "C" int __cdecl PP_407cd0(int v, int start);` inside the generated .inc, which a
     regenerate drops. Moved to `Race/PathProgStep.cpp` next to the batch3 declarations before the `#include`, so the
     .inc stays generator output.
   - `AiTargetClosest.cpp`: integration's header comment with T13's `TG_SubMode -> GetRenderSubMode` line. The
     `TG_SubMode` body is T13's (auto-merged).
   - T13's 0x0040e350 argument (no exe TU stores 0xa or 0xb into `DAT_0063ba8c`) re-checked on this head: no line
     naming 0x0063ba8c was added to `mashedmod/src` or `re/match` between T13's base ca02bde58 and this head.

## Build and lints

`set MASHED_NO_DEPLOY=1&& mashedmod\build.bat` at 7d6f1150e: `=== Build OK ===`, .asi not deployed.
exe sha256 `18f01a41b7c05a44f6e90df997e4cf55bf995bd96931b420d10b67a2aa717ba3`, asi
`022497083a19fb79cd5fe8900d3357056dd2bb7cf262c884eeaaddbf024874c7`.
`[rva-lint] 24 finding(s): CROSS-TARGET=18, DUP-IN-TARGET=6 | allowlisted=24 NEW=0`, `[asm-lint] OK`.
`py -3.12 scripts/lan_release/archive_lint.py --ref HEAD` -> `[archive-lint] OK -- clean git archive of 7d6f1150e`.
`TG_40e370`, `TG_40e470`, `TG_4c3ac0`, `TG_4c3b30` not in the exe map.

## match.py / relocheck at this head (`match_rerun.txt`)

| rva | source (git blob sha256) | match.py | relocheck |
|---|---|---|---|
| 0x0040e370 | `re/match/s02/0040e370.c` (dc117d70255108a5...) | 100.0% 14/14, 34 B BYTE-IDENTICAL | OK checked=1 unchecked=0 |
| 0x00407a20 | `re/match/s01/00407a20.c` (61139c86f195161e...) | 100.0% 4/4, 17 B BYTE-IDENTICAL | OK checked=1 unchecked=0 |
| 0x00407cd0 | `re/match/s01/00407cd0.c` (8c1e867ebea55984...) | 100.0% 73/73, 157 B BYTE-IDENTICAL | OK checked=1 unchecked=0 |
| 0x0040e350 | `re/match/s02/0040e350.c` (b27a00ee9a43b611...) | 100.0% 2/2, 6 B BYTE-IDENTICAL | OK checked=1 unchecked=0 |
| 0x0046c5f0 | `re/match/pilot/a5_0046c5f0.c` (bc95ce9fbd576f94...) | 100.0% 71/71, 210 B BYTE-IDENTICAL | OK checked=4 unchecked=0 |

T13's RESULT lists different 16-hex prefixes for its sources. Those are not git blob hashes (the CRLF checkout
hashes differently, see MATCH-S05-RC). The blob hashes above are the ones to cite.

## Exe callers of every swapped RVA on this build

`exe_callsites.py` (MATCH-S05 tool, list not truncated) on the build above (`exe_callsites.txt`):

| rva | symbol | exe call sites |
|---|---|---|
| 0x0040e370 | `_IsCarSlotActive` | 8: `_TG_414a70+0x65` (= 0x00414ad5), `CarsReg_FrameTick+0x11`, `CarParts_FrameTick+0x29`, `_RaceComputeDistancesTick+0x48`, `PathProg_FrameTick+0x112`, `+0x156`, `HeadingAssist_Frame+0x15`, `HeadingAssist_Player+0xc` |
| 0x00407a20 | `_GetAiLapCounter` | 1: `AiPreTickRubberBand+0x37` |
| 0x00407cd0 | `_PP_407cd0` | 4: `_PP_407e20+0x17f`, `+0x18f`, `+0x2c6`, `+0x2d3` |
| 0x0040e350 | `_GetRenderSubMode` | 2: `_TG_SubMode+0xf`, `Ai::ModeSeven+0x3e7` |
| 0x0046c5f0 | `Collision::TriangleFaceNormal` | 1: `Vehicle::VehicleWheelForceIntegrate+0xe5c` |

Same counts as each row's own RESULT (NAKED 8, T13 1 / 4 / 2 / 1).

## Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the copied-out exe `18f01a41`, private root
`D:/MashedWT/MATCH-0040E370-NAKED_root`, test slot + `mashed_lock.py run --label MATCH-NAKED-T13`
(`D:/MashedWT/MATCH-0040E370-NAKED_data/m3/gates.sh`). Every arm exit 0, self-exited.

| arm | file | lines | sha8 | merge_train.json ref |
|---|---|---|---|---|
| H_DEF (185 s) | step.csv / gates.csv / ptrace.log | 13962 / 54085 / 27042 | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e |
| B_DEF_K0, K1, K26 (19 / 17 / 15 s) | step.csv | 762 each | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

A first launch (`gates_try1_queued.log`) waited an hour in the machine-lock queue behind the merge train and other
sessions, then lost its output pipe when my tool timeout fired. It never started a game. I stopped its PIDs (my own
only) and re-ran.

Raw files (not committed, `D:/MashedWT/MATCH-0040E370-NAKED_data/m3`):

| file | sha256 |
|---|---|
| exe/mashed_re.exe | 18f01a41b7c05a44f6e90df997e4cf55bf995bd96931b420d10b67a2aa717ba3 |
| arms/H_DEF.step.csv | 8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198 |
| arms/H_DEF.gates.csv | ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb |
| arms/H_DEF.ptrace.log | 31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3 |
| arms/B_DEF_K0.step.csv, B_DEF_K1.step.csv, B_DEF_K26.step.csv | 1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457 (all three) |
| gates.log | d741c8ce1f6b0a371d575025cba9b5e083f8d0c835341ccdbf5d276161a84cda |
| build3.log | build log |

## -Full

Not run here. The branch touches `mashedmod/src/mashed_re/Ai/`, `Collision/` and `Race/` (in `full_set_paths`), so the
train's `-Full` pass is the train's leg on enqueue.

## Trackers

NAKED's re-classify (hooks.csv 0040e370 C3 -> C4, c4=match, commit 23703c125) stands. T13's prepared re-classify
(4 rows, its RESULT's last table) is a post-merge leg and is not applied here.
