# PREREG dualcopy3: three thunks for 0x00417640, the verbatim __ftol for the race camera, then D4 item (4)

Committed BEFORE the first measuring run of this lane. Branch `work/dualcopy3`, base `race/first-frame-parity` 05e8c4f0.
Model claude-sonnet-5-5. The parent's decisions (2026-10-16): the exe's pinned `rate = 0.0f` on course 0x21 is a port defect, the
behaviour change is intended; `__ftol` in `Race/RaceCamera.cpp` becomes the verbatim x87 body and d1r2's row-indexed replay is the
camera witness; then item (4), same recipe, outside `Vehicle/`, `Collision/` and the librw render files.

## 1. What the bytes and the tree say (read before any run)

| fact | source |
|---|---|
| `FUN_00426c00` returns `DAT_00644158`; the exe's host binding `aib_track_index()` returns `g_aib.course`, the engine Course_Id (`TrackRenderer.cpp:142`, `:2320`) | `Race/CameraEntries.cpp:26`, `RuleEngine.cpp` Rule10Seed |
| Course 0x21 = 33 = **Warzone** (`Race/GameFlow.cpp` `kAreas`). TRAINING, the G-CTRL and (b) scenario, is course 30 (0x1e) | `kAreas` |
| The exe's `PostStepPowerupBrake` (AiStandalone.cpp) pins `rate = 0.0f` and omits `FUN_0046d570`; the .asi `AiPostStepPowerupBrake` (AiController.cpp) reads both through the original and CAN set `ctrl[5] = 0xff` | the two bodies |
| `0x0088ff50 + v*4` (the gate `FUN_00452ea0` reads) and `0x00684de0` (`FUN_00452eb0`) have **no writer in any exe TU** (grep over `mashedmod/src`, only readers and the A/B driver's seeds) | grep |
| `FUN_0046d6d0` = record `+0x9e4` = `Host::veh_f32(v, 0x9e4)` (exists, `AiStandalone.cpp:740/905`); `FUN_0046d570` has an exe port, the naked `AL_46d570` in `Ai/AiLeafTargets_naked.inc`, no exported accessor yet | AiStandalone.h, AiLeafTargets.cpp |
| `Race/RaceCamera.cpp:170` is the ONLY caller of `Ftol_4a2c48`, fed `prog` (a path index >= 0) | grep |
| `Math/FPURound.cpp:FPURound_4a2c48` is the verbatim naked body (C3), compiled into the exe with no caller; consumes ST0, leaves the x87 stack as it found it, result in EDX:EAX | FPURound.cpp header |

## 2. The changes under test

1. **0x00417640** -> one body in a new shared TU `Ai/AiPostStepBrake.cpp` (in BOTH rsp lists), the `AiController.cpp` text (the body
   the C3 A/B evidence cites) with the dead displacement / magnitude computation elided (its result is never read, as the exe copy's own
   comment says), and per-target thunks for: the track index (`FUN_00426c00` / `Ai::Ai_HostTrackIndex()`), the rate (`FUN_0046d6d0` /
   `Ai_HostVehF32(v, 0x9e4)`), the range (`FUN_0046d570` / a new exported `AiLeaf_46d570`), and the three thresholds (the original's
   `.rdata` `0x005cc72c`, `0x005cc568`, `0x005ccad0` in the .asi, where the A/B driver seeds them; literals in the exe, whose pad is zero).
   `AiStandalone.cpp`'s `PostStepPowerupBrake` and `AiController.cpp`'s `AiPostStepPowerupBrake` are deleted.
2. **0x004a2c48** -> `Math/FPURound.cpp` gains a naked C++ entry `FtolVerbatim(float)` over `FPURound_4a2c48` (`fld [esp+4] / call / ret`),
   `Race/RaceCamera.cpp:Ftol_4a2c48` calls it instead of `static_cast<int>`.
3. **Item (4)**: for each remaining allowlist key outside `Vehicle/`, `Collision/` and the librw render files, read both bodies, then
   re-anchor a non-body, delete a dead copy, consolidate a real pair into one shared TU, or keep it with the blocker stated.

## 3. Gates and predictions

| gate | pass criterion | prediction |
|---|---|---|
| **G-LINT** | `lint_rva_bodies.py` NEW = 0, `lint_inline_asm_abs.py` OK, both targets build | PASS. `DUP-IN-TARGET:00417640` and `DUP-IN-TARGET:004a2c48` leave the allowlist; `CROSS-TARGET:004177b0` stays |
| **G-CTRL** (exe, `run_gctrl_private.py`, TRAINING rule 4, baseline exe built from 05e8c4f0) | step / gates / ptrace sha8 equal to the baseline on the new exe, after each of changes 1 and 2 | UNCHANGED. TRAINING is course 30, so the shared body returns at its first gate before any thunk runs; `Ftol_4a2c48` sees the same in-range values |
| **G-B per regime** (default build, `run_fk` recipe via `run_bscore.py`, K = 0,1,2,4,26,27,54,57, `ai_ctrl_window.py --check-regime re/tools/b_envelopes.json`) | window hashes and per-regime scores before = after | UNCHANGED: A, B, C each score exactly as the baseline does (same TRAINING reason). I do NOT predict a number for the baseline, I measure it |
| **G-WARZONE** (exe, `MASHED_TRACK_VIEW=Warzone`, `MASHED_AI_STEPDUMP`, the same frames) | step hash before vs after | UNCHANGED, because gate 4 (`Table88ff50Get(v) != 0`) cannot hold with no writer in the exe, so the brake branch stays unreachable even though the pinned rate is gone. If it MOVES, the cause is named (a producer I missed) before anything else is claimed |
| **G-AB** (.asi, `AbAiPost` driver, live race, `scenario_launch --track 33` = Warzone, 4 cars; plus track 0) | natural calls with `gate1pass` > 0 on Warzone; `mism = 0`, `fmism = 0`, `f2mism = 0`; `f2brake` and `f2coast` both > 0 (the driver's seeded full-depth pass runs through the new body's real rate / range thunks on live vehicles) | PASS. Natural full-depth writes on Warzone are reported as measured (`wrote`), no prediction |
| **N0** (no-op control for G-AB) | the same run with the driver's MINE side replaced by a second ORIG call, or equivalently two identical runs | `mism = 0` both times, same call mix |
| **M-CTRL** (mutant control for G-AB) | a build whose shared body inverts the brake comparison | the driver reports mismatches on the forced pass (it can see a wrong body); without this a GREEN means nothing |
| **G-POSE** (d1r2 row-indexed replay, `run_replay.py` + `score_d1r2.py pose` on `orig2_trace_v3.csv`, baseline exe vs new exe) | `cam_dump.csv` and `librw_dump.csv` byte-identical (sha8 equal) AND the pose scores equal | IDENTICAL: `static_cast<int>` and the verbatim `__ftol` agree for every value in `[0, 2^31)`, and `prog` is a path index |
| **N0-cam** | two identical replay runs of the new exe | byte-identical dumps |
| **M-FTOL** (mutant control for G-POSE) | an exe whose `Ftol_4a2c48` is `std::lround` (the pre-2026-08-26 `nearbyint` defect, `RaceCamera.cpp:19-39`) | `cam_dump.csv` DIFFERS from the baseline on rows near the ribbon wrap (`prog` 29.5..30), so the replay can see an `__ftol` difference |
| **G-PATH2** | `run_verify_hook.py` for `0x004a2c48`'s registry key (`fpuround_4a2c48` or equivalent) still PASS, full-hook nav boot reaches a race | PASS |
| **Rows** | `hooks.csv` `exe_file == file` for `0x00417640` and for the shared bodies chosen under item (4) | after the transaction |

Knob-took checks: the shared TUs appear in the install manifest once; `FtolVerbatim` is reached (a counter in the M-FTOL / G-POSE runs
is not needed: M-FTOL moving the dump proves the call site is live).

## 4. Decision rule

Change 1 lands default ON if G-LINT, G-CTRL, G-B, G-AB (with N0 and M-CTRL behaving) pass; a moved G-WARZONE hash is a finding to report,
not a failure, provided its cause is explained. Change 2 lands if G-POSE is identical (M-FTOL moving, N0-cam identical), G-CTRL unchanged.
Anything else: revert that change, keep the evidence, report. Collateral after each build: no new warning class, `MASHED_RENDER_LIBRW`
paths untouched, the shared .asi restored byte-for-byte after every deploy window.

## 5. Metric (as the parent asked)

Burn-down is reported with TWO numbers: the allowlist key count AND the honest measure, `hooks.csv` rows at C3/C4 whose `exe_file != file`
(the lint cannot see exe copies with no RVA comment above them). **Start of lane (counted at 05e8c4f0 before the first edit,
`scratchpad/honest.py`, committed as `verify/dualcopy3_20261016/honest.py`): allowlist 56 keys; honest measure 174 of 1210 C3/C4 rows,
split into (A) 39 rows whose `file` is a .cpp (a real evidence-copy vs exe-copy disagreement: Frontend 14, Util 7, Vehicle 7, Gameplay 4,
Boot 2, Hud/HUD 2, Ai 1, Math 1, Collision 1) and (B) 135 rows whose `file` column holds a plate path, so the evidence copy is not
recorded and the row cannot be compared mechanically.** (A) is the number this lane drives; (B) is a tracker-hygiene backlog (the
`file` column is empty of code) that is reported, not burned down here.
