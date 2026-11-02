# RESULT D4-DC-INC-PATHPROG-REMERGE

**Verdict: GREEN.** `orch/D4-DC-INC-pathprog` (`6322ca50d`) hand-merged onto `orch/integration` (`6d9d01a7f`,
after D4-DC-INC-batch3 and MATCH-S07). Specs combined, the three `.inc` files regenerated with
`re/tools/gen_naked_asm.py`, rva-lint 25 to 18 findings with NEW=0, and both default gates byte-identical to the `merge_train.json` refs.

Merge commit: `e8fcea199` on `orch/D4-DC-INC-PATHPROG-REMERGE`. The 7 burn-down rows (D4-DC-INC-00426090 /
-00426bb0 / -00426bb0-X / -00426cb0 / -00426cc0 / -00426cf0 / -00426d00) were not touched, claimed or enqueued here.
Their evidence stays `verify/d4dcinc_pathprog_20261101/RESULT_D4-DC-INC-pathprog.md` (merged in with this branch).

## Conflicts and how each was resolved

Merge base `a147e22d3`. Integration since then (D4-DC-INC-batch3 among others) replaced the private LUT leaf copies
`*_4c39b0` / `*_4c3ac0` / `*_4c3b30` / `*_4c3b90` (and `PP_547450`) with the shared `RwLut*_cd` bodies and dropped
their `patch` rows. The pathprog branch replaced the six private `0x00426xxx` getter copies with the shared C bodies.
The two edits touch different entries, so the combination is the union.

| File | Integration side | Pathprog side | Result |
|---|---|---|---|
| `re/tools/naked_specs/aileaf_g1.json` | drops `AL_4c39b0`, `AL_4c3ac0` + their 4 patch rows, calls -> `RwLutNormalize_cd` / `RwLutLength_cd` | drops `AL_426bb0`, call -> `Global66d6d8Get` | both |
| `re/tools/naked_specs/target_p3.json` | drops `TG_4c3ac0`, `TG_4c3b30` + 4 patch rows, calls -> `RwLutLength_cd` / `RwLutSqrt_cd` | drops `TG_426bb0`, call -> `Global66d6d8Get` | both |
| `re/tools/naked_specs/pathprog_p2.json` | drops `PP_4c39b0`, `PP_4c3b90`, `PP_547450` + 4 patch rows, calls -> `RwLutNormalize_cd` / `RwLutInvSqrt_cd` | drops `PP_426cb0/cc0/d00/bb0/090/cf0`, calls -> `SlotIndexToPtr` / `VehicleTable4cPtrGet` / `FrontendArraySlotGet` / `Global66d6d8Get` / `GlobalDat0066ce58Get` / `GetDat0066d6e4` | both. Funcs left: `PP_408610`, `PP_407e20`, `PP_407be0`, `PP_407cd0` |
| `re/tools/dual_copy_allowlist.txt` | drops 10 keys (004c39b0 / 004c3ac0 / 004c3b30 / 004c3b90 / 00547450 / 004576b0 / 0045c330 / 00484a50 / 00484c90 and the old 0046d4a0 text), adds batch3's compared 0046d4a0 key | drops the 7 keys 00426090, 00426bb0 (DUP + CROSS), 00426cb0, 00426cc0 (CROSS), 00426cf0, 00426d00 | integration's block minus the 7 keys. `grep -E '0042(6090\|6bb0\|6cb0\|6cc0\|6cf0\|6d00)'` = 0 lines |
| `Ai/AiLeafTargets_naked.inc`, `Race/PathProgStep_naked.inc` (conflicted), `Ai/AiTargetClosest_naked.inc` (auto-merged) | | | regenerated from the merged specs, not hand-edited (below) |

Every other file auto-merged (both rsp lists gained `Camera/CameraPathCount.cpp` and `Vehicle/VehicleTable4c.cpp`,
`PathProgStep.cpp`, `PromoLoop_round31.cpp`, `PromoLoop_round2.cpp`, `CHANGELOG.md`).

## Regeneration

`py -3.12 <wt>/re/tools/gen_naked_asm.py <wt>/re/tools/naked_specs/<spec>.json <wt>/mashedmod/src/mashed_re/<inc>`, run
from the main checkout (the generator opens `original/MASHED.exe.unpatched` relative to cwd), for `aileaf_g1` ->
`Ai/AiLeafTargets_naked.inc`, `pathprog_p2` -> `Race/PathProgStep_naked.inc`, `target_p3` -> `Ai/AiTargetClosest_naked.inc`.

