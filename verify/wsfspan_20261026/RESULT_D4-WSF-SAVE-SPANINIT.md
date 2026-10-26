# RESULT D4-WSF-SAVE-SPANINIT (2026-10-26): FUN_004924f0's span init ported into the first-run save

PREREG `PREREG_D4-WSF-SAVE-SPANINIT.md` (commit `75f294067`, alone, before any arm). Driver `run_span.py`, prediction
`predict.py`, static model `model_spaninit.py`, row match `o0_powerup_rows.txt`.

**Base:** `orch/D4-WSF-SAVE-24EFC` @ `748ca9fb7` (24EFC is not in `orch/integration` yet), with `orch/integration` @
`96e7f1ad2` (D4-WSF-SAVE-FLIP, KEEP_IMAGE default ON) merged in, so the tested save code is the post-train state. Exe
sha8 `9824292e`. No original-side run was needed: the original's first-run file is `O_0.bin` from D4-WSF-SAVE-RT.

## Verdict: GREEN

The standalone's first-run gamesave now differs from the original's by **269 bytes** (before: 343 on the 24EFC exe, 342 on
the RT exe). Every remaining byte is in a named bucket with a cited cause (below), 0 unexplained. Saves of a loaded file are
unchanged (KEEP_IMAGE path untouched). Default gates unchanged. No knob: the change only reaches the rebuild, which runs
on the first save with no valid file, and the gates never save.

## The original (static, `re/tools/decomp_pc.py` + `re/tools/disasm_fn.py` on `MASHED.exe.unpatched`)

`FUN_004924f0` (caller `FUN_00492370`), disassembly `0x004924f0..0x0049268d`. The span `0x007f0a40..0x007f0f5f` is BSS, so
on a first run the serializer (`0x00404ee0`) copies what this function left:
- `0x00492502` `REP STOSD` 0xdce9 dwords at `0x7f0f60` (outside the span). `0x00492504..1c` `[0x7f0f2c..0x7f0f3c] = 0`.
- `0x0049252e/33/38` call `0x00431ae0/af0/b00`: `[0x7f0f04]/[0x7f0f08]/[0x7f0f00] = 0x3f333333`. `0x00492542` call `0x00431b10`:
  `[0x7f0f10] = 2` (`0x00431b12`).
- `0x00492550` `[0x7f105c + i*0x4c] = 1` while `< 0x7f12bc` (i = 0..7). The serializer gathers the low bytes into file
  `0x24f54 + i` (`0x00404ef2`).
