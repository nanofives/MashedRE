# RESULT D4-DC-INC-pathprog: one body each for 6 RVAs of the pathprog leaf family (7 allowlist keys)

**Verdict: GREEN.** Both default gates are byte-identical to the `merge_train.json` refs. All 7 keys are gone from
`re/tools/dual_copy_allowlist.txt` (35 findings to 28, NEW=0). Every RVA has exactly one body, in a TU compiled into
both targets.

**Stacking base: `orch/D4-DC-INC-ai` @ `02dda4b80`, not `orch/integration`** (lead's instruction: the ai batch already
edited `pathprog_p2.json`, the regenerated `.inc` files, both rsp lists and the allowlist). Merge this branch after
`orch/D4-DC-INC-ai`. Branch `orch/D4-DC-INC-pathprog`. Rows: D4-DC-INC-00426090, -00426bb0, -00426bb0-X, -00426cb0,
-00426cf0, -00426d00, -00426cc0. All 7 were still flagged on the base: the ai batch did not touch these RVAs.

**Run 29 (recovery after the 14:23 reboot): the ai batch has merged, and this branch now carries `orch/integration`
@ `c82901f6a`.** See "Run 29 re-merge" at the end for the rebuild, lint and gate re-run on the merged tree. The base
note above is history.

## Method

