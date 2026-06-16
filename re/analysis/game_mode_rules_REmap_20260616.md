# Game-mode → race-rule pipeline — RE map (WS-G1, 2026-06-16)

**Goal:** RE the `DAT_0067e9fc` game-mode → race-rule table and replace the
standalone's `MASHED_RACE_MODE`/`MASHED_LAPS` env scaffold
(`exe_main.cpp:1438`) with the real per-mode rule derivation.

Binary anchor: `MASHED.exe` size 2,846,720, SHA-256 (unpatched)
`BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E`. All
addresses below verified read-only via Ghidra MCP on `Mashed_pool7`
(2026-06-16). NO-GUESSING: every constant cites its address; unproven
semantics are marked `[UNCERTAIN]`.

## The two mode globals

There are TWO distinct "mode" globals, set in sequence:

| Global | Meaning | Set by | Read by |
|---|---|---|---|
| `DAT_0067f184` | menu selection index (0..0xb) | the mode-select screen | `FUN_0042f6b0` |
| `DAT_0067e9fc` | **game-mode id** (2..10) | `FUN_0042f6b0` | many (getter `FUN_0042f6a0`) |
| `DAT_007f0fd0` | **race-rule sub-mode** (0..10) | `FUN_0043dfd0` (race-launch) | win-condition `FUN_00410510`, elimination `FUN_00410d10` |

The pipeline is: `selection (0067f184) → game-mode (0067e9fc) → race-rule
(007f0fd0)`.

## Step 1 — selection → game-mode  (`FUN_0042f6b0`, 0x0042f6b0)

Verbatim switch on `DAT_0067f184` (switch base 0x0042f6b8); writes `DAT_0067e9fc`:

```
sel 0  -> mode 2     sel 1  -> mode 3     sel 2  -> mode 4
sel 3  -> mode 6     sel 5  -> mode 5     sel 8  -> mode 7
sel 9  -> mode 8     sel 10 -> mode 9     sel 11 -> mode 10
(sel 4,6,7 absent -> no write)
```

Valid game-mode ids: **{2,3,4,5,6,7,8,9,10}**. Already ported (C3) as
`MenuModeSync` in `Frontend/FrontendState.cpp` (dev `.asi` hook). Getter
`FUN_0042f6a0` (`return DAT_0067e9fc`) ported (C3) as `GetRaceSubMode`.

Mode-family split — `FUN_00430760` (0x00430760, `IsMultiplayerMode`, C3):
returns 1 iff `DAT_0067e9fc ∈ {2,3,4,5,10}` (SP/campaign family) else 0
({6,7,8,9} = MP/party family).

## Step 2 — game-mode → race-rule  (`FUN_0043dfd0`, race-launch action 0xff240000)

`FUN_0043dfd0` (0x0043dfd0, ~10969 B, C2 — too large to port whole) is the
menu action dispatcher. In the race-launch action (`uVar10 == 0xff240000`):

1. `DAT_007f0fd0 = 0;` (0x0043f7a7 region) — default rule before the switch.
2. `switch(DAT_0067e9fc)`:

```
case 2:           rule stays 0   (clears slots; FUN_00429aa0/004298c0/0046dc00)
case 3,4,5:       rule = RuleFromEvent(EventType(mode,track))   [see step 3]
case 6,7,8,9:     rule stays 0   (FUN_00430b60 gate; no rule write)
case 10:          rule stays 0   (FUN_0040e170; push screen 0x18)
```

For modes 3/4/5 the event-type comes from a per-(mode,track) table; the
event-type high-word selects the rule (write sites in `FUN_0043dfd0`):

