# C4 re-earn, triage A1 group M (frontend menu): real canonical scenarios

Evidence only. No tracker edit and no re-classify on this branch (`work/c4a1-m`). The parent promotes.

Date 2026-10-12. Port under test: the deployed `original/mashed_re_dev.asi`
(SHA-256 `C5594D6B933B24008E1FD408DBA41DBE6870B5E4CC6C0B3E7D5F7DC6F6373840`), never overwritten.
`original/MASHED.exe` is the ten-patch boot recipe, no unlock patch. All disassembly below is from
`original/MASHED.exe.unpatched` (the anchor).

## Verdicts

| RVA | row | verdict |
|---|---|---|
| 0x0042aff0 | MenuButtonDetectA | **C4 EARNED** |
| 0x0042b180 | MenuButtonDetectB | **C4 EARNED** |
| 0x0042b310 | MenuButtonDetectC | **C4 EARNED** (coverage note: the type 0x13 / 0x1e branch is not reached, see 6.3) |
| 0x0043c000 | TimerSlotTickDispatcher | **C4 EARNED, with a substituted control for the parent to accept**: the fix-doc witness (dword counter store) cannot show in the game, so the RED control is old defect 2 (unloaded ST0), see 6.4 |
| 0x0042ebe0 | FrontendPlayerSlotCheck | **C4 EARNED, with a stated forced input** (save-progress cells, not the path), see 5 and 6.5 |
| 0x00429aa0 | GameStateSlotsFill | **NOT EARNED**: no natural scenario found that calls the body (0 calls in 12 probe runs over every reachable Single Player route). Missing piece: a route to one of its three call sites, see section 2. |

