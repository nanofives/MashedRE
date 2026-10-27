# RESULT D4-FU-SETTRACKPU (2026-10-26): Set_Track_Powerups rows in the first-run save, write watch on 0x008a95ac

PREREG `PREREG_D4-FU-SETTRACKPU.md` (commit `8dbb53c8f`, alone, before any arm). Driver `run_stp.py`, offline model
`predict.py`, watch extract `watch_hits_summary.txt`. Base `orch/integration` @ `b0174c80f`. Exe sha8 `9d987a6b`.

## Verdict: GREEN

The standalone's first-run gamesave now differs from the original's by **17 bytes** (before: 269). The 252-byte
Set_Track_Powerups bucket is 0. The write watch names the original writer of both remaining non-port-private buckets
(`0x24efc` and gather slots 0..2), which closes the `[UNCERTAIN]` left by D4-WSF-SAVE-SPANINIT. Saves of a loaded file
are unchanged. Default gates unchanged. No knob: the change only reaches the rebuild (no valid file, or
`MASHED_SAVE_KEEP_IMAGE=0`), and the gates never save.

## The original (static, `re/tools/disasm_fn.py` + `xref.py` on `MASHED.exe.unpatched`)
- Boot order: `FUN_00492370` calls `FUN_004924f0` (span init) at `0x0049239f`, then `FUN_00402750` at `0x004923d0`.
  `FUN_00402750` reaches `FUN_0045bae0`, which at `0x0045bb7d` calls `FUN_0047b9b0("powerups_all.lua" 0x005ce628, &local,
  0x00440bc0)`. `0x00440bc0` registers `Set_Track_Powerups` = `0x0047b280` at `0x00440f3b`. The other two script names
  (`powerups.lua` `0x005cd4d4`, `powerups_gold.lua` `0x005cd4f8`) are referenced only by `FUN_004264d0`, not on this path.
- `0x0047b280`: `n = FUN_004b6fc0(L)`, then `FUN_004b7090(L, i)` + `FUN_004a2c48()` on slots `n-6 .. n`, then
  `FUN_00430250(a0, a1)` and five dword stores at the returned row.
- `0x00430250`: NULL if `param_2 == 0`, else the first `r < 0xd` with `**(FUN_0040d430()[r]) == param_1`, returning
  `&DAT_007f0cb0 + r*5`. `FUN_0040d430` returns `[0x005f2770]` = `0x005f2728` (.data, initialized). The 13 records'
  first dwords: 30, 3, 39, 0, 38, 11, 25, 2, 36, 34, 37, 33, 26 (record VAs in `GameSaveFormat.h`). This also names the
  two rows the previous step left as `WARZONE / NEUSTEIN`: row 2 = 39 (NEUSTEIN), row 11 = 33 (WARZONE).
- `POWERUPS_ALL.LUA` (Powerups.piz): 13 `local` constants, comments, 39 seven-argument calls, 3 per course (tier 1, 2,
  3). Every tier is non-zero, so every call stores and the last call per course (tier 3) is what the span holds.

## Arms
| pred | arm | result |
|---|---|---|
| Q1 | N_0 new exe, first run | **held**: `N_0` == `predict.py` image byte for byte (sha `604d373f`). vs `O_0`: **17** differ (col 4 12, gate 1, gather 3, `0x24efc` 1), rows 0. SPANINIT `N_0` vs `O_0`: 269 |
| Q2 | N_P / N_G / N_1, default | **held**: output == input byte for byte, all three |
| Q3 | N_P0, `MASHED_SAVE_KEEP_IMAGE=0`, `F_P` | **held**: vs SPANINIT `N_P0` 252 differ, 0 outside `0x24cb0..0x24db3`. vs `F_P` 3 differ (was 255) |
| Q4 | gates, no env | **held**: G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (= `merge_train.json` refs), all four self-exited, exit 0 |
| Q5 | W, original first-run boot, watch | **held**: armed with no error (14 threads at attach, 85 by the end via the thread observer). Before the save `[0x008a95ac]` = 1, the three gather dwords = 0. `O_W` == `O_0` byte for byte (the original's first-run file reproduces). Writers below |

**Attempt 1 (exe `d3106003`) failed Q1** and is not the evidence: `N_0` came out equal to SPANINIT's `N_0` (252 differ
vs prediction). Cause: a 7-argument `Set_Track_Powerups(...)` line tokenises to 16 tokens and the runner tested for 17, so
every call was skipped. Fixed in `8aa896b31`, checked natively (39 calls, 0 skipped, 13 rows == `O_0`) before the rebuild.
The attempt-1 GATES run was stopped by PID (own processes only) since its exe was about to change. The W arm ran on the
original only and is reused. Logs: `run_stp_attempt1.log`, `build_attempt1.log`.

