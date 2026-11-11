# RESULT RO-WARP-NORMALIZE: normalise the boot counters at the warp on the original

Date 2026-11-11. Worker session cmwbx2zbt2ivgsc1bk1mvi2zq (claude2), parent cmwbssxl22bg4sc1ba3ghf7dh.
Branch `orch/RO-WARP-NORMALIZE` from `orch/integration` `9cf3133eb`. Review: **TEXT-ONLY** (harness under `re/frida/`,
scripts and reports under `verify/replayoracle_20261111/`, no build input, no `mashedmod/` edit).

## Verdict

**GREEN.** Every dword that two tick-clock runs of the original still disagreed on now has a row in
`re/frida/warp_normalize.tsv`, and each row cites its writer RVA. The file has 46 lines and expands to 299 pinned
dwords plus masks and rules.

- **Fewer classes.** Batch 2 (no pins) left 28 to 128 non-pointer dwords per pair, most of them in 4 unnamed classes:
  RESIDUE 17-19, OFFSET 4-6, MODOFFSET 1-98, TOGGLE 2. Batch 3 (pins) leaves **0** RESIDUE, **0** OFFSET,
  **0** MODOFFSET, **0** TOGGLE and **0** UNEXPLAINED in all 10 pairs. What remains is 24 to 28 dwords, each in a
  named MASK or RULE class with a cited writer.
- **No race-state change.** Unpinned batch-2 runs against pinned batch-3 runs: race state 15/15 and normalised 15/15
  (`t2_b`/`n3_b`, `t2_c`/`n3_c`, `t2_neg`/`n3_neg`). Within batch 3: 10 of 10 pairs at 15/15 race state and 15/15
  normalised.
- **The RNG negative control is still caught.** `n3_b` against `n3_neg`: normalised 0/15, race state 0/15, 35,607
  UNEXPLAINED dwords. The per-tick count is IDENTICAL-FOR-N = 7, the same tick as batch 2.
- **Per tick.** The number of blocks that differ at every tick fell from 23-24 (batch 2) to 10. All 10 hold only
  MASK/RULE dwords. Pairs among `n3_b`, `n3_c` and `n3_h` are IDENTICAL-FOR-N 2,724 of 2,724 with no `--n-ok` needed.
  No N misalignment occurred in any batch-3 pair other than the negative control (from tick 8 there).

Two dwords were not in the REPLAY-ORACLE-DESIGN list and were found here: `0x00616110` (stdout `FILE` cursor) and
`0x0077391c` (window-focus flag). Rows below.

## The normalisation set (`re/frida/warp_normalize.tsv`)

Columns: `va count stride width action value writer readers note`. Both sides apply it the same way:
- `PIN` rows are written on the main thread at the warp and again at the race anchor. The race anchor is the first
  main-thread clock call that sees `DAT_00771968 == 3`.
- `MASK:<family>` rows are dropped from the compare (`detorig_report.py --norm`).
- `RULE:<name>` rows are compared by a rule in `detorig_report.py`.

`detorig.py --normalize FILE [--normalize-at warp|anchor|both]` reads the PIN rows. `detorig_report.py --norm FILE`
reads the MASK rows.

### PIN (299 dwords)