```
event(uVar10 = entry & 0xffff0000)   ->  DAT_007f0fd0
  0x70000 (ev 7)                     ->  5     (also DAT_0067ea74=1)
  0x40000 (ev 4)                     ->  10
  0x80000 (ev 8)                     ->  7
  0x50000 (ev 5)                     ->  9
  0x20000/0x30000/0xb0000 (2/3/b)    ->  4     (0x60000 overrides to 8)
  0x60000 (ev 6)                     ->  8
  0x00000/0x10000/0xc0000 (0/1/c)    ->  0     (DAT_0067ea64=1)
  0x90000/0xa0000 (9/a)              ->  0
```

So `RuleFromEvent`: `{0,1,9,a,c}→0`, `{2,3,b}→4`, `6→8`, `7→5`, `8→7`,
`4→10`, `5→9`.

There is also a separate MP/quick-race action (`uVar10 == 0xff420000`,
0x0043e887 region) that, for `mode != 10`, derives the rule from the
game-length setting `DAT_0067ea88`: `0→rule 0`, `1→rule 1`, `2→rule 2`
(`DAT_0067ea64=1`). [Not on the standalone's current race-launch path, which
uses the championship action — recorded for when MP/quick-race is wired,
WS-G2.]

## Step 3 — the event-type table  (`DAT_005f65c8`, .rdata)

Read at 0x0043f44b: `uVar10 = *(uint*)(&DAT_005f65c8 + iVar8) & 0xffff0000`
where `iVar8 = (modeOff + track*3)*4`, `modeOff = 0/1/2` for mode 3/4/5. So
the table is 4-byte entries, indexed `modeOff + track*3` (track-major, 3
modes per track); the **high 16 bits** of each entry are the event-type code.
(The parallel table `DAT_0067f0e0` read into `DAT_0067ea78` is runtime-zeroed
.bss, not static.)

Raw bytes read from `0x005f65c8` (Ghidra MCP, 2026-06-16); the cup region is
the first **39 entries** (13 tracks × 3 modes); entries 39+ have high-word 0.
Decoded event-type per (track, mode), with resulting rule and objective:

```
track  mode3       mode4       mode5
 0     ev9->r0/E   ev3->r4/E   ev1->r0/E
 1     evA->r0/E   ev1->r0/E   evB->r4/E
 2     evA->r0/E   ev4->r10/E  ev5->r9/E
 3     ev3->r4/E   ev1->r0/E   ev7->r5/RACE
 4     ev1->r0/E   ev2->r4/E   ev5->r9/E
 5     ev1->r0/E   evC->r0/E   ev4->r10/E
 6     ev1->r0/E   ev6->r8/E   evC->r0/E
 7     evA->r0/E   ev3->r4/E   ev7->r5/RACE
 8     ev3->r4/E   ev1->r0/E   evC->r0/E
 9     ev1->r0/E   evC->r0/E   ev5->r9/E
10     ev3->r4/E   ev5->r9/E   ev1->r0/E
11     ev1->r0/E   evC->r0/E   ev6->r8/E
12     ev1->r0/E   ev4->r10/E  ev7->r5/RACE
```

(E = elimination active; RACE = non-elimination race — see step 4.) Distinct
cup event-types present: {1,2,3,4,5,6,7,9,a,b,c}. The only non-elimination
cup events are the three event-type-7 cells (track 3/7/12, mode 5).

## Step 4 — race-rule semantics  (`FUN_00410510`, `FUN_00410d10`)

Two independent functions switch on `DAT_007f0fd0`:

- **`FUN_00410510`** (0x00410510, `Race::EvaluateResult`, win/finish checker):
  `FUN_0042f6a0()==2` → returns 0 (mode 2 handled elsewhere). A pre-switch loop
  detects "a car reached finish state 8" via `FUN_0040b6d0(i)` (=
  `(&DAT_008a94e0)[i]`) gated by `DAT_008a94d0`. Then `switch(DAT_007f0fd0)`
  with explicit cases **{4,5,7,8,9,10}** (position via `FUN_0046c7b0`,
  per-car float `FUN_00417730` = `(&DAT_0089a880)[car]` vs thresholds
  `_DAT_005cc31c`/`_DAT_005cc574`, gap `FUN_0046cbb0`). Rules **{0,1,2,3,6}**
  have no case → fall through to the lap/finish detector.

