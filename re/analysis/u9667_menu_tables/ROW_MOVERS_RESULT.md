# Challenge-row movers ported (U-9672 item 2) — 2026-10-12

Branch `work/row-movers`, base `38a9e0f3`. User decision 2026-10-12: the challenge row on screens 6/7 must
behave like the original. This replaces the [Bug #2 / 2026-09-06] directive (row wrapped over
`cup.trackCount`, could rest on a locked row, launch gated in exe_main).

**Verdict.** The standalone now runs FUN_004322c0 (DOWN) and FUN_004323c0 (UP), transcribed from the bytes,
and the original's single launch test. Against the original with matched input and matched save, the
three scenarios the original survives match on row / selection / mode / cursor at every key: 30 key
steps plus 3 screen entries, launch outcome included. The one scenario that differs is the one where the
original crashes (row -1, section 6).

## 1. The original, from the bytes

Capstone over `original/MASHED.exe.unpatched` (anchor BDCAE093...). Full per-instruction citations are in
the source comments (`Frontend/FrontendNav.cpp`). These points go beyond, or correct, the brief and
the old Ghidra-based plate:

- **UP retries move the row by 2.** After the selection walk, FUN_004323c0 does `dec ebx` (0x00432429 /
  0x00432439) and jumps to the loop top, which decrements again (0x004323d6). The DOWN mover's retry is
  `dec ebp` (0x00432398) then `inc ebp` at the top (0x00432300), so it retries the same row. Measured: on
  the edited save, UP from row 3 in mode 4 lands on row 0 in mode 3 (section 5, edited save, step 12).
- **UP can store -1.** When the row is unavailable and the selection is already at the minimum,
  0x0043243c `dec ebx` + 0x0043243d store, with no clamp: row 0 -> -1. The original then crashes (section 6).
- **UP calls FUN_0042f6b0 on every retry** (0x00432424 / 0x00432434), with no mode test. DOWN calls it only
  in modes 3/4/5 (0x0043237e..0x0043238b) and reloads the mode after (0x00432392).
- **DOWN's minimum selection depends on the mode** (`[0x67ea68]` re-read inside the loop at 0x00432332): 1 or
  3 by DAT_0067ea68, 0xb in mode 10 (0x0043234a), 0 in mode 2 (0x00432356). UP has neither override
  (0x004323fa..0x00432403).
- **DOWN clamps at 0xc** (0x00432301..0x00432317) and zeroes DAT_0067f1a4. UP clamps at 0 and zeroes it
  (0x004323df..0x004323e7).
- **FUN_00430910 excludes selection 11 in modes 6..9.** Case B (0x0043095a) falls through 0x0043097c /
  0x0043097f into 0x00430981 `cmp eax,0xb`. The existing C3 port (from the Ghidra decomp) omitted 11 and
  read the table instead. Fixed here (section 3).
- FUN_00430910 returns `mode - 3` for modes outside 3..9 (0x0043091b `ja 0x00430999`), non-zero for
  modes 2 and 10.
- FUN_00430830's column per mode (0x00430848..0x004308da): 2->1, 3->1, 4->2, 5->5, 6->3, 7->8, 8->9,
  9->0xa, 10->1. FUN_004309b0's (the launch gate, 0x004309b0..0x004309df; jump table 0x004309e8): 2->0,
  3->1, 4->2, 5->5, 6..9->3, 10->0xb. The two agree only for modes 3/4/5/6.
- DAT_0067f1a4 is zeroed at the top of FUN_0043dfd0 on every call (harvest L61), so the 1 the UP/DOWN
  arms write (L1346 / L1353) lasts one frame. Every original sample reads 0.
- RIGHT on screens 6/7 (harvest L1596-1645, FUN_0042b540 reads flag byte 0x007f1045 at 0x0042b554) moves
  the selection 1 -> 2 -> 5 then calls FUN_0042f6b0. The standalone's existing tier ring matches it
  (measured below); it now also writes the selection.

## 2. Globals: where each lives in the standalone