The same method as D4-DC-INC-ai. The exe-only copies were private verbatim naked copies that `re/tools/gen_naked_asm.py`
generates from the anchored bytes. Before the edit, `pathprog_p2.json`, `aileaf_g1.json` and `target_p3.json` each
reproduced their committed `.inc` byte for byte (modulo CRLF). Each spec now drops the copy's func and maps the `call`
to the shared C body, and the three `.inc` files are regenerated. The including TUs declare the shared symbols above the
`#include`. (The generator opens `original/MASHED.exe.unpatched` relative to cwd, and the worktree has no `original/`,
so it was run from the main checkout with the worktree's spec and output paths.)

Each deleted copy is the anchored bytes verbatim. Each kept C body was compared against it from `dumpbin /disasm` of
the exe object:

| RVA | Original (verbatim copy) | Kept body, exe object | Clobbers |
|---|---|---|---|
| 0x00426090 | `mov eax, 0x66ce58 / ret` | `B8 58 CE 66 00 / C3`, identical | eax / eax |
| 0x00426bb0 | `mov eax, [0x0066d6d8] / ret` | `A1 D8 D6 66 00 / C3`, identical | eax / eax |
| 0x00426cb0 | `mov eax,[esp+4] / imul eax,eax,0x4c / add eax,0x663664 / ret` | `imul eax,[esp+4],0x4c / add eax,0x663664 / ret` | eax / eax |
| 0x00426cc0 | `mov eax,[esp+4] / imul eax,eax,0x4c / add eax,0x663658 / ret` | `imul eax,[esp+4],0x4c / add eax,0x663658 / ret` | eax / eax |
| 0x00426cf0 | `mov eax, 0x66d6e4 / ret` | `B8 E4 D6 66 00 / C3`, identical | eax / eax |
| 0x00426d00 | `ecx=a1*0x4c; edx=a2*3; eax=ecx+edx*4+0x663670` | `ecx=a1*0x4c; eax=a2*3; eax=ecx+(eax+0x198d9c)*4` (0x198d9c*4 = 0x663670) | eax,ecx,edx / eax,ecx |

Same return value for every input. Each kept body clobbers a subset of what the original clobbers. 0x00426d00's original
also leaves `edx = a2*3`, which the C body does not. Every call site of the six symbols in the three regenerated `.inc`
files was read: none reads `ecx` or `edx` before writing it after the call (the callers are MSVC cdecl code). The
addresses returned (0x00663658..0x0066d6e4) are `.bss` globals, which the standalone maps at the same VAs. None of the
bodies reads `.rdata`.

## Per RVA

| RVA | Key(s) removed | Kept body (both targets) | Removed body | Exe-side note |
|---|---|---|---|---|
| 0x00426090 | DUP-IN-TARGET | `Frontend/GlobalGetters_s4.cpp:GlobalDat0066ce58Get` (already both, unchanged) | PP_426090 (`Race/PathProgStep_naked.inc`) | `Gameplay/RangeTable_ah1.cpp`'s exe binding called `&PP_426090`, now calls `GlobalDat0066ce58Get()`. The .asi binding (original VA) is unchanged |
| 0x00426bb0 | DUP-IN-TARGET, CROSS-TARGET | new `Camera/CameraPathCount.cpp:Global66d6d8Get` (moved unchanged from `Util/PromoLoop_round31.cpp`, .asi-only) | PP_426bb0, AL_426bb0 (`Ai/AiLeafTargets_naked.inc`), TG_426bb0 (`Ai/AiTargetClosest_naked.inc`) | three specs map the call |
| 0x00426cb0 | DUP-IN-TARGET | `Frontend/MenuNearLeaves_s6.cpp:SlotIndexToPtr` (already both, unchanged) | PP_426cb0 | |
| 0x00426cc0 | CROSS-TARGET | new `Vehicle/VehicleTable4c.cpp:VehicleTable4cPtrGet` (moved unchanged from `Vehicle/PromoLoop_round2.cpp`, .asi-only) | PP_426cc0 | `Race/PathProgDot.cpp`'s exe `s_fn_426cc0` pointed at `&PP_426cc0`, now `&VehicleTable4cPtrGet`. The .asi binding (original VA) is unchanged |
| 0x00426cf0 | DUP-IN-TARGET | `Frontend/BatchAA_s1.cpp:GetDat0066d6e4` (already both, unchanged) | PP_426cf0 | |
| 0x00426d00 | DUP-IN-TARGET | `Frontend/BatchAA_s6.cpp:FrontendArraySlotGet` (already both, unchanged) | PP_426d00 | clobber subset, see above |

New TUs added to BOTH rsp lists: `Camera/CameraPathCount.cpp`, `Vehicle/VehicleTable4c.cpp`. `build_objs.ps1`,
`build.bat` and `x87_tus.txt` are unchanged. Every `.cpp` file stays: `PromoLoop_round31.cpp` and `PromoLoop_round2.cpp`
keep their other bodies, with a `[MOVED]` line in the header.

**No .asi body changed.** The two moved bodies are the same source in a new TU. The .asi bindings in
`RangeTable_ah1.cpp` and `PathProgDot.cpp` are untouched (`#else` branches), so no `.asi` operand binding moved and no
`run_diff_parallel.py` re-diff was needed. Only exe-side bindings changed.

## Lint

| Tree | Findings | NEW |
|---|---|---|
| worktree, base `02dda4b80` | 35 (CROSS 22, DUP 13) | 0 |
| worktree, branch | 28 (CROSS 20, DUP 8) | 0 |
| clean `git archive` of the branch, publish.sh file set (`mashedmod scripts re/tools/dual_copy_allowlist.txt`) | 27 | **1: DUP-IN-TARGET 0x00408a50** |
| clean `git archive` of the branch + `re/match` | 28 | 0 |

The 0x00408a50 finding exists on the base. It appeared the same way in D4-DC-INC-ai's archive check (34 there). It is
owned by another lead (HB-LINT-408A50), and this batch was told not to touch it. The publish.sh file set has no
`re/match`, so the scanner's match-include pass does not bind there. This branch neither adds nor removes it.

Clean-archive build (`git archive orch/D4-DC-INC-pathprog @ b75e62c8b` of `mashedmod scripts
re/tools/dual_copy_allowlist.txt re/match` into `D:/MashedWT/D4-DC-INC-pathprog_archive_match`,
`MASHED_NO_DEPLOY=1 mashedmod\build.bat`): `=== Build OK ===`. The build's own lint printed 28 findings, NEW=0. Log:
`D:/MashedWT/D4-DC-INC-pathprog_data/archive_build.log` sha256
`b96532b6f8ee773de95eade4a04467f2cb6bb29258b4d1489bae46a77c7b0101`. That build's exe hashes differently
(`706824be...`) from the worktree exe, which is the build the gates ran.

## Gates

The worktree exe is `mashed_re.exe` sha256 `76faf3f2f5e5d4fb57f03f24dbd656d0c084e82fc86480ceafc802388371ef0e`, built with
`MASHED_NO_DEPLOY=1` (the .asi, not deployed, `fbf8140a...`). Run: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0
B_DEF_K1 B_DEF_K26`, with private root `D:/MashedWT/D4-DC-INC-pathprog_root` (`A4_ROOT`, robocopy of `original\`) and
`A4_OUT=D:/MashedWT/D4-DC-INC-pathprog_data/arms`, as `mashed_lock.py run --label D4-DC-INC-pathprog -- bash -c
'test-slot py -3.12 ...run_arms.py ...'`. All four arms self-exited, exit 0. Refs come from `re/orchestrator/merge_train.json`.

| Arm | Expected | Got | |
|---|---|---|---|
| H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | same |
| B_DEF_K0 / K1 / K26 step | 1d5bf4f9 x3 | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | same |

Raw data (not committed), sha256:
- `D:/MashedWT/D4-DC-INC-pathprog_data/arms/H_DEF.step.csv` `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198`
- `D:/MashedWT/D4-DC-INC-pathprog_data/arms/H_DEF.gates.csv` `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb`
- `D:/MashedWT/D4-DC-INC-pathprog_data/arms/H_DEF.ptrace.log` `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3`
- `D:/MashedWT/D4-DC-INC-pathprog_data/arms/B_DEF_K{0,1,26}.step.csv` `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457`
- run log `D:/MashedWT/D4-DC-INC-pathprog_data/gates_run2.log`. The first attempt (`gates_run1.log`) never booted. The lock
  cannot spawn the `test-slot` sh script directly (`FileNotFoundError`), so run 2 wraps it in `bash -c`.
- worktree build log `D:/MashedWT/D4-DC-INC-pathprog_data/build.log` sha256 `b969e31c4811269a6e97da9fea69a6f71e958b6ed5c351a4cb5a20d52f2f639e`

## Trackers (re-classify, no C-level move)

`hooks.csv`: `file` and `exe_file` of the 6 rows now name the one body's TU, and `notes` gets a D4-DC-INC-pathprog
prefix. 0x00426bb0 (`CameraPath::GetCount`, C3) and 0x00426cc0 (`VehicleTable4cPtrGet`, C4) had an empty `exe_file`
before. `re/analysis/CHANGELOG.md`: 6 `Cn->Cn refile` lines. No U-/S- rows are created or closed. Open uncertainties
still attached, all pre-existing: U-8961 and U-5771 (0x00426cf0, meaning of DAT_0066d6e4), U-8962 and U-5772
(0x00426d00, element type at 0x00663670).

The C3/C4 evidence stays valid. The two moved bodies are the same source in a new TU, compiled with the same flags. The
four unmoved bodies are not edited. The .asi install of all six is unchanged.

## Run 29 re-merge (2026-11-01, after the LIVING-PC reboot)

The run-28 worker died in the 14:23 reboot. The worktree was clean, and every commit of this batch (`b75e62c8b`,
`830a988f9`, `46c880a89`) was intact. `orch/D4-DC-INC-ai` merged GREEN into `orch/integration` (`6e38f3483`), and
`git -c merge.autoStash=false merge orch/integration` (`c82901f6a`) was a clean merge. It added only one
`INTEGRATION_LOG.md` line, because the branch already carried the ai tip `b034f3bbe`. The earlier merges of the ai tip
(`21980efd1`, `a4aff1fa2`, `f63424629`) brought in integration build inputs the run-28 gates never saw (HB-LINT-408A50,
MATCH-S01-RC, D5-INTROMOVIES-AUDIO, HB-ARCTIC-NOLIGHT-CAP, MATCH-TOPDOWN-MAP), so the merged tree was rebuilt and gated
again.

**Build** (worktree, `MASHED_NO_DEPLOY=1 mashedmod\build.bat`): `=== Build OK ===`. `mashed_re.exe` sha256
`ede37d6e1ae7a1ccbd0380f24688f494b82c30ceac351434352dd20c2b59c890`. Log `D:/MashedWT/D4-DC-INC-pathprog_data/build_r29.log`
sha256 `dc569a73da8aabcece73ad95ddb9f1d9bcf557751503161bccb19a95d957cbe3`.

**Lint**

| Tree | Findings | NEW |
|---|---|---|
| worktree build (build.bat's own lint) | 27 (CROSS 20, DUP 7) | 0 |
| clean `git archive HEAD` of `scripts/lan_release/archive_paths.txt` (`mashedmod scripts re/tools/dual_copy_allowlist.txt re/match`) | 27 | 0 |

28 to 27 against run 28: the 0x00408a50 key is gone on integration (HB-LINT-408A50). None of the 7 keys of this batch
is in the allowlist (`grep 0042(6090|6bb0|6cb0|6cc0|6cf0|6d00)` = 0 lines).

**Gates** (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`, `A4_ROOT=D:/MashedWT/D4-DC-INC-pathprog_root`,
`A4_OUT=D:/MashedWT/D4-DC-INC-pathprog_data/arms_r29`, as `test-slot py -3.12 mashed_lock.py run --solo --label
D4-DC-INC-pathprog -- py -3.12 run_arms.py ...`). All four arms self-exited, exit 0. Refs from `re/orchestrator/merge_train.json`.

