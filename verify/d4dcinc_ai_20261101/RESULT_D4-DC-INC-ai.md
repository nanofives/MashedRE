# RESULT D4-DC-INC-ai: one body each for 12 RVAs the .inc-aware scanner exposed (13 allowlist keys)

**Verdict: GREEN.** Both default gates are byte-identical to the `merge_train.json` refs. All 13 keys are gone from `re/tools/dual_copy_allowlist.txt` (48 findings to 35,
NEW=0). Every RVA has exactly one body, in a TU compiled into both targets.

Base `orch/integration` @ `0e3d71a75`. Branch `orch/D4-DC-INC-ai`. Rows: D4-DC-INC-0040e470, -0040e470-X, -004233e0,
-00408af0, -00443300, -00442cc0, -00484c70, -00408590, -00407580, -00407b00, -00407b70, -00407d70, -00407db0.

## Method

The exe-only copies were not hand-deleted. They were private verbatim naked copies that `re/tools/gen_naked_asm.py`
generates from the anchored bytes. Each of the five specs (`re/tools/naked_specs/aileaf_g1.json`, `target_p3.json`,
`mode7_m4.json`, `pathprog_p2.json`, `pickups_m3.json`) drops the func and maps the `call` to the shared body, and
the five `.inc` files are regenerated. Before the edit, each spec reproduced its committed `.inc` byte for byte
(modulo CRLF), except `pickups_m3`, which carried dualcopy4's hand edit for FUN_00405890. That edit is now in the spec
too (`0x00405890 -> Pred405890`), with its note moved to `Gameplay/PickupTick.cpp`. The including TUs declare the
shared symbols above the `#include`.

Where a pair was two naked bodies, they were compared instruction by instruction after normalising syntax, constant
symbols and branch targets (by position). 0x00443300 AL_443300 vs AiSplineEval_x87: 112/112 identical.
0x004233e0 AL_4233e0 vs HeadingAtan2ToGameAngle: 44/44. 0x00408590 PP_408590 vs Dot408590: 39 vs 42, the only
difference is the callee binding (`call PP_426cc0` vs `mov eax, 0x426cc0 / call eax`).

Where the shared body is C, the compiled exe object (`dumpbin /disasm`) clobbers the same registers as the deleted
verbatim copy: CarSlotStateGet `eax, ecx`. AiVehicleFieldPtrGet `eax`. AiVehicleFloat4Get `eax` + `fld`.
WorldObjectsBaseGet `eax, ecx`. SubsystemARecordKey `eax`. The four RangeTable bodies clobber `eax, ecx, edx`, as the
originals do. RangeTableGroupOffset preserves `ecx` (push/pop), as FUN_00407db0 does.

## Per RVA

| RVA | Key(s) removed | Kept body (both targets) | Removed body | Exe-side note |
|---|---|---|---|---|
| 0x0040e470 | DUP-IN-TARGET, CROSS-TARGET | `Ai/CarSlotState.cpp:CarSlotStateGet` (moved unchanged from `Frontend/RaceResults.cpp`, .asi-only) | AL_40e470 (`Ai/AiLeafTargets_naked.inc`), TG_40e470 (`Ai/AiTargetClosest_naked.inc`) | no null guard, same as the deleted copies |
| 0x004233e0 | DUP-IN-TARGET | `Ai/HeadingAtan2.cpp:HeadingAtan2ToGameAngle` (already both) | AL_4233e0 | operands now read exact copies of the 6 .rdata constants (below) |
| 0x00408af0 | CROSS-TARGET | new `Ai/AiVehicleFieldGetters.cpp:AiVehicleFieldPtrGet` (moved unchanged from `Ai/PromoLoop_round1.cpp`) | AL_408af0 | |
| 0x00442cc0 | CROSS-TARGET | `Ai/AiVehicleFieldGetters.cpp:AiVehicleFloat4Get` (moved unchanged from `Ai/PromoLoop_round1.cpp`) | TG_442cc0 (`Ai/AiModeSeven_naked.inc`) | reads 0x005d757c, 0 in the exe = the copy's 0.0 |
| 0x00484c70 | CROSS-TARGET | new `Util/WorldObjectsBase.cpp:WorldObjectsBaseGet` (moved unchanged from `Util/PromoLoop_round20.cpp`) | TG_484c70 | |
| 0x00443300 | CROSS-TARGET | new `Ai/AiSplineEval.cpp:AiSplineEval_x87` (moved from `Ai/AiSplineHooks.cpp`, now `extern "C"`) | AL_443300; the never-called C `AiSplineEval` (1-4 ULP off) | `.asi` install and self-test stay in `AiSplineHooks.cpp` |
| 0x00408590 | CROSS-TARGET | new `Race/PathProgDot.cpp:Dot408590` | PP_408590 (`Race/PathProgStep_naked.inc`); `Util/PromoLoop_sessionB.cpp` copy moved | callee FUN_00426cc0 per target: original in the .asi, PP_426cc0 in the exe |
| 0x00407580 | CROSS-TARGET | `Gameplay/SubsystemARecordKey.cpp` (TU joins `exe_sources.rsp`) | PK_407580 (`Gameplay/PickupCollect_naked.inc`) | |
| 0x00407b00 | CROSS-TARGET | `Gameplay/RangeTable_ah1.cpp:RangeTableFindGroup` (TU joins `exe_sources.rsp`) | PP_407b00 | ShadowAB wrapper, unarmed path = impl |
| 0x00407b70 | CROSS-TARGET | `Gameplay/RangeTable_ah1.cpp:RangeTableMemberTest` | PP_407b70 | |
| 0x00407d70 | CROSS-TARGET | `Gameplay/RangeTable_ah1.cpp:RangeTableCountGroup` | PP_407d70 | |
| 0x00407db0 | CROSS-TARGET | `Gameplay/RangeTable_ah1.cpp:RangeTableGroupOffset` | PP_407db0 | |

