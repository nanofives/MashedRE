# Spec — Playtest #1b "can't select colours" (Player Colour Select has no visible selection)

Scoping (produced 2026-09-07). For a later account3 / Ghidra-capable session. The
INPUT side is fixed and verified this session; what remains is a Ghidra-gated RENDER
parity question.

## What is already done + PROVEN (do not redo)
- The colour-select crash (absolute `call 0x00431b80` in the standalone) is FIXED —
  `Frontend/GameModeCarSelect.cpp` `CallCursorMover` guarded under `MASHED_STANDALONE`.
- Input + cursor logic WORK, verified live 2026-09-07 via a temp `MASHED_DBG_COLOUR`
  probe (since removed): on screen 4, LEFT/RIGHT cycle the real cursor
  `DAT_0067ea98` = 0→1→2→3→4→5 and `g_csel_p1_car` mirrors it (exe_main.cpp:2313 arm,
  `CarSelectCycleColour(0,dir)`). `active=1`, `carsel_ready=1`, `hr=S_OK`.

## The defect
`exe_main.cpp` screen-4 renderer (`Nav_ScreenId()==4` block, ~line 5024) computes
`selCol = g_csel_p1_car` and then discards it — **`(void)selCol;`** at the end of the
block (~line 5123). So pressing LEFT/RIGHT changes the cursor value but NOTHING on
screen moves → the colour looks unselectable. The moving selection cursor was removed
2026-08-28 (comment at ~5117) on the reading that "the original draws NO such element";
that left zero selection feedback.

## Why it is Ghidra-gated (conflicting RE notes — RESOLVE FIRST)
The screen-4 render notes CONTRADICT each other and must be reconciled by decompiling
the case-10 renderer before any pixel is drawn:
- One note (exe_main.cpp ~4918-4932): SIX static colour tiles in a row (64x64 car +
  69x20 swatch at x=146+i*70) + three controller rows; selection = a cursor that MOVES
  to the chosen column (`g_csel_p1_car`).
- Another note (exe_main.cpp ~4933-4943): the original draws the ACTIVE PLAYER'S CHOSEN
  CAR LIVERY (`FUN_0042fab0(colorIdx)` → NFL{Red,Bluejay,...}) as a SINGLE cycled sprite
  at virtual (40,292) 112x112 — no 6-tile palette, no moving cursor.
These cannot both be the real screen. Decomp settles which.

## RVAs to decompile (read-only pool slot)
| RVA | Role |
|---|---|
| `FUN_004368e0` (0x004368e0) | the screen-4 / case-10 content renderer — the authority on WHAT the colour screen draws (6 tiles vs 1 cycled car) and HOW selection is shown. |
| `FUN_004335f0` (0x004335f0) | per-player controller/cursor draw (`DAT_0067eaf8`) — whether a device-icon cursor sits under the selected colour column and moves with L/R, or not. |
| `FUN_0042fab0` (0x0042fab0) | colour→NFL* livery sprite mapper (INTERFACE.TXD) — confirms the per-colour sprite the selected car should show. |
| `FUN_0043dfd0` case-0x18 arm | already reversed for input; re-read only to confirm the cursor RANGE (see wrap bug below). |

## Secondary bug to fix in the same slice (cursor range)
The cursor `DAT_0067ea98` wraps 0..6 (SEVEN states — `FUN_00431b80` tail maps 7→0/1),
but the port draws only SIX colour tiles (0..5). At `ea98=6` the port clamps
`g_csel_p1_car` to 5 (exe_main.cpp:2341), so colour 5 shows for both cursor=5 and =6 and
a R-press from 5 appears to "stick". Decomp must establish the real colour COUNT (6 or 7
— the NFL* livery set has more than 6 entries) and whether the wrap is 6→0 or 7→0 for
this screen, then align both the cursor clamp AND the tile count.

## Acceptance gate (parity harness, not a screenshot — re/analysis/parity_tooling.md)
1. Draw-list diff of screen 4 while cycling colour: original-side burst (the frontend
   walk / `menu_draw_burst.py`) vs the standalone `MASHED_DBG_DRAWSTREAM` dump, per
   colour value, via `re/tools/drawlist_diff.py`. GREEN or RED-with-cited-rows.
2. The selection must be VISIBLE and correct: pressing LEFT/RIGHT visibly moves the
   selection to the matching colour, and the committed choice reaches the livery (this
   also closes the #5 all-red loop once the `ea98→eaf0` writer from
   `re/orchestrator/frontend_playtest_fanout_20260907.md` §A is wired).
3. No invented element: whatever is drawn must be traced to `FUN_004368e0`/`FUN_004335f0`
   (the previous placeholder cursor was reverted precisely for being invented — do not
   re-add it without the decomp behind it).

### Files a later session touches
- `mashedmod/src/mashed_re/exe_main.cpp` — screen-4 render block (~5024-5123, currently
  `(void)selCol`), and the cursor clamp (~2341).