| Arm | Expected | Got | |
|---|---|---|---|
| H_DEF step / gates / ptrace | 8284be46 / ba407a16 / 31fe043e | 8284be46 / ba407a16 / 31fe043e | same |
| B_DEF_K0 / K1 / K26 step | 1d5bf4f9 x3 | 1d5bf4f9 / 1d5bf4f9 / 1d5bf4f9 | same |

Raw data (not committed), sha256:
- `D:/MashedWT/D4-DC-INC-pathprog_data/arms_r29/H_DEF.step.csv` `8284be46b3c692f3ba129f252367b5f5097eef9e39bc49a1be6514e4d33b6198`
- `D:/MashedWT/D4-DC-INC-pathprog_data/arms_r29/H_DEF.gates.csv` `ba407a169a8ec433f486492814e7e450e32c6c48be5f1a0e38e9c53074db1ceb`
- `D:/MashedWT/D4-DC-INC-pathprog_data/arms_r29/H_DEF.ptrace.log` `31fe043e261d2df943fa3dd5688b74da4e8329eb00ea87f090539bb7e854c5e3`
- `D:/MashedWT/D4-DC-INC-pathprog_data/arms_r29/B_DEF_K{0,1,26}.step.csv` `1d5bf4f980bf7eb48ac98211f4fb50ff58a04d1ad408d18695d79ddd8fcf8457`
- run log `D:/MashedWT/D4-DC-INC-pathprog_data/gates_r29.log` `5d7ce7c58250d497509701ef7b63d972e0c74a2d9415c481da3ff609d6592898`