## Write watch (Q5): who writes what before the first-run save
`re/frida/nav_champ_probe.py` `NAV_WATCH_WRITE` (new): debug-register write watchpoints on absolute globals, armed after
attach and on later threads. Data breakpoints trap after the store, so the pc is the instruction after it. 2633 hits:
- **`0x008a95ac` (file `0x24efc`)**: hit at pc `0x004098cb` = the store at `0x004098c6` in `FUN_004098b0` (writes 0;
  return frames `0x0043f9f6` in `FUN_0043dfd0`, `0x004030f9` in `FUN_004030d0`). Then hit at pc `0x00409e3a` = **the store
  `0x00409e30` `MOV [0x008a95ac],1`**, the arm `0x00409e07..0x00409e3a` (message `0x1b7` via `0x0042bfb0`, call
  `0x0045b350`, `[0x008a9588] = 0x11` at `0x00409e26`). Return frame `0x0043e06e`, i.e. the `call 0x0040acd0` at `0x0043e069`
  in `FUN_0043dfd0` with argument `[0x007f1004]`. `0x0040ad34` (`FUN_0040ad30`) did not fire. No other writer.
  Ghidra has no function covering `0x00409e30`.
- **Gather slots 0..2 (`0x007f105c / 0x007f10a8 / 0x007f10f4`, file `0x24f54..56`)**: all writes are at pc `0x00496568`,
  the `rep stosd` (eax 0, ecx 0x13) in `FUN_00496530`. Hits 1, 2, 3 zero slot 0, 1, 2 in that order. `FUN_00496530` copies
  the 0x4c-byte record `0x7f1038 + ebx*0x4c` to `0x7f14f8 + ebx*0x4c` (`0x00496552`) and then zeroes it (`0x00496568`); the
  gather byte is record +0x24. Caller `FUN_004967e0` loops `ebx = 0..3` and calls it at `0x00496820` when
  `FUN_00497450(ebx) != 0`. `FUN_00497450(i)` returns `[0x7e96fc + i*0x200] != 0` for `i <= 3`, else 0 (`0x00497450..6e`).
  The remaining 2629 hits re-zero an already-zero record (`FUN_004967e0` runs repeatedly; callers `FUN_00429290`,
  `FUN_00492d20`). Slot 3 was not watched (four debug registers); `O_0` has 1 there.
- Not observed: `FUN_004924f0`'s own stores (`0x00492550`), since the attach comes after boot init.

## Residual (17 bytes), by bucket
| bucket | bytes | cause |
|---|---|---|
| col 4 rows 1..12 | 12 | Port-private progression: `BuildSpanFromProgress` stores unlock `2` / `0`. Init and `O_0` have `2` on every row |
| `0x24f2c` gate | 1 | Port-private: `any ? 1 : 0`. `O_0` has 0 |
| gather `0x24f54..56` | 3 | `FUN_004967e0` -> `FUN_00496530(i)` zeroes record i when `[0x7e96fc + i*0x200] != 0`, i = 0..2 on this run. Not ported. `[UNCERTAIN]` what sets `0x7e96fc + i*0x200`: not traced here |
| `0x24efc` | 1 | `0x00409e30` sets `DAT_008a95ac = 1` on the path `FUN_0043dfd0` `0x0043e069` -> `FUN_0040acd0([0x007f1004])` before mode select. Port stores its live counter (0). Not ported |

## What changed in source
- `mashedmod/src/mashed_re/Save/GameSaveFormat.h`: `kSpanRowCourseId`, `TrackPowerupsRow` (`0x00430250`),
  `SetTrackPowerups` (`0x0047b280`), `RunTrackPowerupsScript` (the statement set of `POWERUPS_ALL.LUA`; anything else is
  skipped and counted, `[UNCERTAIN]` for a script outside it).
- `mashedmod/src/mashed_re/Race/GameFlow.cpp`: `ApplyBootPowerupsScript` loads `POWERUPS_ALL.LUA` from
  `original/TOASTART/Common/POWERUPS/Powerups.piz` once (fallback `MASHED_ROOT`), and `BuildSpanFromProgress` runs it after
  `InitSpanDefaults`.
- `re/frida/nav_champ_probe.py`: `NAV_WATCH_WRITE=<va>[,<va>..]` (up to 4) hardware write watch, dumped before and after
  the `savewrite` plan's save.