- Depends on / pairs with §A (#5 all-red) in the fan-out ledger — same screen, same
  `ea98`/`eaf0`/`007f1a1c` data flow.

## RESOLVED 2026-09-09 — Ghidra read (Mashed_pool14, read-only), every claim cited

The "conflicting notes" question is settled: the screen draws the SIX static tiles AND
the per-profile device icon is the selection indicator, and that icon both SLIDES and
TINTS. Evidence (all from `mcp__ghidra__decomp_function` / `listing_disassemble_range`
/ `memory_read` on the slot):

| RVA | What it literally does |
|---|---|
| `FUN_004335f0` (0x004335f0) | SP path walks the profile array at `0x0067eaf8` (stride 12: `[-2]` choice, `[-1]` x, `[0]` y). On the first live profile it draws the six tiles: swatch `FUN_00472c60(143+i*70, y-38, 69, 20, local_1c[i+1])`, car `FUN_004739f0(FUN_0042fab0(i), 146+i*70, y-41-40, 64, 64, ...)` (constants `_DAT_005ccd0c`=70.0, `_DAT_005cd910`=38.0, `_DAT_005cd914`=41.0, `_DAT_005cd274`=40.0). Then per live profile: `FUN_0042bcb0(profile, x, y-1.0, argb, flag)` with argb = `local_1c[choice]`; choice 0 -> `e0e0e0`; choice 6 (`8.40779e-45` = int 6) -> `local_70` = `d7d7d7`, flag=1; a clash with another profile's equal choice brightens the argb by `FUN_004a2c48()` (doubled for choice 6). |
| `FUN_004335f0` stack table | `local_1c[0..6]` = e0e0e0 / 98,3a,3d / 4e,89,ae / 61,76,56 / db,c3,62 / eb,a7,a7 / 00,00,00, alpha = `DAT_0067e7ac`. Indices 1..6 are the tile swatches; indices 1..5 + the d7d7d7 override are the icon tints. The port's `kPlayerSwatch[6]` equals `local_1c[1..6]`. |
| `FUN_004332a0` (0x004332a0) | While `DAT_007f1a0c == 0x1000`, per profile: L decrements the choice if `0 < c`, R increments it if `c < 6` (input bytes `0x007f1044`/`0x007f1504` families) — the choice range is 0..6, NON-wrapping. Every frame it then recomputes the icon position: choice -1 -> set to 0; choice 0 -> `x = 0x42aa0000` (85.0), `y = row`; choice 1..6 -> `x = (choice*0x46 - 0x46) + _DAT_005cd90c` = `170.0 + 70*(choice-1)`, `y = row`; `row += _DAT_005cd908` (34.0) per live profile. Profiles not present (`thunk_FUN_00497450(i)==0`) get choice -1. |
| `FUN_0042bcb0` (0x0042bcb0) | 5-arg glyph draw: picks `"keyboard"` when `DAT_007e96fc[profile*0x80] == 2`, the joypad name when `== 1`; draws CENTRED on (x, y): `local_1c = x*_DAT_005cd5a8 - _DAT_005cd18c`, `local_24 = x*_DAT_005cd5a8 + _DAT_005cd18c`; argb passed through to `FUN_004b5750`. It does not choose the colour. |
| `FUN_0043dfd0` sites 0x0043f1f1 / 0x0043f2b3 | `MOV [EAX],EBP` loops over `0x0067eaf0..0x0067eb80` step 0xc — ONE register value into all 12 choice slots, immediately before `push 4; call 0x0043d2a0` (enter screen 4). Bulk reset, not a per-profile commit. EBP's value at that point: `[UNCERTAIN]` (not read this session). |
| `FUN_00431b80` (0x00431b80) | Advances `(&DAT_0067ea98)[EAX]` by ESI with collision avoidance against `DAT_007f1a1c + 1` and the other three `ea98/ea9c/eaa0` values; wraps 7 -> 1 (or 7 -> 0 when param_1 != 0) and 0/-1 -> 6. This is the mover the port's `CarSelectCycleColour` calls; it does not touch `0x0067eaf0`. `[UNCERTAIN]` which original screen drives it — not needed for this slice. |

Consequences for the port (applied to `exe_main.cpp` 2026-09-09, build clean, **screenshot-pending**):
- The keyboard-row icon slides to centre `x = 170 + 70*(sel-1)` (85 when no choice) and tints
  `kPlayerSwatch[sel-1]` for sel 1..5, `d7d7d7` for sel 6, `e0e0e0` for 0. The 2026-09-07 diff had
  the slide base at 146 (the tile's LEFT edge; the original centres the icon at 170) and black for 6.
- **D-11066's "missing writer" does not exist**: in the original `0x0067eaf0` IS the cursor
  (moved in place by `FUN_004332a0`). The port keeps its `0x0067ea98` cursor and synthesises eaf0
  at confirm time — a `[SCOPED]` shim, documented at the site. The faithful shape (make eaf0 the
  cursor and retire ea98 on this screen) is a follow-up slice.
- The cursor RANGE question: the original's choice is 0..6 non-wrapping (7 states incl. 6 = silver
  icon over the black tile). The port's ea98 mover wraps per `FUN_00431b80`; alignment is part of
  the same follow-up slice.
- Only profile 0 is modelled; the original slides/tints EVERY live profile's icon by its own choice.

