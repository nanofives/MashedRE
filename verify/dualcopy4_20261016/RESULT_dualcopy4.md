# RESULT dualcopy4 (2026-10-16)

Branch `work/dualcopy4`, base `race/first-frame-parity` `9b0248128` (dualcopy3's merge), model claude-sonnet-5-5. Pre-registrations, each committed before the first run it covers:
`PREREG_dualcopy4.md` (group a, `44870a431`), `PREREG_dualcopy4_b.md` (block 1, with the code, `19e95b2f2`), `_b2` (0x0042fa00 and the controls, `968b8778b`), `_b3` (camera, `efd983025`),
`_b4` (0x0042d3e0, `276d3f77a`). Baseline = `images/base.{exe,asi}` built from 9b0248128 in this worktree; final = `images/d.{exe,asi}` (`images_sha.txt`). Builds are not byte-reproducible,
so every comparison is image against image. `wa/`, `wb1..3/` are earlier windows on earlier images (a, b), kept as history; the evidence the tracker cites is `wd1/`, `wd2/`, `wm/`.

## Burn-down metric

| | 9b0248128 | after |
|---|---:|---:|
| allowlist lines (`re/tools/dual_copy_allowlist.txt`) | 39 | **22** (CROSS-TARGET 11, DUP-IN-TARGET 11) |
| honest measure: C3/C4 rows with `exe_file != file` (`honest.py`) | 172 of 1211 | **166 of 1212** |
| (A) `file` is a .cpp | 38 | **33** |
| (B) `file` is a plate path | 134 | **133** |

The 1212 includes 0x0042fa00 (C2 -> C3). 0x00405890 went C4 -> C3 (still counted, now `exe_file == file`). **Rows: re-earned 10** (9 at C3, 0x0042fa00 C2 -> C3), **corrected 1** (0x0042d3e0),
**demoted 1** (0x00405890 C4 -> C3), touched without a ladder move 4 (0x004b4650, 0x004c3ac0, 0x004c39b0, 0x004c4d20), **skipped 7** (below).

## Verdict first

* **Group (a) is done.** Timer x4, 0x00431b70, 0x004c5c00: one body each, loser's registry key retired. 0x004b4650 was NOT an identical-twin case: only its dead third copy is gone.
* **Group (b), 8 of 10 RVAs consolidated or corrected**, and the work found two defects in bodies the tracker rated C2/C3: the .asi's INSTALLED hook for 0x0042fa00 clamped team values the original does not
  (the new vectors catch it), and the .asi's C3 `MenuEntryArrayInit` (0x0042d3e0) did not match the bytes (two dwords written that the original never writes, 5 of the original's 14 stores missed;
  invisible to its one-dword key).
* **Default build behaviour changes: yes, one place.** The race camera's `Vec3Norm` now calls `Math::LutNormalize`; d1r2's replay pose error drops (table below). Everything else the exe runs is unchanged:
  G-CTRL `2b4d47bb / 356821c8 / f7a155e1` on base, a, b and d. The .asi changes in two places on purpose (0x0042fa00 installs the faithful body, 0x0042d3e0 is corrected).

## Group (a)

| RVA | change | evidence (final asi) |
|---|---|---|
| 0x0041eda0 / 0x0041f000 / 0x00420d40 | `Util/TimerSlot.cpp` copies SlotBitSet / SlotDataCopy / SlotArrayClear deleted (never installed, no caller in either target); keys `slot_bit_set`, `slot_array_clear` retired; rows -> `Util/TimerInit.cpp` | path1 10/10, 10/10, 10/10 GREEN; path2 full PASS 3/3 entries each. `timer_bit_field_set` path2 and `timer_slot_data_copy` path2 first FAILED with "bad argument count" (one list argument for a 2-arg function): path2 vectors repaired (`fix_path2_vectors.py`), same class dupinstall2 fixed for 7 other keys. Mutant control: `flag == 0` build RED 10/10 (`wm/mut_timer_bit_field_set.txt`) |
| 0x00422120 | `Util/TimerSubarrayInit.cpp` (the plain-call copy TimerInit.cpp:151 calls broken) deleted from tree and both rsp | path1 3/3, path2 PASS 1 entry; `exe_file == file` |
| 0x00431b70 | `BucketMixed_t3.cpp:GetDat007f0f10` deleted, STAGED key `get_dat_007f0f10` retired | path1 10/10, path2 PASS 3 entries |
| 0x004c5c00 | `SpriteCluster.cpp:LinkedListStringSearch` deleted (asi-only, no install, one dead caller); `SpriteLookupC` calls the RVA through a pointer; `file` -> `Util/PromoLoop_sessionB.cpp` (exe has no body) | no `run_diff` handler for `circular_str_search_ci`: early-window lane GREEN 5/5; path2 E9 + rel32 site-verified, entries 0 (a circular list cannot be built by the harness), as recorded 2026-09-06 |
| 0x004b4650 | dead `Render/PromoLoop_round22.cpp:Vec3Lerp` (the whole file) deleted. **Not identical twins**: the exe's `Powerup/PowerupContact.cpp:Vec3Lerp` computes `t*d + a` in C with a float rounding between the multiply and the add, the original adds the unrounded x87 product (`Lerp4b4650` is the verbatim port), plus a `Log()` call | row stays C2, notes say so. Open: needs the x87 body in the exe under a PC_24 scope |

