# HB-TEXANIM-REFS PREREG (2026-10-27)

Committed alone, before any standalone arm runs. Not to be amended after the first arm.

## 1. Question

HB-FOREST-WATER (`verify/hbforestwater_20261027/RESULT_HB-FOREST-WATER.md`) ported the hook-table
texture flip (ring `waterNN`, `FUN_00475090` / `FUN_004751d0` / `FUN_00475060`) behind
`MASHED_TEXANIM=1` and scored it on Forest only. The same table rows run on Sands (Course_Id 11,
clump 1, units x2), Training (30, clump 9, units x1) and Warzone (33, clump 0, units x2), wired but
not scored. L-39 (b): score those three against the original. Flip the default ON only if all
three move toward the original.

## 2. References (original side)

`re/frida/race_draw_burst.py --challenge N --settle S`, run from a private root
`D:/MashedWT/HB-TEXANIM-REFS_root` (its own `original/` copy, gamesave unlocked with
`gamesave_edit.py --rows 0..12 --set c1=1,c3=2`; the shared `original/gamesave.bin` is not touched).
Challenge index: Training 0, Sands 5, Warzone 11 (`verify/geomlight_broadcheck/RESULT.md`). The
tool's own `ORIGINAL TRACK =` line must name the track, or the capture is not used.
Settle S in {8, 14, 20} (three vantages per track, as `verify/water_refs`).

## 3. Arms (standalone side)

Exe `mashedmod/build/mashed_re.exe` built from `orch/HB-TEXANIM-REFS` at the PREREG commit (no
source change), `verify/hbforestwater_20261027/run_render.py` recipe (race demo, deterministic
clock, `MASHED_CAM_POSE` = the capture's `orig_cambasis.txt`), capture `race1/01_grid.bmp`.
`MASHED_TRACK_SEL`: Training 12, Sands 10, Warzone 8.

Per vantage: `OFF` (default) and `ANIM` (`MASHED_TEXANIM=1`). Per track, one `OFF2` on the
s8 vantage.

## 4. Metric

Luma 0.299R + 0.587G + 0.114B. Mask W = `|L(ANIM) - L(OFF)| > 6` (the knob's footprint).
A vantage is **scored** when W covers at least 0.5% of the frame.
dOFF = |mean L_orig(W) - mean L_OFF(W)|, dANIM = |mean L_orig(W) - mean L_ANIM(W)|.

## 5. Rules

- R0: OFF vs OFF2 bit-identical on each track's s8 vantage.
- R1 (per track): TOWARD when the track has at least one scored vantage and dANIM < dOFF on every
  scored vantage. AWAY when dANIM > dOFF on any scored vantage. NOT SCORED when no vantage is scored.
- R2: on each scored vantage, ANIM != OFF outside W dilated 3 px covers at most 0.5% of the frame.
- Flip `MASHED_TEXANIM` default ON (opt-out knob) only when R0 and R2 hold and all three tracks are
  TOWARD. Otherwise no flip, verdict DECISION with the per-track numbers.
- After a flip: the default exe's render equals the pre-flip ANIM render bit for bit on each
  track's s8 vantage, and the default gates (`verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0
  B_DEF_K1 B_DEF_K26`) equal `merge_train.json` `gctrl.ref` / `b.ref`.

Reported, not gating: per-channel means over W, imgdiff global mean orig vs OFF / ANIM.
