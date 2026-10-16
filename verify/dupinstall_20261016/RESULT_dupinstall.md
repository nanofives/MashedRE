# RESULT dupinstall (2026-10-16)

ROADMAP D4 item (3): burn the 27 `DUP-INSTALL` entries of `re/tools/dual_copy_allowlist.txt` down to zero.
Branch `work/dupinstall`, based on `race/first-frame-parity` 98705947. Worktree `.worktrees/dupinstall`.

## Verdict

- **26 of 27** entries removed. 23 by deleting the dead install and the dead body, 3 were already stale
  (`0x0046c730`, `0x0046c750`, `0x0046cbe0`: the 2026-10-11 re-earn consolidated them, the allowlist was never pruned).
  The live lint count was 24, not 27 (these 3 explain the gap).
- **1 left, skipped**: `0x0046cbb0` (`Util/PromoLoop_round25.cpp:63 CarStatePairGet` vs `Vehicle/VehicleState.cpp:183
  VehicleCarStateRead`). `Vehicle/` belongs to the d2posmodel lane. See "Skipped".
- 29 `DUP-IN-TARGET` lines for the same RVAs went with them (the second body is gone, so the finding is gone).
  Lint: 115 findings before, **66 after** (CROSS-TARGET 14, DUP-IN-TARGET 77 to 51, DUP-INSTALL 24 to 1), **NEW=0**.
- **No row moved on the ladder.** 0 demotions, 0 promotions. 23 hooks.csv rows re-pointed (`file`, `exe_file`, one `name`,
  one `status`, appended note), 23 CHANGELOG lines.
- Default build unchanged: G-CTRL **c2e4d60d / b50bd39b / ba90ed88 before and after** (same sha8 on all three artifacts,
  18664 / 56153 / 28076 lines). Nothing referenced the deleted bodies (both targets compile and link after the
  deletion), so `mashed_re.exe` only lost exports no code calls.
- `mashed_re_dev.asi` behaviour changed in exactly one visible way: at 0x00415d00 and 0x00416060 the live hook is now
  the passthrough dispatcher, not the float-chain port (see "User decision").

## How the survivor was chosen

Rule, applied in this order:

1. The body the row's own evidence names (`hooks.csv` `name`, the `frida_diff` csv, the registry key that csv was run on).
2. Tie or both cited: the body that was installed (link order wins; verified below), so the .asi keeps running what it ran.
3. Where the two bodies differ in arithmetic, read both and compare (done for 0x0042af50, 0x00431f30, 0x004f8660/8690;
   identical on every path), or keep the evidence-bearing one and say so (AI).

Before-state, measured not inferred: `install_before_summary.txt`. With `MASHED_HOOK_ONLY=<27 RVAs>` and the deployed
.asi, every one of the 24 pairs shows `[winner]+ [other]-`, i.e. the second install was refused (`DUAL-INSTALL REFUSED`).
The winners are exactly the first TU in `asi_sources.rsp`, as the 2026-09-09 audit said. In 20 of the 23 rows the
survivor is NOT the body that was installed (3 are: 0x00407a20, 0x00426cb0, 0x004955b0), i.e. the cited evidence sat on
the body that lost, and the 2026-09-09 repoint had pointed `file` at a body the evidence did not cover.
After-state: `install_after_summary.txt`, **26/27 single install, survivor named**, the 27th is the skipped pair.

## Table