| va | value | writer | readers | why this value |
|---|---|---|---|---|
| `0x007f0ff0` | 0 | `0x00493453` (`FUN_00493390` `+= 0x32` per tick) | `0x00401343` (`FUN_00401340`); `0x0044660b`, `0x0044687c`, `0x00446893` (`FUN_00446520`, fild, in the `DAT_007f0fd0 == 5` camera branch) | the store at `0x00492679` (`FUN_004924f0` `= 0`) |
| `0x00771978` | 0 | `0x00492737` (`FUN_004926c0` `+= 1`) | `0x00492728` | its own reset at `0x00492762` |
| `0x00771994` | 0 | `0x00492731` (`+=` QPC delta) | `0x0049271e` | its own reset at `0x00492758` |
| `0x0077198c` | 1 | `0x0049334a` (`FUN_00492e90` `+= 1`) | `0x00492ec3` (`== 0` test, then `DAT_00771998 = DAT_007719a0`), `0x0049333b` | only read as `== 0`. 1 keeps the branch that every booted run takes |
| `0x00868348` + `0x44`·k, k < 96 | 0 | `0x004924a8` (`FUN_00492440` row `+0x28 += 1`) | `0x00492447` | its own reset at `0x0049249c` |
| `0x0086834c` + `0x44`·k | 0 | `0x0049245e` (row `+0x2c += +0x20`) | `0x00492453` | reset at `0x00492493` |
| `0x00868350` + `0x44`·k | 0 | `0x00492461` (row `+0x30 += +0x24`) | `0x00492444` | reset at `0x00492499` |
| `0x00636ac4` | 0 | `0x00401e4b` (`FUN_00401da0`, fstp) | `0x00401e30`, `0x00401e91` | frontend float, constant after the menu |
| `0x00636ae8` | 0 | `0x00402f52`, `0x00403124`, `0x004030e6` | `0x004030d9`, `0x004030f9` | the store at `0x00402f80` |
| `0x00636afc` | 0 | `0x00402ff9` (fstp) | `0x00402fb3`, `0x00402fe2` | frontend float |
| `0x0067d858` | 0 | `0x00428aea` (`FUN_00428a30` `= FUN_004950b0()`) | `0x00428af4` (24,000,000 compare) | a real-clock sample taken at the title screen |
| `0x0067d974` | `0x3fc90fdb` | `0x00428578` (fstp) | `0x00428453`, `0x00428568` | the store at `0x004285d4` (`FUN_00428590`) |
| `0x0067e848` | 0 | `0x0042e6ad` (`FUN_0042e5b0` `+= esi`) | `0x0042e745` | frontend sprite-cycle counter |
| `0x0067f0b8` | 0 | `0x0042e6bf`, `0x0042e6df` | `0x0042e6b3`, `0x0042e6d7` | frontend sprite-cycle counter |

The 96 rows at `0x00868320` (`0x44` bytes each) are the loop `0x00868320..0x00869ca0` in `FUN_00492e90` that calls
`FUN_00492440` per row.

**`0x007f0ff0` matters beyond tidiness.** `FUN_00446520` reads it as an integer, multiplies it by `[0x005cd03c]` and
passes the result to `FUN_004a3700` / `FUN_004a37b0` (`0x0044687c..0x0044689f`). That code sits in the branch
`DAT_007f0fd0 == 5 && FUN_00405890() != 0`. The scenario here sets `DAT_007f0fd0` to rule 0, so the branch is not
taken. In a rule-5 race the camera would depend on a counter that started at boot. With the pin, the counter starts
at 0 on both sides.

### MASK and RULE (not pinned)

