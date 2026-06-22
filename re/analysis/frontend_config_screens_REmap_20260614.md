# Frontend gameplay-config screens — RE map for faithful port (2026-06-14)

Clean dual-nav parity harness (`re/frida/frontend_parity.py`, chrome-on-black
both sides) result, 17 non-race screens:

```
FAITHFUL (10): s1 s2 s3 s8 s19 s29 s30 s31 s32 s33   (1–8% ink-diff)
DIVERGENT (7): s4 s6 s7 s15 s16 s18 s24                (15–33%)
```

All 7 divergent screens are the **gameplay-config family**. They render
per-player and per-setting content driven by frontend STATE the standalone does
not yet reproduce. Each needs its draw routine ported verbatim AND the state
globals it reads mirrored/initialized to the original's defaults.

NO-GUESSING: every RVA/global below cited from Ghidra decomp (Mashed_pool1,
2026-06-14) unless marked [UNCERTAIN].

## Draw-function → screen map

| screen | name             | content draw fn         | notes |
|--------|------------------|-------------------------|-------|
| s4     | Player Colour    | `FUN_004335f0`          | 6-car palette + swatches (top) + per-player controller icon+number rows |
| s15    | Ability Select   | [UNCERTAIN — not yet pinned; Elite/Pro/Amateur/Rookie hdr ids?] | 4 player rows w/ car+joypad+"1" |
| s16    | Team Select      | `FUN_0043aa30`          | "Team 1"/"Team 2" hdrs (msg 0x9f/0xa0) + player rows |
| s18    | Game Mode (MP)   | `FUN_004368e0`          | settings list values + car preview + power-up icons |
| s24    | Game Mode (SP)   | `FUN_004368e0`          | same, single-player variant (Difficulty/Opponent rows) |
| s6/s7  | Challenge Select | [UNCERTAIN — not yet pinned] | locked rows show bare stars; only selected named; 4 medal icons |

The orange item ROWS themselves come from the menu item-list draw
`FUN_0043c5b0` (already ported as the standalone's nav draw loop, MenuDrawLoopTwin
GREEN 20/20). The per-screen content fns above ADD car/controller/number/preview
ON those rows. So a faithful screen = (nav item list produces the right rows) +
(content fn draws the per-row/-setting extras).

## Shared draw primitives (already available in the standalone)

- `FUN_0042f8d0(x,y,w,h)` — the orange player-row plate: 5 `FUN_00472c60` quads,
  fill `0x146ef0` @ alpha=AL>>1 + border `0x1050b4` @ alpha=AL. == the
  `0xa0146ef0`/`0x40f8d0e8` item-bar the standalone already draws.
- `FUN_0042fab0(idx 0..9)` — car index → named car/badge sprite via
  `FUN_0040bb90`. Standalone already has the badges (palette renders).
- `FUN_0042bcb0(player, x, y, color, suffixFlag)` — ONE player's controller icon:
  reads device `*(int*)(0x7e96fc + player*0x200)` (1=joypad "joypad", 2=keyboard
  "keyboard"; suffixFlag appends 'i' variant), sprite from pc.txd dict
  `DAT_00636ac8`. Standalone already loads pc.txd keyboard/joypad sprites.
- `FUN_00427e00(msgId,x,y,color,scale,align)` — centered message-table text.
- `FUN_00427f00(cstr,x,y,color,scale,align)` — centered literal-string text
  (used for the per-player number "1".."4": `(char)idx + '1'`).
- `FUN_004282a0(size,scale)` — set font size before number draw.

## State globals (the inputs that must be mirrored)

Probed from the running original at colour-select (jumped-to default state,
`re/frida/probe_player_state.py`):

```
device  *(int*)(0x7e96fc + p*0x200) for p=0..3  = [1, 1, 2, 0]
        => 3 active players: P0 joypad, P1 joypad, P2 keyboard, P3 inactive(0)
player slots DAT_007f1a14+p*0x10 (order/active) = [0,0,0,0]
player slots DAT_007f1a1c+p*0x10 (car index)    = [0,0,0,0]
gamemode DAT_0067e9fc = 0     track DAT_0067f17c = 0
```