| RVA | C | survivor (TU) | deleted | basis |
|---|---|---|---|---|
| 0x00407640 | C3 | CopterFind639dc4 (Util/PromoLoop_round69) | FindCopterByKeyIndex (Gameplay/RangeTable_ah1) | row cites diff_copter_find_639dc4 |
| 0x004077e0 | C3 | FloatIdx639de0 (Util/PromoLoop_round61) | GetSelectedCopterField60 (Gameplay/Thresholds_ah4) | row cites diff_float_idx_639de0 |
| 0x00407a20 | C4 | GetAiLapCounter (Gameplay/RangeTable_ah1) | Table8a9648Get (Util/PromoLoop_round42) | re-earn csv and C4 racediff ran on the installed body; `name` moved |
| 0x004098a0 | C4 | Ret63a5f0 (Util/PromoLoop_round40) | GetLedEntryArrayBase (Gameplay/RangeTable_ah1) | row cites diff_ret_63a5f0 |
| 0x0040b970 | C3 | Bool0Out8a94e0 (Util/PromoLoop_round67) | PlayerScoreZeroTest (Gameplay/ScoreMasks_ah3) | row cites diff_bool0_out; deleted had no registry key |
| 0x0040b9a0 | C4 | MaxScoreFlags40b9a0 (Util/PromoLoop_sessionB) | PlayerScoreMaxTest (Gameplay/ScoreMasks_ah3) | path1 on survivor; 2026-07-24 install-observe had run the deleted one |
| 0x0040ba00 | C3 | BoolMinOut8a94e0 (Util/PromoLoop_round67) | PlayerScoreMinTest (Gameplay/ScoreMasks_ah3) | row cites diff_boolmin_out |
| 0x0040ba60 | C4 | Active4Slots40ba60 (Util/PromoLoop_sessionB) | PlayerScoreGateInvert (Gameplay/ScoreMasks_ah3) | as 0x0040b9a0 |
| 0x00415d00 | C3 | AiWallAhead_Entry + WallAhead (Ai/AiWallAhead) | AiWallAhead (Ai/AiTargeting) | notes cite the A/B self-test in this TU |
| 0x00416060 | C3 | AiLos_Entry + LosCheck (Ai/AiLineOfSight) | AiLineOfSight (Ai/AiTargeting) | notes cite the A/B self-test in this TU |
| 0x00426cb0 | C3 | SlotIndexToPtr (Frontend/MenuNearLeaves_s6) | FrontendSlotTablePtr426cb0 (Frontend/FrontendLeaves_ad2) | tie (row cites both batches, both path1 GREEN), installed body kept |
| 0x004298c0 | C3 | Clear67d99c_x4 (Util/PromoLoop_round70) | MenuQuadGlobalZero (Frontend/MenuLeaves_af5) | row cites diff_clear_67d99c_x4 (re-earn) |
| 0x0042af50 | C3 | MenuReadinessCheckC (Frontend/MenuStateMachine) | MenuReadinessCheck (Frontend/TimerReset) | row cites diff_menu_readiness_check_c; paths read identical |
| 0x0042bde0 | C3 | HudRectEmitter5 (Frontend/BatchAA_s1) | HudRectEmitter (Frontend/MenuNearLeaves_s6) | row cites draw_quad_observe csv; deleted was path1 RED on baseline |
| 0x00431f30 | C3 | FrontendPageIdDispatch (Frontend/MenuLeaves_af6) | FrontendPageFlagSet (Frontend/SkeletonAndScatter_t6) | row cites diff_frontend_page_id_dispatch; same case set |
| 0x004955b0 | C3 | DInputInitPredicate (Boot/RwEngineInit) | CreateDInputObjectBool (Input/DirectInput) | deleted was a forwarder to the survivor |
| 0x00496930 | C3 | Table773030Get (Util/PromoLoop_round30) | TimerTable773030Get (Util/PromoLoop_round17) | row cites diff_table_773030_get (re-earn) |
| 0x00498bf0 | C4 | DisplayActiveFlagGet (Boot/FrameDispatch) | DisplayGetCursorGate (Boot/VideoConfig) | `name`; both are `return *(u32*)0x00773204` |
| 0x004f8660 | C3 | PluginDataDwordA (Render/StateBatchGetters) | PluginFieldReadA8 (Render/PluginFields_ah4) | row cites diff_plugin_data_dword_a; survivor is also in the exe |
| 0x004f8690 | C3 | PluginDataDwordB (Render/StateBatchGetters) | PluginFieldReadAC (Render/PluginFields_ah4) | as 0x004f8660 |
| 0x00556cc0 | C3 | SetDat00912a20 (Frontend/BatchAA_s3) | FontAtlasSlotSet (Frontend/Cluster_v1) | row cites parallel_diff_set_dat_00912a20 |
| 0x00556cd0 | C3 | GetDat00912a20 (Frontend/BatchAA_s6) | FontAtlasSlotGet (Frontend/Cluster_v1) | row cites parallel_diff_get_dat_00912a20 |
| 0x005b3580 | C3 | AudioListInit (Audio/AudioList_ag) | AudioListHeaderInit (Audio/AudioLeaves_ab2) | row cites diff_audio_list_init (count_header_list_ring) |
| 0x0046cbb0 | C4 | SKIPPED | | Vehicle/ is d2posmodel's |
| 0x0046c730, 0x0046c750, 0x0046cbe0 | C3 | already one body (Race/SubStateCallees, Vehicle/SmallLeaves_o5) | | stale allowlist entries only |

