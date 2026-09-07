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
