# RESULT kfdot (2026-10-22): 0x00532b80 RpHAnimStdKeyFrameInterpolate

Branch `work/kfdot`, base `6ddf1e1d5`. Not merged, default NOT flipped. Predictions: `PREREG_kfdot.md` (committed before any
`MASHED_KF_INTERP=1` run). Evidence off git: `D:/MashedWT/kfdot_data`. Final exe `59a88a51`, .asi `a1d8ef82`.

## 1. Verdict

**Every prediction held.** The one body (`Track/KfInterpolate.cpp`) equals the original bit for bit in-process. With the knob ON
nothing measurable moves on any gate or any of the 13 tracks, including Dump: the draws and the rendered frames are byte-identical.
The reason: every natural Dump call takes the plain-lerp branch (dot >= 0.999), where the dot is only compared, so neither
departure of the old copy (dot order, `std::sqrt`) can change an output unless a dot lands on the 0.999 or 0 boundary.
**Recommendation: flip the default ON and delete the legacy branch** (no opt-out needed: it gives the same bytes on all data
seen). That closes `DUP-IN-TARGET:00532b80` (allowlist 9 -> 8 findings once the line is removed).

## 2. Findings beyond the kickoff

- The Dump copy departed from the listing TWICE, not once: the reversed dot order and `std::sqrt` where the original calls the
  RW LUT root `FUN_004c3b30` (`0x00532dc9`; `0x004c3b40..0x004c3b79` reads the table at `*DAT_007d3ff8 + DAT_007d3ffc`).
- The registration comment claimed Dump's `SetClumpAnim(LINEAR, 0, 3)` gets type 0. `dump/COURSE.LUA:133` declares
  `local LINEAR = 1` and the port resolves locals, so type 1 (`LinearAnimMatrix`, `FUN_004b47e0`). Comment corrected. Only Dump
  has a live `SetClumpAnim` among the 13 `COURSE.LUA` files.
- Lint false negative: with `LinearAnimMatrix` calling the shared body under the knob, the scanner treats it as a call-through
  and no longer counts `0x00532b80`, although the legacy code still runs by default. The allowlist line is kept until the flip.
- The clump-anim matrices feed only the prop instance matrix (render). G-CTRL, B and D2 run on Training (no live SetClumpAnim).

## 3. P1: in-process A/B on the original (hooks live, private root `D:/MashedWT/kfdot_root`)

`ab_leaf.sh` (`MASHED_LEAF_SELFTEST=1`, `scenario_launch.py --hooks 0x00532b80 --cars 4`): per call, original twice through a
trampoline (`noo`), then the shared body (`ndiff`); out's q / t and b's 9 dwords compared.

| run | calls | noo / ndiff | branches |
|---|---|---|---|
| Dump `--track 10`, 60 s | 3328 | 0 / 0 | all lerp (dot >= 0.999), no negative dot |
| Warzone `--track 11`, 60 s, `SWEEP=1` | 4 (boat) | 0 / 0 | 2 lerp, 2 through the acos |
| sweep (synthetic) | 48 (12 dots x 4 times: 0.99995 .. -0.95, tiny, 0, 0.5 edge) | 0 / 0 | every branch |

Track numbers from `scenario_launch.py:4137` (10 = Dump, 11 = Warzone).

## 4. Gates (exe `59a88a51`)

| gate | P0 knob OFF | P2 / P3 knob ON |
|---|---|---|
| G-CTRL | `H_DEF-1`, `H_DEF-2`: `bacfc747 / 8dd1c004 / 6f9932b6` | `H_KF-1`: same |
| B K0 / K1 / K26 | `f9deb9b9 / d3601fcc / 9da9a26a` | same |
| (b) | 9/9 | 9/9 |
| (e) | 3/3, launch 1425.7 / 2052.5 / 2055.0 | same |
| D2 (alone) | PASS 3/3 (drvmed 1965.35 x3) | `D2_KF_1` PASS (drvmed 1964.25, a value dcstream's OFF runs also gave) |
| 13 tracks | table identical to dcstream's, 878 rows, 0 RED, gates PASS | table identical to OFF; all 39 port draw dumps and 39 frames byte-identical to OFF |

The diff reports differ OFF vs ON only in the example draw quoted for an original-only (A-only) group, i.e. the listing order of
the original capture's side, which the exe cannot change. Files: `arm_hashes.txt`, `vsum_b_{DEF,KF}.txt`, `e_check_{DEF,KF}.txt`,
`score_d2.txt`, `tracks_{off,kf}_score.txt`.

## 5. Trackers (proposal, not applied)

`0x00532b80` C2 -> C3: one body `Track/KfInterpolate.cpp`, in-process A/B 3332 natural calls + 48 sweep cases 0 diff. `file` =
`exe_file` = that path. C4 not proposed until the flip makes the exe's Dump path execute it by default.

## 6. For Mariano

1. Flip `MASHED_KF_INTERP` default ON and delete the legacy branch in `LinearAnimMatrix` (recommended; zero measured cost)?
2. Apply the C2 -> C3 tracker row?
3. Merge `work/kfdot`?

## 7. Flip (Mariano 2026-10-23: flip, delete the legacy branch, C2 -> C3, merge)

`LinearAnimMatrix` now always calls `Track::KfInterpolateFields`; the knob `MASHED_KF_INTERP` and the reversed-dot /
`std::sqrt` copy are deleted (no opt-out). Allowlist line `DUP-IN-TARGET:00532b80` removed: 9 -> 8 findings, lint 0 NEW.
Final exe `d70eb347`:

| gate | result |
|---|---|
| G-CTRL x2 | `bacfc747 / 8dd1c004 / 6f9932b6` (`arm_hashes_flip.txt`) |
| B K0 / K1 / K26 | `f9deb9b9 / d3601fcc / 9da9a26a` |
| (b) / (e) | 9/9 (`vsum_b_flip.txt`) / 3/3, launch 1425.7 / 2052.5 / 2055.0 (`e_check_flip.txt`) |
| D2, 3 runs alone | PASS 3/3, drvmed 1965.35 x3 (`score_d2_flip.txt`) |
| 13 tracks | table identical (`tracks_flip_score.txt`), all 78 draw dumps + frames byte-identical to the OFF run |

Trackers applied (`apply_trackers.py`): `0x00532b80` C2 -> C3; the `0x00532b80` half of U-9741 / D-11079 marked resolved,
the `0x004c4680` half stays open.

**Follow-up seen, not worked:** the copter sampler's acos (`AnimAcos`, `CopterAnimMatrix`, `FUN_004752f0` via `FUN_00546e70`)
still takes `std::sqrt` where the original calls the LUT root `FUN_004c3b30` (`0x0054702b`, `0x005470cf`). Render only.
