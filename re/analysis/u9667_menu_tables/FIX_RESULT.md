# U-9667 fix: one 0x0042ac90 body, cursor -1 ported

Branch `work/u9667-fix` (fix commit a05b9c14). No tracker edits and no re-classify. The parent closes U-9667.

**Verdict.** Both builds now call one table walk for 0x0042ac90. On screens 6 and 7 the standalone's slot cursor now
goes to -1 the way the original's does, and its confirm at -1 takes the same path as the original. The split the dump
saw ("screen 24 in 1 of 3 runs, phase 3 -> 0 in 2 of 3") is **not timing**. It is the game mode, which the route's
first pick sets. Quick Race (mode 10) pushes screen 0x18. Challenge Cup (mode 3) launches the race.

| check | result |
|---|---|
| `run_diff menu_entry_get` (shared body in the .asi) | **GREEN 16/16**, 0 mismatches (`evidence/diff_menu_entry_get.csv`) |
| `run_diff menu_entry_get --self-baseline` | **GREEN 16/16** (`evidence/diff_menu_entry_get_selfbaseline.csv`) |
| `scripts/lint_rva_bodies.py` | OK, 0 NEW (115 known, all allowlisted). 0x0042ac90 has one anchored body |
| nav_coverage (static), HEAD sources vs branch | identical output |
| standalone vs original, screen 6/7 slot cursor | match: 0 at entry, -1 after DOWN, stays -1 after UP |
| standalone vs original, confirm at -1 | match: mode 3 launches, mode 10 pushes screen 24 |
| standalone vs original, challenge row | differs, by the 2026-09-06 user directive (below) |

## 1. What changed, with RVAs

### Shared body (`Frontend/MenuNav.cpp`, `Frontend/MenuNav.h`)

- `mashed_re::Frontend::MenuEntryWalk(const uint32_t* table, int32_t cursor)` is the table walk of 0x0042ac90,
  transcribed from the bytes. The disasm is in the source comment: `0x0042aca5..0x0042ad0a`.
- The .asi export `MenuEntryGet` (anchored `// 0x0042ac90`, `RH_ScopedInstall` unchanged) reads the three globals
  and calls it. The globals are the slot index `0x0067e9f8`, the table `0x0067ed38+slot*0x40` read at `0x0042ac99`,
  and the cursor `0x0067ed40+slot*0x40` read at `0x0042ac9f`.
- The standalone's `Nav_Select` calls the same function through `Nav_SelectAction()`, the analogue of the call at
  `0x0043e4ca`.
- `ItemActionCode` (old `MenuNavSM.cpp:432`) is deleted, along with `NavTest_ItemActionCode`, which became
  `NavTest_MenuEntryWalk`.
- The one deliberate textual change from the old .asi body is the pass counter. It now follows the bytes'
  `dec ecx / jne` at `0x0042acce` (stop at 0). The old body stopped at `> 0`. The two agree for every cursor >= -1.
  At cursor <= -2 the original walks off the table either way. No vector uses such a cursor.

### Layout: one body, both builds

| TU | asi_sources.rsp | exe_sources.rsp | holds |
|---|---|---|---|
| `Frontend/MenuNav.cpp` | yes (:56) | yes (:106) | `MenuEntryWalk` (the body) and `MenuEntryGet` (export, install) |
| `Frontend/MenuNavSM.cpp` | no | yes (:107) | callers only: `Nav_SelectAction`, `Nav_Select` |

The walk carries no RVA-first anchor ("Table walk of 0x0042ac90 ..."), so `lint_rva_bodies.py` sees exactly one body
at the RVA: the export.

### Cursor -1 port (`Frontend/MenuNavSM.cpp`, `.h`)

1. **Item count (`nav_count`, the analogue of `DAT_0067edb4` / slot +0x3c).** FUN_0043d2a0's build loop increments
   the count only when the item's string id, read through FUN_0042ad90 into a 16-bit local, is not -1 (decomp of
   0x0043d2a0: `if (sVar2 == -1) ... else local_4 = local_4 + 1`, stored to `*(0x0067edb4 + local_1c)`).
   - `NavCountItems` reproduces this. It matches the live +0x3c word on **33/33** pushed screens (`dump_push.json`).
   - The count is 0 on screens 4, 5, 6, 7, 12, 14, 15, 16, 20, 22, 23 and 25, whose single item has id 0xffffffff.
   - `item_count` (all items, used for records and layout) is unchanged.
