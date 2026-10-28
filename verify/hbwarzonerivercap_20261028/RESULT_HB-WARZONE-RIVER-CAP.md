# HB-WARZONE-RIVER-CAP RESULT (2026-10-28): non-letterboxed Warzone river, port +2.8 luma, flat across distance

**Verdict: GREEN, no port code change.** Seven new original Warzone captures (challenge 11, the
`poses_r2.json` basis pinned) were taken in race state 3, before the "Current Standings" overlay.
All seven have 0 letterbox rows, and the port frame overlays them at identity (best fit scale 1.00,
offset 0, 0). Under the unchanged PREREG2 mask the port's river is **+2.7 to +3.0 luma (OFF)** and
**+2.9 to +3.1 (ANIM, the L-42 default)** brighter than the original, on both poses. The offset is
flat from the horizon to the near water (+1.8 to +3.4 by 25-row band). That sits inside the
Training (3.3 / 3.6) and Sands (3.9 / 3.9) residuals HB-TEXANIM-REFS accepted. It replaces the
PREREG2 Warzone numbers (+16.5 / +10.1), which scored a letterboxed frame.

- The far-bright / near-dark band pattern in HB-WARZONE-RIVER section 3 does not reproduce. With
  matched geometry every band has the port brighter by 1.8 to 3.4. That pattern came from the
  mismatched projection of the letterboxed view, as that RESULT warned.
- Only harness code changed (`re/frida/race_draw_burst.py`, section 4). No port source, no build,
  so gates not run. The port frames are HB-TEXANIM-REFS' round-2 arms, unchanged.
- 12 original launches: 3 probe cases (probes 1 and 3 used their retry, so 5 boots) and 7 scored
  captures. `capture.sh` checks the audio endpoint once per case, so 10 checks, and the 2 probe
  retries started seconds after their case's check without a fresh one. Every check found 1
  endpoint (`Philips FTV (NVIDIA High Definition Audio)`, status OK). No AV at `0x005baf48`. No
  U-9677 piz-collision crash.

## 1. Captures

Recipe: `capture.sh` (copy in this folder). After the runs its audio check moved inside the retry
loop, so the committed copy checks before every launch. The runs above used the per-case form. Private root `D:/MashedWT/HB-WARZONE-RIVER-CAP_root`
(robocopy of `original/`), each boot under `mashed_lock.py run --label HB-WARZONE-RIVER-CAP`, through
`re/tools/run_with_unlocked_save.py` with the `gamesave_unlocked.bin` HB-TEXANIM-REFS used
(sha `b46217c0...`, copied from `D:/MashedWT/HB-TEXANIM-REFS_root/original/`). The root's
`gamesave.bin` was restored and sha-verified (`bd18788182b2343e`) after every boot.

```
race_draw_burst.py --challenge 11 --pose <poses_r2.json basis> --settle S --settle-from-pose
```

`S` counts from the first pinned-camera write (0.8 s after race entry on all seven runs).
`DAT_0063ba8c` sampled every 0.5 s through the settle (3 = driving, 5/6/7 = standings, per
memory `no-driving-hud-race-ui-is-standings`):

| capture | S (s) | `0x63ba8c` samples | black rows | track | r at identity | best fit (scale, ox, oy) |
|---|---|---|---|---|---|---|
| warzone_p2_s1 | 1 | 3 x3 | 0 | WARZONE | 0.518 | 1.00, 0, 0 |
| warzone_p2_s2 | 2 | 3 x5 | 0 | WARZONE | 0.523 | 1.00, 0, 0 |
| warzone_p2_s3 | 3 | 3 x7 | 0 | WARZONE | 0.531 | 1.00, 0, 0 |
| warzone_p2_s4 | 4 | 3 x9 | 0 | WARZONE | 0.510 | 1.00, 0, 0 |
| warzone_p3_s2 | 2 | 3 x5 | 0 | WARZONE | 0.346 | 1.00, 0, 0 |
| warzone_p3_s4 | 4 | 3 x9 | 0 | WARZONE | 0.322 | 1.00, 0, 0 |
| warzone_p2_s8 | 8 | 3 x14, then 5 x3 | 0 | WARZONE | not scored | |