- **`FUN_00410d10`** (0x00410d10, per-frame camera-proximity elimination +
  scoring, C2; ported as `Race/RaceCamera`): `switch(DAT_007f0fd0)` cases
  **{4,5,7,8,9,10}**. Cases **5 and 7 early-`return`** before the
  proximity-elimination + scoring block (`FUN_0040eee0`, `FUN_004922e0`);
  cases 4,8,9,10 may return-1 (end) or fall through INTO it; the **default**
  (rules {0,1,2,3,6}) goes straight into the elimination block.

**Mechanically-proven objective split** (from `FUN_00410d10` control flow):
proximity-elimination runs for **every rule except {5,7}**. Therefore:

```
rule ∈ {5,7}   -> NON-elimination race (position/checkpoint via FUN_0046c7b0)
rule otherwise -> ELIMINATION active (camera-pull + FUN_0040eee0 scoring)
```

`[UNCERTAIN U-8988]` the exact win metric of `FUN_00417730`
(`(&DAT_0089a880)[car]`, a per-car float compared to `_DAT_005cc31c`/
`_DAT_005cc574` in rules 4/7/8/9/10). `[UNCERTAIN U-8989]` the win-condition of
rules 5/7 (position/checkpoint via `FUN_0046c7b0`) vs a lap-count race — the
standalone has only two objectives (elim vs non-elim), so rules 5/7 map to its
"laps" objective as the closest available; the exact win-condition is deferred
(D-11052).

## Standalone mapping (what the port does)

The standalone race layer (`D3d9Render/TrackRenderer`,
`Race/RaceSession`) exposes two objectives: `race_mode_` 0 = elimination
(camera-pull + scoring), 1 = non-elimination race (first to `lap_target_`,
`SetRaceMode`). `Race/RaceModes` ports steps 1–4:

