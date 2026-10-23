# RESULT: savelink2 (a), the knob's progression side effect on the cup screens

PREREG: `PREREG_knob_progress.md` (f442b2f73). Exe `35a50e1f`, private root `D:/MashedWT/savelink_root`, sandbox disabled,
runner `run_knob_arms.py`. Evidence: `D:/MashedWT/savelink2_data/` (`knob_S0.txt`, `knob_arms.txt`, `knob_diff.txt`,
`knob/<arm>.{drawstream.json,gameflow.log,exe.log}`).

**Verdict: P1, P2 and P3 hold.** With the shipped save, `MASHED_SAVE_ORIGINAL=1` loads the standalone's progression as
13/13 unlocked, and screen 6 draws the same thing as knob OFF. The load side does not block a default flip.

## Merge gate (exe 35a50e1f, knob OFF, `arms_merge.txt`)

| Arm | Hash | Reference |
|---|---|---|
| H_DEF-1 step / gates / ptrace | bacfc747 / 8dd1c004 / 6f9932b6 | same |
| B_DEF_K0 / K1 / K26 | f9deb9b9 / d3601fcc / 9da9a26a | same |

## P1, log (held)

| Arm | `progress loaded` | Boot restore (R2-2) |
|---|---|---|
| S0, S0r, S0d (OFF) | 1/13, counter 0 | LOADED, original/gamesave.bin |
| S1, S1d (ON) | 13/13, counter 0 | LOADED, original/gamesave.bin |
| S1e (ON, root save col 4 = 0 on rows 1..12) | 1/13, counter 0 | LOADED, shipped copy via MASHED_SAVE |

## P2, draw (held, at the noise floor)

The draw streams are not byte-identical run to run (S0 032c2624 / b0e65276, S0r 6d72ef94, all 324,929 bytes). A structural
walk finds that only vertex blobs (`/v`) differ. Draw count (272 per frame), order, texture, blend and retaddrs are the same
in all arms. `drawlist_diff.py --scale-b 1 --tol-anim 4`:

| Pair | Verdict | Divergent draws |
|---|---|---|
| S0 vs S0r (noise floor, both OFF) | RED 1084 / 4 | draw 158 only |
| S0 vs S1 | **GREEN** 1088 / 0 | none |
| S1 vs S1e | RED 1084 / 4 | draw 158 only |
| S0 vs S1e | RED 1084 / 4 | draw 158 only |
| S0d vs S1d | RED 1084 / 2+2+2 | draw 158 only (moved, or missing/extra when the size gap is large) |

Draw 158 is a white quad, tex 86, blend 1:5/6, `HudIm2DQuad+0x188` called from `WinMain@16+0x1c9c`. Its size runs between
45.08 and 60.32 across frames and runs. It diverges between two knob-OFF runs, so it is a wall-clock animation and not a
knob effect. Every other draw matches in every pair.

## P3, hygiene (held)

The root save sha256 was the same before and after every arm (S1e: the edited 0d2be29582a88e28 throughout). The root
save was restored to shipped bd18788182b2343e after each arm. No arm wrote a save.

## Why (static, matches the measurement)

- The standalone restores `original/gamesave.bin` into 0x007f0a40 at boot regardless of the knob (exe_main.cpp:9445-9488).
  The shipped span has col 4 = 2 on all 13 rows, which equals the original's live dump in `savelink_data/arm_C.log`.
- `Campaign_CurrentCup` ORs live col 4 with `g_progUnlock` (GameFlow.cpp:316), so the cup's `.unlocked` is all-true in both
  arms. Screen 6's row icons, movers and launch gate read the live table and never `g_progUnlock`.
- S1e removes the knob's contribution (`g_progUnlock` = 1/13) and draws the same thing, so `g_progUnlock` has no visible
  path on screen 6 with this save.

## Not covered

- **Row 3 is not measured.** `MASHED_NAV_KEYS=d,d,d` did not move the row after a `MASHED_GOTO=6` push. NAV_KEYS logged
  depth 1, cursor -1, row 0 for all 409 frames. So S0d/S1d are a second row-0 pair. The detail panel (cols 4/6/7 by the
  selected row) reads the live table either way.
- **Screen 7 and race-result writes.** Under the knob, `Campaign_OnRaceResult` writes col 5 (trophy, original meaning
  `[UNCERTAIN]`) into the original's file. Col 4 writes are no-ops on the shipped save (already 2). Not run here.
- **The original's own cup screen with this save** was not captured in this lane. The original's table equals the file
  (arm_C), and the standalone's screen 6 parity is not acceptance-grade (no draw-list GREEN exists for screen 6).

## What this decides

"All-unlocked under the knob" is the state the shipped save already puts live on both sides, not a side effect. What still
stands against a default flip is unchanged from savelink: every default run would write the diffing reference
`original/gamesave.bin`, and the col 5 trophy cell is written with a standalone-private meaning.
