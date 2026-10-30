# PREREG D5-PARTMORPH-FLIPPREP: prepare the D-11077 flip (MASHED_PARTMORPH + MASHED_CAR_STATE0), do not flip

Branch `orch/D5-PARTMORPH-FLIPPREP` from `orch/integration` `0cd4241d4` (MASHED_SUBSTATE default ON since
D3-SUBSTATE-FLIP). Committed alone, before any arm.

## 1. What changes

The two knobs are the ones D5-PARTMORPH (`MASHED_PARTMORPH`, FUN_0041f770 @ 0x0041f770) and
D5-PARTMORPH-PRODUCER (`MASHED_CAR_STATE0`, the record-state-0 producer, FUN_0041ef60 @ 0x0041ef60 callers
FUN_00448220 / FUN_00446520) shipped default OFF (`TrackRenderer.cpp` `PartMorphEnabled` / `CarState0Enabled`).

A default flip in this tree is a row in `kFaith` (`mashedmod/src/mashed_re/Core/FaithDefaults.cpp`). Each row
also gives the opt-out `MASHED_NO_<name>=1` (the seed loop skips the row), and `MASHED_NO_FAITH=1` drops all
rows. So the opt-outs are the rows `{ "PARTMORPH", "1" }, { "CAR_STATE0", "1" }`.

- Commit ON: the two rows in `kFaith` behind `#if MASHED_FLIP_D11077` with `#define MASHED_FLIP_D11077 1`.
  That exe is "both defaults ON". Arms in section 3 run on it.
- Commit OFF (the branch head, what merges): the same source with `#define MASHED_FLIP_D11077 0`. The
  default stays OFF. The flip is a one-character change (0 -> 1), filed as a decision row.

## 2. Exes

- `EXE_ON` = build of commit ON (`MASHED_NO_DEPLOY=1`), copied to `D:/MashedWT/D5-PARTMORPH-FLIPPREP_data/exe_on`.
- `EXE_OFF` = build of commit OFF (the merge head).

Runner `verify/d5partmorphflip_20261030/run_arms.py` = `verify/savelink_20261023/run_arms.py` (the merge-train
runner) with OUT / ROOT paths changed and four stacks added, none with a behaviour knob:
- `LOG` = `MASHED_DBG_CAR_STATE0=<arm>.s0.csv`, `MASHED_DBG_PARTMORPH=<arm>.pm.csv` (diagnostic logs only).
- `NOPM` = `MASHED_NO_PARTMORPH` + LOG. `NOS0` = `MASHED_NO_CAR_STATE0` + LOG. `NOB` = both opt-outs + LOG.
Checkers: `verify/partmorph_20261028/law_check.py` (G4 / G5 `--dormant`) and `s0_check.py` (P3 replay), unchanged.

## 3. Gates

References: `re/orchestrator/merge_train.json` @ `0cd4241d4`: G-CTRL `8284be46 / ba407a16 / 31fe043e`
(H_DEF step / gates / ptrace), B `1d5bf4f9 x3` (B_DEF_K0 / K1 / K26).

| Gate | Exe | Arms | PASS when |
|---|---|---|---|
| F1 ON default unchanged | EXE_ON | H_DEF, B_DEF_K0 / K1 / K26 | all six sha8 = references |
| F2 ON default reaches the law | EXE_ON | H_LOG, B_LOG_K0 | sim sha8 = references; `law_check.py` G4 PASS with state-0 rows > 0 and vtx lerp mismatch 0 (with `--piz original/TOASTART/VEHICLES/Advantag.piz --entry ADVANTAGE0.DFF` when that is the drawn car's DFF, else without the DFF pair check, stated); `s0_check.py` PASS |
| F3 opt-outs | EXE_ON | B_NOB_K0, B_NOPM_K0, B_NOS0_K0 | sim sha8 = B_DEF_K0 reference. NOB: no `.s0.csv` L rows and pm log `--dormant` PASS (or absent). NOPM: s0 log has L rows, `s0_check.py` PASS, no pm rows. NOS0: no s0 L rows, pm `--dormant` PASS |
| F4 merge head default unchanged | EXE_OFF | H_DEF, B_DEF_K0 / K1 / K26 | all six sha8 = references |
| F5 merge head is OFF | EXE_OFF | B_LOG_K0 | no s0 L rows, pm `--dormant` PASS (or absent) |

GREEN = F1..F5 PASS. A piz-collision crash (U-9677) is a re-run. No original-side run (no audio endpoint).

## 4. Not in scope

The 13-track run13 table, the D2 arms and any visual parity check of the flipped exe are not gates here (the
step's gates are G-CTRL, B and G4). They are named in the decision row as the flip pass's work.
