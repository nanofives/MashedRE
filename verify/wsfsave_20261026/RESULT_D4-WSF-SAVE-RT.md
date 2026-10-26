# RESULT D4-WSF-SAVE-RT (2026-10-26): whole-file save round trip + videocfg/contcfg default bytes

PREREG `PREREG_D4-WSF-SAVE-RT.md` (commit `5b1e7f2d9`, alone, before any arm). Driver `run_wsf.py`, comparator
`re/tools/save_fullfile.py`, full compare output `compare.txt`. Branch exe sha8 `071e296b`. Private root
`D:/MashedWT/D4-WSF-SAVE-RT_root` (copy of `original/`, ASI loader renamed to `dinput8.dll.off`, so no `.asi` loads into the
original), restored to the shipped save and canonical videocfg afterwards. `original/` untouched.

## Verdict: DECISION

The original's load -> SerializeToBuffer -> SaveWrite is whole-file identity except 1 byte of live frontend state. The
standalone's default writer is not: 333 / 3,219 / 342 bytes differ. The new default-OFF knob `MASHED_SAVE_KEEP_IMAGE=1`
makes the standalone's write identical to its input on every byte, and so equal to the original's write except that same 1
byte. Flipping it is a decision. videocfg.bin: the original writes a 512-byte default on a first run, the standalone writes
nothing (RED, a fix is new behaviour). contcfg%d.bin: neither writes one (GREEN). Default gates unchanged.

| pred | arm | result |
|---|---|---|
| P1 | W0 `F_P` -> `O_P` (original) | **held**: 151,456 compared, 1 differs, `0x24f55` 1 -> 0 (gather) |
| P2 | W1 `F_G` -> `O_G` (original, ghost file) | **held**: 151,456 compared, 1 differs, `0x24f56` 1 -> 0 (gather); ghost block 150,076 of 150,076 equal (2,888 non-zero) |
| P3 | S1 standalone default vs original write | **held (RED)**: `F_P` 333 differ (table 34, span_rest 292, `0x24efc` 0 -> 1, gather 6); `F_G` 3,219 differ (block 2,888 = every ghost byte zeroed, table 33, span_rest 292, `0x24efc` 0 -> 1, gather 5). `S1_P` == `S1_G` byte for byte |
| P4 | S2 `MASHED_SAVE_KEEP_IMAGE=1` | **held**: `S2_P` sha == `F_P`, `S2_G` sha == `F_G`. vs the original's write: 1 byte each, the same gather byte as W0 / W1 |
| P5 | W2 `O_0` vs S1 first run | **held (RED)**: 342 differ (table 45, span_rest 292, gather 5); `0x24efc` 1 in both. S2 first run == S1 first run (no image to keep) |
| P6 | W2 videocfg / S3 | **held (RED)**: `V_0` 512 bytes, `+0x0` = 5. Standalone booted 30 s without videocfg.bin: alive, wrote only `mashed_re.log` |
| P7 | W2 / S3 contcfg | **held (GREEN)**: no contcfg*.bin from either |
| P8 | gates, knobs unset | **held**: G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (= `merge_train.json` refs) |

## Findings

- **`0x24efc` is not a counter in the original.** `DAT_008a95ac` writers: `0x00404ec1` (load), `0x004098c6` / `0x004098eb` /
  `0x00409c5e` (0), `0x00409e30` (1), `0x0040ad34` (`FUN_0040ad30([0x008990d8])`, call at `0x0043fda6`). None increments. Its
  reader `0x004099a0` gates the autosave start. The standalone writes `++g_saveCounter` (`Race/GameFlow.cpp:212`, also the
  `MASHED_SAVE_ORIGINAL` path), so one standalone save of a 0-valued file hands the original a non-zero value.
- **The gather bytes are live state.** `0x24f54..0x24f5f` come from `0x007f105c` stride `0x4c` (`0x00404ef0`). Each boot to the
  mode-select menu cleared one more of them (W0 index 1, W1 index 2). Which writer did it (`0x0043fdd2`, `0x004401ec`,
  `0x0044072c`) is `[UNCERTAIN: no --watch-write run]`. The standalone has no `0x007f105c` model, so even the knob keeps the
  loaded byte.
- **Ghidra's decomp of `0x00404ee0` orders the `0x00828254` store before the span copy; the disassembly stores it after**
  (`0x00404f23` after `REP MOVSD` at `0x00404f19`). The disassembly is what `GameSaveFormat.h` already follows.
- **Tail `0x24f60..0x24f9f`**: no writer in SerializeToBuffer; zero in every file here.
- **First-run gamesave**: the original writes `FUN_004924f0`'s init values (cols 1 / 3 / 4 / 11, the `0xffffffff` rows at
  `0x007f0cb0`, `9` / `0x3b` / `0x3f7d70a4` at `0x007f0de8`, ...); the standalone rebuild writes zeros there. Not fixed here.