| va | class | writer | why not pinned |
|---|---|---|---|
| `0x00616110` | RULE:FILEPTR | `0x004a4fd6` (`write_char`, `inc [FILE+0]`), reached through `printf` `0x004a2cbd`, which loads `esi = 0x616110` at `0x004a2cc9` | The race start calls `printf("start of race\n")` at `0x0040ff29` (format `0x005ccde4`). Write watch in run `n3_w`: 1,256 hits, pc `0x004a4fd8`, backtrace `0x004a5282 / 0x004a4fd8 / 0x004a2cf2 / 0x0040ff2e`. The cursor is a heap address, unaligned, so the PTR rule misses it. The rule compares cursor minus buffer base (`0x00616118`) |
| `0x00616038` | MASK:crt-cookie | `0x004a5825`, `0x004a582d` (`FUN_004a57e4`: GetSystemTimeAsFileTime, PID, TID, GetTickCount, QPC) | the `/GS` cookie. Every `/GS` prologue reads it (`0x00401006` ...), so changing it while frames are live would fail `__security_check_cookie` on return. Only a process-start install can set it (RO-ORIG-ASI) |
| `0x007de100` (8 bytes) | MASK:crt-clock | `0x005c2a35`, `0x005c2a3a` (`___inittime`, GetSystemTimeAsFileTime) | Its only readers are `clock()` (`0x005c29e9`, `0x005c29f2`), which subtracts it from the real FILETIME. A pin would not make `clock()` equal |
| `0x007e9584`, `0x00772fb0`, `0x007d4104`, `0x009120fc` | RULE:HWND | `0x00499c9a` (`CreateWindowExA`), `0x00496038` / `0x004960a9`, `0x004c7e06`, `0x004c81d5` | OS-assigned window handle. The rule checks that each copy equals that run's `DAT_007e9584` |
| `0x007d70c8`, `0x007d7108` | MASK:renderware | `0x004dc8bf` ... (dynamic VB ring slot); `0x004e0943` / `0x004e0f4e` `= 0`, `0x004e1019 +=` (index start passed to device vtable `+0x148` at `0x004e1007`) | render-internal fill offsets. The exe uses librw. Pinning them mid-frame would move index data the device still uses |
| `0x007d70cc..0x007d70ec` | RULE:RING4 | `0x004dccc8` (`FUN_004dcbb0`) | existing rule |
| `0x007dc948..0x007de000` | MASK:audio | audio engine `0x005a59bc..0x005ba1b3` | RESULT_RO-AUDIO-ISOLATION.md |
| `0x007dc764` | MASK:rw-fs | `0x005505dc` (`FUN_00550580`, hooks.csv `third-party-library[renderware]`) | written after the overlapped reads (DESIGN 1.5). Values `0x3e5` / `0x3e4` differed in batch 1 |
| `0x006036c0`, `0x006036cc`, `0x006904c4..0x006904dc` | MASK:audio-glue | `0x0045df8d` / `0x0045dfb0`, `0x0046590c` / `0x00465918`, `0x004658e1` | follow the audio voice status (RESULT_RO-AUDIO-ISOLATION.md) |
| `0x006147d8`, `0x00771a00`, `0x0077391c` | MASK:os | `0x00493eeb` / `0x0049479a`; `0x00493ef0` / `0x00494794`; `0x0049985e` (`FUN_004997b0`, a 4-argument callback (`ret 0x10`) that stores `[esp+0xc] == 1`) | `0x0077391c` is the focus flag read at `0x004996cc` (the gate that `no_focus_pause` makes `0x004996d3` jump past). It changed at tick 215 in run `n3_a` only |
| `0x007e95a0` (28 bytes) | RULE:WINMSG | `PeekMessageA` `0x0049969d` | existing rule |
| `0x007ec9e4` ring, `0x006fe790` particle ring | RULE:STACKROW / RINGSTALE / RWPAD | RESULT_RO-STACKROW-READERS.md | stale stack, compared by rule |

**Correction to REPLAY-ORACLE-DESIGN.** The ignore range `0x007dc400-0x007dd400` and the DESIGN's "audio
(`0x007dc000..0x007de000`)" are wider than the audio engine. `0x007dc000..0x007dc944` holds RenderWare, RW-Physics
and qhull globals, including the RW RNG plugin offset `0x007dc578`. Game code reads `0x007dc57c` (`0x00441070` ...).
The audio mask here starts at `0x007dc948`, so those globals stay compared dword by dword at the checkpoints
(`audio_readers.tsv`). The per-tick compare still skips block `0x007dc400` because it holds the masked `0x007dc764`.
RO-MASKS should make the per-tick ignore dword-level.

## Before and after (checkpoint classes, 15 checkpoints per pair)

`reports/norm_scores.txt` (`score_norm.py`). Batch 2 as published (`reports/batch2_scores.txt`, before this branch's
rules):

| pair | RESIDUE | OFFSET | MODOFFSET | TOGGLE | RULE | PTR |
|---|---|---|---|---|---|---|
| t2_a / t2_b | 19 | 6 | 97 | 2 | WINMSG 2 | 3,316 |
| t2_a / t2_c | 18 | 6 | 97 | 2 | WINMSG 2 | 3,316 |
| t2_a / t2_h | 17 | 4 | 1 | 2 | RING4 2, WINMSG 2 | 3,315 |
| t2_b / t2_c | 19 | 6 | 97 | 2 | WINMSG 2 | 3,316 |
| t2_b / t2_h | 17 | 6 | 98 | 2 | RING4 2, WINMSG 2 | 3,315 |
| t2_c / t2_h | 19 | 5 | 98 | 2 | RING4 2, WINMSG 2 | 2,863 |

