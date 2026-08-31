# Original-side Arctic-race nav: findings (2026-08-30, branch race/nav-champ)

Goal (T-ARCTIC): reach a NON-TRAINING (Arctic) in-race frame on the stock
`original/MASHED.exe` via Frida, to produce a pose-matched Arctic reference that
gates shipping `race/geomlight`.

**Verdict: ARCTIC IS NOT REACHABLE from the reference save. It is unlock-gated.**
The whole capture is blocked by championship progression that this repo must not
fabricate (`original/gamesave.bin` is the diffing reference; unlocking it corrupts
every comparison). Evidence below.

## Screen sequence (live depth = DAT_0067e9f8, phase = DAT_0067eca4)

Driven by `re/frida/nav_champ_probe.py` (nav_agent.js return-override of
FUN_00497310). Depth numbers are the live values read during the walk.

| screen | depth | evidence PNG | notes |
|---|---|---|---|
| Single Player menu | 3 | `d1_title.png`, `d2.png` | items: **Challenge Cup**, **Quick Battle**, **Time Trial** (Time Trial is GREYED / LOCKED) |
| Player Colour Select | 4 | `d3_modeselect.png`, `d3_cursor0.png` | colour/player picker (icons row + "1") |
| Challenge Select | 5 | `d4_playercolour.png`, `d5_challengeselect.png`, `chall_step0..5.png` | shows **"Angel Peak"** (gold, starred) + **3 padlock** icons; "Bronze Cup / Battle Game / battle against 1 opponent" |
| (in race) | — | — | confirming Angel Peak loads **TRAINING.PIZ** |

NOTE: the shot filenames lag the visible screen by one press (the shot fired
after the next confirm); the `evidence PNG` column lists what each file ACTUALLY
shows, not its name.

## Global selection state (RVA-cited)

- **game_mode = DAT_0067e9fc** (0x0067e9fc). Written on confirm, not on cursor
  move. Measured: Challenge Cup cursor 0 -> **3**, Quick Battle cursor 1 -> **10**,
  the Top Dog/Team branch -> **6**. (Matches FUN_0043dfd0 ApplyActionGameMode:
  0xff3d0000->3, 0xff4d0000->10, 0xff2c/2e0000->6.)
- **Mode-select cursor = DAT_0067ed80 + (depth-1)*0x40**; at depth 3 that is
  **0x0067ee00**. Writing it via `setsel` changes the launched game_mode (verified
  by the cursor->game_mode sweep), so it IS the Single-Player-menu cursor.
- **Challenge Select cursor is NOT 0x0067ee80.** Writing 0x0067ee80 (the per-depth
  cursor for depth 5) or moving it (down -> 0xFFFFFFFF) does not change the
  on-screen challenge: `chall_step0.png`..`chall_step5.png` all still read
  "Angel Peak". Only animation floats (0x0067ea60 / 0x0067ed30) changed across
  down-presses. The real challenge index was NOT isolated because there is only
  ONE selectable entry (the rest are locked, so nothing to move to).
- **Control codes 11/12 (documented "up"/"down" in nav_agent.js) do NOT move any
  selection** — verified on both the mode-select (cursor stayed 0) and the
  Challenge Select. Only **confirm (4)** and **setsel (direct write)** work. There
  is currently no press-based directional-nav primitive for the frontend.

## The unlock gate (authoritative — DAT_007f0a40, dumped from the current save)

`0x007f0a40` is the 13-row x 12-dword championship table, loaded from
`original/gamesave.bin` at boot. Launch gate (FUN_0043dfd0 harvest, transcribed
exe_main.cpp:1963-1966): `(&DAT_007f0a40)[FUN_004309b0(mode) + track*0xc] == 0
-> blocked`. FUN_004309b0: **mode 3 -> col 1**, **mode 10 -> col 11**.

Dumped 2026-08-30 (`--plan dumptable`):

```
row 0: [0, 1, 0, 2, 2, 0, 0, 0, 0, 0, 0, 1]
row 1: [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0]
row 2: [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0]
row 3: [0, 0, 0, 2, 2, 0, 0, 0, 0, 0, 0, 0]
row 4..12: [0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0]
```

- **Column 1 (mode-3 gate)**: row0 = 1, every other row = 0.
- **Column 11 (mode-10 gate)**: row0 = 1, every other row = 0.

=> For both single-player race modes, **only track row 0 is launchable.** Row 0
races TRAINING. Every other cup entry (the 3 padlocks in `d5_challengeselect.png`)
is locked. This is exactly the on-screen state.

## Why Arctic specifically is out

- Arctic = kAreas[0], courseId 0 (GameFlow.cpp:30). Its cup place-name is
  "Timgidski" (msgid 0x4c), the 4th track of Challenge Cup 1 per the RE survey
  (cup_place_names_REmap_20260616.md; GameFlow.cpp:54-61). That makes Arctic cup
  row 3, whose col-1/col-11 gate = 0 -> LOCKED.
  [UNCERTAIN] the exact cup-row for Arctic: row 0 ("Angel Peak") races TRAINING,
  not a scenic "Angel Peak" area, so the naive place-name = area pairing does not
  hold on this save/build. A locked row cannot be launched, so the row->area map
  for rows 1-3 cannot be behaviourally confirmed. What is CERTAIN: only row 0 is
  unlocked, and no navigable path loads ARCTIC.PIZ.
- Time Trial (which might allow free track choice) is itself LOCKED/greyed on this
  save (`d1_title.png` / `d2.png`).

## What is missing to unlock Arctic (do NOT do this to original/)

`0x007f0a40` row-N col-1 (Challenge Cup) / col-11 (Quick Battle) must be nonzero
for Arctic's row. On the stock game that comes from winning the earlier Challenge
Cup races (progression persisted into gamesave.bin). The forbidden shortcut is
`patch_mashed_unlock_tracks` / `unlock_all` / a live table write — all of which
mod the diffing reference and are barred by CLAUDE.md. `original/gamesave.bin`
(151456 = 0x24FA0 bytes, dated 2025-09-17, sha bd18788182b23...) is the reference
and was left untouched.

## Path to actually capturing the Arctic reference later

Two non-corrupting options for a future session:
1. Play the Challenge Cup on a SEPARATE copy of the save (never `original/`) until
   Arctic's row unlocks, point MASHED at that copy, then run the capture.
2. Reach Arctic through a flow that does not consult the unlock gate, if one is
   found (none was: Quick Battle and Challenge Cup both gate to row 0; Time Trial
   is locked; Top Dog/Team (mode 6) stalls at depth 4 — needs a 2nd player).
