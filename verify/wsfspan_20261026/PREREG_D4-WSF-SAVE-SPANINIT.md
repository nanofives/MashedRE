# PREREG D4-WSF-SAVE-SPANINIT (2026-10-26): FUN_004924f0's span init as the first-run rebuild base

Committed alone, before any arm. Branch `orch/D4-WSF-SAVE-SPANINIT`, based on `orch/D4-WSF-SAVE-24EFC` @ `748ca9fb7`
(24EFC is not in `orch/integration` yet) with `orch/integration` @ `96e7f1ad2` (D4-WSF-SAVE-FLIP, KEEP_IMAGE default ON)
merged in, so the tested save code is the post-train state.

## Change
`Save/GameSaveFormat.h` `InitSpanDefaults()`: buffer form of every FUN_004924f0 write that lands in the span
`0x007f0a40..0x007f0f5f` (disassembly `0x004924f0..0x0049268d`, cited per line in the header), plus the 8 gather bytes
(`0x00492550` writes `[0x7f105c + i*0x4c] = 1`, serializer gathers their low bytes at `0x00404ef2`). `Race/GameFlow.cpp`
`BuildSpanFromProgress` starts from it instead of `memset 0`. With KEEP_IMAGE on, the rebuild runs only when there is no
valid file (first run). No knob: the gates never save, and the rebuild is not on any gate path.

Static check already done (`model_spaninit.py` on `O_0.bin`): the model matches every span byte of the original's first-run
file except the `0x007f0cb0` rows (252 bytes, overwritten later by the Lua binding `Set_Track_Powerups` `0x0047b280`), gather
bytes `0x24f54..0x24f56` (live clear) and `0x24efc` (`DAT_008a95ac`, not span).

## Predictions
| id | arm | prediction |
|---|---|---|
| Q1 | N_0: new exe, `MASHED_SAVE_RT_IN=none` | output == `predict.py`'s image byte for byte; vs `O_0` **269** differ (table 104, span_rest 161, state 1, gather 3). Before: `SF_0` (24EFC exe) 343, `S1_0` (RT exe) 342 |
| Q2 | N_0 residual | exactly: 252 in `0x24cb0..0x24db3` (Set_Track_Powerups rows), 12 at col 4 rows 1..12 (`0x24a80 + 0x30*k`, port progression 0 vs init 2), 1 at `0x24f2c` (port gate 1 vs 0), 3 at `0x24f54..0x24f56`, 1 at `0x24efc` |
| Q3 | N_P / N_G / N_1: new exe, default (KEEP_IMAGE on), inputs `F_P` / `F_G` / `F_1` | output == input byte for byte (this step does not touch the keep path) |
| Q4 | N_P0: new exe, `MASHED_SAVE_KEEP_IMAGE=0`, `F_P` | informative: differs from the 24EFC exe's `SF_P` only inside the span (`0x24a40..0x24f5f`), at bytes `InitSpanDefaults` sets and the overlay does not own |
| Q5 | gates, new exe, no env | G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (`merge_train.json` refs) |

RED on Q1 / Q2 / Q3 / Q5 = the change is wrong or reaches a path it should not. Raw files to
`D:/MashedWT/D4-WSF-SAVE-SPANINIT_data/`.
