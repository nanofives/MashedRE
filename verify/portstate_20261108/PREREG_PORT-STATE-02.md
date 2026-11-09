# PREREG PORT-STATE-02 (2026-11-08): the L-99 (a) seed table, MASHED_PORTSTATE_SEED default OFF

Branch `orch/PORT-STATE-02` off `orch/PORT-STATE-01` @ `9c653e93b`. Code commits `84d098ac9` (generator, lint, exe
consumer) and `894f7a084` (inventory hash with CRLF folded). Frozen build from `894f7a084` with `MASHED_NO_DEPLOY=1`:
`mashed_re.exe` sha256 `8cdd450f86390993e057e989f21919a05286bb5a1fcd44da8b5a3a21ddb587ff`, `mashed_re_dev.asi`
`2e1a8bb79bf9510be3426f12f62cced95ccac31da2fc879c5f1c83d5c4845f1b` (copies in `D:/MashedWT/PORT-STATE-02_data/exe/`).
This file is committed alone, before any run with the seed ON.

## What the change is

- `re/tools/gen_portstate_seed.py` maps the 100 addresses of `verify/portstate_20261108/SEED_CANDIDATES.tsv` (1,430
  bytes) through the PE section table of `original/MASHED.exe.unpatched` (SHA-256 anchor checked first) and writes
  `mashedmod/src/mashed_re/Compat/PortStateSeed.inc`, FNV-1a `0xdd093ea5`. It refuses a non-anchor binary, an address
  in no section or in a section's uninitialized tail, overlapping ranges, and a string entry that does not end on its
  NUL. All 100 resolve (23 .rdata strings, 9 .rdata scalars, 66 .data scalars/arrays, 2 .data pointers).
- `--check` (wired in `mashedmod/build.bat` after asm-lint) fails when the table differs from the inventory, from the
  binary, or from its own recorded FNV.
- `Compat/PortStateSeed.cpp` (exe only): the first statement of `WinMain` copies the table into the B17 pad when
  `MASHED_PORTSTATE_SEED` is set (read once). After the log truncate it writes one `[portstate-seed]` line. OFF writes
  nothing and logs nothing.

## Static reading behind the predictions

The exe code that names a seeded address (SEED_CANDIDATES `exe` = lit/name, plus the compiled matched rows):

| address | exe reader | why the seed does not change what it reads in a race |
|---|---|---|
| 0x005f2770 | `TrackRenderer.cpp` `aib_veh_type`, the s==6 count, `AiLeafTargets.cpp`, `AiLeaderTimer.cpp` | `Ai_BridgeLoad` writes the same value 0x005f2728 (`TrackRenderer.cpp:661`, default ON). Before that write the readers either test the pointer (0 = no table) or, seeded, read `0x005f2728 + 0x34 + 4v` = 0x005f275c..0x005f2768, which are not seeded and stay 0: both give 0 |
| 0x00613108 / 0x00613114 / 0x00613130 / 0x0061313c | `AiStandalone.cpp:1643` `GearConstGet` | `VehicleInit.cpp:149-150` `A3PublishHandling` writes all four at vehicle init (`MASHED_PORTSTATE_RECORDS`, default ON since L-88) before the AI reads |
| 0x005f7a48 .. 0x005f9368 (13 camera tables) | `CameraEntries.cpp:266-281` | the exe branch (`#ifdef MASHED_STANDALONE`) uses its private `CameraEntryTables.h` copies, not the addresses |
| 0x00613288 / 0x00613290 / 0x006132c4 / 0x006132c8 | `RfxEmitter.cpp:47-62` | exe branch uses private copies |
| 0x005cf010, 0x005cfe70, 0x005d00d8, 0x005d013c, 0x005d0158 | `SettingsConfig.cpp:54-58` | exe branch uses string literals |
| 0x005ccac0 | `MenuNav.cpp:59` `kHoldThreshold` | declared, never read |
| 0x00614718 | `SmallLeaves_n2.cpp:120` `ElapsedVsThresholdCheck` (0x004307a0) | the live exe path is `MenuNavSM.cpp` `Q_CupContinueVariant` on its private `kCupContinueThreshold`; the export is reached only from `Match/MatchS08_0043aee0.cpp` |
| the compiled matched rows with a data blocker: 0x0042aff0, 0x0042b180 (0x005ccac0), 0x00429aa0 (0x00614718), 0x004926c0 (0x006147b8) | | `hooks.csv` port = `body` for all four: compiled, not called |

