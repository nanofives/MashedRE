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

---

## Results — wave 1 (2026-09-06, orchestrated child sessions)

All fixes are **measured/RVA-cited but UNVERIFIED AT RUNTIME** (headless auto-races;
verify on desktop with `MASHED_NAV_DEMO=1`). Committed `47ed32db`.

- **#1 colour cycle — FIXED (pending desktop).** The original's screen-4 handler is
  the `case 0x18` arm of `FUN_0043dfd0`: LEFT/RIGHT dec/inc the per-player colour
  global `0x0067ea98`(/9c/a0) wrap 0..6, then `FUN_00431b80(EAX=player, ESI=dir)`.
  Ported: `exe_main` sid==4 LEFT/RIGHT → `GameModeCarSelect::CarSelectCycleColour(0,±1)`.
- **#5 all-red — PARTIAL, NOT closed.** #1 cycles the colour cursor (`0x0067ea98`),
  but liveries read `0x007f1a1c`, which `CarSlotAssign` (`0x0042b9e0`) writes from
  `0x0067eaf0` — so the pick only shows after the confirm path runs `CarSlotAssign`.
  This matches the original; forcing a copy would exceed the LEFT/RIGHT scope. Needs
  its own slice: confirm on desktop whether the colour shows post-confirm, and if
  multi-player per-slot colours are expected, wire `CarSlotAssign`'s input.
- **#2 track cursor — FIXED (pending desktop); hypothesis was WRONG.** Not
  avail[]-locking: screens 6/7 have `item_count==1` (one `0xff040000` item in
  kT6/kT7), so `Nav_MoveCursor` wrapped mod-1 and pinned row 0 while the draw shows
  `cup.trackCount` rows. Fix: screen-6/7 branch in `Nav_MoveCursor` traverses all cup
  rows (rest-on-locked) + a locked-track launch guard in exe_main. `[UNCERTAIN]`
  whether the original rests-on vs skips fully-locked rows (`FUN_00430830`).
- **#4 points — DIAGNOSIS ONLY (no fix; needs runtime).** Non-team scoring arithmetic
  is faithful. Candidate defect: the original, for single-player races with AI
  survivors, calls the progress-based finish-order resolver `FUN_0040d590` and returns
  early, whereas the port eliminates cars one-by-one and awards by that order — if the
  orders differ, opponent points are wrong. Also possible: round-end timing in
  `Race/RuleEngine` scoring nobody who was never eliminated. Needs a scored single-player
  race with `DAT_008a94e0[0..3]`, `DAT_0067e9fc`, `DAT_0067ea64` logged per elimination,
  and whether `FUN_0040d590` fires. Fix would be in `TrackRenderer`/`RuleEngine`.
- **#6 focus-gate in-race input — in progress (wave 2).**
- **#3 pause menu — queued (wave 2); a new in-race feature.**

Orchestration note: the Agent worktree isolation forked children from a ~10-day-stale
commit (`350ac4ca`), so the two worktree fixes (#2, #4) were re-based onto HEAD by
hand (#2's hunks re-applied; #4 was diagnosis-only). Wave 2 runs in the main tree to
avoid that. Memory: `[[agent-worktree-forks-stale]]` (to be filed).

## Results — wave 2 (2026-09-06)

- **#6 focus-gate in-race input — FIXED (pending desktop).** Committed `11fd98e7`.
  One guard `s_live_input_ok = g_active || g_nav_demo || g_race_demo ||
  g_cfgedit_demo` ANDed onto the three in-race input reads (free-cam, RMB look,
  steer/accel). Demo/capture bypasses + `g_det_clock` preserved; no pause reintroduced.
- **#3 pause menu — FIXED core (pending desktop); genuine bug, not faithful-as-is.**
  Committed `10d737cc`. The original HAS a pause (FUN_004929d0 mode 3<->7; race tick
  FUN_004111c0 skipped in mode 7). Ported the faithful core: ESC pauses+freezes,
  ESC resumes, ENTER quits (-0xce0000); demo/capture paths keep exit-on-ESC. The
  overlay is a `[SCOPED]` placeholder — the original menu chrome (event 0xff210000 /
  FUN_0043d7c0 Resume/Restart/Quit strings + Restart -0xe00000 -> FUN_0040de10) is
  NOT reversed and deliberately not invented.

## Desktop verification checklist (one manual run + one race)

Run `MASHED_WIN_POS=left-bl mashedmod\build\mashed_re.exe` (NO `MASHED_NAV_DEMO`),
click the window to focus it, then play a race manually:

**Do NOT set `MASHED_NAV_DEMO=1` for a manual playtest** (corrected 2026-09-07 after
it cost a run). `exe_main.cpp:1868` bypasses the #6 focus gate when `g_nav_demo` is
set, and DirectInput is `DISCL_BACKGROUND`, so global key state drives the menu:
launching from a terminal made the terminal's own ENTER confirm screen 6/7 and
auto-launch a race before the frontend ever settled (`[gameflow] RequestRace` on the
first frames, no `NAV_DEMO` lines, no `walk_*.bmp`). Nav-demo is the automated screen
walker, not a playtest mode.

1. **#1** Colour Select: LEFT/RIGHT cycles the car colour.
2. **#2** Challenge Select: UP/DOWN moves across all 4 cup rows; a locked row won't launch.
3. **row icons** (earlier commit db4da943): selected row shows MultiPlayer, others a
   faint check, no star. (Same screen as #2.)
4. **#6** In a race, unfocus the window + press arrows -> car/camera must NOT move;
   refocus -> they do.
5. **#3** In a race, ESC -> cars freeze + "PAUSED" overlay; ESC -> resume; ENTER while
   paused -> back to menu.

## Remaining follow-ups (open)

- **#5 all-red** — confirm on desktop whether the colour shows after confirm (it reads
  0x007f1a1c via CarSlotAssign, not the 0x0067ea98 cursor #1 writes). If per-slot
  multiplayer colours are wanted, wire CarSlotAssign's input. Own slice.
- **#4 points** — needs a scored single-player race with DAT_008a94e0[0..3] /
  DAT_0067e9fc / DAT_0067ea64 logged per elimination + whether FUN_0040d590 fires;
  fix in TrackRenderer/RuleEngine. Own slice.
- **#3 full pause menu** — reverse the 0xff210000 / FUN_0043d7c0 chrome + Restart,
  replace the placeholder overlay. Own slice.