- `SelectionToGameMode(sel)` — verbatim `FUN_0042f6b0`.
- `EventType(mode,track)` — verbatim table read (baked verified `DAT_005f65c8`
  cup bytes; standalone can't read MASHED .rdata — image-pad zeroes it).
- `RaceRuleFromEvent(ev)` / `RaceRule(mode,track)` — verbatim step 2/3.
- `IsEliminationRule(rule)` = `rule != 5 && rule != 7` (step 4).

`exe_main` now derives `cfg.raceRule` (the real 0067e9fc-rule) + sets
`cfg.raceMode = IsEliminationRule(rule) ? 0 : 1`. `MASHED_RACE_MODE` /
`MASHED_LAPS` remain as dev overrides applied AFTER the derivation.
Default `cfg.gameMode == 6` (the challenge-select scaffold default) → rule 0 →
elimination — **no change to the flagship demo default**.

## Verification status

- **DATA-VERIFIED:** the `DAT_005f65c8` cup table bytes (read from Ghidra;
  decoded + baked; re-decode cross-check in `re/analysis/` script output).
- **LOGIC-PORTED (decomp-faithful):** steps 1–4 transcribe the decompilation
  literally with RVA citations. `FUN_0042f6b0`/`FUN_0042f6a0`/`FUN_00430760`
  are already C3 (Frida A/B GREEN). The new derivation is a fragment of the
  C2-too-large `FUN_0043dfd0` + the C2 `FUN_00410510`/`FUN_00410d10`, so it is
  not independently diff-original-able as a unit — it stays data/logic-verified
  until the standalone race-rule engine is built out (WS-A/WS-H).

## WS-G2 (2026-06-16) — frontend mode selection wired to the real game mode

The five real frontend MODE items now SET the standalone game mode on select,
reproducing `FUN_0043dfd0`'s per-item `DAT_0067f184` (selection index) write +
`FUN_0042f6b0` (sel → game-mode `DAT_0067e9fc`) + the `DAT_0067ea64` team flag.
Map transcribed verbatim from the harvested decomp
(`re/analysis/standalone_menu_sm/harvest/FUN_0043dfd0.c`), names from
`original/TOASTART/Common/FONT/Defines.txt`:

| menu item (msgid, name)            | screen | action      | sel (`f184`) | game-mode | `ea64` |
|------------------------------------|--------|-------------|--------------|-----------|--------|
| 0xe5  `challengecupmess`           | 2 SP   | 0xff3d0000  | 1            | **3**     | 0      |
| 0xe6  `quickracemess`              | 2 SP   | 0xff4d0000  | 0xb          | **10**    | 1      |
| 0x24  `timeattackmess`             | 2 SP   | 0xff400000  | 0            | **2**     | 0      |
| 0x13e `topdogmess`                 | 3 MP   | 0xff2c0000  | 3            | **6**     | 0      |
| 0x140 `teamgamemess`               | 3 MP   | 0xff2e0000  | 3            | **6**     | 1      |

(Decomp lines: 0xff3d L259/L270, 0xff4d L931/L938, 0xff40 L262/L270, 0xff2c/0xff2e
L827/L829.) The main menu (kT1, title 0x40 `gametypeselectmess`) is Single Player
/ Multi Player / Options / Bonus Features / Exit; the mode items live one level
down on the Single-Player (kT2, 0x21) and Multi-Player (kT3, 0x22) screens — so
"each real mode" = these five items, NOT the main-menu rows. Modes 4/5 are the
later championship cup tiers (advanced by cup progression, not a menu pick);
modes 7/8/9 are MP game-type variants set on the game-mode config screen.

**Wired:** `Frontend/MenuNavSM.cpp` `ApplyActionGameMode` (called from
`Nav_Select`) sets `Nav_GameState().game_mode`. `exe_main` race launch now reads
that mode (default 3 = Challenge Cup when unset; `MASHED_GAME_MODE` overrides for
verification) → `Race/RaceModes::RaceRule` → objective. Mode 3, track 0 → event
0x9 → rule 0 → elimination (unchanged demo default).

**Verified (logic/data, GREEN):**
- `mashedmod/src/mashed_re/Race/tests/racemodes_test.cpp` — selection→mode (12),
  full `(mode 3/4/5, track 0..12)`→rule table vs the hand-decoded ground truth
  (39 cells), modes 2/6/7/8/9/10→rule 0, event→rule (13), elimination split (11),
  end-to-end for the 3 non-elim cup races (mode5 tracks 3/7/12). 0 failures.
- `navsm_test.cpp` Piece 4 — the five actions → game_mode + two end-to-end nav
  flows (SP Challenge Cup→mode 3→push colour; MP Top Dog→mode 6). 0 failures.
Both are pure-logic harnesses (no booted original); this stays data/logic-verified
per the same status as steps 1–4 (not independently `diff-original`-able as a unit).

## Residuals / deferred (WS-G2 follow-ups, WS-A, WS-H)

- Team Game (mode 6, `ea64=1`) shares the rule path of Top Dog (mode 6); the
  team-array setup (`FUN_0046dc00`, `DAT_0067e850`) is not yet wired standalone.
- MP/quick-race rule from `DAT_0067ea88` (game length) via the 0xff420000 action
  uses `RaceRuleFromGameLength` (already in RaceModes), but the game-length
  setting isn't live in the standalone yet — race launch uses the championship
  `RaceRule` path. Wiring the 0xff420000 game-length path: follow-up.
- Real lap counts come from `LAPDATA.LUA` (WS-F4), not env; `cfg.laps`
  default stays 3 + `MASHED_LAPS` override.
- Rules 5/7 (position/checkpoint races) and the per-car float win metric
  (`FUN_00417730`/`FUN_0046c7b0`/`FUN_0046cbb0`) need the standalone race-rule
  engine before they can be faithfully implemented: **WS-A/WS-H**.
