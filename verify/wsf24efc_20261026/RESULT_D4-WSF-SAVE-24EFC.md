# RESULT D4-WSF-SAVE-24EFC (2026-10-26): the port-only `0x24efc` increment removed

PREREG `PREREG_D4-WSF-SAVE-24EFC.md` (commit `c7e1ec7eb`, alone, before any arm). Driver `run_24efc.py`, output
`compare.txt`. Base `orch/integration` @ `637d58d28` (not the KEEP_IMAGE flip branch: the fix is on the default rebuild
and the `MASHED_SAVE_ORIGINAL` paths, and KEEP_IMAGE already leaves the dword alone). Base exe sha8 `98913943`, fixed exe
sha8 `869e2006`. Private root `D:/MashedWT/D4-WSF-SAVE-24EFC_root` (copy of `original/`, ASI loader renamed to
`dinput8.dll.off`), restored to the shipped save and canonical videocfg afterwards. `original/` untouched.

## Verdict: GREEN

The original stores the live `DAT_008a95ac` at file `0x24efc` and never increments it. The standalone stored
`++g_saveCounter`. It now stores `g_saveCounter` unchanged. Across a write the dword is now unchanged (0 -> 0, 1 -> 1), as the
original's (1 -> 1, measured). No other byte of any written file moved. Default gates unchanged (gates never save), so this is
a faithfulness fix with no knob.

## The original's write path (static)

`re/tools/decomp_pc.py` (`D:/.../decomp_write_path.txt`) and `re/tools/disasm_fn.py` on `MASHED.exe.unpatched`:

- Serialize `FUN_00404ee0`: `MOV EAX,[0x008a95ac]` `0x00404f03`, `MOV [0x00828254],EAX` `0x00404f23` (`0x00828254` = buffer
  `0x00803358` + `0x24efc`), after the span `REP MOVSD` `0x00404f19`. Write `FUN_00404f50`: `FUN_004b3bb0("gamesave.bin",
  &DAT_00803358, 0x24fa0)`, then `FUN_004099e0(0)`. Call sites: Serialize `0x00409da6` / `0x0040a05c` / `0x0040a89c`, write
  `0x00409db8` / `0x0040a082` / `0x0040a8c2` (all in the save state machine on `DAT_008a9588`).