Per-screen player-position arrays (set by each screen's ENTER handler, NOT by the
bare `FUN_0043d2a0` push — so jumped-to may differ from real-flow):
- colour-select: `DAT_0067eaf8` (12 entries × 3 ints: [-2]=color/active(-NAN=off),
  [-1]=x, [0]=y), iterated `0x67eaf8..0x67eb88`.
- team-select:   `DAT_0067e938` (`0x67e938..0x67e9c8`), + player slots `DAT_007f1a14`.
- game-mode content `FUN_004368e0`: `DAT_0067e9fc` (mode 2/3/4/5/10), track
  `DAT_0067f17c`, settings table `DAT_005f65c8` (`(iVar+track*3)*4`), player car
  slots `DAT_007f1a1c..4c`, power-up icons via `FUN_00430670(slot)` +
  `FUN_0042fab0`.

## Open RE items (next session)

1. Pin the ability-select (s15) and challenge-select (s6/s7) content draw fns
   (callers of `FUN_0043c5b0`/the screen dispatch for screen ids 15/6/7).
2. Find each screen's ENTER handler that populates `DAT_0067eaf8`/`DAT_0067e938`
   so the standalone can initialize identical default state.
3. Decode the colour swatch table in `FUN_004335f0` (local_1c[] byte triples:
   car0={e0,e0,e0}, car1={98,3a,3d}, car2={4e,89,ae}, car3={61,76,56},
   car4={db,c3,62}, car5={00,00,00}) vs the standalone's `kSwatch[6]`.
4. Port order suggestion (tractability): s16 team (small, hdr+rows) → s4 colour →
   s18/s24 game-mode (FUN_004368e0 is ~10KB; values + preview + power-ups) →
   s6/s7 challenge.

## PROGRESS (2026-06-14)

Ported to FAITHFUL this session (parity diff, chrome-on-black):
- **s4 Player Colour**: 14.9% → **0.9%**. 3 per-player rows (controller icon +
  number), device default {1,1,2,0}=joypad/joypad/keyboard, measured geometry.
- **s15 Ability**: 28.8% → **3.5%**. 4 orange header tabs (Elite/Pro/Amateur/
  Rookie) + 4 rows car0+joypad+"1".
- **s16 Team**: 28.1% → **2.7%**. Orange Team1/Team2 tabs + 4 rows.

- **s6/s7 Challenge**: 33% → **2.8%/2.9%**. Gold-"Star"-per-row list (Star
  sprite loaded from INTERFACE.TXD), row0 = orange bar + star + "Angel Peak",
  rows 1–6 = lone star, + 4 devil icons + separator. Replaced the wrong
  all-track-names + preview + 5-car row.

Frontend now **15/17 non-race screens faithful**. ONLY REMAINING: s18/s24
(game mode).

## Game-mode state (probed at s18/s24, re/frida/probe_gamemode_state.py)

At BOTH nav screens 18 (DAT_0067e9f8=2) and 24 (=3), jumped-to default:
```
f_af10=1 (FUN_0043af10 option-row drawer ACTIVE)  gamemode DAT_0067e9fc=0  track=0
ea74=1 (game-type)  ea7c=0 (difficulty)  ea80=0 (powerups)  ea88=0 (length)
ea90=1  ea94=0 (vehicle)  ea98=4 ea9c=2 eaa0=3 (opponents)  ea78=0 eaac=0
```
Observed value strings (verify/parity/orig_s24.bmp): Power Ups→Standard,
Difficulty→Rookie, Vehicle→Hammerhead, Opponent1→Gold, Opponent2→Bluejay,
Opponent3→Meteor, Game Mode→One-against-all. The row STRUCTURE differs by screen
(s18 has Game Length/Air Strike/Extra Opponents; s24 has Difficulty/Opponent
1-3) — driven by the per-screen row-type table DAT_0067ed3c+screen*0x40 + the row
count FUN_0042ac00(), NOT by the ea* values (identical both screens).

