# PREREG rpct: the exe's race_pct readers load the slot 0x008a96ec, as 0x00408ad0 does (2026-10-22)

Written before any code change. Branch `work/rpct` from `492c4c8c1` (dcmix + r7keep merged). Opened on Mariano's go after
`verify/dcmix_20261022/RESULT_dcmix.md` decision 1. Survey (worker, read-only) summarised in section 2.

## 1. The finding

`FUN_00408ad0` is a load: `mov eax,[esp+4]; imul eax,eax,0x30c; fld dword [eax+0x8a96ec]` (`0x00408ad0..0x00408ada`). The exe
has two readers that compute a stand-in instead, `fmod(race_[car].progress, n) / n * 100` on the gate count:
`TrackRenderer::RacePct` (used by `ProgBehind` and the AI step dump's `racepct` column) and the inline copy in
`FillRaceCamCars` (`RaceCamCar::race_pct`, read only by `RaceCamera::EliminationCheck`). The one body of the load,
`Frontend/SmallLeaves_t1.cpp:RaceScoreFloatGetBySlot`, is compiled into both targets and called by neither exe reader.

## 2. What the default build already does (survey, cited in the worker output saved as `survey.txt`)

- The slot is written on default by the verbatim `FUN_00408610` step on the live row (`PathProg_FrameTick`), because
  `Core/FaithDefaults.cpp` seeds `MASHED_PATHPROG` and `MASHED_PATHPROG_LATE`; the bridge and arm R stand down. u9779
  (`verify/u9779_20261021/RESULT_u9779.md`) measured that slot bit-equal to the original's over k 0..219 and at the kill.
  (`RESULT_dcmix.md` section 6.1 named the bridge as the default writer: out of date, corrected by this lane's result.)
- `EliminationCheck` reads `race_pct` only after the zoom gate (`required_zoom == 10.0`): it decides WHICH car of the
  `MostSeparatedPair` pair dies (the car behind, with the 80 / 20 wrap), never WHEN. Camera pose and pair choice do not read it.
- `ProgBehind` runs only with team play, which recipe F does not set.
- Frame order matches the original: the elimination reads the previous frame's slot in both (UpdateRace before the late tick;
  `0x004112cd` before `0x004112d5`).

**Pre-gate (zero code, exe `dc230217`, `H_ET-1` with `MASHED_ELIM_TRACE`, hashes = the G-CTRL reference):** 18 racing rows at
zoom 10 over 6 rounds; on 5 of them some active+alive pair orders differently under the slot than under the stand-in
(`pregate_H_ET-1.txt`; pairs (0,1) x2, (1,2), (1,3) x3, (2,3)). The trace does not carry the pair, so this bounds the effect on
victims from above and does not predict it.

## 3. Change under test

Knob `MASHED_RACEPCT_SLOT` (default **OFF**). ON: `TrackRenderer::RacePct(car)` and `FillRaceCamCars`'s `race_pct` return
`RaceScoreFloatGetBySlot(car)` (the shared load). OFF: unchanged. No other reader moves.

## 4. Predictions

| id | arm | prediction | fails if |
|---|---|---|---|
| P0 | knob OFF, recipe F | every reference unchanged: G-CTRL `c3bfcef3 / 4ed4849b / 4c3548a1` x2, B `4985eb3b / b6e3d073 / 6717c15e`, (b) 9/9, (e) 3/3 same launch, D2 3/3, 13 tracks identical to dcmix | any differs |
| P1 | knob ON + trace (`H_RSET-1`) | on every trace row `pct<i> == o_pct<i>` bit for bit (the wiring loads the slot) | any row differs |
| P2 | knob ON, recipe F, step dump | the step dump differs from the reference in the `racepct` column; whether any other column moves depends on the victims (P3) | (reported, not pass / fail) |
| P3 | knob ON vs OFF, victims | REPORT: the per-round victim sequence (car, frame) from the traces of `H_ET-1` and `H_RSET-1`. Round 1 is scored against the original's capture `D:/MashedWT/u9779_data/arms/o_ep_1.msd.elim.csv` (victim car 0, frame 1053) | round 1 victim differs from the original under knob ON |
| P4 | knob ON, (b) / (e) / D2 / 13 tracks | (b) 9/9, (e) 3/3 same launch values, D2 3/3, 13 tracks table identical | any differs |

## 5. Decision rule

- P0, P1, P4 hold and the victims are identical ON vs OFF -> the knob is value-faithful with a step-dump-column-only effect:
  recommend flip ON with the G-CTRL step hash rebased (new hash recorded), Mariano decides.
- P0, P1, P4 hold and some victim changes -> report the rounds that change. Round 1 is scored against the original. Rounds 2+
  have no original counterpart on this recipe (the original's round ends at k 220, u9779), so no flip recommendation for
  them without a new original capture.
- P0 or P1 fails -> stop and report.