- `verify/settrackpu_20261026/`: PREREG, RESULT, `run_stp.py`, `predict.py`, `watch_hits_summary.txt`.

Not changed: the live `g_save_span` the frontend preview reads (`exe_main.cpp:7293`). On a first run the original's live
rows hold the script values from boot. The port's live span holds them only after a save is loaded. Separate item.

## Next
1. Port the `0x00409e30` arm (`DAT_008a95ac = 1`, `DAT_008a9588 = 0x11`) where the port's frontend reaches the
   `FUN_0040acd0([0x007f1004])` dispatch, to close `0x24efc` (1 byte).
2. Port `FUN_004967e0` / `FUN_00496530` record clear, after tracing what writes `0x7e96fc + i*0x200` (gather, 3 bytes).
3. Seed the live span at boot with `InitSpanDefaults` + the script (frontend preview on a first run), behind a knob.

## Raw evidence (off git, `D:/MashedWT/D4-FU-SETTRACKPU_data/`), SHA-256
| file | sha256 |
|---|---|
| `N_0.bin` (= `predicted_first_run.bin`) | `604d373fc3b60fdcaf5903257ab70d62cf32611fdbadcd42226adf8d74a718dd` |
| `N_P.bin` (= `F_P`) | `697b7af8175b418f24fcd9da81851b8fa42b4d236ec8fb6b90d110d48e53f078` |
| `N_G.bin` (= `F_G`) | `e2eabeacda9820bac9b2937bbc055b6abbd72f88b32eefecabd966be7ab289e9` |
| `N_1.bin` (= `F_1`) | `dca746c9d4339c13434daeda4d56077119441652a59285319400159cfeda35b8` |
| `N_P0.bin` | `283cd129271ea7eef7b7e6a132515a716f7dba71d6b46338073ae9474015519a` |
| `O_W.bin` (= `O_0`) | `95dd10e73553a6897b4b428d234088f24ef3104eb9b35dbdd5ba1f51ff57bf60` |
| `W_probe.log` (all 2636 watch lines) | `1bc00d746b56e05842a5adccdb56bc6f79ddeb5bca58689888cc1f1e3c7770b7` |
| `run_stp.log` | `81cc56682b04e391ced4c05be7c0132075d32f94c858f387a5a6cd4d62804114` |
| `run_stp_attempt1.log` | `12396838c5e51e0c1b3881aee8fc635d46cb454703caf02b389b18315735fd63` |
| `run_console.log` (attempt 1 + W) | `0a67ce77135fd4d44ac78e5299fa0f27726ba2dc2cb4e5925feb8c72df846e8c` |
| `run_console2.log` (attempt 2) | `d18b7484ace0c7383e65cd4c7bd3a7ce4e5d22e050c1c5af97b1df1aac08b6d5` |
| `gates.log` | `55c2e474f3b98601282987b806258ad9544044cca0e633a0c51cb81d19fbaecd` |
| `arms/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms/H_DEF.gates.csv` | `b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f` |
| `arms/H_DEF.ptrace.log` | `09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1` |
| `arms/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `arms/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `build.log` | `13467c0d41bdf80bf6c472a9931ae3dc6c572d2f53ade12d388d7f3d2e00a95c` |
| `build_attempt1.log` | `bdb554c534a3a1c60203026c861c55368b9a6b18f6fc15674f8e23e634c49775` |
| `D4-FU-SETTRACKPU_root/extracted/POWERUPS_ALL.LUA` | `40cb6b7cbeadd6818b2b0c503ed1f15cc13dd50715b93b2e9f3f051503cc79b0` |
| `mashedmod/build/mashed_re.exe` (branch build) | `9d987a6ba974b796df6d95e2230d1f19a41675688ac652715719e55916cd7bea` |

Inputs `O_0` / `F_P` / `F_G`: `verify/wsfsave_20261026/RESULT_D4-WSF-SAVE-RT.md`. `F_1`:
`verify/wsf24efc_20261026/RESULT_D4-WSF-SAVE-24EFC.md`. SPANINIT `N_0` / `N_P0`:
`verify/wsfspan_20261026/RESULT_D4-WSF-SAVE-SPANINIT.md`. Private root `D:/MashedWT/D4-FU-SETTRACKPU_root` (robocopy of
`original/`, `dinput8.dll` renamed `.off`). `original/` untouched, `original/mashed_re_dev.asi` not redeployed. Trackers
not edited (re-classify not run: no promotion is claimed here; `0x0047b280` stays C2 in `hooks.csv`).