| original | role in the movers | standalone |
|---|---|---|
| DAT_0067f17c | row | `g_challenge_row` (MenuNavSM.cpp), `Nav_ChallengeRow()`. Unchanged |
| DAT_0067f184 | mode selection, walked down | **new** `MenuGameState::sel_f184`. Written by ApplyActionGameMode (0xff3d0000 -> 1 L259, 0xff400000 -> 0 L262, 0xff4d0000 -> 0xb L931, 0xff2c0000 / 0xff2e0000 -> 3 L827) and by the LEFT/RIGHT tier ring in exe_main (mode 3/4/5 <-> sel 1/2/5) |
| DAT_0067e9fc | game mode | `MenuGameState::game_mode`. Unchanged |
| DAT_0067ea68 | minimum-selection switch | **new** `MenuGameState::ea68`. 0 on 0xff3d0000 / 0xff400000 (L275) and 0xff4d0000 (L943), 1 on 0xff710000 / 0xff720000 (L1200 / L1206) |
| DAT_0067f1a4 | flag | **new** file-static `g_nav_flag_f1a4`: 1 from both arms, 0 from the movers and from `Nav_FrameInputBegin()` each frame (L61). Nothing in the standalone reads it. Logged |
| 0x007f0a40 table | input of FUN_00430830, FUN_00430910, the launch gate | read **live at 0x007f0a40** |

Fresh values: 0x0067f184 and 0x0067ea68 lie past `.data`'s raw size (raw ends 0x00637000), so both are 0 at
load. The original samples agree (selection 0 at screen 2).

**How FUN_00430830 is fed.** The standalone's Campaign unlock (`Campaign_CurrentCup`: column 4 OR the
sidecar progression) is NOT what FUN_00430830 reads. It reads a mode-selected column of the 13x12 table at
0x007f0a40. The standalone already mirrors the save span to that live address (`Nav_GameStateLoadSave`,
the analogue of the REP MOVSD at 0x00404e91), so the ported callees read exactly the table the original
restored from the same save. Positive control: the original's live table at screen 6 showed the edited
cells (`row_movers/orig_unl_m3.json`, `tbl_at_last_level`). Campaign unlock is no longer consulted on
this screen. It still drives the standalone's own progression display.

## 3. The port

One body per RVA, two bindings. `Frontend/RowMovers.h` declares the cores. The hook export of each RVA
binds its core to the image's globals. The standalone binds it to `MenuGameState`.

| RVA | core | file | hook export |
|---|---|---|---|
| 0x004323c0 | `RowMoveBack(io)` | FrontendNav.cpp | `MenuCursorBack` (C3), re-bound to the core. Its callees are still the image's, called by VA |
| 0x004322c0 | `RowMoveFwd(io)`, from 0x004322ea | FrontendNav.cpp | none. FUN_0042aa00(1) and the kind 6/7 test stay in the caller (`Nav_MoveCursor`: `CursorStep` + `screen_id` 6/7) |
| 0x00430830 | `SplitScreenTrackAssignmentFor(mode, p)` | MenuScoreSort.cpp | `SplitScreenTrackAssignment` |
| 0x00430910 | `MenuOptionSlotGetFor(mode, sel, row)` | FrontendState.cpp | `MenuOptionSlotGet`, now with the selection-11 exclusion in modes 6..9 |
| 0x004309b0 | `FrontendModeIndexFor(mode)` | FrontendMode.cpp | `FrontendModeIndex` |
| 0x0042f6b0 | existing `Race::RaceModes::SelectionToGameMode` | RaceModes.cpp | unchanged. The standalone writes the mode only when it returns >= 0 (no write for sel 4/6/7/>11, as the switch default at 0x0042f6b8) |

Hook-side regression, path1 against this branch's `.asi` (copies in `row_movers/run_diff/`):
`menu_cursor_back`, `menu_option_slot_get`, `frontend_mode_index`, `split_screen_track_assignment` all
GREEN, 0 mismatches. These vectors run at quiescent menu state. They do not reach selection 11 in modes
6..9, so they say nothing about the FUN_00430910 fix.

## 4. Directive removed, launch gate