- Every `.text` reference to `0x008a95ac` (byte scan for `ac 95 8a 00`, 9 hits, equal to Ghidra's 9 xrefs). Reads `0x00404f03`,
  `0x004099a0` (autosave guard), `0x0040ad20` (getter). Writes `0x00404ec1` (Deserialize, `= [0x00828254]`), `0x004098c6`
  (`FUN_004098b0`, eax = 0 from `xor` at `0x004098bf`), `0x004098eb` (`FUN_004098d0`, eax = 0 from `xor` at `0x004098d0`),
  `0x00409c5e` (`MOV [..],0`), `0x00409e30` (`MOV [..],1`, after `MOV [0x008a9588],0x11` at `0x00409e26`), `0x0040ad34`
  (`FUN_0040ad30`, `= param_1`, called from `FUN_0043dfd0` at `0x0043fda6` / `0x0043ffce` / `0x0044050d`). No `INC` or `ADD`.

## Arms

| pred | arm | result |
|---|---|---|
| Q1 | W3 original, `F_1` (`F_P` with `0x24efc` = 1) | `O_1` `0x24efc` = **1** (in 1). 2 bytes differ from input, `0x24f55` / `0x24f56` (gather live state, as D4-WSF-SAVE-RT W0 / W1), none in the dword |
| Q2 | SB base exe, `F_1` | **held**: 1 -> 2 |
| Q3 | SF fixed exe default, `F_P` / `F_G` / `F_1` | **held**: 0 -> 0, 0 -> 0, 1 -> 1 |
| Q4 | SF vs S1_P, S1_G (D4-WSF-SAVE-RT), S1_0, SB_1 | **held in substance**: each pair differs in 1 byte, inside the `0x24efc` dword, 0 bytes outside. The PREREG said "exactly 4 bytes". That count was wrong: 0 / 1 and 1 / 2 differ in the low byte only |
| Q5 | SF first run (none) | **held (residual)**: 0. The original's first run wrote 1 (`O_0`, D4-WSF-SAVE-RT P5). The old standalone's 1 was `++0`, not a model of that writer |
| Q6 | SO fixed exe, `MASHED_SAVE_ORIGINAL=1`, `F_1` | **held**: 1, and `SO_1` == `F_1` byte for byte |
| Q7 | gates, fixed exe, no knob | **held**: G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (= `merge_train.json` refs), exe `869e2006`, all self-exited |

`SF_1` vs `O_1`: 331 bytes differ, 0 in the dword. Those are the default rebuild's span and gather bytes (D4-WSF-SAVE-RT P3),
which the KEEP_IMAGE flip (D4-WSF-SAVE-FLIP) addresses, not this step.

## Residual (not fixed here)

First-run `0x24efc`: the original wrote 1 with no input file. The writer is `0x00409e30` (state `0x11` arm) or `0x0040ad34`
(`FUN_0040ad30([0x008990d8])`). Which one is `[UNCERTAIN: no --watch-write run on the first-run boot]`. The port models neither,
so its first-run write now stores 0. This goes with the first-run init work (`FUN_004924f0`, the SPANINIT step): a
`--watch-write` on `0x008a95ac` during the W2 boot names the writer.

## What changed in source

- `mashedmod/src/mashed_re/Race/GameFlow.cpp` `SaveProgress`: `++g_saveCounter` -> `g_saveCounter` on the default rebuild and
  on the in-place (`MASHED_SAVE_ORIGINAL`) path, RVA-cited. Comments updated there and in `Save/GameSaveBuffer.cpp`.
- `verify/wsf24efc_20261026/run_24efc.py`: the arm driver.

## Raw evidence (off git, `D:/MashedWT/D4-WSF-SAVE-24EFC_data/`), SHA-256
| file | sha256 |
|---|---|
| `F_1.bin` (= `SO_1.bin`) | `dca746c9d4339c13434daeda4d56077119441652a59285319400159cfeda35b8` |
| `O_1.bin` | `6cd13410e74755bf966972b65a4a3be677565f73b89ec914a0c1db744cf6d39a` |
| `SB_1.bin` | `694378907c96ebfabfa583d09951172282ac0466095fb772c6253bccf9f591b4` |
| `SF_P.bin` (= `SF_G.bin`) | `7640efaf367fd8806082defdd7d44e10b761536e47dce2d3c2d68459e0367ef7` |
| `SF_1.bin` | `54f3f898f8b300f12ff37e3d7319e270b801bb6e3986c4a703c865706bb0df74` |
| `SF_0.bin` | `89892bcb383b3aa840af10d9901ba8e24f301a63d51c6d9bb4ce2615df5f642d` |
| `W3_probe.log` | `48ca47b392a2bd43bb3009027f1424b23bd8fd6506777de080af3a1b6f02a5b0` |
| `run_24efc.log` | `f0b38d3de199b47730e189b4c6a44d250f994d234f7e143940b2efdda86f2d81` |
| `decomp_write_path.txt` | `40ee6892ec77581a7ddfe23c73713afc6bd9cdbb7002ba5899f32bb3b9141a6a` |
| `mashed_re_base.exe` | `989139435144dd6a8cd13d89d7fc08c42cfaac3b6f658cf56b076cd6f0f2117e` |
| `gates.log` (arms in `arms/`, `RUNLOG.txt`) | `316afc2b1b594d3b697b36f9982f751c07e87efabfdc2e25839e3a5ad32620e5` |
Inputs `F_P` / `F_G` and `S1_*` / `O_0`: as listed in `verify/wsfsave_20261026/RESULT_D4-WSF-SAVE-RT.md`.