Fit = gradient correlation, full frame, grid scale 0.98..1.02 and offset -4..4 px. Identity wins on
every capture. The old letterboxed p2 needed scale 0.81 / 0.79 (HB-WARZONE-RIVER section 3).

The state goes 3 to 5 between 7.0 and 7.5 s after the first pinned-camera write (p2_s8). The frame
taken at 8 s has no black rows yet. [UNCERTAIN] Whether the pinned camera brings the overlay
earlier than an unpinned race: HB-TEXANIM-REFS' unpinned `warzone_s8` was clean, but it timed its
settle from race entry and logged no state. Missing: an unpinned run with the state log.

All 4 cars sit at the same grid positions on every run (CARPROJ, x 40.58..43.89), off-screen on
both poses, so no car is in the scored region.

## 2. PREREG2 mask, full frame

Mask `|L(MASK) - L(OFF)| > 6` from the HB-TEXANIM-REFS round-2 arms, unchanged.
`measure_cap.py` section 2, full output in `measure_cap.txt`:

| case | original | n | original L | OFF L | ANIM L | OFF - orig | ANIM - orig |
|---|---|---|---|---|---|---|---|
| warzone_p2 | old orig2 (letterboxed, PREREG2) | 133657 | 62.4 | 72.5 | 72.7 | +10.1 | +10.3 |
| warzone_p2 | new s1 / s2 / s3 / s4 | 133657 | 69.5 / 69.8 / 69.7 / 69.7 | 72.5 | 72.7 | +3.0 / +2.7 / +2.8 / +2.8 | +3.1 / +2.9 / +3.0 / +3.0 |
| warzone_p3 | old orig2 (letterboxed, PREREG2) | 136328 | 54.9 | 71.4 | 71.6 | +16.5 | +16.7 |
| warzone_p3 | new s2 / s4 | 136328 | 68.7 / 68.6 | 71.4 | 71.6 | +2.7 / +2.8 | +2.9 / +3.0 |

The original's river luma moves 0.3 (p2) and 0.1 (p3) across settle times, so the water animation
phase does not move the score.

## 3. Distance bands (original rows, 3 px eroded mask)

Same projection on both sides, so a row band samples the same distances on both. Rows 240..476,
row 240 is the horizon side.

| rows | p2 n | p2 orig mean (spread s1..s4) | p2 OFF - orig | p2 ANIM - orig | p3 n | p3 orig mean (spread) | p3 OFF - orig | p3 ANIM - orig |
|---|---|---|---|---|---|---|---|---|
| 225-249 | 2250 | 96.6 (0.1) | +1.8 | +1.8 | 1534 | 96.4 (0.0) | +1.9 | +1.9 |
| 250-274 | 9843 | 89.7 (0.1) | +2.4 | +2.5 | 7631 | 89.6 (0.3) | +2.3 | +2.4 |
| 275-299 | 10574 | 81.9 (0.2) | +2.6 | +2.7 | 10680 | 82.0 (0.1) | +2.7 | +2.8 |
| 300-324 | 12546 | 75.4 (0.2) | +2.9 | +3.0 | 12461 | 75.5 (0.1) | +2.9 | +3.0 |
| 325-349 | 14449 | 70.4 (0.3) | +2.8 | +3.0 | 13681 | 70.5 (0.2) | +2.8 | +3.0 |
| 350-374 | 15294 | 66.0 (0.2) | +2.7 | +2.9 | 13299 | 66.2 (0.1) | +2.6 | +2.8 |
| 375-399 | 14768 | 61.9 (0.4) | +3.1 | +3.3 | 13847 | 61.8 (0.2) | +3.2 | +3.4 |
| 400-424 | 14552 | 58.9 (0.4) | +2.8 | +3.1 | 13105 | 59.0 (0.2) | +2.5 | +2.8 |
| 425-449 | 11813 | 55.7 (0.5) | +3.1 | +3.5 | 13481 | 55.6 (0.4) | +3.2 | +3.4 |
| 450-474 | 7932 | 52.7 (0.3) | +3.4 | +3.4 | 12807 | 52.9 (0.1) | +3.1 | +3.4 |
| 475-499 | 514 | 52.4 (0.4) | +2.9 | +2.8 | 1067 | 52.2 (0.2) | +2.8 | +3.3 |
| all | 114535 | 68.2 (0.3) | +2.8 | +3.0 | 113593 | 67.0 (0.2) | +2.8 | +3.0 |