Batch 3, with the pins and `--norm` (10 pairs among `n3_a`, `n3_b`, `n3_c`, `n3_h`, `n3_w`): RESIDUE, OFFSET,
MODOFFSET, TOGGLE and UNEXPLAINED are 0 in every pair. Every pair is normalised 15/15 and race state 15/15. Per
pair: MASK:audio 8-9, MASK:crt-clock 1-2, MASK:crt-cookie 1, MASK:os 2, MASK:renderware 1-2, RULE:FILEPTR 0-1,
RULE:HWND 4, RULE:RING4 0-2, RULE:WINMSG 2-5.

Batch 2 re-scored with the masks and rules but no pins: RESIDUE 3-7, OFFSET 2 and MODOFFSET 97 remain. Those are the
pinned counters and frontend floats, so the pins, not the masks, remove them.

Per tick (`reports/batch3_identical_for_n.txt`, ignore `0x006147d8-dc, 0x00771a00-04, 0x007dc948-0x007de000,
0x007dc764-68`):
- pairs among `n3_b`, `n3_c`, `n3_h`: IDENTICAL-FOR-N 2,724 / 2,724
- `n3_a` against the others: 215, because of the focus flag `0x0077391c` at tick 215, which is masked now
- `n3_neg`: 7

## Runs

6 launches of `MASHED.exe` for this row (batch 3; 14 across the three rows) from `D:\MashedWT\RO-WARP-NORMALIZE_root`, a robocopy of
`D:\MashedWT\REPLAY-ORACLE-DESIGN_root` with the same binaries: `MASHED.exe` `b9977dab...`, `d3d9.dll` `47901951...`,
`mashed_qol.asi` `1567c95b...`, `mashed_re_dev.asi` `906f8601...` (not hooking), `dinput8.dll` `5f850f29...`.

- Each launch ran inside `mashed_lock.py run --label RO-WARP-NORMALIZE` with `test-slot` outside.
- Only the PIDs that `detorig.py` spawned were killed.
- 14 of 14 completed with 15 checkpoints, 0 crashes, 0 piz collisions.
- Batch 3 (`batch3.sh`) is the batch-2 configuration plus `--normalize re/frida/warp_normalize.tsv`:
  - `n3_a`, `n3_b`, `n3_c`: no load hold
  - `n3_h`: 50 ms load hold
  - `n3_neg`: negctl 1
  - `n3_w`: write watch on `0x00616110`
- Load lengths: 120 / 118 / 118 / 118 / 119 / 119 main-thread clock calls.

## Harness changes

- `re/frida/js/detorig.js`:
  - `pinNorm()` (`cfg.norm`, `cfg.normAt`)
  - `audioOff()` (RO-AUDIO-ISOLATION)
  - the stale-field poison CModule (RO-STACKROW-READERS)
- `re/frida/detorig.py`:
  - options `--normalize`, `--normalize-at`, `--audio-off`, `--poison`, `--poison-value`
  - `load_norm()`
- `re/frida/detorig_report.py`:
  - `--norm` (MASK rows)
  - rules HWND, FILEPTR, RINGSTALE, RWPAD
- `re/frida/warp_normalize.tsv` (new).
- `verify/replayoracle_20261111/`:
  - `run_arm_norm.sh`, `batch3.sh` / `batch4.sh` / `batch5.sh`
  - `score_norm.py`, `audio_readers.py` / `.tsv`, `ring_census.py`
  - `reports/`, `meta/<run>.json`
  - `raw_sha256_ro_orig_side.txt`: SHA-256 of every raw file, 273 lines
- Raw data: `D:/MashedWT/RO-WARP-NORMALIZE_data/<run>/` (1.4 GB).

For the exe side (RO-EXE-REPLAY-KNOB, RO-MASKS): apply the PIN rows at the warp tick and at the first phase-3 tick,
at the same VAs. Score with `--norm` and the same rules. A dword that is outside this file and differs is a first
divergence, not noise.

Gates: not run. The change is under `re/frida/` and `verify/`, no build input changed, and the exe gate hashes cannot
move. No tracker edit, no `ROADMAP.md` edit, no default flip, no patch to `original/MASHED.exe`.
