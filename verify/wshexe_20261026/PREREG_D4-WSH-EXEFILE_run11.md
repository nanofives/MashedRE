# PREREG D4-WSH-EXEFILE run 11: MASHED_ANIM_TRUNC arms + the three consolidations

Committed alone, before any arm of this run. Code under test: `orch/D4-WSH-EXEFILE` @ `de6a16a34`
(base `orch/integration` @ `b0174c80f`, merged at `6fd8bfef8`). Build: worktree, `MASHED_NO_DEPLOY=1`.
The new `.asi` goes into the private root `D:\MashedWT\D4-WSH-EXEFILE_root\original\` only, never
`original\mashed_re_dev.asi`.

## What changed

1. `Frontend/MenuAnimTickTwin.cpp` (`.asi`-only): `MASHED_ANIM_TRUNC=1` makes `Trunc48` a plain
   `(int)v` (truncation, the `FUN_004a2c48` form). Unset or `0`: `std::nearbyint`, as before.
   The exe's `Anim_Tick` (`Frontend/MenuNavSM.cpp`, `AnimStep`) already truncates and is not touched,
   so this knob cannot move an exe hash.
2. `0x00432b30`: `Frontend/PromptStripTwin.cpp` joins `exe_sources.rsp`. The jump-table dispatch
   (`PromptStripTag`, `PromptStripResolve`) is the one body. The `.asi` body still calls `FUN_0042ad10`
   before the dispatch's getter calls, as the original does. The exe's `PromptStripAppend` calls the same
   dispatch, getters `ModeCodeLookup` / `ConstantGetter22`.
3. `0x0040b890` / `0x0040b8e0`: `Gameplay/Thresholds_ah4.cpp` joins `exe_sources.rsp`.
   `GridCount8or12Core` / `ScoreThreshold7or10Core` take the three inputs as arguments. The `.asi`
   exports read them from the game in the original's order. `exe_main.cpp` and `LibRw/RwRaceFx.cpp`
   call the cores instead of their inline copies.

## Arms and predictions

| Arm | Command | Prediction |
|---|---|---|
| G | `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` on the new exe | G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (merge_train.json refs) |
| A0k | `menu_anim_diff.py`, `MASHED_ANIM_TRUNC=1`, game dt | GREEN 20/20 |
| A1k | same, `MENU_ANIM_DT=0.016666668` | GREEN 20/20 |
| A2k | same, `MENU_ANIM_DT=0.033333336` | GREEN 20/20 |
| A2d | `MENU_ANIM_DT=0.033333336`, knob unset | RED, first difference record 0, `+0x10`, original `485`, twin `484` (replicates run 10's A2) |
| P1 | `menu_prompt_diff.py` (nav-driven, JMP patch per sample) on the shared body | GREEN on every sample |
| T1 | `run_diff_scenario.py grid_count_8or12 --scenario race` | GREEN, value `8` (not the menu's 12) |
| T2 | `run_diff_scenario.py score_threshold_7or10 --scenario race` | GREEN, value `7` (not the menu's 10) |
| I1 | `canonical_install_observe.py 0x0040b890,0x0040b8e0,0x00432b30` | 3/3 JMP live, no crash |

`menu_prompt_diff.py`, `run_diff_scenario.py` and `canonical_install_observe.py` get the same
`MASHED_ROOT` extension `menu_anim_diff.py` got in `0fff2aed5` (default unchanged) so they boot the
private root. All boots under `mashed_lock.py run --label D4-WSH-EXEFILE`.

## Decision rules

- G differs from the refs: the consolidation changed exe behaviour. RED, find the cause, no tracker change.
- A0k, A1k, A2k all GREEN and A2d RED as predicted: the knob makes the twin match the original on every
  arm. File one `roadmap.ps1 decision` row for the default flip (`MASHED_ANIM_TRUNC` default ON,
  `MASHED_NO_ANIM_TRUNC=1` opts out). Do not flip. `0x004325c0` stays C2 until the flip; after it, C3
  needs A0-A2 on the default body and C4 a canonical-scenario run.
- A2k RED: the twin differs from the original for a second reason. Record the signature, no decision row.
- A0k RED or A2d not RED: harness or state drift, re-run once, then report.
- P1, T1, T2, I1 GREEN and G equal: `re-classify` sets `exe_file := file` on `0x00432b30`, `0x0040b890`,
  `0x0040b8e0` (level unchanged, one body). Any of them RED: that row is demoted per the rubric instead.
- A piz-collision crash (`MASHED.exe+0x1509b4` / `+0xe66db`) is a re-run.
