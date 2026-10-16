# RESULT dualcopy3 (2026-10-16)

Branch `work/dualcopy3`, rebased onto `race/first-frame-parity` 88a98a2f before every measurement below. PREREG committed before the first
run (`PREREG_dualcopy3.md`). Model claude-sonnet-5-5 for the build-out, claude-opus-5-5 after the session restart.
Baseline = an exe/asi built from 88a98a2f in this worktree; new = this branch's build (`images_sha.txt`). The pre-rebase runs on 05e8c4f0
gave the same verdicts and are kept in `pre_rebase/`.

## Burn-down metric (both numbers, as asked)

| | 88a98a2f | after |
|---|---:|---:|
| allowlist keys (`re/tools/dual_copy_allowlist.txt`) | 56 | **39** |
| honest measure: hooks.csv C3/C4 rows with `exe_file != file` (`honest.py`) | 174 | **172** |
| of which (A) `file` is a .cpp (a real evidence-copy vs exe-copy split) | 39 | **38** |
| of which (B) `file` holds a plate path (evidence copy not recorded) | 135 | **134** |

The two rows re-earned below were C2 and enter C3 with `exe_file == file`, so they do not count against the metric. The two drops are
`0x00431b10` (A) and `0x00485340` (B), whose `exe_file` named a call-through, not a body.

## 1. 0x00417640 AiPostStepPowerupBrake: one body, three thunks, C2 -> C3

`Ai/AiPostStepBrake.cpp` is in both rsp lists; the AiController.cpp (.asi) and AiStandalone.cpp (exe) copies are deleted. Thunks: the track
index (`FUN_00426c00` / `Host::track_index`), the rate (`FUN_0046d6d0` / `veh_f32(v, 0x9e4)`), the range (`FUN_0046d570` / new export
`AiLeaf_46d570` over the naked `AL_46d570`), and the three thresholds read from the unpatched image (`0x005cc72c` = 30.0, `0x005cc568` =
100.0, `0x005ccad0` = 90.0) as literals in the exe. The exe's pinned `rate = 0.0f` is gone (parent decision: a port defect).

| gate | result | file |
|---|---|---|
| path1 `ai_post_step_powerup_brake` (new registry key, cache_setter_observe) | GREEN 11/11, non-degenerate: 4 brake (`0xaaaaff00`), 3 coast (`0xaaaa0000`), 4 gate-closed no-writes (`0xaaaaaaaa`), incl. the 100.0 and 30.0 boundaries and vehicle 1 | `w1/path1_*.txt`, `w1/diff_*.csv` |
| path2 | PASS, E9 + rel32, 2/2 entries | `w1/path2_*.txt` |
| AbAiPost live race, **Warzone** (engine track 11 = Course_Id 33 = 0x21) | 7937 calls mism=0. The course gate passes NATURALLY on all 7937 (`gate1pass=7937`) but `wrote=0`: the predicate word is never non-zero there. Forced 7937 fmism=0; full-depth 7937 f2mism=0 (brake 3969 / coast 3968) | `w1/ab/AbAiPost.warzone.log` |
| AbAiPost, Training | 12033 calls mism=0, gate1pass=0; forced 12033 fmism=0; full-depth 12033 f2mism=0 (brake 6017 / coast 6016) | `w1/ab/AbAiPost.track0.log` |
| M-CTRL (asi with the brake comparison inverted) | caught: f2mism 5633 / 5633 | `wm/ab/AbAiPost.warzone_mut.log` |
| nav boot, full hook set / ONLY this hook | both reach a race alive, no detach | `w1/nav_probe_*.txt` |
| full set, 45 s | 1290 / 1331 installed incl. `0x00417640` (manifest index 646), survived | `w1/install_*` |
| **exe G-CTRL** (TRAINING, rule 4) | `2b4d47bb / 356821c8 / f7a155e1` on base and new (the announced default) | `b88_gctrl.log`, `n88_gctrl.log` |
| **D3 (b) per regime** (run_fk recipe, K = 0,1,2,4,26,27,54,57) | every step dump byte-identical base vs new; score **A 1/3, B 2/3, C 0/3** before and after (A: cars 1 and 3 `abs_steer~` 0; B: car 2 `c1#` 60, `steer#` 93; C: all three) | `b88_K*.log`, `n88_K*.log` |
| exe on Warzone (3600 frames, step + gates) | `b726286f / b6360eef` on base and new | `*_warzone.log` |