**Overlap with the queued `orch/D4-DC-INC-batch3` @ `65ecb9496` (read-only diff against integration, not merged here).**
The two batches remove disjoint RVAs. batch3 drops 0x004c39b0 / 0x004c3ac0 / 0x004c3b30 / 0x004c3b90 / 0x00547450 and
the pickup / registry copies. This batch drops 0x00426090 / bb0 / cb0 / cc0 / cf0 / d00. No body is edited twice. Both
edit `pathprog_p2.json`, `aileaf_g1.json` and `target_p3.json` on adjacent lines (the `funcs` list and the `calls` map),
the regenerated `PathProgStep_naked.inc` / `AiLeafTargets_naked.inc` / `AiTargetClosest_naked.inc`, both rsp lists,
`hooks.csv`, `CHANGELOG.md` and the allowlist. Whichever merges second gets textual conflicts there. Resolution: take the
union of both spec edits (drop both sets of funcs, map both sets of calls, drop batch3's LUT `patch` entries) and
regenerate the three `.inc` files from the merged specs with `re/tools/gen_naked_asm.py`. Do not hand-merge the `.inc`.

**Second merge, run 29:** integration moved to `a147e22d3` (D4-DC-77b0-STATE5-ORIGREAD) while the gates ran, and was
merged in cleanly. It changes `re/frida/scenario_launch.py`, `INTEGRATION_LOG.md` and files under `verify/` only. No
`mashedmod/`, `re/tools/dual_copy_allowlist.txt` or `re/match` input changed, so the build, lint and gates above stand
for this tip. batch3 was still unmerged at this point.