Summary (all 20 runs alive at the end, 0 access violations. Exit `0xDEADBEEF` is the harness's own `dev.kill`):

| row | hook live (0xE9 pre-attach, target = export) | game calls through the JMP per run | mmOrig, hook-live runs | discriminating calls | control | control result |
|---|---|---|---|---|---|---|
| A | on1/on2/late1/onF1, `mashed_re_dev.asi+0x12430` | 2505..2543 | 0 / 0 / 0 / 0 | 36..37 per run | 4-byte stride | RED 37/37, 37/37 (mmCtl 0) |
| B | `+0x12450` | 2505..2543 | 0 / 0 / 0 / 0 | 35..38 | 4-byte stride | RED 36/36, 37/37 (mmCtl 0) |
| C | `+0x12e20` | 2502..2544 | 0 / 0 / 0 / 0 | 3 | 4-byte stride, 8 reads | RED 3/3, 3/3 (mmCtl 0) |
| T | `+0x18ed0` | 3286..3385 | 0 / 0 / 0 / 0 | 2666..2764 (state-1 ticks) | unloaded ST0 (defect 2) | RED 2698/2698, 2656/2656 |
| T | | | | | dword counter store (fix-doc witness) | GREEN 0/2613, 0/2724: not distinguishable, see 6.4 |
| F | `+0x15880` | 15903..16029 natural, 17490 forced | 0 / 0 / 0 / 0 | 0 natural, 5247 forced | rows 1..9 dropped | RED 6372/6372, 6460/6460 (forced) |

## 1. Call sites and the screens that reach them

Call sites (`re/tools/callsites.py`, every hit confirmed by capstone):

| row | call site(s) | gate |
|---|---|---|
| A 0x0042aff0 | `0x0043fca4` in FUN_0043dfd0 | `cmp [esp+0x24],ebx` / `je` at `0x0043fc9a`: screen opcode `ff0b` (re/tools/screen_opcodes.py) |
| B 0x0042b180 | `0x0043fedd` | `[esp+0x28]` at `0x0043fed7`: opcode `ff0c` |
| C 0x0042b310 | `0x0043ff29` | `[esp+0x2c]` at `0x0043ff1f`: opcode `ff0d` |
| T 0x0043c000 | `0x0043e05d` | none: runs on every FUN_0043dfd0 tick after the `0x00494ef0` branch (`0x0043e031..0x0043e058`) |
| F 0x0042ebe0 | `0x00439736`, `0x00439a71`, `0x00439e08`, `0x00439e31` in FUN_00439210 | called for p >= 10 (`cmp esi,0xa` / `jl` at `0x00439730`, `cmp eax,0xdc` at `0x00439e00`) |
| G 0x00429aa0 | `0x0043dda5` (FUN_0043d7c0), `0x0043e927`, `0x0043f99a` (FUN_0043dfd0) | see section 2 |

Input bytes (the cook at `0x00496633..0x0049669c`, player in EBX, record stride 0x4c): control 9 sets
`0x007f1044` (C's held byte, `0x00496648`), control 11 sets `0x007f1046` (A, `0x00496680`), control 12 sets
`0x007f1047` (B, `0x0049669c`). All presses in this work go through the `FUN_00497310(p, c)` return override that
statenav.py uses (no OS input).

Reachability probe (`re/frida/c4_m_probe.py`, stock game, entry-only interceptors on the original entries, no hook):
the Quick Battle route (statenav order) calls A and B on screen types 1, 2, 6, 7, 24, C on 4, 6, 7, 24, F on 6, 7, 24
(p = 10, 11, 12 every time), and T on every frontend tick. These are the screens the scenario drives.
Screen types 6 and 7 are the ones where A/B take the hold-repeat branch (`cmp eax,6` / `cmp eax,7` at
`0x0042b0bc` / `0x0042b0c1`).

## 2. GameStateSlotsFill 0x00429aa0: why NOT EARNED

Zero calls in all 12 probe runs (`evidence/c4_m/probes/`): Quick Battle x2, Challenge Cup x2, the column/row
movers on Challenge Select, the Single Player mode list at cursor 2..5 (it has two entries, Challenge Cup = game
mode 3 and Quick Battle = game mode 10, `0x0067e9fc`, so cursor 2..5 wrapped onto them), and 1..4 down-presses on
Game Type Select. Those presses landed before the Load modal and did not move the cursor, so all four of those runs
ended in Single Player too. Every run except `challcol` went to the race and back through Race Results. Screen types
reached: 0, 1, 2, 4, 5, 6, 7, 22, 24. Other Game Type Select entries were not reached.

The three call sites and what blocks each, from the bytes:
1. `0x0043e927`: the case for game mode 2 of the jump table at `0x0043e8de` (`[0x0067e9fc] - 2`, table
   `0x00440aac`, entry 0 = `0x0043e8e5`). Mode 2 is written only by `0x0042f6c1`, when the column index
   `0x0067f184 == 0` (switch at `0x0042f6b8`, table `0x0042f724`). On the stock save the progress table has
   `0x007f0a40` (row 0, column 0) = 0, and the column never moved off 1 or 11 under up/down presses on Challenge
   Select (`c4_m_challcol.json`).
2. `0x0043dda5`: FUN_0043d7c0 (called from `0x00492dd3`) reaches it when the pending action `[0x0067eabc]` is
   `0xff240000` (`0x0043db28`) or `0xff270000` (`0x0043dc5e`), then needs byte `[0x0067eac4] != 0`. The probe read
   `0x0067eac4 = 1` but `0x0067eabc = 0` on every sample, including the 2 s after the Race Results confirm
   (`c4_m_qb2.json`). The results confirm went straight to screen type 7.
3. `0x0043f99a`: item opcode `0xff4a0000` (`0x0043f962`). The only screen stream holding it is screen 21
   (`0x005f6828`), never reached.

Path A of the body (the `fld` + `call 0x004a2c48` stores at `0x00429ab1` / `0x00429aca`, the only place the old
"unloaded ST0" defect shows) also needs `[0x0067ed6c] != 0` (`0x00430820`). That dword is set to 1 or 2 only at
`0x0043e7b5` / `0x0043e7e6`, inside a handler that is entered only when it is already non-zero (`0x0043e78c`), and
it is cleared at `0x0043e88f` (challenge launch) and `0x0043df09`. It read 0 in every probe sample. So even a route
to path B would leave the control indistinguishable.

Missing piece for the parent: a natural route to `0x0043e927` (a save with the column-0 cell unlocked, then a
mode-2 launch) plus a state where `0x0067ed6c != 0`. Neither was found. No scenario was built.

## 3. Scenario (rows A, B, C, T, F)

`re/frida/c4_m_scenario.py`, one fresh MASHED per run, own PID only, killed by PID at the end:

1. Boot, title, Game Type Select, Load modal, Single Player (statenav order, unchanged).
2. Window `sp_down`: player 0 control 12 (Quick Battle). B returns 1 here.
3. Colour select (type 4). Window `t4_c9`: control 9 by players 0, 1, 2, then control 10 by player 0.
4. Track select (type 6). Windows `t6_hold12` / `t6_hold11`: player 0 holds 12, then 11, 1.5 s each (the
   repeat branch). Window `t6_taps`: 11, 12, 9 by player 1, the same by player 2, then 12 and 11 by player 0.
5. Game setup (type 24). Window `t24_taps`: 11, 12, 9 by player 1, 11 by player 2, 12 and 11 by player 0.
6. Start, race with statenav's 1 Hz control-4 pulses only while `0x00771968 == 3`, natural Race Results (type 5).
7. Confirm to type 7. Windows `t7_hold12`, `t7_hold11`, `t7_taps` as on type 6.

Every screen is entered by the game's own navigation, with no push and no poke. The one exception is the F-forced
arm set (section 5), which writes progress-table cells and says so.

Live slot records (identical in every run): `0x007f1a0c = 1` (slot path, not 0x1000) from Single Player on. Records
`0x007f1a14 + i*0x10` hold ids `0, -1, -1, -1` on Single Player, colour select and track select, and `0, 6, 7, 8` on
type 7 after the race. `FUN_0040e470(i)` reads `1, 0, 0, 0` before the race and `1, 2, 2, 2` after it, so only pad 0
is eligible on the 0x10-stride walk. The 4-byte walk of the old ports reads the dwords `0x007f1a14..0x007f1a50` as
ids. On track select those are `0, 0, 1, 0, -1, 0, 0, 0, -1, ...` with `FUN_0040e470(2) = 0`, so pad 1 becomes
eligible at index 2. After the race they are `0, 0, 1, 0, 6, 1, 0, 0, 7, 1, 3, 0, ...`, so pad 1 is eligible at
index 2 and at index 5 (`FUN_0040e470(5) = 6235944`). That is what the A/B/C controls hit: a player-1 press that the
original ignores.

## 4. Instrument

Per call of each row, at the original entry (off/stock) or at the E9 target (hooked rows), every input the bytes
read is snapshotted at entry and every output at leave. Two models are evaluated on the snapshot:
- `m_orig`: the original bytes. A/B `0x0042aff0..0x0042b173` (both x87 precision variants of the hold compare
  `0x0042b0c6..0x0042b0e3`, accepted if either matches). C `0x0042b310..0x0042b533`, including the `0x0067f1b8`
  clear. T `0x0043c000..0x0043c5aa`: the reset at `x == 0.0` (`0x0043c007..0x0043c12c`), the 18-slot pass in byte
  order, the `0x0067e7d8` self-store `0x0043c1c6`, every counter byte, the three bytes above each counter, every
  state dword, and EAX. F `0x0042ebe0..0x0042ed64`.
- `m_ctl`: the row's old defect (A/B 4-byte stride 16 reads, C 4-byte stride 8 reads, F row 0 only). T has no
  `m_ctl`: under either T control the counter of every state-1 slot changes, so `disc` counts calls with a state-1
  slot.

`mmOrig` / `mmCtl` count calls whose real output differs from the model. `disc` counts calls where the two models
differ, i.e. calls on which the scenario can tell the fix from the defect.

Consumer channel, per press window: each row's count of ret == 1, and the menu state at the end of the window
(depth, type, cursor `0x0067ed40 + depth*0x40`, `0x0067f17c`, `0x0067f184`, `0x0067ee80`). Original noise
(off1 vs off2): the ret == 1 count inside a 1.5 s hold differs by one repeat (A `t6_hold11` 6 vs 7, `t7_hold11`
7 vs 6), frame-timing dependent. That count is excluded for hold windows. The end-of-window state is kept there.
Everything else is identical between the two original runs.

## 5. Arms and controls

| arm | setup |
|---|---|
| off1, off2 | shim loads the deployed `.asi`, `MASHED_HOOK_ONLY=__none__` |
| on1, on2 | shim loads the deployed `.asi`, `MASHED_HOOK_ONLY` = the five RVAs |
| stock1 | `MASHED_RE_NO_AUTO_HOOK=1`, nothing loaded |
| late1 | `NO_AUTO_HOOK=1`, then `Module.load(original/mashed_re_dev.asi)` + `InjectHooks` at the title, five RVAs |
| ctl{A,B,C,T}{1,2} | as late, control `.asi` #1, `MASHED_HOOK_ONLY` = that one RVA |
| ctlTd{1,2} | as late, control `.asi` #2, `MASHED_HOOK_ONLY=0x0043c000` |
| offF1, onF1, ctlF{1,2} | off / on / late + control #1 (F only), with `--force-f` |

Control `.asi` #1 (SHA-256 `564953D55786DC4729C654FACBF0BBEBC06607E8949133B815A57B63B84FEE5E`), built with
`MASHED_NO_DEPLOY=1` from `225236b5` plus `evidence/c4_m/ctl1.diff`:
- A/B: `kPlayerSlotRecStride = 4` (both walks, 16 reads, index 0..15), the old port (`249aa785^`, fix_menuinput witness 2).
- C: both ScreenGate walks 4-byte stride over 8 iterations, the old port (`249aa785^`, fix_menuinput witness 2).
- T: state 1 calls `0x004a2c48` with nothing loaded. This is fix_hud defect 2, see below for why not defect 1.
- F: rows 1..9 dropped, row 0 only (fix_fstate witness).

Control `.asi` #2 (SHA-256 `C642C5E995627EE487F45F25CC54BA1D3FBA623F0CC40156539B81B50B4720B8`, `ctl2.diff`): T
stores the state-1 counter as a dword (fix_hud's witness, defect 1). Statically, the deployed body has
`mov byte ptr [edi + 4], bl` where #2 has `mov dword ptr [edi + 4], ebx` (`evidence/c4_m/asi_static_diff.txt`).
#2 also carries #1's A/B/C/F edits, but only `0x0043c000` is installed on its arms.

Sources were restored with `git checkout` and touched after both builds. Neither control was deployed.

F forced input (`--force-f`, F arms only): at Single Player, after the Load modal, the harness writes 1 to
`0x007f0c24` / `0x007f0c54` / `0x007f0c84` (the enable cells `[0x7f0a44 + p*0x30]` for p = 10, 11, 12, read at
`0x0042ecf3`) and to `0x007f0a48 + r*0x30`, r = 0..9 (the p = 11 column). This is save-progress data, the
input the body reads. It is not the path to the body: FUN_00439210 calls 0x0042ebe0 with p = 10/11/12 on track
select either way, the same 4 call sites, the same counts. Why it is needed: on the stock save the three enable
cells are 0, so the body returns 0 at `0x0042ecfe` before the row check matters. With them set, p = 10 (column
`0x7f0a44`: row 0 = 1, rows 1..9 = 0) returns 0 in the original and 1 under the row-0-only defect, and p = 11
(full column) returns 1, so the body's return-1 path runs too.

## 6. Results

Per-run table: `evidence/c4_m/c4_m_compare.csv` (one row per run and function). Per-run JSON with every press
window, the pre-attach witness and up to 12 mismatch examples per row: `evidence/c4_m/runs/`.

Original vs itself: off1, off2, stock1 and offF1 all read mmOrig = 0 on every row, so the byte models agree with the
original's own outputs on every live call (A/B about 2500 calls per run, C about 2500, T about 3400, F about 16000
natural and 17410 forced). off2 vs off1 consumer: 9/9 windows on every row, with the hold-window repeat count
excluded (section 4).

Hook live: on1, on2, late1, onF1. Each row read `0xE9` at its RVA before Frida attached anything, and the rel32
target resolved into `mashed_re_dev.asi` at that row's export (A `+0x12430`, B `+0x12450`, C `+0x12e20`,
T `+0x18ed0`, F `+0x15880`, matching the deployed export table). Every call was counted at that target, so each
one is the game's own `CALL` going through the inline JMP. On the late arms the RVA read the stock first byte before
the load and `0xE9` after `InjectHooks`.

### 6.1 MenuButtonDetectA 0x0042aff0

- Calls through the JMP: 2543 / 2539 (on1/on2), 2508 (late1), 2505 (onF1), on screen types 1, 2, 6, 7, 24.
  ret == 1: 14..17 per run, from the hold-repeat branch on types 6 and 7 (`0x0042b0e5`) and the phase-3 edge.
- Behaviour: mmOrig 0 on all four hook-live runs. Consumer 9/9 windows vs off1 (on arms) and stock1 (late).
- Control (4-byte stride, ctlA1/ctlA2, A alone): mmOrig 37 and 37, mmCtl 0 and 0. Every departure from the original
  bytes is exactly the old-port model's output. Consumer 6/9: in `t6_taps`, `t24_taps` and `t7_taps` the player-1 up
  press returns 1 (ret1 2 vs 1). On game setup it leaves the cursor at 7 where the original has 0.
- Survival: every run alive to the end, 0 AV.

### 6.2 MenuButtonDetectB 0x0042b180

- Calls: 2543 / 2539 / 2508 / 2505, same screens. ret == 1: 15..17.
- Behaviour: mmOrig 0 on all four hook-live runs. Consumer 9/9.
- Control (ctlB1/ctlB2, B alone): mmOrig 36 and 37, mmCtl 0 and 0. Consumer 6/9 in the same three windows,
  game-setup cursor 1 vs 0.
- Survival: all alive, 0 AV.

### 6.3 MenuButtonDetectC 0x0042b310

- Calls: 2544 / 2541 / 2525 / 2502 on types 4, 6, 7, 24. ret == 1: 1 per run (the player-0 control-9 edge on colour
  select).
- Behaviour: mmOrig 0 on all four. Consumer 9/9.
- Control (4-byte stride over 8 reads, ctlC1/ctlC2, C alone): mmOrig 3 and 3, mmCtl 0 and 0. These are the three
  player-1 control-9 presses (track select, game setup, type 7). The original returns 0 there and the control
  returns 1. Consumer 6/9 (ret1 1 vs 0 in those three windows).
- Coverage, stated plainly: 3 discriminating calls per run. Screen types 0x13 and 0x1e were never reached in any run
  or probe, so the branch at `0x0042b3dd..0x0042b405` did not run live. That branch holds the row's principal old
  defect (sub 0/1 on type 0x13). It is covered only by the C3 fixture (`fix_diff_menu_button_detect_c.csv`,
  witness 1 RED 4/14).
- Survival: all alive, 0 AV.

### 6.4 TimerSlotTickDispatcher 0x0043c000

- Calls: 3385 / 3385 / 3343 / 3286 (every frontend tick). 2666..2764 calls per run had at least one slot in state 1
  (slots seen in state 1: `0x0067e7a8`, `0x0067e7b8`, `0x0067e7c8`, `0x0067e7f0`). State 2 appears on the same slots,
  so both ftol paths (`0x0043c137`, `0x0043c15f`) and the reset at x = 0.0 run.
- Compared channel: written memory. After every call the harness compares all 19 state dwords, all 18 counter bytes,
  the three bytes above each counter and EAX against the byte model. EAX is compared even though the caller
  ignores it (`0x0043e062..0x0043e069` load EDX and call `0x0040acd0` without reading EAX). mmOrig 0 on all four hook-live runs. There is no
  window-level consumer to compare: the counters only drive fades, and the T controls read 9/9 on windows too.
- Control used, ctlT1/ctlT2 (control #1, T alone): state 1 calls `0x004a2c48` with nothing loaded (fix_hud defect 2).
  mmOrig 2698 and 2656, equal to `disc` in each run: every call with a state-1 slot reads wrong. Example: x = 0.96,
  counter `0x0067e7ac` = 0 where the bytes give `trunc((1 - 0.96) * 255)` = 10. No crash over the run.
- Fix-doc witness control, ctlTd1/ctlTd2 (control #2, dword counter store, T alone): GREEN, mmOrig 0 on 2613 and 2724
  state-1 calls. Why: that defect changes only the three bytes above each counter (`counter + 1..3`). Those bytes read
  0 at entry on every call of every run, on both sides (`padNZ = 0`). The dword store writes a value 0..255, so it
  writes zeros over zeros. The defect cannot change game memory while those bytes are 0, and they were 0 in all 20
  runs. So that witness cannot be the RED control here. The parent decides whether defect 2 meets the "old defect
  restored alone" bar: it is also an old defect of this row, restored alone, and statically visible in control #1 as
  `mov ecx, 0x4a2c48` ahead of the state compare. If it does not meet the bar, the verdict is NOT EARNED, missing
  piece "the witness defect is not observable live".
- Survival: all alive, 0 AV, including the unloaded-ST0 control.

### 6.5 FrontendPlayerSlotCheck 0x0042ebe0

- Calls: 16029 / 16011 / 15903 natural, 17490 forced (onF1), p = 10, 11, 12 from FUN_00439210 on types 6, 7, 24.
- Natural (stock save): every call returns 0 on every arm, because the enable cells `[0x7f0a44 + p*0x30]` are 0
  (`0x0042ecfe`). mmOrig 0, but there are no discriminating calls: the natural scenario cannot tell the row-0-only
  defect apart. Hence the forced input (section 5).
- Forced (`--force-f`): offF1 returns p10 -> 0 (5223 calls), p11 -> 1 (6964), p12 -> 0 (5223). onF1, hook live,
  returns p10 -> 0 (5247), p11 -> 1 (6996), p12 -> 0 (5247). mmOrig 0, so the return-1 path at `0x0042ed02` ran
  live and matched. Per-window ret == 1 counts are not compared for F: three calls per frame make the count track
  frames on screen (onF1 1740 vs offF1 1760 in `t6_taps`). The per-p split is the value FUN_00439210 consumes per
  track, and it is identical.
- Control (rows 1..9 dropped, ctlF1/ctlF2, F alone, forced): p10 -> 1 on every call (6372 and 6460), mmOrig 6372 and
  6460, mmCtl 0 and 0. RED.
- Only one original run (offF1) was taken in the forced setup. The natural setup has two (off1, off2) plus stock1, all
  mmOrig 0.
- Survival: all alive, 0 AV.

### 6.6 GameStateSlotsFill 0x00429aa0

Probes only. See section 2.

## 7. C4 checklist per row

| | A | B | C | T | F |
|---|---|---|---|---|---|
| game calls the body in a natural scenario | yes | yes | yes | yes | yes (forced input for the control leg only) |
| 0xE9 pre-attach, target = export, call count > 0 | yes | yes | yes | yes | yes |
| behavioural diff on output or consumer, original noise measured | yes, 0 mismatches over 4 runs | yes | yes | yes (written memory) | yes |
| control RED on the same scenario | yes 2/2 | yes 2/2 | yes 2/2 (3 calls each) | defect 2: yes 2/2. Witness: GREEN 2/2, not observable | yes 2/2 |
| survival | 20/20 | 20/20 | 20/20 | 20/20 | 20/20 |

## Files

- harness: `re/frida/c4_m_scenario.py`, `re/frida/c4_m_compare.py`, probe `re/frida/c4_m_probe.py`
- run matrix: `evidence/c4_m/run_matrix.sh`
- runs: `evidence/c4_m/runs/c4_m_<tag>.json` (20), table `evidence/c4_m/c4_m_compare.csv`
- probes: `evidence/c4_m/probes/` (12 reachability runs, plus the first off-arm harness test)
- controls: `evidence/c4_m/ctl1.diff`, `evidence/c4_m/ctl2.diff`, `evidence/c4_m/asi_static_diff.txt`. The control
  `.asi` files are not committed (SHA-256 above).