So the prediction for every flow below is **no observable change** with the seed ON. The only intended difference is
the `[portstate-seed]` log line.

## Arms and predictions

Private roots `D:/MashedWT/PORT-STATE-02_root` and `_root13` (robocopy of `original\`). Every game run: `test-slot`
outside, `mashed_lock.py run --label PORT-STATE-02` inside (`--solo` for D2), only own PIDs stopped.

| id | arm | prediction |
|---|---|---|
| S1 | static | build OK both targets; rva-lint `NEW=0`; asm-lint OK; seed `--check` OK; `backfill_exe_file.py` and `backfill_port.py` change no row (no RVA body was added or moved) |
| P1 | default `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26` | G-CTRL `8284be46/67931834/31fe043e`, B `1d5bf4f9` x3 (the `merge_train.json` refs) |
| P2 | seed ON `H_PSS B_PSS_K0 B_PSS_K1 B_PSS_K26` | the same hashes as P1 |
| P3 | seed witness | P2's logs carry `[portstate-seed] on entries=100/100 bytes=1430/1430 outside_pad=0 fnv table=dd093ea5 want=dd093ea5 mem=dd093ea5`; P1's carry no `[portstate-seed]` line |
| P4 | D2 `D2_DEFH_1..3` (default) and `D2_DEFHPSS_1..3` (seed ON), scored by `verify/d2band42_20261029/score_d2_v2.py` | all six inside the O42 band and H1 pass; the ON triple's three metrics equal the default triple's to the printed digits (D2 is wall-clock bound, so a run under 1600 motion_diag lines is re-run, not scored) |
| P5 | 13 tracks, `verify/rsky_20261016/run13.py` on `_root13`, seed OFF and seed ON, same exe | ON = OFF on every track (same binary, so no layout confound: the OFF run is the no-op control for the ON run). OFF scored against `verify/run13p3_20261029/tracks_score_sub_p3.txt`: no new RED row. If a non-TRAINING track's OFF row differs from the reference, the layout control is the PORT-STATE-01 frozen exe (`D:/MashedWT/PORT-STATE-01_data/exe/mashed_re.exe`, sha `1797934e`) run13 on the same root, and the row is attributed to layout only if that control moves the same way. Render arms need the LIVING-PC TV on (D3DERR 0x8876086A = INFRA, not a result) |
| P6 | boot-to-menu: `run_seed.py` with `MASHED_NAV_KEYS=x` (one no-op step after the menu is up), OFF and ON | both: `B17-SUMMARY reached-main-loop`, self-exit 0, identical NAV_KEYS sha8 |
| P7 | one Quick Battle by key script (route fixed by an OFF probe before this file's first ON run; recorded in the RESULT), OFF and ON | identical NAV_KEYS sha8 (tap, depth, screen, cursor, mode, flow per step), the flow reaches the race in both, self-exit 0 |
| P8 | lane-A retry, experiment build E (not merged, patch committed under this folder): `Save/SettingsConfig.cpp`'s exe bodies of 0x00498910 / 0x00498950 / 0x004989b0 replaced by the byte-matched `re/match/s16/00498950.c` (MATCH-S16, 100%); callees bound to the exe CRT (`_fsopen`/`fread`/`fwrite`/`fclose`) and to the exe log cores; buffers at 0x007731e8 / 0x00773208 | E, seed OFF: the matched `ConfigLoad` passes the zero bytes at 0x005cf010 as the fopen mode; the UCRT rejects an empty mode as an invalid parameter and the default handler ends the process, exit `0xC0000409`, before the menu. E, seed ON, videocfg.bin present: `[videocfg] videocfg.bin present, no write`, menu reached. E, seed ON, videocfg.bin absent: `[videocfg] first-run write ret=1` and a file byte-identical to the default exe's first-run file (same root, same adapter) |

## Not predicted / out of scope

- No .asi run: the .asi is unchanged in behaviour (`PortStateSeed.cpp` is exe-only) and must not be deployed.
- No tracker leg: no RVA body changes in the committed tree.
- The default flip is a decision row (`roadmap.ps1 decision PORT-STATE-02`), not done here.