- `Nav_MoveCursor`: the wrap over `cup.trackCount` is gone. On screens 6/7 it calls `RowMoveBack` /
  `RowMoveFwd`. The comments that cited the directive are updated.
- exe_main: the [D-11054] mode 4/5 tier gate and the [Bug #2] mode-3 Campaign guard are replaced by the
  original's one test, from FUN_0043dfd0's 0xff260000 arm (harvest L405-409):
  `(&DAT_007f0a40)[FUN_004309b0() + DAT_0067f17c*0xc] == 0` -> no launch, no push. The original does refuse
  at that point, so a gate stays, now in the original's form. It still matters under the movers: the
  columns differ for modes 2/7/8/9/10, the movers can leave the row on an unavailable 0 or -1, and
  LEFT/RIGHT changes the mode without moving the row. `MASHED_CUP_TIERS=0` no longer disables a launch
  gate (it still gates the LEFT/RIGHT ring).
- The gate uses `launchMode` (the game mode with exe_main's existing 0 -> 3 default). Mode 0 on screen 6
  happens only on dev routes that jump there: `MASHED_RACE_DEMO=1 MASHED_GOTO=6` is the D2 capture recipe
  in `re/tools/ab_report.py`, with no mode or track override. With the raw mode 0 the column would be
  FUN_004309b0(0) = -2 and that recipe would never launch. The original cannot be on screen 6 in mode 0.
- The screen 6/7 detail panel read the image's `0x0067f17c` directly. The standalone never writes that
  address, so the panel always showed row 0. It now reads `Nav_ChallengeRow()`.
- `NAV_KEYS` log lines now also print `sel`, `ea68`, `f1a4`.

## 5. Traces: original vs standalone

Original: `re/frida/u9667_menu_table_dump.py --lane input --route <r> --last-keys <keys>` (new option: an
explicit key list at the last route level instead of the cursor sweep; the dump now also records
DAT_0067f184, DAT_0067ea68, DAT_0067f1a4, the 13x12 table and the 12 dwords before it). Each original
cell is a sample after the press. Standalone: `MASHED_GOTO=2` + `MASHED_NAV_KEYS` (prefix `e,e` for
Challenge Cup, `d,e,e` for Quick Race), log `./mashed_re.log`; each standalone cell is the state logged
at the next tap, i.e. after the press. RIGHT is statenav code 10 (two below DOWN's 12, flag bytes
0x007f1045 vs 0x007f1047); confirmed by the selection moving 1 -> 2.

Saves: the reference save is `original/gamesave.bin` (SHA-256 BD18788182B2343E..., DEADBEEF, column 1 set
on row 0 only). The edited saves are copies with only table cells changed (`row_movers/mk_save.py`).
The original booted them from an install copy via `MASHED_ROOT` (top-level files copied, TOASTART /
toastaudio junctioned, `MASHED.exe` hash equal to the live one). `original\` was never written. The
standalone read the same files via `MASHED_SAVE`.

#### Reference save, Challenge Cup (mode 3)

Original `orig_ref_m3.json`, standalone `sa_ref_m3_raw.txt`. Cells are row / sel / mode / cursor.

| # | key | original | standalone | match |
|---|---|---|---|---|
| 0 | entry | 0 / 1 / 3 / 0 | 0 / 1 / 3 / 0 | yes |
| 1 | d | 0 / 1 / 3 / -1 | 0 / 1 / 3 / -1 | yes |
| 2 | u | 0 / 1 / 3 / -1 | 0 / 1 / 3 / -1 | yes |
| 3 | r | 0 / 2 / 4 / -1 | 0 / 2 / 4 / -1 | yes |
| 4 | d | 0 / 1 / 3 / -1 | 0 / 1 / 3 / -1 | yes |
| 5 | u | 0 / 1 / 3 / -1 | 0 / 1 / 3 / -1 | yes |
| 6 | r | 0 / 2 / 4 / -1 | 0 / 2 / 4 / -1 | yes |
| 7 | r | 0 / 5 / 5 / -1 | 0 / 5 / 5 / -1 | yes |
| 8 | u | 0 / 1 / 3 / -1 | 0 / 1 / 3 / -1 | yes |
| 9 | e | 0 / 1 / 3 / -1 (phase 0 = launch) | 0 / 1 / 3 / -1 (flow 1 = launch) | yes |

Step 4 is the DOWN selection walk: row 1 has no mode-4 cell, the walk drops the selection to 1, FUN_0042f6b0
sets mode 3, the retry of row 1 fails again and the row reverts to 0. Step 8 is the UP walk from mode 5.

#### Reference save, Quick Race (mode 10)

Original `orig_ref_m10.json`, standalone `sa_ref_m10_raw.txt`.

| # | key | original | standalone | match |
|---|---|---|---|---|
| 0 | entry | 0 / 11 / 10 / 0 | 0 / 11 / 10 / 0 | yes |
| 1 | d | 0 / 11 / 10 / -1 | 0 / 11 / 10 / -1 | yes |
| 2 | u | 0 / 11 / 10 / -1 | 0 / 11 / 10 / -1 | yes |
| 3 | e | 0 / 11 / 10 / 0 (push screen 24, 0x5f7370) | 0 / 11 / 10 / 0 (push screen 24) | yes |

#### Edited save (some tracks unlocked), Challenge Cup

Cells changed: column 1 (mode 3) = 1 on rows 0, 1, 2, 4, 5 (row 3 left 0), column 2 (mode 4) = 1 on row 3.
Original `orig_unl_m3.json`, standalone `sa_unl_m3_raw.txt`.

| # | key | original | standalone | match |
|---|---|---|---|---|
| 0 | entry | 0 / 1 / 3 / 0 | 0 / 1 / 3 / 0 | yes |
| 1 | d | 1 / 1 / 3 / -1 | 1 / 1 / 3 / -1 | yes |
| 2 | d | 2 / 1 / 3 / -1 | 2 / 1 / 3 / -1 | yes |
| 3 | d | 2 / 1 / 3 / -1 | 2 / 1 / 3 / -1 | yes |
| 4 | d | 2 / 1 / 3 / -1 | 2 / 1 / 3 / -1 | yes |
| 5 | u | 1 / 1 / 3 / -1 | 1 / 1 / 3 / -1 | yes |
| 6 | u | 0 / 1 / 3 / -1 | 0 / 1 / 3 / -1 | yes |
| 7 | u | 0 / 1 / 3 / -1 | 0 / 1 / 3 / -1 | yes |
| 8 | d | 1 / 1 / 3 / -1 | 1 / 1 / 3 / -1 | yes |
| 9 | d | 2 / 1 / 3 / -1 | 2 / 1 / 3 / -1 | yes |
| 10 | r | 2 / 2 / 4 / -1 | 2 / 2 / 4 / -1 | yes |
| 11 | d | 3 / 2 / 4 / -1 | 3 / 2 / 4 / -1 | yes |
| 12 | u | 0 / 1 / 3 / -1 | 0 / 1 / 3 / -1 | yes |
| 13 | d | 1 / 1 / 3 / -1 | 1 / 1 / 3 / -1 | yes |
| 14 | d | 2 / 1 / 3 / -1 | 2 / 1 / 3 / -1 | yes |
| 15 | r | 2 / 2 / 4 / -1 | 2 / 2 / 4 / -1 | yes |
| 16 | r | 2 / 5 / 5 / -1 | 2 / 5 / 5 / -1 | yes |
| 17 | d | 3 / 2 / 4 / -1 | 3 / 2 / 4 / -1 | yes |
| 18 | e | 3 / 2 / 4 / -1 (phase 0 = launch) | 3 / 2 / 4 / -1 (flow 1 = launch) | yes |

Steps 3-4: the row stops before the gap (row 3 has no mode-3 cell, selection already at the minimum).
Step 12: the UP double decrement, row 3 -> 0. Step 17: DOWN from mode 5 walks the selection 5 -> 4 -> 3 -> 2
(4 and 3 excluded by FUN_00430910 case A), switches to mode 4 and lands on row 3. Every value here was
predicted from the bytes before the original was run.

#### Edited save, row 0 locked in mode 3

Cells changed: column 1 = 0 on row 0, 1 on row 1. Original `orig_r0lock_m3_exc.json`, standalone
`sa_r0lock_m3_raw.txt`.

| # | key | original | standalone | match |
|---|---|---|---|---|
| 0 | entry | 0 / 1 / 3 / 0 | 0 / 1 / 3 / 0 | yes |
| 1 | u | process died | -1 / 1 / 3 / -1 | no |
| 2 | d | - | -1 / 1 / 3 / -1 | - |
| 3 | u | - | -1 / 1 / 3 / -1 | - |
| 4 | e | - | -1 / 1 / 3 / -1 (flow 0 = refused) | - |

## 6. Row -1 crashes the original

Every press that stores row -1 by the bytes killed the original, and no other press did:

| run | keys | result |
|---|---|---|
| `orig_r0lock_m3.json` | u,... | died on the first `u` |
| `orig_r0lock_m3_run2.json` | u,... | died on the first `u` |
| `orig_r0lock_ctl.json` (control) | d,d,u | `d` -> 1, `d` -> 1 (revert), then died on `u` (row 1 -> 0 unavailable -> -1) |
| `orig_r0lock_m3_exc.json` | u,... | died on the first `u`, exception captured |
| `orig_zero_m3.json` (all-zero save) | d,u | `d` -> 0, then died on `u` |

Exception (`Process.setExceptionHandler`, no Interceptor): access violation at **0x004c5c2d**, read of
0x00000000 (0x3274 on the first capture), with DAT_0067f17c = -1. Backtrace 0x0040bb81 <- 0x0042e803 <-
0x0043c678. Mechanism from the bytes: FUN_0042e590 reads the sprite name from
`[0x005f79d8 + (arg + DAT_0067f17c*2)*4]` (0x0042e594..0x0042e59d) and tail-jumps to 0x0040bb70, which calls
FUN_004c5c00 (dictionary name search) with it. At row -1 the name is the dword before that table, not a
string pointer, and the compare loop faults at 0x004c5c2d.

The standalone stores the same -1 (faithful mover) but does not crash: its renderer clamps the row for
display and launch (exe_main, `Nav_ChallengeRow()` clamped to `cup.trackCount`), and the launch gate reads
`tbl[1 - 12]` = 0x007f0a14, which is 0, so the confirm is refused. On the original 0x007f0a14 also holds 0
(`tbl_pre_at_last_level`), but it never reaches the confirm.

Reachability: -1 needs the current mode's cell for row 0 to be 0 with the selection at its minimum. On
the reference save row 0 is set in column 1 (modes 2, 3, 10), so the shipped save never reaches it.

## 7. Blank save

An all-zero `gamesave.bin` (magic 0) leaves the original's table all zero at screen 6
(`orig_zero_m3.json`): there are no code defaults on this path. The standalone with an absent save also
has an all-zero table. Both keep row 0 on DOWN. On UP both store -1: the original crashes, the standalone
refuses the launch (`sa_blank_m3_full.txt`, `MASHED_SAVE` set to a missing file). Before this change the standalone launched row 0 here through the Campaign fallback
("first track unlocked"), which the original does not have. (`gamesave.bin.refbak_20260830` is not
blank. It is a DEADBEEF save with the same table as the reference: `orig_refbak_m3.json`.)

## 8. Regression

- `re/tools/nav_coverage.py` before (HEAD sources) and after: identical (`row_movers/nav_coverage_*.txt`).
- U-9667 trace scenarios (`screen4_push6_traces.txt`): mode 3 `GOTO=2 e,e,d,u,e` -> screen 4, screen 6,
  `d`, `u`, launch (`sa_u9667_m3_full.txt`). Mode 10 `d,e,e,d,u,e,e` -> screen 6, `d`, `u`, push 24,
  launch (`sa_ref_m10_raw.txt`). Both match the old traces except the row column: after `d` the row is
  now 0 (the original's value), where the directive gave 1.
- `re/analysis/standalone_menu_sm/harvest/build_navsm_test.bat`: MenuNavSM.cpp now calls the cores, so the
  harness links FrontendNav.cpp, FrontendState.cpp, MenuScoreSort.cpp and Race/RaceModes.cpp too. It then
  passes Pieces 1-3 and stops in "R2-2: save-driven game state", exactly as HEAD does when built from a
  `git archive` of 38a9e0f3. Cause, not fixed here: `Nav_GameStateLoadSave` writes the live 0x007f0a40,
  which a plain test exe does not map.
- Depth numbering differs (original 5/6, standalone 3/4). That predates this change and is not part of it.

## 9. What still differs, and [UNCERTAIN]

- Row -1: the original crashes (section 6), the standalone refuses the launch. Reproducing a crash is not
  a goal. Recorded so nobody reads the standalone's refusal as the original's behaviour.
- The standalone's challenge list has `cup.trackCount` = 8 rows. The original's row ranges over 0..0xc.
  The movers now produce 0..0xc. Display and launch still clamp to the 8 cup tracks, so a row of 8..0xc
  would show and launch the last cup track. Not reached on the reference save (column 1 set on row 0
  only).
- [UNCERTAIN] The refusal keeps the standalone's "menu navigation" SFX, and `_DAT_0067e840 = 0.0` from the
  locked branch (L407) is not ported. What the original plays on that path is not captured.
- Not ported, as before: the hold-repeat of FUN_0042aff0 / FUN_0042b180 (edge-only here) and the
  `DAT_00898ab0 == 0` condition on both arms (L1347 / L1353).
- The detail panel still reads the image's `0x0067ea64` (team play) directly. The standalone keeps that
  flag in `MenuGameState::flag_ea64`, so the panel's Team Play heading never switches. Same class as the
  row read fixed here. Left alone: not part of the row.
- Mode 0 on screen 6 (dev jump routes only) gates on mode 3's column (section 4).

## 10. For the parent (tracker items, not edited here)

- U-9672 item 2: resolved by this branch (user decision 2026-10-12).
- 0x004322c0 (C2, hooks.csv `FUN_004322c0`): now has a body, `RowMoveFwd` in FrontendNav.cpp (compiled
  into both targets, called only by the standalone, no hook install). Its standalone behaviour is measured
  above (33 matched samples), but there is no
  hook-side path1 diff for it.
- 0x004323c0 MenuCursorBack (C3): body re-bound to `RowMoveBack`. Same final state as before. path1 GREEN.
- 0x00430910 MenuOptionSlotGet (C3): behaviour change. Selection 11 in modes 6..9 now returns 0 as the
  bytes do. Its C3 vectors (`arg_type none`, quiescent menu) never exercise that case. It needs a vector
  that seeds mode/selection/row to have evidence for the fix.
- 0x00430830, 0x004309b0: refactor only (core + binding). path1 GREEN.

## Reproduce

```
# original (kill-by-PID is built in; MASHED_ROOT optional, points at an install copy)
py -3.12 re/frida/u9667_menu_table_dump.py --lane input --route 0,0,k --last-keys d,u,r,d,u,r,r,u,e --out <json>
py -3.12 re/frida/u9667_menu_table_dump.py --lane input --route 1,0,k --last-keys d,u,e --out <json>
MASHED_ROOT=<copy> py -3.12 re/frida/u9667_menu_table_dump.py --lane input --route 0,0,k \
    --last-keys d,d,d,d,u,u,u,d,d,r,d,u,d,d,r,r,d,e --out <json>
# edited save: py -3.12 -I re/analysis/u9667_menu_tables/row_movers/mk_save.py original/gamesave.bin <copy>/original/gamesave.bin \
#     1:1=1 2:1=1 4:1=1 5:1=1 3:2=1        (row 0 locked: 0:1=0 1:1=1)
# standalone, from the worktree root (MASHED_SAVE=<edited save> for the edited legs):
MASHED_GOTO=2 MASHED_NAV_KEYS=e,e,d,u,r,d,u,r,r,u,e mashedmod\build\mashed_re.exe   -> NAV_KEYS lines in .\mashed_re.log
```
