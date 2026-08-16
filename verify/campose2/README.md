# MASHED_CAM_POSE — re-verification + U-9039 evidence (2026-08-15)

## Why this exists: the first verification was invalid

Commit `53855ee1` claimed MASHED_CAM_POSE was "VERIFIED ... pose_t06 vs nopose_t06 differ
on 47.91% of pixels". **Those captures were on the Challenge Select MENU, not in-race**
(`MASHED_GOTO=6` alone parks there; reaching a race also needs `MASHED_RACE_DEMO=1`).
The menu carries the DirectShow video backdrop, which was nondeterministic until the R10b
fix landed later the same day. So that 47.91% was backdrop noise attributed to the camera.
The conclusion happened to be right; the evidence was not.

Third time in one session that this backdrop confounded a measurement — the other two
being R10b itself and the `02_back_to_menu` residual.

## Proper verification

In-race, deterministic (`MASHED_DETERMINISTIC=1` freezes the backdrop), identical env apart
from `MASHED_CAM_POSE`:

```
nopose  mean=[145.6, 115.3, 60.4]  stddev=[86.6, 70.6, 31.4]  distinct=42171
pose    mean=[ 29.5,  17.8, 11.0]  stddev=[12.8, 13.2, 12.8]  distinct= 4550
01_inrace_track: 96.01% of pixels differ
```

**The mechanism works** — the standalone consumes the variable and repoints the camera.

## What it exposed: U-9039

The pose fed in was the original's, captured live from a Quick-Battle race:
`eye=(0.0932, 3.2637, 4.7482) at=(0.0275, -2.7777, -6.0828)`.

Arctic's radius is **80.46** (`mashed_re.log`, R4 track load). A pose of magnitude ~5 is at
the track *centre*. `pose.png` shows the result: the camera buried between two walls, a
near-uniform dark frame.

So the original's `DAT_00897fe0` pose is not directly usable as a standalone world pose —
scale, origin or basis differ. Filed as **U-9039**, which independently reproduces the
finding of `ba7c104f` (2026-06-19, branch `ws-visual-polish`, retired 2026-08-15) that had
never been recorded in any tracker on main.

**Consequence:** same-view parity is still blocked, and so is deciding which renderer is
faithful (ROADMAP v3 D1). `verify/d1_measure/` compares the two standalone renderers to
each other; adjudicating them against the original needs this transform first.