REMAINING PORT (game mode, the last 2 screens): FUN_0043af10 per-row
setting→msg-id state machine (the switch(fStack_3c) maps row index → which ea*
setting; iStack_38 row-type picks the branch) + the value msg strings + the L/R
"Arrow" sprite; FUN_004368e0 vehicle preview + per-player car row + animated
power-up icons ("Powerupshadow" + FUN_00430670/FUN_0042fab0). The opponent
value→name map (ea98=4 → "Gold") is NOT a direct kCarColorNames index — needs
FUN_0042a940 / the opponent-name table RE before rendering (NO-GUESSING).

## GAME-MODE PORT STATUS (2026-06-14) — labels+values+plates DONE

The full game-mode row state machine is RE'd + ported (exe_main.cpp dedicated
block for screens 18/24; the nav item rows are suppressed for these two because
the nav's generic centered+grayed layout doesn't match). Confirmed maps:

- Descriptors decoded (MenuNavSM kT18/kT24): s24 rows (label msgids) = 0x150
  PlayGame, 0x56 PowerUps, 0xfd Difficulty, 0xfe Vehicle, 0x100/0x101/0x102
  Opp1/2/3, 0x24d GameMode; s18 = 0x150, 0x56, 0x24c GameLength, 0x103 ExtraOpp,
  0xfe Vehicle, 0xdb AirStrike, 0x24d GameMode.
- Value msgids (FUN_0043af10 switch on probed default state, ALL confirmed vs
  Font36.piz/ENGLISH.DAT — NOT the loose FONT/English.dat which is a different
  table): PowerUps 0x5b="Standard", Difficulty 0xd4="Rookie", Vehicle
  0x104="Hammerhead" (NOT 0x105=Shuriken), Opp1/2/3 via local_1c[ea98/9c/a0]=
  0x7c/0x7a/0x7b="Gold/Bluejay/Melon", GameMode s24 0x25e="One-against-all" /
  s18 0xec="Battle Game", GameLength 0xe0="5 mins", ExtraOpp/AirStrike
  0x59="Off". local_1c = {0x7f,0x79,0x7a,0x7b,0x7c,0x7d,0x7e}.
- Layout: Q_ListBaseY(count,0x1c)-58 → s24 row0 Y=84 (8 rows), s18 row0 Y=98
  (7 rows), pitch 28; plate x=58 (w 250/169), blue idle (0x40f8d0e8) / orange
  sel (0xa0146ef0), black label (left) + black value (right-aligned).

RESULT: s18/s24 21.7%/20% → **12.5%/12.8%** — labels + values + the "→" Arrow
sprite + the Hammerhead vehicle preview (car1 from TRACKIMAGES.TXD) + the 4
red-devil "vs" icons all render and are pixel-aligned to the original
(verify/parity/cmp/s24.png is visually near-identical). JUST over the 12% metric
line. The SINGLE remaining gap is the 4 power-up icons (bottom-centre) — drawn
by FUN_004368e0's powerup loop (FUN_00458630(id) over the default powerup-set
array DAT_007f0cb0); no clearly-named 4-icon sprite set exists in INTERFACE/
BADGES/FX.TXD, so reproducing them faithfully needs FUN_00458630 + the default
powerup-set RE. NOT guessed (NO-GUESSING). Everything below is DONE except #2:
  1. Vehicle PREVIEW (Hammerhead car + "HAMMERHEAD V8") center-right: needs
     FUN_0042a980(slot)→sprite idx → PTR_DAT_005f6710[idx] sprite NAME → load
     the vehicle TXD. Biggest remaining dark element.
  2. 4 power-up icons (bottom-centre): need the power-up sprite set.
  3. 4 red-devil "vs car" icons + separator (bottom-left): = kHandleCar0 ×4
     (already loaded) — quick add, low ink.
  4. The "→" Arrow sprite before each value (FUN_0040bb50("Arrow",...)): the
     Arrow is in BADGES.TXD, ALREADY loaded by the standalone's badges loader —
     just draw it left of each value. (FGDC20 glyph 0x81 is the green "Select"
     circle — wrong shape/colour; do not use it.)

