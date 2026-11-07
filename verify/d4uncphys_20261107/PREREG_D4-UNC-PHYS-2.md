# PREREG D4-UNC-PHYS-2 (2026-11-07): GAMEFLOW stops publishing 0x0067ea94 from the course (U-9783)

Committed alone, before any arm of this change runs. Branch `orch/D4-UNC-PHYS-2` from `main` @ `51e8a7a30`.

## Basis (original side, measured this session before this PREREG)

No-hook original (`MASHED_RE_NO_AUTO_HOOK=1`), private root `D:\MashedWT\D4-UNC-PHYS_root`, audio endpoints present.

1. Harness route (`scenario_launch.py`, mode 10, tracks 0 / 3 / 10 / 11, car 0 and car 2): `0x0067ea94` = 0 from
   the menu through the race, no write to it (hardware watch), A3 `0x0046b540` writes `_DAT_00613108` = 105.0
   (type 6) on every track.
2. Harness with `0x0067ea94` = 3 / 10 / 11 written before the launch: 100.0 / 42000.0 (type 18), 95.0 / 40000.0
   (type 12), 100.0 / 45000.0 / 1.4 (type 66). The chain `(*(int**)0x005f2770)[v][1]` is the image table, with no
   write to `0x005f2770` from attach through the race.
3. Menu route (`statenav.py`, Quick Battle, mode 10): `0x0067ea94` = 0, 105.0. Its only stepping is the setup
   screen option (screen `0x18` row 3, controls 9 / 10), filtered by `0x007f0a44 + i*0x30`. On this install's save
   only row 0 is available. With rows 1 and 3 made available (CONTRIVED poke) and row 3 picked: `0x0067ea94` = 3,
   A3 gives 100.0 / 42000.0 (type 18), while `0x0067f17c` = `0x0063ba7c` = `0x0063ba78` = 0.

So the original keeps the handling row (`0x0067ea94`) and the course row (`0x0063ba7c`) as separate selections.
`MASHED_PORTSTATE_GAMEFLOW` (default OFF) currently writes `0x0067ea94 = k` (the loaded course's row) at course
load (`Race/GameModeState.cpp`, `GameModeState_OnCourseLoad`), which no original writer does.

## Change

`GameModeState_OnCourseLoad` no longer writes `0x0067ea94`. `0x0067f17c`, `0x0063ba7c` and `0x0063ba78` are still
written as before. Comment-only edits in `Race/GameModeState.h` and `D3d9Render/TrackRenderer.cpp`. Exe only,
inside the default-OFF knob. No default changes.

## Arms

`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26 H_PSG B_PSG_K0 B_DEFA_K0 B_PSGA_K0` on the
exe built from this branch (`MASHED_NO_DEPLOY=1`), private root, `test-slot` outside, `mashed_lock` inside.

## Predictions

| id | arm | prediction |
|---|---|---|
| P1 | H_DEF, B_DEF_K0/K1/K26 (knob OFF) | G-CTRL `8284be46 / 67931834 / 31fe043e`, B `1d5bf4f9` x3 (the `merge_train.json` refs) |
| P2 | H_PSG, B_PSG_K0 (Training, knob ON) | witness `course=30 k=0 mode=10 row=0 qbrow=0 sel=0 ld=0 v_rec_id=30 v_key=6` and `a3 mode=10 v=0 type=6`; step / ptrace hashes equal to P1's, B_PSG_K0 `1d5bf4f9` |
| P3 | B_PSGA_K0 (Arctic, knob ON) | witness `course=0 k=3 mode=10 row=3 qbrow=0 sel=3 ld=3 v_rec_id=30 v_key=6` and `a3 mode=10 v=0 type=6` (was `qbrow=3 ... v_key=18`, `v=3 type=18`) |
| P4 | B_PSGA_K0 vs B_DEFA_K0 (report only) | H0: equal step hashes, i.e. the handling row was the only GAMEFLOW difference on Arctic's first 1200 frames. A difference is reported with its first differing frame, not counted as a failure of P1..P3 |

P1 failing means the layout change moved a default gate: the change is then not shippable as is (report RED).
P2 or P3 failing means the change does not do what it states (report RED).
