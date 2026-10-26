# PREREG D4-WSF-SAVE-RT (2026-10-26): gamesave.bin whole-file round trip + videocfg/contcfg default bytes

Committed alone, before any arm. Extends `verify/savelink_20261023` (13x12 table, ghost block) to every byte of the file,
and adds first-run default-write checks (S-DoD criterion 5, scope per L-22 (a), `ROADMAP.md:152`).

## What the original does (read 2026-10-26, `Mashed_pool10` via `decomp_pc.py`, disassembly from `MASHED.exe.unpatched`)

File = 1:1 image of the buffer at `0x00803358`, size `0x24fa0` (`SaveLoad 0x00404e50` / `SaveWrite 0x00404f50`, `PUSH 0x24fa0`).

| file range | buffer | writer in `SerializeToBuffer 0x00404ee0` | reader in `DeserializeFromBuffer 0x00404e80` |
|---|---|---|---|
| `0x0..0x3` magic | `0x00803358` | `0xdeadbeef` (last store) | none |
| `0x4..0x24a3f` ghost block | `0x0080335c` | `REP MOVSD 0x928f` dwords from `*DAT_008a94a8` (`0x00404f2a..0x00404f34`), only if non-NULL | same copy back (`0x00404ec8..0x00404ed2`) |
| `0x24a40..0x24f5f` span | `0x00827d98` | `REP MOVSD 0x148` dwords from `0x007f0a40` (`0x00404f0a..0x00404f19`) | copy back (`0x00404e82..0x00404e91`) |
| `0x24f54..0x24f5f` (inside the span) | `0x007f0f54` | 12 bytes gathered from `0x007f105c` stride `0x4c`, BEFORE the span copy (`0x00404ef0..0x00404f01`) | zero-extended back to `0x007f105c` stride `0x4c` after the span copy (`0x00404ea0..0x00404eb2`) |
| `0x24efc` (inside the span) | `0x00828254` | `DAT_008a95ac`, stored AFTER the span copy (`0x00404f03`, `0x00404f23`) | `DAT_008a95ac = [0x00828254]` (`0x00404ebc..0x00404ec1`) |
| `0x24f60..0x24f9f` tail (0x40 bytes) | `0x008282b8` | nothing | nothing (whatever SaveLoad read stays in the buffer) |

`DAT_008a95ac` writers: `0x00404ec1` (load), `0x004098c6` / `0x004098eb` (store 0), `0x00409c5e` (store 0), `0x00409e30` (store 1),
`0x0040ad34` (`FUN_0040ad30(param)`, called at `0x0043fda6` with `[0x008990d8]`). No writer increments it. The standalone's
`SaveProgress` stores `++g_saveCounter` there (`Race/GameFlow.cpp:212`).

`0x007f105c` (stride `0x4c`) writers: `0x00404ea7` (load), `0x0043fdd2` (copy of 12 dwords from `0x00899160` when the screen
field `[0x67ed3c + screen*0x40] == 0x1c`, `0x0043fdb7`), `0x004401ec` / `0x0044072c` (XOR toggle), `0x00492550` (init to 1, 8
entries, `FUN_004924f0`). So those 12 file bytes are live frontend state, not a pure load/store echo.

videocfg.bin: `FUN_00499400(param_1)` is called by `FUN_004951f0` with `[0x006147bc]`; the boot patch `skip_selector` forces 0.
With `param_1 == 0` and `ConfigLoad 0x00498950` failing: `DAT_0077340c = 0`, `DAT_00773200 = FUN_00498e40()`, then
`FUN_00498ea0` fills the 0x200-byte buffer `0x00773208` (`+0x0` = 5, `+0x4` subsystem info via `[DAT_007d3ff8+0xcc]`, `+0x104`
`FUN_00498a00`, `+0x184` / `+0x188` subsystem / mode index, `+0x18c..+0x198` mode fields) and `ConfigSave 0x004989b0` writes it.
So the patched original writes a default videocfg.bin on a first run. Its bytes depend on the adapter's mode list.