The two poses share the camera height (25.0) and pitch (up row y 0.93098 on both), and their band
profiles match to 0.3 luma. The port follows the original's falloff with distance and adds +1.8
near the horizon up to +3.4 on the near water.

Mean RGB under the eroded mask (p2, orig s1): original 65.4 / 69.8 / 66.3, OFF 69.9 / 71.8 / 70.6,
ANIM 70.1 / 71.9 / 70.8. The port adds +4.5 R, +2.0 G, +4.3 B, so its water is less green than the
original's. In the side-by-side (`warzone_p2_s2_vs_OFF.png`, raw data) the original also shows a
sharp water-to-haze edge near row 233, where the port's water fades into the haze. Both are
recorded for a later river colour step. This step does not explain either one.

## 4. Harness change: `re/frida/race_draw_burst.py`

1. `--settle-from-pose`: with `--pose`, the settle clock starts at the first pinned-camera write.
   The phase flag flips at load start, so `--settle 2` without it captured during the load
   (probe 1: 0 pose writes, camera all zero).
2. `DAT_0063ba8c` is sampled every 0.5 s through the settle and printed. The settle length is
   unchanged.
3. Challenge flow: when the N down-presses leave `DAT_0067f17c` below N, the script presses again
   until it reaches N (at most 3N + 6 presses), then exits non-zero if it is still not N. Before,
   a missed cursor captured TRAINING and reported PASS. Probes 1 and 2 here did exactly that
   (`challenge index 0x67f17c=0 (wanted 11)`, `ORIGINAL TRACK = TRAINING`), because the private root
   lacked the unlocked save. Probe 3 aborted on the new check, and the save swap fixed the runs.

## 5. What this changes

- The Warzone river residual is +2.8 (OFF) / +3.0 (ANIM) luma, flat across distance, on matched
  geometry. Warzone now sits next to Training and Sands. The L-42 flip needs no revisit.
- A future river colour step has a reference: these captures (state 3, identity alignment) plus
  the RGB split above.
- Original-side Warzone captures with a pinned pose: use `--settle-from-pose` and keep the settle
  under 7 s, or the standings overlay begins.

## 6. Raw data (not committed), SHA-256

`D:/MashedWT/HB-WARZONE-RIVER-CAP_data/` (full list in `SHA256SUMS.txt`, this folder):

```
4b2ee6434f99130d8f2024fc618799544a018b6159bd4f3731321f02a40619ac *orig/warzone_p2_s1/orig.bmp
af7f7540ef1baa961d6f2fd1cbcaa4de11de92a1178d9eb388829c052cb7abb9 *orig/warzone_p2_s2/orig.bmp
a1e652207c4ab4ca5db0658d96a9c1771ff1569941dc83739fa89495af48dd57 *orig/warzone_p2_s3/orig.bmp
2385178b844dfc212464a5b8ab718108bbb5c20b0255e414948191ebf9228035 *orig/warzone_p2_s4/orig.bmp
e616888dee6ae58986811e6a51aa8dfa90f957f08d8499b534150f93bb849574 *orig/warzone_p2_s8/orig.bmp
138977c0acf445bf0b33aaebe34481fc537e6026d5ab81331e1722d5c2914a2b *orig/warzone_p3_s2/orig.bmp
c59426c565f6554198b4f83be968cf0f409ff0e616cdb0e5eae95d19834ef6ce *orig/warzone_p3_s4/orig.bmp
686833aa5aca4afc84a475406ce59b641889fb9deb7fa803932fd1241547cc8a *warzone_p2_s2_vs_OFF.png
ea88320da551c455ddc975509338e02c3634537b675bba64e497647bd74f8ec9 *warzone_p3_s2_vs_OFF.png
```

Port inputs (HB-TEXANIM-REFS, unchanged, hashes in `verify/hbwarzoneriver_20261028/RESULT_HB-WARZONE-RIVER.md`
section 7): `D:/MashedWT/HB-TEXANIM-REFS_data/render2/warzone_p{2,3}_{OFF,ANIM,MASK}/race1/01_grid.bmp`.
`*_vs_OFF.png` = new original | port OFF, unmodified.