For each deleted body the registry key was removed from `re/frida/hooks_registry.py` (14 keys) and the three negctl
fixture rows that would have re-created them as key-only orphans (`b08.py`, `b10.py`, `race.py`; the registry loader
adds any fixture key it does not find). `HOOKS` has no entry without `rva`/`export` after the change.

## Evidence

All under `verify/dupinstall_20261016/`.

| What | Result | File |
|---|---|---|
| Baseline build, both targets | exit 0 | `build_baseline.log` |
| Build after, both targets, `MASHED_NO_DEPLOY=1` | exit 0, 0 errors | `build_after.log` |
| rva lint | 115 to 66 findings, NEW=0 | `lint_after.txt` |
| G-CTRL on a frozen copy of the baseline exe | c2e4d60d / b50bd39b / ba90ed88 | `gctrl_base.log` |
| G-CTRL on the rebuilt exe | c2e4d60d / b50bd39b / ba90ed88 | `gctrl_after.log` |
| path1 baseline (both bodies of every pair, 35 keys) | survivors 21/21 GREEN; the 14 deleted keys: 10 GREEN, 3 degenerate (no verdict: `selected_copter_field60`, `table_8a9648_get`, `menu_quad_global_zero`), `hud_rect_emitter` RED 10/10; the other 9 deleted bodies had no registry key at all | `path1_baseline/`, `path1_baseline_driver.log` |
| path1 after (21 survivors, rebuilt .asi) | **21/21 GREEN** | `path1_after/`, `path1_after_driver.log` |
| Install manifest before (deployed .asi, 27 RVAs) | 24 dual (1 refused), 3 single | `install_before_summary.txt` |
| Install manifest after (rebuilt .asi, 27 RVAs) | **26 single**, 1 dual (the skipped pair); all 27 first bytes `0xE9`, 30 s boot survived | `install_after_summary.txt`, `path2_after/install_after.log` |
| path2 `run_verify_hook`, 21 survivor keys | **install proven on 18/21** (opcode 0xE9 and rel32 equal the survivor's export), 3 not reached | `path2_summary.txt`, `path2_after/` |

path2 detail, because the numbers mean different things:

- Install (E9 plus rel32 pointing at the survivor's export): 18 of 18 that reached the check.
- Call-through (interceptor entries equal to vector count, no exception): 10 of 18. The other 8 fail on vector shape in
  the registry entry, not on the install: `copter_find_639dc4` ("expected an unsigned integer", early-window vector
  `[key, placeAt]`), `bool0_out_8a94e0` / `boolmin_out_8a94e0` (AV at 0x0/0x1, out-pointer passed as a small int),
  `maxscoreflags_40b9a0` ("bad argument count"), `active4slots_40ba60` and `audio_list_init` ("expected a pointer"),
  `frontend_page_id_dispatch` ("expected an integer"), `hud_rect_emitter5` ("system error" at the menu, its path1 draw
  observation is GREEN). These are arg_types with an early-window-only handler, which the path2 template cannot drive;
  not edited (the hard rule forbids touching `diff_template.js`).
- Not reached: `plugin_data_dword_a`, `plugin_data_dword_b`, `table_773030_get`. All three are `scenario='race'` and the
  nav drive loses its Frida session during "booting to menu". **The same failure reproduces against the unmodified
  shared .asi** (`path2_race_baseline_asi_plugin_a.txt`), so it is the full auto-hook boot under this harness, not this
  change. For those three the proof is path1 GREEN (race scenario, hooks off) plus the install manifest.
- The two AI rows have no registry key and no exported entry point (anonymous namespace), so path1 and path2 do not
  apply; their proof is the install manifest (`AiWallAhead_Entry`, `AiLos_Entry`, single, `0xE9`) and the unchanged,
  already-verified A/B body.

## C-level statements (what the evidence supports, nothing more)

- C3 rows (16, plus the 2 AI rows below): survivor is the body the row's csv was run on, path1 GREEN again on the
  rebuilt .asi, install proven. Level unchanged.
- C4 rows (5: 0x00407a20, 0x004098a0, 0x0040b9a0, 0x0040ba60, 0x00498bf0): kept at C4. For 0x00407a20 the survivor is the
  body the C4 racediff and the 2026-10-11 re-earn ran on, nothing to re-earn. For 0x0040b9a0 and 0x0040ba60 the C4 was
  the 2026-07-24 install-observe, which ran the DELETED bodies (`PlayerScoreMaxTest` / `PlayerScoreGateInvert`,
  `re/account3_c4_ready_batch_2026-07-24.md`); for 0x00498bf0 it was the 2026-06-06 install-observe, which could not tell
  two identical one-liners apart. The same class of run now exists on the survivors: path1 GREEN, 0xE9 live with the
  survivor named in the manifest, 30 s boot survived. The 0x004098a0 notes record no C4 basis at all, so for that row
  this is the first recorded run of that class on either body. If a stricter reading of "C4 needs a canonical-scenario
  run with the hook installed" is wanted, these four are the ones to challenge; none has an in-race run on either body.
- AI rows: C3 stays. The A/B evidence is on a body that is unchanged. What changed is which body installs.

## Skipped

- **0x0046cbb0** (C4). The pair is `Util/PromoLoop_round25.cpp CarStatePairGet` and `Vehicle/VehicleState.cpp
  VehicleCarStateRead`, and the cheap fix needs no `Vehicle/` edit: the C4 racediff
  (`re/analysis/phys_c4_evidence/c4_racediff_result_2026-08-15.json`) ran with the hook installed, and the installed
  body is `VehicleCarStateRead` (manifest `[19]+VehicleCarStateRead [693]-CarStatePairGet`, both before and after).
  Deleting `CarStatePairGet` plus `car_state_pair_get` is therefore the consolidation that keeps the C4 evidence, and it
  touches only `Util/PromoLoop_round25.cpp`, `hooks_registry.py`, hooks.csv. Left for whoever owns the A-chain rows so
  that lane's `hooks.csv` and registry edits do not race with this one.

## Stale entries that are not mine (left alone)

Allowlist lines that no longer match a live finding and belong to other RVAs: `DUP-IN-TARGET` for `0x00417640`,
`0x00418860`, `0x0042b8c0`, `0x005aea00`. The lint tolerates stale lines, so nothing breaks; whoever works item (4)
should prune them.

## User decision

0x00415d00 and 0x00416060: before, the .asi ran `Ai/AiTargeting.cpp`'s float-chain ports (installed first by link order,
no registry key, no A/B). Now it installs the dispatchers in `Ai/AiWallAhead.cpp` / `Ai/AiLineOfSight.cpp`, which return
the ORIGINAL body's result unless `MASHED_AI_WALLAHEAD_SELFTEST` / `MASHED_AI_LOS_SELFTEST` is set. That is the body the
hooks.csv C3 notes cite (A/B 3585 and 1153+ calls, 0 mismatches), so no C-level moves, but the dev .asi stops running
the unverified port at those two RVAs. The opposite choice (keep the port live, delete the dispatchers) means demoting
both rows to C2 and earning a new A/B on the float-chain bodies. Recommend as done. `mashed_re.exe` is not affected
(its AI is `Ai/AiStandalone.cpp`).

## Incidents and corrections

- The warm pool runner (`run_diff_warm.py`) gives false REDs ("bad argument count", AV at 0x0) on early-window arg
  types and loses its session on others; first baseline results from it (`path1_baseline.log`, `path1_baseline_s1.log`)
  were discarded, `run_path1.py` (one spawn per key, early-window fallback) produced the numbers above.
- `install_observe.py` upstream (`canonical_install_observe.py`) only kills the process it spawned when `psutil` is
  importable, and `psutil` is missing for `py -3.12` here. My first observation left one MASHED.exe (pid 17984, started
  by me) running; I killed that PID and nothing else, and the copy in this directory kills its pid unconditionally.
- The first after-state observation lost its manifest because `MASHED_HOOK_MANIFEST` was a relative path opened by the
  game (cwd `original\`). Re-run with an absolute path in a second deploy window; both windows restored the shared
  .asi (sha256 `270e036f...` before and after, backup deleted, no MASHED.exe left).
- Shared .asi safety rule used for both windows (`run_path2_deployed.py`): no MASHED.exe at start (polled), backup with
  hash, copy, runs, restore in `finally`, hash compared. Windows: 09:51:29-09:55:04 and 09:56:37-09:57:17, 2026-10-16.
- The tracked `log/diff_*.csv` files my runs overwrote were restored (`git checkout`); the copies are in
  `path1_baseline/` and `path1_after/`.

## Files in this directory

`run_path1.py`, `run_path2_deployed.py`, `install_observe.py` (worktree-safe copy), `run_gctrl_private.py` (output kept
out of the main checkout), `manifest_summary.py`, `path2_summary.py`, `prune_registry.py`, `allowlist_stale.py`,
`tracker_edit.py` (the hooks.csv + CHANGELOG transaction, re-runnable with `--apply` on a clean tree).