- 13 rows (`ecx = 0x7f0cb0 + r*0x14` while `< 0x7f0db4`, `0x00492663`): 5 dwords `0xffffffff` (`0x00492575..80`), `[0x7f0db4+r*4]
  = 9` (`0x00492583`), `[0x7f0de8+r*4] = 0x3b` (`0x0049258a`), `[0x7f0e1c+r*4] = 0x3f7d70a4` (`0x00492590`), **byte** `[0x7f0e50 +
  r*12 + c] = 1` (`0x004925a3`, `mov byte ptr`; Ghidra's decomp types it as a dword array). Jump table `0x00492690`, row base
  `0x7f0a40 + r*0x30`: col 1 / col 11 `= (ecx < 0x7f0cc4)` (`0x004925c5` / `0x0049263d`, row 0 only), col 3 `= ((ecx >=
  0x7f0d00) - 1) & 2` (`0x004925e2..ec`, rows 0..3 = 2), col 4 `= 2` (`0x004925f8`), cols 0 / 2 / 5..10 `= 0`.
- Static check (`model_spaninit.py` on `O_0.bin`): this model equals every byte of the original's first-run span except the
  three buckets listed under residual (a), (d), (e).

## Arms
| pred | arm | result |
|---|---|---|
| Q1 | N_0 new exe, first run | **held**: `N_0` == `predict.py` image byte for byte (sha `32ee57a9`). vs `O_0`: **269** differ (table 104, span_rest 161, state 1, gather 3). Before: `SF_0` 343, `S1_0` 342 |
| Q2 | N_0 residual buckets | **held**: 252 + 12 + 1 + 3 + 1, `other` = 0 (table below) |
| Q3 | N_P / N_G / N_1, default | **held**: output == input byte for byte, all three |
| Q4 | N_P0, `MASHED_SAVE_KEEP_IMAGE=0`, `F_P` | informative: vs `SF_P` 521 differ, 0 outside the span. vs `F_P` 255 differ (before, `S1_P` vs the original's write: 333) |
| Q5 | gates, no env | **held**: G-CTRL `5943c5f1 / b493262b / 09551e6a`, B `3a03670c / e85ea6fb / 53778664` (= `merge_train.json` refs), all self-exited |

## Residual (269 bytes), by bucket
| bucket | bytes | before | cause |
|---|---|---|---|
| (a) `0x24cb0..0x24db3`, the `0x7f0cb0` rows | 252 | 71 | The Lua binding `Set_Track_Powerups` (`0x0047b280`, registered at `0x00440f3b` with name `0x005cdb34`) writes 5 dwords per row through `FUN_00430250` (`return &DAT_007f0cb0 + i*5` for the row whose `**FUN_0040d430()[i] == course id`). All 13 `O_0` rows equal the last `Set_Track_Powerups` call per course in `POWERUPS_ALL.LUA` (Powerups.piz) (`o0_powerup_rows.txt`). The byte count rose because init now writes `0xffffffff` (4 bytes differ per overwritten dword) where the old zero differed in 1. Not ported here |
| (b) col 4 rows 1..12 | 12 | 12 | Port-private progression: `BuildSpanFromProgress` stores unlock `2` / `0` at col 4; init (and `O_0`) has `2` on every row |
| (c) `0x24f2c` gate | 1 | 1 | Port-private: `any ? 1 : 0`. Original writers of `0x7f0f2c`: `0x00492504` (0), `0x004305ba` (`FUN_00430290`), `0x00492991` (`FUN_004927c0`, 1). `O_0` has 0 |
| (d) gather `0x24f54..0x24f56` | 3 | 5 | Init sets 1, `O_0` has 0: a live writer clears gather slots 0..2 before the first-run save. `[UNCERTAIN: writer, no --watch-write run]`, same open item as D4-WSF-SAVE-RT's gather finding |
| (e) `0x24efc` | 1 | 1 | `DAT_008a95ac`, not span. Port 0, original 1 |

The 253 bytes "other" before (cols 1 / 3 / 11, the 9 / `0x3b` / `0x3f7d70a4` rows, the `0x7f0e50` bytes, the `0x7f0f00..10`
dwords, gather slots 3..7) are 0 now.

## 0x24efc residual: not folded in
The two candidate writers stay `[UNCERTAIN]`. `0x00409e30` `MOV [0x008a95ac],1` sits in the arm `0x00409e07..0x00409e3a` (draws
message `0x1b7` via `0x0042bfb0`, calls `0x0045b350`, `MOV [0x008a9588],0x11` at `0x00409e26`), and `0x0040ad34` is
`FUN_0040ad30(param)` from `FUN_0043dfd0`. Which one runs before the first-run Serialize needs a runtime write watch on
`0x008a95ac` during a no-save boot. `scenario_launch.py --watch-write` watches car-record offsets only, so this needs a
global-address watch, which no registered tool has. Not cheap, so it is the next item.

## What changed in source
- `mashedmod/src/mashed_re/Save/GameSaveFormat.h`: `InitSpanDefaults()`, buffer form of every span write above, each
  RVA-cited.
- `mashedmod/src/mashed_re/Race/GameFlow.cpp` `BuildSpanFromProgress`: `InitSpanDefaults(span)` replaces `memset 0`. The
  overlay of col 4 / col 5 / the gate is unchanged.
- `verify/wsfspan_20261026/`: PREREG, RESULT, `run_span.py`, `predict.py`, `model_spaninit.py`, `o0_powerup_rows.txt`.

## Next
1. Port `Set_Track_Powerups` for the span rows (252 bytes): needs the `FUN_0040d430` row table (13 entries, course id at
   `**entry`) and which powerups script the boot runs (`POWERUPS_ALL.LUA` matches all 13 `O_0` rows; the selector between
   `powerups.lua` / `powerups_gold.lua` / `powerups_all.lua` is not read here). The frontend preview already reads
   `Nav_SaveSpanData() + 0x270` (`exe_main.cpp:7293`).
2. `0x24efc` first-run writer and the gather slot 0..2 clear: one boot with a global write watch on `0x008a95ac` and
   `0x7f105c / 0x7f10a8 / 0x7f10f4`.

## Raw evidence (off git, `D:/MashedWT/D4-WSF-SAVE-SPANINIT_data/`), SHA-256
| file | sha256 |
|---|---|
| `N_0.bin` (= `predicted_first_run.bin`) | `32ee57a99627e245bc7356be6c64d0a0eaa4979c76424bc2aa49c55b7707c529` |
| `N_P.bin` (= `F_P`) | `697b7af8175b418f24fcd9da81851b8fa42b4d236ec8fb6b90d110d48e53f078` |
| `N_G.bin` (= `F_G`) | `e2eabeacda9820bac9b2937bbc055b6abbd72f88b32eefecabd966be7ab289e9` |
| `N_1.bin` (= `F_1`) | `dca746c9d4339c13434daeda4d56077119441652a59285319400159cfeda35b8` |
| `N_P0.bin` | `c6decaccd6c54617c5f8ce10e3dc406b3054ea81f0087c0979fbf84978c8a739` |
| `run_span.log` | `4f71e56a5928f0914c39f786c3de712cf57b5076aa094cb8d4da20e6bd2abb87` |
| `gates.log` | `50d5e8859d899932f5a01aa8ea033e8d513c4d3f7ccf6f63e603002a7dd55f91` |
| `arms/H_DEF.step.csv` | `5943c5f1c3e2f3f2510139f7af666ffa87941f6dfa66201b15277430b92be415` |
| `arms/H_DEF.gates.csv` | `b493262bed8ea6ecad4399f0ebaa53b902965423e74bfa07359188559fff479f` |
| `arms/H_DEF.ptrace.log` | `09551e6a5b1b456b3421a0e1aac768ced7851c072c99a9341a22ca966cdfe3e1` |
| `arms/B_DEF_K0.step.csv` | `3a03670c3991b702af9fd44b1d49dbdb4f91d6be00876723cb1ba7891895b83f` |
| `arms/B_DEF_K1.step.csv` | `e85ea6fb266223f9eed3a90bf9aa30ea50291fd552ac3881d3bfbea843a759f8` |
| `arms/B_DEF_K26.step.csv` | `5377866478b33a9827ae5c343b8861189f8f0a838785bf2b83da5f6118931e2d` |
| `decomp_spaninit.txt` (FUN_004924f0 / 0040ad30) | `618d5ffe0a1dc8109af6ad2f6d10be3fb97a053f9970735cc5accfb73971ddfc` |
| `disasm_004924f0.txt` | `52ffa043e5abb51026386a3973cbf95c30476838b5d805577418e08bb7f12bda` |
| `disasm_00409d80.txt` | `61b01324b3fdeaacf89ab338d31d1c22eb8f4381fb5e8a6e4f53314f84b23e83` |
| `datarefs_7f0cb0.txt` | `bd3ade94b47f5430dd1e3f5850974740a3a58e9aab545c2ab5fd633c599cba79` |
| `datarefs_7f0f2c.txt` | `8a78670541a2cdd7f6c3a389bda2316a75c8311f70cbf6f89939d2adb8f162c6` |
| `decomp_00430250.txt` | `935fe85159fa0c3d45f7c0e87641977b652b5508b7c0e7afb39acb15f92062ff` |
| `decomp_0047b280.txt` | `d3ffb196a8525bb3f27a7d1bdc7de2ed09f583bb9f4aa1e7542fbfc66c04c47c` |
| `xrefs_0047b280.txt` | `cf98c1f2aa7bfd89fcf5f9c4d53ba51bbf38e974c6ad463b0754fb3f6373fe24` |
| `powerups_piz/POWERUPS_ALL.LUA` | `40cb6b7cbeadd6818b2b0c503ed1f15cc13dd50715b93b2e9f3f051503cc79b0` |
| `build.log` | `2c687e270838c3d1259e34da4e5bf05e75f33fa6999b6b9fdebee45d7f7d3f5a` |
| `mashedmod/build/mashed_re.exe` (branch build) | `9824292edb9a0de1d6556d54601da32ea85c8ecc366e84045384be27b83cf071` |
Inputs `F_P` / `F_G` / `O_0` / `S1_0`: as listed in `verify/wsfsave_20261026/RESULT_D4-WSF-SAVE-RT.md`. `F_1` / `SF_0` / `SF_P`:
`verify/wsf24efc_20261026/RESULT_D4-WSF-SAVE-24EFC.md`. Private root `D:/MashedWT/D4-WSF-SAVE-SPANINIT_root` (gates only).
`original/` untouched, `original/mashed_re_dev.asi` not redeployed.