`git diff orch/integration HEAD -- '*_naked.inc'` holds only the pathprog delta: the forward declarations and bodies of
`AL_426bb0`, `TG_426bb0`, `PP_426cb0/cc0/d00/bb0/090/cf0` removed, and each `call` to them rewritten to the shared
symbol (27 call sites: 5 x `Global66d6d8Get` (2 AiLeaf, 1 Target, 2 PathProg), 7 x `SlotIndexToPtr`,
4 x `VehicleTable4cPtrGet`, 8 x `FrontendArraySlotGet`, 2 x `GlobalDat0066ce58Get`, 1 x `GetDat0066d6e4`). No line
naming a `RwLut*_cd` symbol differs, so integration's LUT bindings are unchanged. The diff of the whole branch against integration under `mashedmod/` is the same 14 files, +120/-144, that
the pathprog branch had against its own base.

## Build and lint

| Tree | rva-lint findings | NEW |
|---|---|---|
| `orch/integration` `6d9d01a7f`, clean archive (`scripts/lan_release/archive_lint.py --ref 6d9d01a7f`) | 25 (CROSS 18, DUP 7) | 0 |
| this branch, worktree build (`MASHED_NO_DEPLOY=1 mashedmod\build.bat`, `=== Build OK ===`) | 18 (CROSS 16, DUP 2) | **0** |
| this branch, clean archive (`archive_lint.py --ref e8fcea199`): include-closure 0 unresolved, rva-lint, asm-lint | 18 | **0**, OK |

25 to 18 = the 7 keys of the pathprog batch. asm-lint OK on 973 source files.

Raw files (not committed):
- `D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/build.log` `1fbcf65a223e8000caef57bc2a05275d912d70e58bf889b763b15d0166bd5cf5`
- `D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/archive_lint.log` `4e756a9778997f15192d403e5afcc66e3c091f77a4f8c1c17215331c8b74418e`
- worktree `mashedmod/build/mashed_re.exe` `0027dd66479a6abc1c9cb92b3dbe33bebfaa315e4df8994168eec2b092fa4d70`
- worktree `mashedmod/build/mashed_re_dev.asi` `f5484af170413d10f03f3919eefe00988cee9d22833436791cac033ea9ffd58f` (not deployed)

## Gates

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the worktree exe (`0027dd66`),
`A4_ROOT=D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_root` (robocopy of `original\`), `A4_OUT=D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/arms`,
as `test-slot ... mashed_lock.py run --solo --label D4-DC-INC-PATHPROG-REMERGE -- py -3.12 run_arms.py ...`, opened only
after `mashed_lock.py status` showed no merge-train entry (checked again inside the test slot, right before the lock call).
All four arms self-exited, exit 0. Refs from `re/orchestrator/merge_train.json` (`gctrl.ref`, `b.ref`).

| Arm | Expected | Got | |
|---|---|---|---|
| H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | same |
| B_DEF_K0 | 1d5bf4f9 | 1d5bf4f9 | same |
| B_DEF_K1 | 1d5bf4f9 | 1d5bf4f9 | same |
| B_DEF_K26 | 1d5bf4f9 | 1d5bf4f9 | same |

Raw files (not committed):
- `D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/arms/H_DEF.step.csv` `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198`
- `D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/arms/H_DEF.gates.csv` `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb`
- `D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/arms/H_DEF.ptrace.log` `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3`
- `D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/arms/B_DEF_K{0,1,26}.step.csv` `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457` (all three)
- run log `D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/gates.log` `d060020b4e79b4617a2a0cd25c93195530e4382682028de2e572ec439b4ee129`

The first attempt (`gates_run1_nooutdir.log`, `bf2b467a6d4b4013702e838fef940ce0f05a31115be28d2ca5f639f7bf1f19af`) ran H_DEF
with `A4_OUT` pointing at a folder that did not exist yet, so the exe could not write the step/gates CSVs and
`run_arms.py` stopped on the log copy. No hash came out of it. The second attempt waited for the two merge trains
(`20261101_191148`, `20261101_205241`) to leave the lock before opening its window.

## Next

The lead re-enqueues the 7 rows (D4-DC-INC-00426090 / -00426bb0 / -00426bb0-X / -00426cb0 / -00426cc0 / -00426cf0 /
-00426d00) from this branch, SOURCE -Full. Nothing was enqueued or merged into `orch/integration` here. If integration
moves again before the train takes it, the only conflict-prone files are the three specs, the allowlist and the
regenerated `.inc` files: resolve the specs and allowlist, then regenerate the `.inc` files with `gen_naked_asm.py`.