ALL SPRITE SOURCES (turnkey for the content-body finish):
- Vehicle preview = sprite "car1".."car8" in **SFX.piz/TRACKIMAGES.TXD** (slot
  = FUN_0042a980(ea94); Hammerhead default = "car1"). The standalone's
  TRACKIMAGES loader (LoadTrackPreviews, kNames[24]) decodes this TXD already —
  add "car1".."car8" to a new slot/handle range and draw the selected one
  center-right (preview box measured ~x340-450 in orig_s24.bmp).
- "Powerup" sprite in **SFX.piz/INTERFACE.TXD** (the standalone already decodes
  this dict for cars/vs/Star) — for the 4 bottom-centre power-up icons.
- "Arrow" + "Button" in **SFX.piz/BADGES.TXD** — already loaded.
- 4 red-devil "vs" icons = kHandleCar0 (loaded) ×4 + separator, like the
  challenge screen's bottom row.
This is the ONLY remaining frontend work: 16/17... err 15/17 fully faithful +
s18/s24 core (labels/values/plates) faithful; content body is the last residual.

## Per-screen content dispatcher (FUN_0043bf30, tail of FUN_0043c5b0)

Each screen sets a per-screen "active" flag; the dispatcher calls the matching
draw. Confirmed map:
```
DAT_0067e7a8 -> FUN_004335f0  Player Colour (s4)      [PORTED]
DAT_0067e7e0 -> FUN_0043aa30  Team (s16)              [PORTED]
DAT_0067e7d8 -> FUN_0043a610  Race results (post-race)
DAT_0067e7f0 -> FUN_0043af10  Game-setup OPTION ROWS (→value + arrows; s18/s24)
DAT_0067e7b8 -> FUN_00439210  Game-mode screen (calls content body FUN_004368e0)
DAT_0067e7c8 -> FUN_00434720  Championship/cup standings (uses "Star" sprite)
DAT_0067e7b0 -> FUN_0042f0c0 | e7f8->FUN_00430b90 | e838->FUN_00431240 |
DAT_0067e830 -> FUN_004314b0 | e820->FUN_00431710 | e7e8->FUN_0042fb70 |
DAT_0067e810 -> FUN_0042fe90 | e818->FUN_00430120   [challenge/other — UNPINNED]
```

## Remaining screen ports (next sessions)

- **Challenge Select (s6/s7)**: fresh-save layout = 8 rows, row0 orange bar +
  GOLD STAR + "Angel Peak" (unlocked), rows 1–7 = lone gold star (locked); +4
  red-devil icons + vertical separator at the bottom (no preview, no car row —
  the standalone currently WRONGLY shows all 7 track names + preview + 5 cars).
  The star is the `"Star"` named sprite (FUN_0040bb50("Star",...), seen in
  FUN_00434720). Pin the exact challenge draw fn among the unpinned dispatch
  branches; confirm the lock→star logic + the 4-devil source. Need the "Star"
  texture loaded into the standalone (check INTERFACE.TXD / badges.txd).
- **Game Mode (s18/s24)**: the heavy one. `FUN_0043af10` draws each option row's
  inline `→ value` + L/R "Arrow" sprite from ~15 setting globals (DAT_0067ea74
  game-type, ea80 powerups, ea7c difficulty, ea88 length, ea90, ea94 vehicle,
  ea98/9c/a0 opponents, …) + msg-table value strings; `FUN_004368e0` draws the
  selected-vehicle preview + the per-player car row + animated power-up icons
  (FUN_00430670 + FUN_0042fab0). Needs the full game-setup default state mirrored
  + the "Arrow"/"Powerupshadow" sprites. s18 currently shows blank blue bars
  (no value text); s24 shows labels but no values/preview/icons.

## Harness note

`re/frida/frontend_parity.py` now renders chrome-on-black on BOTH sides:
- RE: `MASHED_PARITY=1` clears black + gates video/preview/arc (exe_main.cpp).
- ORIG: agent `nopBackdrop()` patches `FUN_00473c20`/`FUN_00474890`/`FUN_00473ee0`
  entry → `0xc3` (all cdecl, plain RET — safe NOP). Run is 2 spawns total.
Side-by-sides: `verify/parity/cmp/s<N>.png`.