2. **FUN_0042aa00 (0x0042aa00) ported verbatim** as `CursorStep`. It steps the cursor over the count and wraps. It
   returns on the first `avail == 1`. It sets the cursor to -1 when the count is 0 or when no item is available after
   count+1 steps.
3. **UP/DOWN gates.** FUN_0043dfd0 runs its UP arm only when the table's flag block (0xff090000..0xff0a0000) holds
   0xff0b0000, and its DOWN arm only when it holds 0xff0c0000 (harvest L201-L242 flag scan, L1345-L1355 arms).
   `Nav_MoveCursor` now checks the same flag.
   - Screens 4, 5, 12, 14, 15, 16, 22 and 25 have count 0 and no flag, so their cursor stays 0, as in the dump
     (`L1-d4/down1 cursor=0`).
   - Screens 6, 7, 20 and 23 carry both flags with count 0, so their cursor goes to -1.
4. **Challenge row = `DAT_0067f17c`, separated from the slot cursor.** `g_challenge_row` / `Nav_ChallengeRow()`.
   - Ghidra xrefs show its only writers: FUN_004322c0 (`0x00432304`, `0x00432311`, `0x004323a9`), FUN_004323c0
     (`0x004323d7`, `0x004323e1`, `0x0043243d`), and three resets to 0 in FUN_0043dfd0 (`0x0043f1c0`, `0x0043f4b7`,
     `0x0043fadf`) on the mode items 0xff2c/0xff2e, 0xff3d/0xff40 and 0xff4d.
   - The port resets the row on those five actions (`ApplyActionGameMode`). Nothing else writes it, so it persists
     across pushes, pops and the continue-cup push of 7.
   - exe_main's Challenge Select render and launch read the row (`Nav_ChallengeRow()`), not `Nav_Cursor()`.
5. **No cursor gate before the walk.** `Nav_Select` no longer exits for cursor < 0 or for `avail[cursor] != 1`. The
   original reads no slot cursor between FUN_0043dfd0's entry and the call at `0x0043e4ca`: the harvest L55-L248 has
   no `0x0067ed40`/`0x0067ed80` read. This closes the [UNCERTAIN] in RESULT.md about a check before `0x0043e3a0`.

### What FUN_0043dfd0 does after the call at cursor -1

On kT6/kT7, `MenuEntryWalk(table, -1)` returns `0xff260000`, the same code as at cursor 0 (orig(-1) == orig(0)). The
original then runs the 0xff260000 arm, which reads no slot cursor. The arm is in harvest L402-L813.

1. `FUN_004309b0()` maps the mode to an unlock column. If `(&DAT_007f0a40)[col + DAT_0067f17c*0xc] == 0`, it sets
   `_DAT_0067e840 = 0.0` and does nothing else (L406-409).
2. `switch (DAT_0067e9fc)`. Case 10: `FUN_0040e170(row)` then `FUN_0043d2a0(0x18,0)`, which **pushes screen 24**
   (L788-792).
3. `FUN_00430760()` returns 1 for modes 2, 3, 4, 5 and 10.
   - If it returns 0 (modes 6..9 and others): `FUN_0043d2a0(0x12,0)`, which **pushes screen 18** (L794-797).
   - If it returns non-zero and the mode is not 10: `LAB_0043f371`, which runs `FUN_0043d2a0(0,2)`, sets
     `DAT_0067e914 = 1` and **launches the race** (L812-813, L907-911). `DAT_0067e914` drives phase 3 -> 4 (L197) on
     the way out of the frontend.

Ported as `Nav_ChallengeArm(mode)`. exe_main's screen-6/7 confirm keeps its existing unlock gates (the tier gate and
the locked-row gate stand in for step 1). It then calls the arm. If the arm pushes a screen it returns. Otherwise it
launches. A forced `MASHED_GAME_MODE` keeps the direct launch, as before.

