# RESULT D4-DC-INC-PATHPROG-REMERGE

## Hand merge 2 (2026-11-02, merge commit c99523820)

**Verdict: GREEN, hand merge done, gates not measured.** Current `orch/integration` (`6a997bfa3`) merged into
`orch/D4-DC-INC-PATHPROG-REMERGE` (which held hand merge 1, `e8fcea199` + RESULT `700a97365`) with
`git -c merge.autoStash=false merge orch/integration`. Build OK (`MASHED_NO_DEPLOY=1`), archive lint NEW=0. The default
gates did not run: D3D9 CreateDevice fails with `0x8876086A` machine-wide (see Gates). The exe objects of every TU this
branch still changes compile to the same instructions and relocation targets as integration's.

### What changed on integration since hand merge 1

196 commits. MATCH-S06 (`ab947d820`) and the MATCH-TOPDOWN / MATCH-0040E370-NAKED merges did the same job as the pathprog
branch, in a different place: each of the six `0x00426xxx` getters (`00426090`, `00426bb0`, `00426cb0`, `00426cc0`,
`00426cf0`, `00426d00`) now has one byte-matched body in `Match/MatchS06_<rva>.cpp` (both rsp files), the naked
`PP_`/`AL_`/`TG_` copies are gone, and the 7 allowlist keys (6 DUP-IN-TARGET + CROSS-TARGET `00426bb0` / `00426cc0`)
are already removed. `pathprog_p2.json` also drops `PP_407cd0` (MATCH-TOPDOWN-T13, `Match/MatchS01_00407cd0.cpp`) and
`target_p3.json` drops `TG_40e370` (bound to `IsCarSlotActive`, `Match/MatchS02_0040e370.cpp`).

### Conflicts (12) and resolution

