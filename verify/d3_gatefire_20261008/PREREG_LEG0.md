# PREREG: D-11073 leg 0, item 2 (one original-side `orig_rampwatch.py --cam` capture)

Written 2026-10-08 BEFORE the run. Brief: `re/BRIEF_D11073.md` §2 leg 0 item 2. READ-ONLY on the
game (ReadProcessMemory poll). No build, no port change, no C-level.

## Static basis (pool clone `Mashed_pool0`, `re/tools/decomp_pc.py`, read-only)

- `FUN_00426c00` (6 B) returns `DAT_00644158` (plate `re/analysis/ai_update/0x00426c00.md`, operand
  at `0x00426c01`). `DAT_00644158` is **BSS** (`.data`, past SizeOfRawData), so the image cannot
  answer the track id. Only a live read can.
- `FUN_00442600` switch: case `0x1e` -> `&DAT_005f8d50`; the `default` arm -> `&DAT_005f82a8`.
  Guard `DAT_0089898c != 1`, then writes it 1 (BSS).
- `0x005f8d50` is file-backed (`.data` file offset `0x1f8d50`). `[0]` = 7.0. Rows of 12 floats from
  `+4`. Row 0 = `(1.5, 9.7, 34.5, 5.0, 180.0, -15.0, 1.0, 0.0, 0, 0, 0, 0)`. Column 7 over rows 0..6 =
  `0,1,1,1,2,2,3`.
- `FUN_00441b30(entry, row)`: `local_c/8/4 = row[0..2]`, then `FUN_004c51a0(entry+0xc, &local_c, 0)`.
  `FUN_004c51a0` (RwMatrixTranslate) with combine 0 writes `param_1[0xc..0xe] = v[0..2]`, i.e.
  **entry `+0x3c/+0x40/+0x44` = row[0], row[1], row[2]**. `entry+4` = `FUN_004a2c48()` (ftol; the
  operand is not shown by the decompiler, [UNCERTAIN] which row column it converts; column 7 is the
  only one matching the live types).
- `FUN_0041da90` (17 B): `*param_1 = DAT_0063d588`. This is the value `FUN_004103a0` compares
  against `_DAT_005ccdf4` = **1.86f** (`7b14ee3f`, `.rdata`).
- `FUN_0041d930` (336 B): `DAT_0063d584 += 1; DAT_0063d588 += _DAT_007f100c` at its tail.
  Its only caller is `FUN_0040fc00`, which calls it **unconditionally**.
- `FUN_0040fc00` sets `_DAT_007f100c = (float)ESI * _DAT_005cc948` before that call.
  `_DAT_005cc948` = 1/3000 (`3ec3ae39`). At 50 ticks/frame the step is **0.0166667**.
- `FUN_004111c0`: cases 3, 4, 5 and 6 each run their handler, then `FUN_0040fc00`.
- `FUN_0041d910` (21 B): `DAT_0063d584 = 0; DAT_0063d588 = 0`. Its caller on this path is `FUN_0040dbd0`
  (state 4), first statement.

**This refines the brief.** The brief predicts "`DAT_0063d588` ramps 0 -> 1.86 over phase 5". By the
reads above, the timer **also runs through phase 3**. It is zeroed only on the phase-4 frame, and it
**keeps running in phase 6** (no reset in `FUN_004103a0`'s exit arm).

## New columns (opt-in `--cam` only; default output unchanged)

`trk_644158` (i32), `g_898c` (i32), `t_d588` (f32), `n_d584` (i32), `e0_y` (f32, entry 0 `+0x40`).

## Gates

| id | prediction | PASS | what it CAN see | what it CANNOT see |
|---|---|---|---|---|
| G-NOPRESS | `--statediff-out` took | `in0..in7` = 0 on every sub-state-3 sample, AND phase 3 >= 600 frames by `clk_0ff4`/50 | the input bytes and the hold length | n/a |
| G-TRK | track id == `0x1e` (30) | `trk_644158` == 30 on every sample where `n994` == 7 | the global's value at each poll | the value at the one instant `FUN_00442600` ran. Covered only if the value is constant over the race |
| G-GUARD | the once-guard is set when the entries exist | `g_898c` == 1 on every sample where `n994` == 7 | value per poll | the frame it was set (a 60 Hz poll can miss it) |
| G-TBL | entry 0's initial position = row 0 | at the FIRST sample with `n994` == 7: abs(`e0_x` - 1.5), abs(`e0_y` - 9.7), abs(`e0_z` - 34.5) each < 0.05. Also report every `e<k>_type` against column 7 | the position at the first poll after init, which may be up to ~2 frames of motion late | the init write itself. A FAIL with a large delta on an early sample means motion, not a wrong table. Report clk distance from phase-3 start |
| G-STEP | `t_d588` advances 0.0166667 per frame | median of `d(t_d588)/d(n_d584)` over consecutive same-phase sample pairs within 1e-4 of 0.0166667. `n_d584` is the frame witness, because the poll is not atomic | the ratio per sample pair | intra-frame order |
| G-RUN3 | timer runs in phase 3 | `t_d588` is non-decreasing over sub-state 3, and its max there is > 1.0 | sampled values | whether something resets it before phase 3 (reported, not gated) |
| G-RESET | phase 4 zeroes it | the first sub-state-5 sample (or the 4 sample if caught) has `n_d584` <= 3 and `t_d588` <= 0.05 | the value after the reset | the reset frame itself |
| G-EXIT | phase 5 ends at 1.86 | over sub-state-5 samples: max `t_d588` in [1.80, 1.8667 + 1e-3], max `n_d584` <= 112; the first sub-state-6 sample has `t_d588` >= 1.86 | sampled bounds | frames between polls |
| G-RUN6 | no reset at the 5 -> 6 exit | `t_d588` keeps growing in sub-state 6: its value on the last sub-state-6 sample > 1.86 + 1.0 | sampled values | n/a |

Predicted phase-5 length: the handler reads `j` steps on its `j`-th phase-5 frame and exits when
`j * 0.0166667 >= 1.86`, so on **j = 112** (112/60 = 1.8667; float accumulation error over 112
adds is far below the 0.0067 margin). Sub-state-5 samples carry `n_d584` in 1..112.

Any FAIL is reported as measured and stops leg 1 planning on that point. No re-run to "fix" a FAIL
unless G-NOPRESS fails (a harness void, not a result).

Scenario: `--track 0 --mode 10 --cars 4 --car 0 --poke-ctrl-slots --hold 30 --statediff-out <scratch>`,
`--hz 60`, the same as `RESULT_CADENCE.md` run 3.
