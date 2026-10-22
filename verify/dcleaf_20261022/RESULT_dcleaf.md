# RESULT dcleaf: the vehicle-side dual copies 0x0046dbe0, 0x00468b40, 0x00442c80 (+ 0x00442ce0), 0x0040e350, 0x0046b1c0 (2026-10-22)

Branch `work/dcleaf` from `7160a38d4`. Private roots `D:\MashedWT\dcleaf_root` (original-side A/B) and `D:\MashedWT\dcleaf_root13`
(13-track replay); evidence `D:\MashedWT\dcleaf_data` (`MOVED_OFF_GIT.txt`). Baseline (no change) exe `05a5a446` / asi `5894a27f`
(`base`); entries 1-4 exe `80c3ef57` / asi `d1b49695` (`d1`); final exe `766099a0` / asi `b311c53a` (`final`). No knob was added
or changed, so there is no PREREG: every exe change is knob-free and every reference was predicted unchanged.

## Verdicts

| entry | static diff (checked against `MASHED.exe.unpatched`) | what changed | default vs original |
|---|---|---|---|
| 1. `0x0046dbe0` | IDENTICAL (`mov eax,[esp+4]; imul eax,eax,0xd04; mov eax,[eax+0x8815a8]; ret`) | one body, `Vehicle/VehicleState.cpp:VehicleRacePositionGet`, record base through `VehRecord`; exe copy `ForceIntegratorLeaves.cpp:CarContactCount` deleted | .asi code instruction-identical to the C4-measured one; path1 8/8 |
| 2. `0x00468b40` | IDENTICAL (flag test `[eax+0x80]` first at `0x00468b50`, key `[edx+0x34]`, 0x20 slots) | one body, `VehicleState.cpp:VehicleContactHistoryLookup`; exe copy `Collision/CarWorldContacts.cpp:ContactHistoryLookup` deleted, `ContactSolvers.h` forwards | .asi code instruction-identical; path1 7/7 |
| 3. `0x00442c80` + `0x00442ce0` | IDENTICAL logic; the exe and .asi differ only in INPUTS (mode word U-9784, `DAT_007f0fd0`, refdist `0x008989b0`, ref car `0x008989c8`) | `Vehicle/ForceIntegratorLeaves.cpp` in BOTH rsp files, thunks only for those inputs; the .asi installs both (`RubberBandGateEntry`, `RubberBandGripEntry`); `Util/UtilMid.cpp:ModeGatedPlayerCheck` deleted | clean: in-process A/B 0 diff (section 2) |
| 4. `0x0040e350` | not a pair: `Vehicle/PhysicsChainHooks.cpp:118` is a call-through, bound by the scanner's second pass through the header `// FUN_0040e350  Fi_GameMode` | comment reworded; key dropped | n/a |
| 5. `0x0046b1c0` | arithmetic IDENTICAL at PC_24 (same groupings `(A+B)*0.5`, `(A+A+B)*0x3eaaaa9f`; `0x005ce034` = `0x3eaaaa9f` read from the image); split was the record base and the return value | `Vehicle/VehicleContactHull.cpp` in BOTH rsp files, record base through `VehRecord`, returns the original's 1 / 0; .asi installs it as `VehicleSlotAabbExpand`; naked copy `Vehicle/VehicleSlotAabbExpand.cpp` deleted | clean: in-process A/B 0 diff (section 2) |

Lint `scripts/lint_rva_bodies.py`: **18 findings, all allowlisted, 0 NEW** (was 23 findings / 22 lines); the five keys are gone from
`re/tools/dual_copy_allowlist.txt`. `scripts/test_rva_body_scan.py` 6/6. Honest measure (`hooks.csv` C3/C4 with `exe_file != file`):
**164 -> 160** (31 -> 28 with a .cpp `file`).

Static notes from this lane, all cited from the binary: `0x00442ce0` never reads its second argument (every return is `factor *
[esp+0x10] + 1.0`, the default factor is the zeroed local at `0x00442cfe`); `FUN_0040e340` is `mov eax,[0x008a94d0]; ret`, so the
exe's no-op `Fi_GameModeTick` is exact (the result is discarded at `0x00442d06`). Call sites (scan of `.text`): `0x00442c80` from
`0x0041340a` and `0x0046e233`; `0x00442ce0` from `0x0046e20f`; `0x0046dbe0` from `0x00442d28` / `0x00442d35`; `0x00468b40` from
`0x0046942d` / `0x0046cfd2`; `0x0046b1c0` from `0x0040ed62` / `0x0040ff86`. The record `+0x08` word the grip reads is written only by
`FUN_0046dc00`, called from `0x0043dd4c..0x0043fac2`.

## 2. In-process A/B on the original (dev .asi, hooks live through `MASHED_HOOK_ONLY`, private root)

`MASHED_LEAF_SELFTEST=1`, `scenario_launch.py --hooks <rvas> --cars 4` (track 0 mode 10 rule 0; Warzone `--track 11`). Per call: the
original twice through a trampoline (`noo`), then the shared body on the same inputs (`ndiff`); the game gets the shared body's
value. `0x0046b1c0`: snapshot of the 0xa8 output bytes put back before each run, 42 dwords + return compared. Every mismatch is
logged regardless of the log cap. Logs `D:\MashedWT\dcleaf_data\leaf_ab\*.leaf.log`.