| File | Resolution |
|---|---|
| `re/tools/naked_specs/pathprog_p2.json`, `target_p3.json` | integration side (a superset: same bindings as this branch, plus `PP_407cd0` / `TG_40e370` removed) |
| `re/tools/naked_specs/aileaf_g1.json` (auto-merged) | this branch's side kept. Integration hand-edited `AiLeafTargets_naked.inc` to call `Global66d6d8Get` but left `AL_426bb0` in the spec, so the spec and the .inc disagreed. With this side the spec generates the .inc integration already has |
| `Race/PathProgStep.cpp`, `Race/PathProgDot.cpp`, `Gameplay/RangeTable_ah1.cpp`, `Util/PromoLoop_round31.cpp`, `Vehicle/PromoLoop_round2.cpp` | integration side (bodies and comments point at the MATCH-S06 files) |
| `Camera/CameraPathCount.cpp`, `Vehicle/VehicleTable4c.cpp` (added by this branch, no conflict) | deleted, with their two lines in each rsp: each defined `Global66d6d8Get` / `VehicleTable4cPtrGet` a second time next to `Match/MatchS06_00426bb0.cpp` / `_00426cc0.cpp` (duplicate symbol at link) |
| `Ai/AiLeafTargets.cpp`, `Ai/AiTargetClosest.cpp` (this branch's extern pointing at CameraPathCount.cpp) | integration side, plus one `extern "C" void Global66d6d8Get();` before the `#include` of the regenerated .inc |
| `Race/PathProgStep.cpp` | plus six `extern "C" void <sym>();` prototypes before the `#include` (`GlobalDat0066ce58Get`, `Global66d6d8Get`, `SlotIndexToPtr`, `VehicleTable4cPtrGet`, `GetDat0066d6e4`, `FrontendArraySlotGet`) |
| `Ai/AiLeafTargets_naked.inc`, `Ai/AiTargetClosest_naked.inc`, `Race/PathProgStep_naked.inc` | regenerated with `re/tools/gen_naked_asm.py` from the merged specs (run from the main checkout root: the script reads `original/MASHED.exe.unpatched` relative to cwd, and this worktree's `original\` holds no binaries). Against integration's hand-edited copies the instruction lines are identical. What differs: comment column spacing, plus the `extern "C" void X();` lines and `MATCH-S06:` comments integration added by hand. The generator emits no prototypes, so the including TUs now declare them (rows above) |
| `re/tools/dual_copy_allowlist.txt` | integration side (the 7 keys are already gone there) |
| `hooks.csv` (6 rows: `00426090`, `00426bb0`, `00426cb0`, `00426cc0`, `00426cf0`, `00426d00`) | integration side. Integration's rows are C4 (route match, MATCH-RC-S06S07) with `file` = `Match/MatchS06_<rva>.cpp`. This branch's rows were C3 "refile" notes naming the files this merge deletes. No promotion or demotion here, the landed state is kept |
| `re/analysis/CHANGELOG.md` (auto-merged) | this branch's 6 "refile: ONE body, <file>" lines dropped. They never landed on integration and name bodies (`Camera/CameraPathCount.cpp`, `Vehicle/VehicleTable4c.cpp`, `Frontend/...`) that are not the one body after this merge |

Net tree difference against `6a997bfa3`: `aileaf_g1.json`, the three regenerated .inc files, the prototypes in the
three TUs, and this branch's two RESULT files.

### Build, lint, object comparison

- `cmd /c "set MASHED_NO_DEPLOY=1&& mashedmod\build.bat"`: `=== Build OK ===`, 0 errors, `.asi` not deployed.
  exe `653267ccb6c9959779d20130be153fbf44049496584d637bc612bb7ce7021490`, asi `59c78b816e4496487d9c97d02d440c63b22702d41901afdc6100da7941699c7b`.
- `scripts/lan_release/archive_lint.py`: `--ref 6a997bfa3` and `--ref c99523820` both give include-closure 0 unresolved,
  rva-lint 2 findings (CROSS-TARGET 1, DUP-IN-TARGET 1), allowlisted 2, **NEW=0**, asm-lint OK on 1133 files.
  The burn-down already landed with MATCH-S06, so this merge removes no key.
- Object comparison: integration's six source files checked out over this worktree, the three TUs compiled with the
  exe flags (`/EHa /W3 /O2 /DMASHED_STANDALONE`), then the worktree restored (`git checkout HEAD`). `dumpbin /disasm
  /relocations` compared with this build's `build/obj/exe/*.obj`, the relocation symbol-table index column normalized
  (it shifts because the number of declared symbols changes). The only remaining difference per TU is the `.debug$S`
  section size (comment and line info), 4 diff lines each for PathProgStep, AiLeafTargets and AiTargetClosest. Code and
  relocation targets are identical. The three TUs are exe-only (0 lines in `asi_sources.rsp`), so the .asi is unaffected.

### Gates

Not measured. `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` (worktree exe `653267cc`,
`A4_ROOT=D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_root`, under `test-slot` + `mashed_lock.py run --solo`) ran after the
merge train's priority claim cleared. All four arms ended in 1 s with exit 3, and `mashed_re.log` shows 8 of 8
`InitD3D9: CreateDevice(...) -> fail hr=0x8876086A` (D3DERR_NOTAVAILABLE, `re/diag/KNOWN_ISSUES.md` D3D9-NOTAVAIL: no
presentable desktop surface). The same signature is in every MASHED root on this machine since about 09:20 local
(merge train `mtrain_root`, MATCH-PORTSTATE-RECORDS, HB-GATE-FLAKE-GAMEFLOW, D4-DC-77b0-RUNTIME-PARKED). L0 note: a sleeping
display is not a finding and not a gate change. `scripts/diag.py probe-render` returned UNKNOWN (it reads
`mashed_re.log` from the worktree root, where the exe does not write). A direct 12 s probe of the same exe on the private
root, under `mashed_lock.py run`, failed the same way (exit 3, 8 fail lines). Expected on a working display
(`merge_train.json`): G-CTRL `8284be46/ba407a16/31fe043e`, B `1d5bf4f9` x3. The object comparison above is the static
evidence that this branch's exe code equals integration's for every TU the merge touches.

### Raw files (`D:/MashedWT/D4-DC-INC-PATHPROG-REMERGE_data/m2/`)

- `build.log` `359f4959182355b52bec72711494329e859b58f0efdf80a26d3aeb75df56d4db`
- `archive_lint.log` `f3083f23ffe0569de7c512fd652ae4807541528ae7e7a0556bcd5ab937dc6808`
- `gates.log` `e6544395457d6767a21c0e0dd3d95cff0b24c9057ae8e2c554422690d3b9ec1c`
- `arms/RUNLOG.txt` `aed5587e71c82f056bf2c563c5e87e6e08243d9d4485ed84a7de8069e27caee8`
- `arms/H_DEF.log` `780ccd1d97ecb2e8f1e863bbe50717b61882ecb7eb79ca48351e846883ece03a` (the three B arm logs are the same size, 4165 B)
- `probe_mashed_re.log` `780ccd1d97ecb2e8f1e863bbe50717b61882ecb7eb79ca48351e846883ece03a`
- `objcmp/summary.txt` `da749573fe32ed73dd4f5373193cf5ea1acd52be4c029b8dc50949eaefe5f6a6`
- `objcmp/int_exe_PathProgStep.dis` `280b7727c4e6188045b90984d51b55877be2ac66e8a349a8887a71eccdb12466`, `br_exe_PathProgStep.dis` `63a13f78fdbbad34908d08b16cad2aa6d1c744f974374768c4093a6480d7d3c5`
- `objcmp/int_exe_AiLeafTargets.dis` `5d758e7688b999b8f5285279fa67bddd58611e46490e56236d22fb1040a7dae7`, `br_exe_AiLeafTargets.dis` `65e6886d290c3206d5e3a15d2b97ba829f998b8df8dd91d6ff34966ab3fea904`
- `objcmp/int_exe_AiTargetClosest.dis` `86bb7fecc7371794efcaa2c64137dd0a8e01f6e9b3d1ce8163b5920ec735b3c2`, `br_exe_AiTargetClosest.dis` `4d9ec0f0bc0415b0ec94f0db930acb8c36ea533fd8270cdd455b79a12daf7087`

### For the lead

The 7 burn-down rows' work is already on integration through MATCH-S06. What this branch still adds is spec/.inc
consistency only (`aileaf_g1.json` and three regenerated .inc files). Re-run the two default gates once the display
wakes, or leave that to the merge train.

## Hand merge 1 (2026-11-01, merge commit e8fcea199, red-merged by train run 20261101_233753)

**Verdict: GREEN.** `orch/D4-DC-INC-pathprog` (`6322ca50d`) hand-merged onto `orch/integration` (`6d9d01a7f`,
after D4-DC-INC-batch3 and MATCH-S07). Specs combined, the three `.inc` files regenerated with
`re/tools/gen_naked_asm.py`, rva-lint 25 to 18 findings with NEW=0, and both default gates byte-identical to the `merge_train.json` refs.

Merge commit: `e8fcea199` on `orch/D4-DC-INC-PATHPROG-REMERGE`. The 7 burn-down rows (D4-DC-INC-00426090 /
-00426bb0 / -00426bb0-X / -00426cb0 / -00426cc0 / -00426cf0 / -00426d00) were not touched, claimed or enqueued here.
Their evidence stays `verify/d4dcinc_pathprog_20261101/RESULT_D4-DC-INC-pathprog.md` (merged in with this branch).

### Conflicts and how each was resolved

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

### Regeneration

`py -3.12 <wt>/re/tools/gen_naked_asm.py <wt>/re/tools/naked_specs/<spec>.json <wt>/mashedmod/src/mashed_re/<inc>`, run
from the main checkout (the generator opens `original/MASHED.exe.unpatched` relative to cwd), for `aileaf_g1` ->
`Ai/AiLeafTargets_naked.inc`, `pathprog_p2` -> `Race/PathProgStep_naked.inc`, `target_p3` -> `Ai/AiTargetClosest_naked.inc`.

`git diff orch/integration HEAD -- '*_naked.inc'` holds only the pathprog delta: the forward declarations and bodies of
`AL_426bb0`, `TG_426bb0`, `PP_426cb0/cc0/d00/bb0/090/cf0` removed, and each `call` to them rewritten to the shared
symbol (27 call sites: 5 x `Global66d6d8Get` (2 AiLeaf, 1 Target, 2 PathProg), 7 x `SlotIndexToPtr`,
4 x `VehicleTable4cPtrGet`, 8 x `FrontendArraySlotGet`, 2 x `GlobalDat0066ce58Get`, 1 x `GetDat0066d6e4`). No line
naming a `RwLut*_cd` symbol differs, so integration's LUT bindings are unchanged. The diff of the whole branch against integration under `mashedmod/` is the same 14 files, +120/-144, that
the pathprog branch had against its own base.

### Build and lint

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

### Gates

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

### Next

The lead re-enqueues the 7 rows (D4-DC-INC-00426090 / -00426bb0 / -00426bb0-X / -00426cb0 / -00426cc0 / -00426cf0 /
-00426d00) from this branch, SOURCE -Full. Nothing was enqueued or merged into `orch/integration` here. If integration
moves again before the train takes it, the only conflict-prone files are the three specs, the allowlist and the
regenerated `.inc` files: resolve the specs and allowlist, then regenerate the `.inc` files with `gen_naked_asm.py`.
