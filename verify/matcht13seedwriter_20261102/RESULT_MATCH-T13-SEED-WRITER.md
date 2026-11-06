# RESULT MATCH-T13-SEED-WRITER (run 33, 2026-11-06)

**Verdict: GREEN.** Every prediction in `PREREG_MATCH-T13-SEED-WRITER.md` holds. Default gates unchanged.
`MASHED_DIFF360_LIVE` stays default OFF. No default flipped.

## What changed (source, commit 983b2ccbf)

- `0x00414060` and `0x00413fa0` compile the byte-matched `re/match/s03` sources as their one body in both targets
  (`Match/MatchS03_00414060.cpp`, `Match/MatchS03_00413fa0.cpp`).
- `DAT_0089a360` producer chain behind `MASHED_DIFF360_LIVE=1`: `FUN_00414060` at StartMatch (`0x0040ffc7` analogue),
  `FUN_00414120` when the nav reaches screen 1 (`0x0043d3d0`), the `DAT_0089a37c` term of `FUN_00417750` at
  `DAT_0063ba8c` = 9, `DAT_0067f17c` bound to the challenge row at launch, and dev inputs `MASHED_DIFF360_ROW` /
  `MASHED_DIFF360_INIT_TABLE` for the arms.
- Default path: the race-load call restores `DAT_0089a360`, so only the step-6 stores land (same as the old hand tail).

## Build

Branch merged `orch/integration` (68f5865c1) as 8f04a2fae, rebuilt with `MASHED_NO_DEPLOY=1`.
`mashed_re.exe` sha256 `21dfac342e46d746...`, copied to `D:/MashedWT/MATCH-T13-SEED-WRITER_data/exe33/`.
Run 31's arms (2026-11-02) all failed CreateDevice `0x8876086A`. Run 33 booted every arm.

## D0 gates (knob unset, `run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`)

| arm | file | hash | ref (`merge_train.json`) |
|---|---|---|---|
| H_DEF | step.csv | 8284be46 | 8284be46 |
| H_DEF | gates.csv | ba407a16 | ba407a16 |
| H_DEF | ptrace.log | 31fe043e | 31fe043e |
| B_DEF_K0 / K1 / K26 | step.csv | 1d5bf4f9 x3 | 1d5bf4f9 x3 |

## L arms (knob ON) vs original OEV arms (`verify/t13seed_20261102/seeds_scored.txt`)

| arm | a360_raw raceinit (pred) | timer_raw (orig) | bonus_raw (orig) | result |
|---|---|---|---|---|
| L_SANDS | 40c00000 (40c00000) | 417b3333 (417b3333) | 408b3333 (408b3333) | hold |
| L_NEU | 40800000 (40800000) | 418e6666 (418e6666) | 40799999 (40799999) | hold |
| L_CITY | 40e00000 (40e00000) | 418f3333 (418f3333) | 40be6667 (40be6667) | hold |

Both `[rule10-seed]` lines per arm (site=round, site=setrule) carry the same values. `a37c_raw` = 0 on every line.

## C arms (knob unset) = H0-CTRL

| arm | live360_raw | timer_raw (pred) | `[diff360]` lines |
|---|---|---|---|
| C_SANDS | 40200000 | 41860000 (41860000) | none |
| C_NEU | 40200000 | 41920000 (41920000) | none |
| C_CITY | 40200000 | 419a0000 (419a0000) | none |

## Witnesses

- W-NAV: every L arm logs `tableinit_dev` once and `tableinit_navinit` once. Holds.
- W-37C: no `[diff360] roundend37c` line in any L arm. Not reached.

## Stacking

Stacked on orch/MATCH-T13-SEED @ 8c8373015, which is **not** merged into `orch/integration` (merge-base check
2026-11-05). That row is at `review` (L-85 = c). This branch contains it, so merging this branch carries 8c8373015.

## Evidence files

Committed: `larms_run33.txt` (L/C arm lines), `raw_sha256.txt`. Raw data in `D:/MashedWT/MATCH-T13-SEED-WRITER_data/`
(`arms_g33/`, `larms/`, `run33c.txt`); SHA-256 of each is in `raw_sha256.txt`.
