# HB-TEXANIM-REFS PREREG2 (2026-10-27): pinned water-view poses

Committed alone, after round 1 (`measure_r1.txt`) and the standalone-only pose scan, before any
original capture or standalone arm of this round. PREREG.md is not amended; this is a second,
separately registered round. The flip rule of PREREG.md section 5 is replaced by section 5 below
for the decision (round 1 is reported alongside).

## 1. Why a second round

Round 1 scored only Training s20 (mask 0.57%, toward). On the grid vantages Sands and Warzone have
a 0.00% knob footprint:

- Sands `WATER.DFF` is one plane at y = -1.23 spanning x, z in [-169, 169] (`dff_dump.py`); the grid
  camera never sees it.
- Warzone `RIVER.DFF` ships material texture `water00`, which is the ring's node 0. Until the
  ring advances, ANIM and OFF draw the same texture, so `|ANIM - OFF|` cannot find the water.

## 2. Mask arm (dev only)

`MASHED_TEXANIM_MASKTEX=<name>` (commit `13ca1b0aa`, `RaceSubmit_TexAnimCreate`): with
`MASHED_TEXANIM=1`, the ring is that one texture, so the water clump draws it. Unset = no effect.
Mask texture: Sands `BarrelRed`, Warzone `BarrelRed`, Training `Brick`. Mask
W = `|L(MASK) - L(OFF)| > 6`.

## 3. Poses

Scan (`poses_scan.json`, `scan_table.txt`): 28 synthetic bases aimed at each water body, standalone
OFF and MASK only (no original pixels read). Pick rule: the two poses with the largest W per track.
Picked (`poses_r2.json`): sands_p11 (61.87%), sands_p8 (61.07%), training_p3 (52.99%),
training_p4 (51.09%), warzone_p3 (44.38%), warzone_p2 (43.51%).

## 4. Arms

- Original: `race_draw_burst.py --challenge N --pose <basis> --settle 8` from the private root
  (unlocked save copy), challenge Training 0, Sands 5, Warzone 11. The `ORIGINAL TRACK =` line must
  name the track. The standalone uses the capture's own `orig_cambasis.txt`.
- Standalone, exe `9f2afe92` (`13ca1b0aa`), per pose: OFF, ANIM, MASK. OFF2 on each track's first pose.

## 5. Rules

Luma 0.299R + 0.587G + 0.114B, means over W. dOFF = |orig - OFF|, dANIM = |orig - ANIM|.

- R0: OFF vs OFF2 bit-identical on each track's first pose.
- R1 (per track): TOWARD when dANIM < dOFF on both poses. AWAY when dANIM > dOFF on either pose.
  Anything else is MIXED (not TOWARD).
- R2: on each pose, ANIM != OFF outside W dilated 3 px covers at most 0.5% of the frame.
- Flip `MASHED_TEXANIM` default ON only when R0 and R2 hold and all three tracks are TOWARD.
  Otherwise no flip, verdict DECISION with the per-track numbers.
- After a flip: default render == pre-flip ANIM on each track's first pose, bit for bit, and the
  default gates equal `merge_train.json` refs.

Reported, not gating: per-channel means over W, per-pixel mean |orig - arm| over W. Expected for
Warzone (stated before the arms): OFF draws `water00` and ANIM draws the ring frame current at the
capture, and the 32 ring textures differ by at most 1.5 luma (HB-FOREST-WATER section 1), so
Warzone's dOFF and dANIM are expected within about 1.5 of each other.