Why the exe does not move, even on Warzone: gate 4 reads `0x0088ff50 + v*4` and gate 2 reads `0x00684de0`, and no exe TU writes either (their
producer is the unported powerup-seek `FUN_00414f00`). The body is now faithful and its inputs are the open item. The original shows the same
picture on its own Warzone run (`wrote=0`), so in a 4-car Quick Battle this branch does not fire in the original either.

C3, not C4: the A/B driver commits the original's result.

## 2. 0x004a2c48 __ftol: RaceCamera calls the verbatim body, C2 -> C3

`Math/FPURound.cpp` gains `FtolVerbatim(float)` (`fld [esp+4] / call FPURound_4a2c48 / ret`); `Race/RaceCamera.cpp:Ftol_4a2c48` calls it.

| gate | result |
|---|---|
| body bytes | the 117 bytes `0x004a2c48..0x004a2cbc` match the unpatched original exactly in base.exe, new.exe, base.asi and new.asi; new.exe has the wrapper calling it (`byte_check.txt`) |
| G-POSE (d1r2 row-indexed replay of `orig2_trace_v3.csv`) | `cam_dump.csv` sha8 `1e7d2274`, `librw_dump.csv` `b33ca1b4` on base and on new: byte-identical, so d1r2's pose scores are unchanged by construction |
| N0-cam (the same exe again) | identical, `1e7d2274` |
| M-FTOL (exe with `std::lround`) | moves to `f274bae2`, so the replay does see an `__ftol` change |
| G-CTRL | unchanged (above) |

## 3. ROADMAP D4 item (4), outside Vehicle/, Collision/ and the librw render files

18 keys gone, 1 narrowed (`allowlist_edit.py`). All were call-throughs or prose the scanner bound as bodies, re-anchored by comment edits
only (no exemption): `Audio/MixedC3Sweep.cpp` (7 trampolines), `Save/SettingsConfig.cpp` and `Save/SettingsCfg.cpp` (push/ret thunks),
`Physics/SmplFzxStateBlock.cpp` and `Input/DirectInput.cpp` (thunks), `Frontend/IntroSplash.cpp` (2 call-throughs),
`Util/PromoLoop_round72.cpp` and `Boot/SubsystemInit.cpp` (prose). `CROSS-TARGET:004cbd30` was narrowed to its real pair (RwWorldStream.cpp
vs TextureLoader_q6.cpp, render files, left). `exe_file` cleared on `0x004987b0`, `0x00485340`, `0x00431b10`.

Surveyed and left, with the reason:
- identical twins that each carry their own registry evidence, so deleting one orphans a key: Timer `0x0041eda0` / `0x0041f000` /
  `0x00420d40` / `0x00422120` (TimerInit vs TimerSlot / TimerSubarrayInit; the losers' installs are already commented out), `0x00431b70`
  (two getters), `0x004c5c00`, `0x004b4650`. Merge = retire the loser's registry key with its evidence row, one transaction each.
- real exe-vs-asi pairs that differ in behaviour: `0x0042f6b0` (RaceModes returns a value, the asi writes a global), `0x0042ad90` and
  `0x0045baa0` (the exe returns an index, the original an element or pointer), `0x00410510`, `0x0040e180` (camera), `0x0042fa00`, `0x00420de0`
  (fastcall x87 compare vs SSE), `0x0042d3e0` (RecordsZero clears the exe's own array, a different object), `0x00405890` (RuleEngine's Persist).
- `0x004177b0` (AiPreTick vs RuleEngine) and RaceCamera's Vec3Mag / Vec3Norm / RotateAboutAxis (`0x004c3ac0`, `0x004c39b0`, `0x004c4d20`);
  the camera ones are the next natural use of the d1r2 replay witness.
- skipped by rule: everything in Vehicle/, Collision/, LibRw/ and D3d9Render/.

## Builds and hygiene

Both targets build, rva-lint NEW=0, asm-lint OK. The C4244 warnings at AiStandalone.cpp:185/198 are on the baseline too. Builds are not
byte-reproducible run to run (two clean builds of the same tree hashed differently; the PE timestamp would explain it but was not
confirmed), so every measurement used the snapshot images in `images_sha.txt`. The shared `original\mashed_re_dev.asi` was restored to
`270e036f...` after both windows (`matches original: True`). Only PIDs this lane spawned were stopped. MB-sized step / gate / ptrace dumps
were deleted before the commit; the sha8 lines in the logs are the evidence.