The arm hands off to screens 24 and 18, whose item 0 is `0xff420000`. Without that action the standalone would dead-end
there, so the 0xff420000 launch is wired too (harvest L954-1035). Mode 10 counts the opponent fields
`DAT_0067ea98/ea9c/eaa0`. When all three are 0 it opens modal 0x24f and does not launch. Every mode then runs
`FUN_0043d2a0(0,2)` and sets `DAT_0067e914 = 1`. The launch code moved unchanged out of the 6/7 branch into
`MenuLaunchRace(launchMode)`, so both entry points share it.

**Not ported (stated, not guessed):**
- Modal 0x24f. A zero opponent count refuses the launch silently.
- The per-mode writes in L418-L792 (`DAT_0067ea64/ecdc/ea74/ea70/ea78`, `DAT_007f0fd0`, the `DAT_007f1a14..` slot
  table) and the 0xff420000 arm's AI slot assignment. `MenuLaunchRace` builds its own `RaceConfig` as it did before.
- The hold-repeat of FUN_0042aff0/FUN_0042b180. The standalone stays edge-only.
- The `DAT_00898ab0 == 0` condition on the row move.

## 2. Traces

### Original (`fix_orig_r00m1.json`, `fix_orig_r10m10.json`)

These come from `u9667_menu_table_dump.py --lane input`, which now also records `mode` = `[0x0067e9fc]` and `row` =
`[0x0067f17c]`.

| step | route 0,0,m1 (Challenge Cup) | route 1,0,m1,0 (Quick Race) |
|---|---|---|
| screen 2 confirm | cursor 0, ret 0xff3d0000, mode -> 3 | cursor 1, ret 0xff4d0000, mode -> 10 |
| screen 4 confirm | ret 0xff1d0000, pushes **screen 6** (0x5f6da0), depth 5 | same, screen 6, depth 5 |
| screen 6 entry | cursor 0, row 0 | cursor 0, row 0 |
| DOWN, DOWN, UP | cursor -1, -1, -1; row 0, 0, 0 | cursor -1, -1, -1; row 0, 0, 0 |
| confirm at -1 | 0x0042ac90(cursor -1) = 0xff260000; depth 5 -> 5, phase 3 -> 5 (launch) | 0xff260000; depth 5 -> 6, **screen 24** (0x5f7370), phase 3 |
| screen 24 confirm | n/a | cursor 0, ret 0xff420000; phase 3 -> 0 (launch) |

The row stays 0 on DOWN because FUN_004322c0 increments it and then reverts it when FUN_00430830 reports the next row
unavailable on a fresh save.

### Standalone after the fix (`fix_sa_after_*.txt`)

The standalone was driven with the new `MASHED_NAV_KEYS` knob, 30 frames between taps. Each line shows the state
before that frame's input is consumed. `flow=1` is GameFlow LoadingRace, the standalone's launch.

| scenario | trace (screen: cursor/row/mode) | vs original |
|---|---|---|
| `MASHED_GOTO=2,4,6`, keys `d,d,u,e` | s6: 0/0/0, d: **-1**/1, d: -1/2, u: -1/1, e: flow=1 (launch, mode 0 -> 3) | cursor matches. Row differs (directive). Launch matches mode 3 |
| `GOTO=2`, keys `e,e,d,u,e` | s2 e: mode 3 -> s4. e: **s7**. d: -1/1. u: -1/0. e: flow=1 (launch) | cursor and outcome match. Lands on 7, not 6 (see 3.2) |
| `GOTO=2`, keys `d,e,e,d,u,e,e` | s2 d,e: mode 10 -> s4. e: s7. d: -1/1. u: -1/0. e: **s24 depth+1**. e: flow=1 (launch) | cursor and outcome match (push 24, then launch on 0xff420000) |

### Standalone before the fix (`fix_sa_before_*.txt`, HEAD~1 sources plus the knob only, row logged as -2)

| scenario | before (screen: cursor) | after | original |
|---|---|---|---|
| `GOTO=2,4,6`, `d,d,u,e` | s6: 0, d: **1**, d: **2**, u: 1, e: launch | 0, **-1**, -1, -1, launch | 0, -1, -1, -1, launch (mode 3) |
| `GOTO=2`, `e,e,d,u,e` (mode 3) | s7: 0, d: **1**, u: 0, e: launch | s7: 0, **-1**, -1, launch | s6: 0, -1, -1, launch |
| `GOTO=2`, `d,e,e,d,u,e,e` (mode 10) | s7: 0, d: 1, u: 0, e: **launch from s7** (flow 1, then 2) | s7: 0, -1, -1, e: **push s24**, e: launch | s6: 0, -1, -1, e: push s24, e: launch |

