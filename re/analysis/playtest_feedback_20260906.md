# Playtest feedback triage — 2026-09-06 (standalone, real desktop)

Six issues reported from actual play of `mashed_re.exe`. Root causes grounded in
the source where I could trace them; hypotheses are marked as such. None fixed yet
— several span subsystems and two involve a design decision. Priority + ownership
at the bottom.

## The frontend-input cluster (one theme: per-screen value input not wired)

The standalone wired ENTER-navigation between screens and UP/DOWN cursor movement,
but several per-screen VALUE adjustments were never wired into `UpdateMenuSelection`
(`exe_main.cpp:1857`). The `LEFT`/`RIGHT` block there handles only the challenge
screens' mode cycle (sid 6/7, D-11054) — there is no `sid == 4` handler.

**1. Player Colour Select (screen 4) can't cycle colours.**
CONFIRMED. `UpdateMenuSelection` has no `sid == 4` LEFT/RIGHT case, so the colour
cursor is never changed. The machinery exists but is unused: `GameModeCarSelect.cpp`
wraps `FUN_00431b80` (the colour-change function, non-standard EAX/ESI convention,
U-1655) and the globals `0x0067ea98`/`ea9c`/`eaa0` (per-player colour slots). Nothing
calls it from input. **Fix:** add a `sid == 4` LEFT/RIGHT arm that drives
`FUN_00431b80` for the active player slot, same shape as the sid 6/7 mode cycle.

**5. All players are red.** DOWNSTREAM of #1. Car colour per slot comes from
`SlotColour(slot)` (`exe_main.cpp:705`) reading the per-player colour table; with the
colour screen non-interactive it stays at the default (car 0 = red) for every slot,
and the team/ability rows draw `kHandleCar0 + SlotColour(slot)` = car 0 for all. Fixing
#1 (and writing the per-player slot globals) fixes this.

**2. Challenge Select auto-selects Angel Peak, can't choose another track.**
HYPOTHESIS (needs a desktop check). `Nav_MoveCursor` (`MenuNavSM.cpp:1054`) only lands
on entries where `avail[c] == 1`, skipping unavailable ones. On a fresh save only
track 0 (Angel Peak) is unlocked, so if the cup's `avail[]` marks locked tracks as
0, the cursor can never leave row 0. The original lets the cursor rest on locked rows
(they show but can't be confirmed). **Fix candidate:** on the challenge cup, mark all
cup rows `avail=1` for cursor travel and gate the LAUNCH (confirm) on unlock instead —
verify against the original's screen-6 cursor behaviour. [UNCERTAIN] whether the
port's cup `avail[]` is the culprit or the cursor is reset per frame; confirm by
watching `Nav_Cursor()` while pressing UP/DOWN on screen 6.

## In-race items

**3. No pause menu — ESC in a race goes straight to the main menu.**
NOT IMPLEMENTED. The in-race ESC path requests exit to Frontend; there is no pause
state (freeze + overlay + resume/quit). This is a race-loop feature, not a bug in
existing code. Scope: a new in-race state.

**4. Points not counted properly.**
NEEDS INVESTIGATION. The team-scoring arms were ported exe-side in `TrackRenderer`
(`ScoreOnEliminationTeams`, Finding 31, 2026-09-04) and observed firing, but "points
not properly counted" in normal (non-team) play points at the non-team scoring path
or the standings display accumulation. Not root-caused here — needs a scored race
with the score global watched. Related trackers: the standings/score work in
`race_hud_capture_20260902.md`.

**6. Camera moves with arrow keys while the window is unfocused.**
HYPOTHESIS. Menu input IS focus-gated — `UpdateMenuSelection` returns early on
`!g_active` unless a demo driver is active (`exe_main.cpp:1863`). The IN-RACE input
path (drive/camera) apparently does not check `g_active`, so it reads global key
state (DirectInput is `DISCL_BACKGROUND|NONEXCLUSIVE`) while unfocused. The standalone
deliberately bypasses the focus PAUSE gate (CLAUDE.md, for unattended capture), but
that should not mean gameplay input runs unfocused. **Fix:** gate the in-race input
read on `g_active` too (keep a demo/capture bypass env, as the menu path does).

## Priority / ownership

1. **Colour cluster (#1 → #5)** — highest playability value, contained, frontend lane
   (my area). One `sid == 4` handler + writing the per-player colour globals.
2. **#2 track cursor** — frontend lane; quick once the `avail[]`-vs-original question
   is checked on a desktop.
3. **#6 focus-gate in-race input** — small, one guard; needs the capture-bypass env
   kept.
4. **#4 points** — needs a scored-race investigation (own slice).
5. **#3 pause menu** — a new in-race feature (largest); own slice.

All of these need desktop runtime verification to accept (this session is headless
and cannot drive the frontend — the standalone auto-races without a real interactive
desktop). None started, so nothing is half-changed.