## Group (b)

| RVA | change | evidence |
|---|---|---|
| 0x00420de0 | dead plain-C `Vehicle/Damage.cpp:VehicleDamageAccumulatorAdd` + its `/export` pragma deleted (it compared the rounded f32 sum; the original compares st0 at 0x00420df1). `Accum420de0` (naked x87) is the body | early-window GREEN 5/5; path2 site-verified (entries 0: "bad argument count", the key's vectors are early-window shape) |
| 0x00405890 | `Race/CollectDone.h:CollectDoneBody405890` is the one body; `CollectDone.cpp:Pred405890` (new, both rsp) reads the two globals and installs; `RuleEngine::CollectAllDone` and `PickupCollect_naked.inc:PK_458920` call it; `PromoLoop_round78.cpp` and the naked `PK_405890` deleted | early-window GREEN 10/10 (7/10 RED on the `done != total` mutant); path2 PASS 3 entries; exe M3 pickup replay 6/6 and 3,888/3,888 at PC_24 and PC_53, same as base (`m3r/`); ruleengine host test GREEN. **C4 -> C3**: the canonical race (rule 4) never calls the predicate, so the strict-C4 rule cannot be re-earned without a mode-5 collect race |
| 0x0042f6b0 | `Race/SelectionToGameMode.h` is the one table; `MenuModeSync` calls it; `RaceModes::SelectionToGameMode` is a wrapper | path1 13/13 (negctl fixture seeds DAT_0067f184), path2 PASS; racemodes + navsm host tests GREEN |
| 0x0042ad90 | the C body `Find42ad90` moves to `Frontend/KvFind.cpp` (both rsp), the .asi's naked shim calls it, exe `KvLookup` is a call into it | early-window 2/2 (small: n=0 -> 0xAAAA, n=1 -> 0xBBBB); `kvfind_equiv_test`: old `KvLookup` vs new, 400,000 random streams, 0 mismatches; navsm host test (34-screen BFS) GREEN. path2 N/A: the key drives the stack-ABI export, the install is the register shim |
| 0x0042fa00 | `MenuTeamSelectTick` (follows the original: guard `!= 0`, no clamp; decomp in `decomp_42fa00.txt`) is now the install; the asi's `PlayerSlotEdgeAdjust` (guard `> 0` + clamp [0,2]) and its key deleted. Two vectors added (v7 team -1 + UP -> original -2; v8 team 3 -> stays) | path1 9/9 GREEN, path2 PASS 3/3. **Twin control** (`wm/`): the retired `PlayerSlotEdgeAdjust` from base.asi is RED on exactly v7 and v8, GREEN on v0..v6 (as pre-registered). **C2 -> C3** |
| 0x0042d3e0 | **correction**: disassembly (`disasm_42d3e0.txt`): 14 stores, dwords entry +0 +4 +8, bytes +12..15, dwords +16 +20 +24 +28 +36 +40 +44, **+32 not written**, 30 entries. The old asi body wrote entry-4 (the previous entry's +48), bytes +9..12, +32, and missed +13..15, +28, +44. `MenuEntryZero.h:MenuEntryZeroBody` written from the bytes | host `maskcheck_42d3e0`: new = the 14 stores exactly; old differs at exactly the predicted offsets. Key rewritten as `cache_setter_observe` over 79 dwords x 2 fills: final asi GREEN 2/2, **old body on base.asi RED 2/2**; path2 PASS. The exe's `RecordsZero` runs the same body but still clears MORE (pad20 +0x20 which MenuNavSM writes and reads as `row20`, pad30, entry 30): net effect unchanged, so this row stays split on purpose (pre-registered G-REC predicted "no reader", it was wrong) |
| 0x004c3ac0 / 0x004c39b0 | `RaceCamera::Vec3Mag` -> shared `Vec3Magnitude`; `Vec3Norm` -> `Math::LutNormalize` | d1r2 replay `cam_dump` sha8: base `1e7d2274`; Vec3Mag only `1e7d2274` (**byte-identical**); with LutNormalize `87078a7a`, N0 identical. Pose position median 1.105e-4 -> 7.97e-5, p95 1.020e-3 -> 7.88e-4, max 2.56e-3 -> 1.76e-3; at-axis p95 3.5e-3 deg -> 2.3e-3 deg; right-axis median 1.36e-5 deg -> 1.7e-6 deg (`pose_scores.txt`). Pre-registered "equal" was wrong in the good direction. The allowlist key narrows to RwLutTables vs RwV3dNormalize (user decision pending) |

## Skipped, with the reason (7: 0x00410510, 0x004c4d20, the exe halves of 0x0042d3e0 and 0x004b4650, and the three briefed)

* **0x00410510**: exe `EvaluateResult` is a pure partial port (no end-block writes, 14 original callees by address, a different rule-10 test and `won0 ||`, per the survey). A single body needs a Host layer for 15 callees: a D3 slice, not a mechanical merge.
* **RotateAboutAxis (0x004c4d20)**: shared `RwMatrixRotate` is C2, needs a RwMatrix, x87 FSIN/FCOS and a vector transform; the camera it would replace is 1e-4-accurate, so the replay cannot validate the swap.
* **0x0042d3e0 exe side**: see above (needs a frontend parity capture to make `RecordsZero` faithful).
* **0x004b4650 exe copy**: above. **0x0045baa0, 0x004177b0, 0x0040e180**: as briefed.

## Findings that change other work

1. **Foreign sessions overwrite the shared .asi between a window's steps.** 3 of 11 path2 reports in my window B2 loaded another build (export RVAs did not match `images/b.asi`). `window.py` now logs the shared hash before and after every step and re-deploys; `check_asi_identity.py` verifies each path2 report against the asi's export RVAs. All final reports are OK.
2. `run_diff.py` has no handler for `circular_str_search_ci`, `fastcall_float_clamp`, `two_global_predicate`, `edx_ebx_edi_find`: those keys' path1 is the early-window lane only (their `log/diff_*.csv` are early-window files).
3. `scripts/backfill_exe_file.py` did not fail on multi-line rows (hooks.csv has none: 5,934 records, 5,934 lines). It died on record 11, a QUOTED comment row (`"# exe_file:`) its `startswith("#")` test missed, and 5 data rows with unquoted commas (records 173, 1194, 1823, 3115, 3125). Fixed: record-level parse, byte-exact round trip, comment rows by first field, malformed rows left untouched and warned. **Do not run it without `--check`**: its channel B reads the stale 2026-09-29 audit CSV and would re-add already-removed false anchors (00405890, 00431b70, 004987b0, 00485340...).
4. The nav-demo BMP walk is not a usable A/B: all 12 screens differ between two runs of the SAME image.
5. The tracker's C3 for 0x0042d3e0 rested on a one-dword observation: `void_write_observe` keys are blind to the stores around the observed address.

## Hygiene

Both targets build (`images/build_*.log`), rva-lint NEW=0, asm-lint OK, no new warning class. The shared `original\mashed_re_dev.asi` was restored to `270e036f...` after every window (`matches original: True`). Only PIDs this lane spawned were stopped.
MB-sized dumps (step / gates / ptrace, cam / librw) and the nav BMPs were deleted before the commit: the sha8 lines in `b_gctrl.log`, `a_gctrl.log`, `nb_gctrl.log`, `nd_gctrl.log`, `baselines_c*.out` and `pose_scores.txt` are the evidence. `tracker_final.py` therefore cannot be re-run as is (its `cam()` reads the deleted dumps).
Edits to `re/frida/hooks_registry.py` preserve its bytes (mojibake, CRLF); `log/` files rewritten by harness runs were restored to HEAD.

## Follow-ups

* 0x00410510 Host layer (D3); RotateAboutAxis; a frontend parity capture to make `RecordsZero` faithful; a mode-5 collect race to re-earn C4 for 0x00405890; the x87 `Lerp4b4650` in the exe under PC_24 for 0x004b4650.
* Put `circular_str_search_ci`, `fastcall_float_clamp`, `two_global_predicate`, `edx_ebx_edi_find` into `diff_template.js`, or document the early-window lane per key.
* Drop channel B from `backfill_exe_file.py` (or regenerate the audit CSV).