contcfg%d.bin: the only writer is `FUN_00497230`, called only from `FUN_004982a0` when its dialog returns 1, and `FUN_004982a0`
is called only from the dialog proc `FUN_00498360`, opened by `FUN_00498510` only when `param_1 != 0` (patched to 0 by
`skip_controller_dialog`). So the patched original writes no contcfg file on a first run.

The standalone: no exe path calls `ConfigSave` (`Save/SettingsConfig.cpp:20-23`) and no exe source opens `contcfg%d.bin` for
writing. Its gamesave writer is `SaveProgress` (`Race/GameFlow.cpp:182-214`): default path = `BuildSpanFromProgress` (zero span
except col 4 / col 5 of 13 rows and the `+0x4ec` gate) + `BuildImage` (zero block and tail).

## Inputs (all copies, never `original/`)
- `F_P` = `D:/MashedWT/savelink_data/arm_P.bin` (shipped save with r2c1 = 1), `O_P` = `arm_R_written.bin` = the original's
  load -> SerializeToBuffer -> SaveWrite of `F_P` (savelink arm R, 2026-10-23).
- `F_G` = `D:/MashedWT/savelink_data/ghost_g1/gamesave_captured.bin` (original-written, ghost record, 2,888 non-zero block bytes).

## Arms
- **W0** (no run): whole-file compare `F_P` vs `O_P`.
- **W1** (one original boot, private root, no ASI loader): `F_G` as gamesave.bin, `nav_champ_probe.py --plan savewrite` -> `O_G`.
- **W2** (one original boot, private root, no ASI loader): no gamesave.bin, no videocfg.bin, no contcfg*.bin. `savewrite`
  at the mode-select menu -> `O_0` (first-run gamesave); videocfg.bin as written at boot -> `V_0`; list of contcfg*.bin.
- **S1** (standalone, default): new default-OFF harness `MASHED_SAVE_RT_IN=<in>` / `MASHED_SAVE_RT_OUT=<out>` (requires
  `MASHED_SAVE_PATH` = scratch): copies `<in>` to the save path, runs the live `Campaign_LoadProgress` + `Campaign_SaveNow`,
  copies the result to `<out>`, exits. Inputs `F_P`, `F_G`, none (first run).
- **S2** (standalone, new default-OFF knob `MASHED_SAVE_KEEP_IMAGE=1`): `SaveProgress` patches the loaded image in place
  (owned cells only, as `MASHED_SAVE_ORIGINAL` does) and stores the loaded `0x24efc` value unchanged. Same inputs as S1.
- **S3** (standalone boot to the frontend, private root without videocfg.bin / contcfg*.bin, 30 s): files created.
- **Gates**: G-CTRL `H_DEF`, B `B_DEF_K0 B_DEF_K1 B_DEF_K26` on the branch exe, knobs unset.

## Predictions
- P1 (W0): 151,456 bytes compared, differing bytes only inside `0x24f54..0x24f5f`.
- P2 (W1): 151,456 compared; the ghost block equal on all 150,076 bytes; differing bytes only inside `0x24f54..0x24f5f`.
- P3 (S1 vs original's write of the same input): RED. Differences in the block (all 2,888 ghost bytes for `F_G`), in span
  cells outside col 4 / col 5, and in `0x24efc` (stored + 1).
- P4 (S2): output == input on every byte (S2 is load-identity), so S2 vs the original's write differs only where W0 / W1 do.
- P5 (W2 vs S1-first-run): RED in the span (the original writes `FUN_004924f0`'s init values, the standalone zeros). Not
  fixed by S2 (no image to keep). Reported, not fixed here.
- P6 (W2 videocfg): `V_0` exists, 512 bytes, dword `+0x0` = 5. S3: the standalone writes no videocfg.bin -> RED, fix is a
  decision (new behaviour).
- P7 (W2 / S3 contcfg): neither writes a contcfg*.bin -> GREEN for the patched boot recipe.
- P8 (gates): unchanged against `merge_train.json` refs G-CTRL `5943c5f1/b493262b/09551e6a`, B `3a03670c/e85ea6fb/53778664`.

A prediction that fails is reported as it came out. A piz-collision crash (U-9677) is a re-run.