- **videocfg default** (`V_0`, this machine, LIVING-PC): `+0x4` "NVIDIA GeForce RTX 3070", `+0x104` "640 x 480 x 16
  [Fullscreen]", `+0x184` 0, `+0x188` 1, `+0x18c..0x198` 640 / 480 / 16 / 1, rest zero. It depends on the adapter
  (`FUN_00498ea0`, `[DAT_007d3ff8+0xcc]`), so a byte check across machines can only check the layout and `+0x0` = 5.
- W2 booted with no videocfg.bin, no gamesave.bin and no contcfg and reached the menu (`SAVEWRITE ret=0`).

## What changed in source (all default OFF)
- `Race/GameFlow.cpp`: `MASHED_SAVE_KEEP_IMAGE` (in-place write on any save path, `0x24efc` left as loaded);
  `MASHED_SAVE_RT_IN` / `MASHED_SAVE_RT_OUT` harness (needs `MASHED_SAVE_PATH`, runs the live load + save, exits).
- `re/frida/nav_champ_probe.py`: `NAV_KEEP_VIDEOCFG=1` skips the canonical videocfg copy (first-run boots).

## Decision for Mariano
1. Flip `MASHED_SAVE_KEEP_IMAGE` default ON? (a) flip: the standalone's save of an original-written file keeps the ghost block,
   every span cell and the `0x24efc` value, 0 differing bytes vs input; (b) keep OFF. Recommendation (a), through the decided-flip
   protocol. The flip changes no gate here (the gates never save).
2. videocfg.bin first-run write: (a) port the write (new exe behaviour, adapter-dependent bytes, a file the exe never reads);
   (b) reword the D5 checklist line to "first run works with no videocfg.bin / contcfg%d.bin present" and exempt the write.
   Recommendation (b), same reasoning as L-22 (a).
3. First-run gamesave init values (P5): a separate step, port `FUN_004924f0`'s span init into the rebuild.

## Raw evidence (off git, `D:/MashedWT/`), SHA-256
| file | sha256 |
|---|---|
| `D4-WSF-SAVE-RT_data/O_G.bin` | `3118745fa68034cd4784e84707dbe12893e544737f38e52bdf9346c17e61cbb6` |
| `D4-WSF-SAVE-RT_data/O_0.bin` | `95dd10e73553a6897b4b428d234088f24ef3104eb9b35dbdd5ba1f51ff57bf60` |
| `D4-WSF-SAVE-RT_data/V_0.bin` | `cdba8772972fbf72b7c2d4aab142b6c09f02863fc93ccf853447a230f6fee3a7` |
| `D4-WSF-SAVE-RT_data/S1_P.bin`, `S1_G.bin` | `54f3f898f8b300f12ff37e3d7319e270b801bb6e3986c4a703c865706bb0df74` |
| `D4-WSF-SAVE-RT_data/S1_0.bin`, `S2_0.bin` | `c33e740db03c5d95db19f9593c09c6a5d9925efc4888a823bda61c58397e03a8` |
| `D4-WSF-SAVE-RT_data/S2_P.bin` (= `savelink_data/arm_P.bin`) | `697b7af8175b418f24fcd9da81851b8fa42b4d236ec8fb6b90d110d48e53f078` |
| `D4-WSF-SAVE-RT_data/S2_G.bin` (= `savelink_data/ghost_g1/gamesave_captured.bin`) | `e2eabeacda9820bac9b2937bbc055b6abbd72f88b32eefecabd966be7ab289e9` |
| `savelink_data/arm_R_written.bin` | `29a7dca09cc0cd68d541aadacaff12b3711944372fed380622ee9de4a9ddfb71` |
| `D4-WSF-SAVE-RT_data/W1_probe.log` | `ba2e06aa60352d39de6ee74c58ff545c57b834ea7a37a36c56aaf96f2f095ea3` |
| `D4-WSF-SAVE-RT_data/W2_probe.log` | `d3d98f9c93a096f172cffc2188b7f6f53f1af312e19a9d4e612c4c579cd40b77` |
| `D4-WSF-SAVE-RT_data/run_wsf.log` | `15aa5b7363d74eae4a23af1f4faa744857d329da2a5b319d90654ed351d79080` |
| `D4-WSF-SAVE-RT_data/gates.log` (arms in `D4-WSF-SAVE-RT_data/arms/`) | `a3a242d0bf12610575d0e2a3fd6076f898d2977f775442cd268a41e70d375f50` |

Note: the first S3 pass listed `original\gamesave.bin` as modified (mtime rule, W2's write inside the 1 s window); re-run with
content hashes, only `mashed_re.log` changed. The second pass is the reported one.
