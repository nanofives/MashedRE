# PREREG D4-FU-SPANINIT-WRITERS (2026-10-27): first-run save writers + boot span seed, default-OFF knobs

Base `orch/integration` @ `1158f5c1c`. Code commit `4f61c31a1` (before this file). Exe built from it with
`MASHED_NO_DEPLOY=1`, sha8 `dc0f596d`. Follows `verify/settrackpu_20261026/RESULT_D4-FU-SETTRACKPU.md` (Next 1..3)
and `verify/fu24efc_20261027/RESULT_D4-FU-24EFC-WRITER.md` (branch `orch/D4-FU-24EFC-WRITER` @ `1174fc7da`).

## What changed (both knobs default OFF)

- `MASHED_SAVE_FIRSTRUN_WRITERS=1` (`Race/GameFlow.cpp`):
  - (a) `Campaign_LoadProgress` with no save file (`nRead == 0`) sets `g_saveCounter = 1`. Original: `0x00409e30`
    `MOV [0x008a95ac],1`, case 16 of switch `0x00409b00`, which runs after the case-14 creation write `0x00409db8`.
    Serialize copies `[0x008a95ac]` to file `0x24efc` (`0x00404f03` / `0x00404f23`). The creation write is not ported.
  - (b) the rebuild path (`SaveProgress` after `BuildSpanFromProgress`) stores 0 at span `+0x514 + i` for each slot
    `i` in 0..3 with a derived type != 0. Original: `FUN_004967e0` calls `FUN_00496530` (`0x00496820`) for each `i`
    with `FUN_00497450(i) != 0` (`[0x007e96fc + i*0x200] != 0`). `FUN_00496530` zeroes record `0x7f1038 + i*0x4c`
    (`rep stosd` `0x00496568`, ecx `0x13`). Serialize gathers byte `[0x7f105c + i*0x4c]` (record `+0x24`) into
    `0x7f0f54 + i` (`0x00404ef0` / `0x00404ef2`). Slot types: `DeriveInputSlots` (`exe_main.cpp`, port of
    `FUN_00498510`: `i < J` -> 1, `i == J` -> 2, else 0, J = attached game controllers).
  - The save round-trip harness (`MASHED_SAVE_RT_IN`) runs before `WinMain`. With the knob on it derives the slots
    window-less first (`ExeInput_DeriveSlotsHeadless`, same `DeriveInputSlots`), and logs the `input slots:` line.
- `MASHED_SPAN_BOOT_SEED=1` (`exe_main.cpp` boot save restore, `Frontend/MenuNavSM.cpp`): when the boot restore did not
  load a save, the live span (`g_save_span` and `0x007f0a40`) gets `InitSpanDefaults` (`FUN_004924f0`) + the
  `POWERUPS_ALL.LUA` rows (`0x0047b280`), with the same re-derive of the menu gate fields as `Nav_GameStateLoadSave`.
  `MASHED_SPAN_BOOT_SEED_DUMP=<path>` (arm test only) writes the 0x520 seeded bytes and exits.

## Arms (driver `run_spw.py`, private root `D:/MashedWT/D4-FU-SPANINIT-WRITERS_root`, lock + test-slot)

Let `A` = the set of slots `i` with type != 0 in the K_0 run's logged `input slots:` line. The original's first-run
file `O_0` (`D:/MashedWT/D4-WSF-SAVE-RT_data/O_0.bin`) has gather bytes `0x24f54..57` = `0 0 0 1` (slots 0..2 active).

| pred | arm | prediction |
|---|---|---|
| P1 | `N_0`: knob off, first-run round trip | byte-identical to D4-FU-SETTRACKPU `N_0` (sha `604d373f`) |
| P2 | `K_0`: `MASHED_SAVE_FIRSTRUN_WRITERS=1`, first run | vs `N_0`: differs only at `0x24efc` (0 -> 1) and `0x24f54 + i` for `i` in `A` (1 -> 0). vs `O_0`: `13 + m` differ, `m` = number of `i` in 0..3 where (`i` in `A`) != (`O_0` gather byte is 0). If `A` = {0,1,2}: **13** (col 4 rows 1..12 = 12, gate `0x24f2c` = 1), `0x24efc` and gather 0 |
| P3 | `K_P`, `K_G`, `K_1`: knob on, loaded files, default KEEP_IMAGE | output == input byte for byte |
| P4 | `K_P0`: knob on, `MASHED_SAVE_KEEP_IMAGE=0`, `F_P` | vs D4-FU-SETTRACKPU `N_P0`: differs only at `0x24f54 + i` for `i` in `A` where `N_P0` has non-zero. `0x24efc` unchanged (loaded value) |
| P5 | `S`: `MASHED_SPAN_BOOT_SEED=1`, `MASHED_SAVE=<absent path>`, boot to the restore, dump | dump vs `O_0` span (`0x24a40..0x24f5f`): **4** bytes differ, at span `+0x4bc` (`0x24efc`, O_0 1, dump 0) and `+0x514..+0x516` (gather 0..2, O_0 0, dump 1). Log line `span boot seed:` present |
| P6 | `GATES`, no env | `merge_train.json` refs: G-CTRL `5943c5f1 / 364350ae / 52052890`, B `3a03670c / e85ea6fb / 53778664` |

Failure rules: P1 or P6 failing = RED (default moved). P2 / P4 failing with `A` as logged = RED for that writer.
P5 failing = RED for the seed. A piz-collision crash (`+0x1509b4`, `+0xe66db`) is a re-run.

## Not claimed

- No flip. Any default change is a decision row.
- (a) is modelled for a missing file only. What the state machine does with an invalid file (`nRead != 0`, bad magic)
  or a loaded file before state 16 is `[UNCERTAIN]` (state 1 to 12 sequence not decoded, RESULT_D4-FU-24EFC-WRITER).
- (b) models the record clear's effect on the save, not the per-frame records themselves.
- The gather result depends on attached controllers. The original's `O_0` was captured with 3 active slots.
- No tracker change. `0x00496530` / `0x004967e0` / `0x00409e30` statuses do not move here.