| run | hooks | `0x00442c80` calls / `noo` / `ndiff` (return 1) | `0x00442ce0` calls / `noo` / `ndiff` | `0x0046b1c0` |
|---|---|---|---|---|
| `t0` 60 s | c80, ce0 | 28000 / 0 / 0 (1653) | 13000 / 0 / 0 | - |
| `t0_full` 120 s | c80, ce0 | 41000 / 0 / 0 (3110) | 22000 / 0 / 0 | - |
| `wz_full` 120 s | c80, ce0 | 53000 / 0 / 0 (1986) | 24000 / 0 / 0 | - |
| `t0_sweep` 40 s, `SWEEP=12` | c80, ce0 | 18000 / 0 / 0 (1050) | 9000 / 0 / 0; **sweep 1536 cases / 0 / 0, 516 on a tier value** | - |
| `hull_t0` 40 s (final .asi) | b1c0, c80, ce0 | 18000 / 0 / 0 (784) | 9000 / 0 / 0 | 20 calls / 0 / 0 |
| `hull_wz` 40 s (final .asi) | b1c0, c80, ce0 | 19000 / 0 / 0 (1378) | 9000 / 0 / 0 | 24 calls / 0 / 0 |

Counts are the last 1000-step summary of each run (lower bounds). Game-mode word seen by the gate: 3, 4, 5, 6.

**Coverage gap (U-9787):** no natural `0x00442ce0` call took a tier branch. Refdist == 0 occurs (the leader), but record `+0x08` of the
car and of the reference car was 0 / 0 or 1 / 1 on every call, and `band` was 1.0 on every call. The tiers are covered by the
in-place sweep (`MASHED_LEAF_SELFTEST_SWEEP`): on the first 12 calls, all 16 `(i1, i2)` pairs with refdist 0, times 8 band values
(live 1.0, 0.2, pi, 0.001, -2.5, FLT_MAX, the smallest denormal, +inf), seeded in place and put back. Synthetic seeding:
path1-grade for those branches, not a natural sample.

**path1** (`run_diff.py`, worktree .asi): `mode_gated_player_check` (export now `RubberBandGateEntry`), `vehicle_race_position_get`,
`vehicle_contact_history_lookup`, `vehicle_slot_aabb_expand`: all GREEN, 0 mismatches. path2 (`run_verify_hook.py`) was not run:
from a worktree it maps the shared deployed .asi first (`run_verify_hook.py:42`); the installs are shown live by the runs above
(every sampled call went through the new entry points).

## 3. Gates on the exe (recipe F), final exe `766099a0`, knob-free

| gate | reference (dcphys2 / p2flip) | final |
|---|---|---|
| G-CTRL x2 (`H_DEF-1/-2`) | `c3bfcef3 / 4ed4849b / 4c3548a1` | **equal, x2** (baseline exe `05a5a446` and entries 1-4 exe `80c3ef57`: equal too) |
| B K0 / K1 / K26 | `4985eb3b / b6e3d073 / 6717c15e` | **equal** |
| (b) envelopes `vsum.py` | 9/9 | **9/9** (`vsum_b_def.txt`) |
| (e) `ai_speed_env.py --check` K0 | PASS 3/3, launch 1425.7 / 2052.5 / 2055.0 | **PASS 3/3, same values** (`e_check_def.txt`) |
| D2 `score_d2_v2.py port`, 3 runs alone | PASS 3/3 | **PASS 3/3**: slip 0.1900 / 0.2550 x3, drvmed 1965.67 / 1965.35 / 1964.25, H1 398/400 (`score_d2_def.txt`) |
| 13 tracks `run13.py` + `cites_rres.json` | 39 frames, 878 rows, 0 RED | **score table identical** to `verify/dcphys2_20261022/tracks_def_score.txt`, gates PASS (`tracks_def_score.txt`) |

## 4. Trackers (re-classify, `apply_trackers.py`)

| RVA | level | file / exe_file |
|---|---|---|
| `0x0046dbe0` | C4 kept | exe_file -> `Vehicle/VehicleState.cpp` (= file) |
| `0x00468b40` | C3 kept | exe_file -> `Vehicle/VehicleState.cpp` (= file) |
| `0x00442c80` | C4 kept, new evidence above; plate `[UNCERTAIN]` x2 resolved by citation | file = exe_file = `Vehicle/ForceIntegratorLeaves.cpp` |
| `0x00442ce0` | **C2 -> C4** (hook live, canonical race, clean A/B, no stub in the body; natural tier coverage U-9787) | file = exe_file = `ForceIntegratorLeaves.cpp` |
| `0x0046b1c0` | **C3 -> C4** (hook live, canonical race, clean A/B on every spawn call, path1 7/7) | file = exe_file = `Vehicle/VehicleContactHull.cpp` |
| `0x0040e350` | C3 unchanged (row untouched; the exe stand-in remains U-9784) | - |

New row: U-9787 (coverage, `0x00442ce0` tiers).

## 5. U-9784 (the exe's game-mode word): static read, no change

All exe hosts of `FUN_0040e350` return 6 in the default build: `Fi_GameMode` (`ForceIntegratorStubs.cpp:124`, 6 or the d2engage hold
tick), `aib_game_sub_mode` (`TrackRenderer.cpp:171`, 6 unless `MASHED_SUBSTATE` or `MASHED_ROUND_END7`, both default OFF), `PK_SubMode`
/ `TG_SubMode` (forward to it). The producer of a real `DAT_0063ba8c` in the exe already exists behind `MASHED_SUBSTATE`. Binding
`Fi_GameMode` to it is a behaviour change on every A-chain tick below 6 (the original's word reads 3, 4, 5 in these runs), so it is a
knob + PREREG lane, not a cheap fold. Left as filed.

## 6. Decisions for Mariano

1. Accept `0x00442ce0` at C4 with U-9787 (the tier branches covered by seeding only), on the dcphys2 precedent (U-9786)? Or hold it at
   C3 until a natural race drives `FUN_0046dc00`?
2. Not merged. The shared `original/mashed_re_dev.asi` is not redeployed.
3. U-9784 producer lane (section 5): open it next, or leave it?