Two exe-only defects the move had to fix, both because the standalone maps 0x00500000..0x009fffff blank:

- `Gameplay/RangeTable_ah1.cpp`'s `FUN_00426090` helper called the original VA 0x00426090. In the exe it now calls
  PP_426090 (the exe's verbatim `mov eax, 0x0066ce58 / ret`); the .asi still calls the original.
- `Ai/HeadingAtan2.cpp` read its constants at `ds:[0x005d757c]` etc., which read 0 in the exe (memory
  naked-asm-rdata-reads-zero-in-exe). It now reads `s_k_*` copies in both targets. Bits re-read from
  `original/MASHED.exe.unpatched` on 2026-11-01: 0x005d757c `00000000`, 0x005ccae0 `404ca5dcc0000000`, 0x005cd09c
  `43340000`, 0x005ccac4 `43b40000`, 0x005ccad0 `42b40000`, 0x005cd324 `43870000`. These are the same values
  AL_4233e0 read from `g_al_c_*`.

New TUs added to BOTH rsp lists: `Ai/AiVehicleFieldGetters.cpp`, `Ai/AiSplineEval.cpp`, `Race/PathProgDot.cpp`,
`Util/WorldObjectsBase.cpp`. Added to `exe_sources.rsp` only (already in the .asi list): `Gameplay/SubsystemARecordKey.cpp`,
`Gameplay/RangeTable_ah1.cpp`. That also compiles RangeTable_ah1's other four bodies into the exe (0x00409300,
0x00409330, 0x00407a00, 0x00407a20). The exe has no other body for them (lint) and no caller. `build_objs.ps1`,
`build.bat` and `x87_tus.txt` are unchanged. Every `.cpp` file stays: `Frontend/RaceResults.cpp` keeps no body now,
only its header note.

## Lint

| Tree | Findings | NEW |
|---|---|---|
| worktree, base `0e3d71a75` | 48 (CROSS 33, DUP 15) | 0 |
| worktree, branch | 35 (CROSS 22, DUP 13) | 0 |
| clean `git archive` of the branch, publish.sh file set (`mashedmod scripts re/tools/dual_copy_allowlist.txt`) | 34 | **1: DUP-IN-TARGET 0x00408a50** |
| the same archive of base `orch/integration` `0e3d71a75` | 47 | **1: the same 0x00408a50** |
| clean archive of the branch + `re/match` | 35 | 0 |

The 0x00408a50 finding exists on the base and is owned by another lead (HB-LINT-408A50). The publish.sh file set has
no `re/match`, so the scanner's match-include pass does not bind there. This branch neither adds nor removes it.

Clean-archive build (`git archive orch/D4-DC-INC-ai @ 669fccf0a` of `mashedmod scripts re/tools/dual_copy_allowlist.txt
re/match` into `D:/MashedWT/D4-DC-INC-ai_archive`, `MASHED_NO_DEPLOY=1 mashedmoduild.bat`): `=== Build OK ===`. The
build's own lint printed 35 findings, NEW=0. Log: `D:/MashedWT/D4-DC-INC-ai_data/archive_build.log`.

## Gates

The worktree exe is `mashed_re.exe` sha256 `7bad1dd1d6bf17d665d294cef3337a53e19627eecef79cf9099904889aeb2428`, built
with `MASHED_NO_DEPLOY=1`. Run: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, with private
root `D:/MashedWT/D4-DC-INC-ai_root` (`A4_ROOT`) and `A4_OUT=D:/MashedWT/D4-DC-INC-ai_data/arms`. The run was wrapped in
`mashed_lock.py run --label D4-DC-INC-ai` around the test slot. All four arms self-exited, exit 0. Refs come from
`re/orchestrator/merge_train.json`.

| Arm | Expected | Got | |
|---|---|---|---|
| H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | same |
| B_DEF_K0 / K1 / K26 step | 1d5bf4f9 x3 | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | same |

Raw data (not committed), sha256:
- `D:/MashedWT/D4-DC-INC-ai_data/arms/H_DEF.step.csv` `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198`
- `D:/MashedWT/D4-DC-INC-ai_data/arms/H_DEF.gates.csv` `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb`
- `D:/MashedWT/D4-DC-INC-ai_data/arms/H_DEF.ptrace.log` `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3`
- `D:/MashedWT/D4-DC-INC-ai_data/arms/B_DEF_K{0,1,26}.step.csv` `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457`
- run log `D:/MashedWT/D4-DC-INC-ai_data/gates_run1.log`

## Trackers (re-classify, no C-level move)

`hooks.csv`: `file` and `exe_file` of the 12 rows now name the one body's TU, and `notes` gets a D4-DC-INC-ai prefix.
0x00407b00's `file` named a `.md` before. `re/analysis/CHANGELOG.md`: 12 `Cn->Cn refile` lines. No U-/S- rows are
created or closed. Open uncertainties still attached: U-1300 (0x0040e470), U-7562 (0x00408af0), U-7601 (0x00442cc0),
all non-blocking. C3 evidence stays valid for these reasons:
- Ten of the bodies moved without a code change (only linkage changed for AiSplineEval_x87).
- HeadingAtan2ToGameAngle and Dot408590 changed only operand bindings. Inside MASHED.exe these resolve to the same
  bits and the same callee.
- They were NOT re-diffed in this step. To re-earn the evidence directly, run
  `re/frida/run_diff_parallel.py heading_atan2 dot_408590`, which loads the worktree's `.asi` through Frida, so no
  deploy is needed.