Before the fix, the screen-6/7 cursor was the cup-row index (MenuNavSM.cpp:1073-1080 at HEAD~1), so it never reached
-1. The confirm went straight to a race launch for every mode. Mode 10 skipped screen 24 entirely. After the fix,
the slot cursor and the mode-dependent outcome match the original in all three scenarios. The remaining differences
are listed in section 3.

## 3. What still differs, and why

1. **Challenge row movement.** The original's FUN_004322c0/FUN_004323c0 clamp the row to 0..0xc and skip
   unavailable rows, so on a fresh save it stays 0. The standalone wraps over `cup.trackCount` and rests on locked
   rows, gating only the launch. This is the [Bug #2 / 2026-09-06] user directive. U-9667 kept it and moved it onto
   the row variable.
2. **Screen 4's 0xff1d0000 pushes 7 in the standalone and 6 in the original.**
   - Harvest L343-387: when `FUN_00430760() != 0` (modes 2, 3, 4, 5, 10) the original pushes 6. Otherwise it goes to
     the team branch (0xf/0x10 or a modal). There is no push of 7 in that arm.
   - Both live routes pushed 6 (table 0x5f6da0).
   - `ActionToScreen` (MenuNavSM.cpp, `case 0xff1d0000u`) returns 7 for ecdc == 0 and ed6c == 0, under a 2026-06-15
     "(user)" correction.
   - This is outside U-9667 and was not changed. It needs a decision, because the correction note names the user.
3. **Screens 20 and 23** carry the 0xff0b/0xff0c flags with count 0, so their cursor now goes to -1 on UP/DOWN as the
   bytes say. Confirm at -1 there returns item 0's 0xff4c0000, the same as at 0. No original-side capture was made for
   these two screens. The claim rests on the FUN_0042aa00 bytes and the count rule (33/33).
4. **Draw parity.** The entry state of screens 6 and 7 is unchanged (cursor 0, row 0), so entry-frame draw lists are
   unaffected. After a DOWN press the slot cursor is -1 and no item record is highlighted. No draw-list capture of a
   post-DOWN frame exists on either side: `menu_draw_burst.py` pushes screens and does not press keys. So the
   post-DOWN draw was not diffed.

## 4. Side findings and tooling

- `navsm_test` (offline nav harness) did not link at HEAD (`Campaign_CurrentCup` unresolved). It now links with two
  stubs and passes Pieces 1-3. It then faults at R2-2, where `Nav_GameStateLoadSave` writes the raw address
  0x007f0a40 (added in 2528c8b3), which is not mapped in a plain test exe. That fault predates this branch. The BFS
  section, which now calls `MenuEntryWalk` only on rows that exist, sits after R2-2 and did not run.
- `MASHED_NAV_KEYS` / `MASHED_NAV_KEYS_GAP` (exe_main.cpp) is a scripted key list with a per-frame `NAV_KEYS` log line
  to `./mashed_re.log`. Like `MASHED_NAV_DEMO`, it bypasses the focus gate.

## [UNCERTAIN]

- None on 0x0042ac90 or the cursor -1 path.
- The 0xff260000 arm's non-nav side effects (L418-L792) and the 0xff420000 AI slot assignment are not ported. Section
  1 lists them as not ported. They are not open questions about the bytes.

## Reproduce

```
py -3.12 re/frida/run_diff.py menu_entry_get [--self-baseline]
py -3.12 re/frida/u9667_menu_table_dump.py --lane input --route 0,0,m1   --out <json>
py -3.12 re/frida/u9667_menu_table_dump.py --lane input --route 1,0,m1,0 --out <json>
# standalone, from the worktree root:
MASHED_GOTO=2 MASHED_NAV_KEYS=d,e,e,d,u,e,e mashedmod\build\mashed_re.exe   -> NAV_KEYS lines in .\mashed_re.log
```
