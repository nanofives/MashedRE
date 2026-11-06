# PREREG MATCH-T13-SEED (2026-11-02)

Written and committed before any arm of this step runs.

## Question

`FUN_004046a0` (0x004046a0, rule-10 seed) computes `f = _DAT_005cc320 - FUN_004173a0() * _DAT_005cc56c`
(= `1.0 - DAT_0089a360 * 0.1`) and seeds the rule-10 countdown `DAT_007f0fe4` and the checkpoint table from `f` on
course ids 0xb / 0x1a / 0x27 (`FUN_00426c00` = `DAT_00644158`). Every other course takes the default branch
(`DAT_007f0fe4 = 30.0`, no `f`). The port (`Race::RuleEngine::Rule10Seed`, `Race/RuleEngine.cpp`) gets a literal
`0.f` for the difficulty at both call sites (`TrackRenderer::SetRaceRule` and the per-round reseed in
`TrackRenderer.cpp`), so `f = 1.0`. The exe holds `DAT_0089a360 = 2.5f` after the AI race reset
(`TrackRenderer.cpp`, `Ai::F32(0x0089a360u) = 2.5f`), so the matched body `FUN_004173a0` would give `f = 0.75`.
MATCH-TOPDOWN-T13 left 0x004173a0 unswapped for this reason
(`verify/match_topdown_20261101/RESULT_MATCH-TOPDOWN-T13.md`).

Which `f` does the original use on those three courses, at each `FUN_004046a0` call (two race-setup calls and one
per segment end, `verify/d3_modes_20260926/rules_oracle_r10seed2.json`)?

## Change under test (default OFF)

- The matched source `re/match/s03/h1_004173a0.c` becomes the one body of 0x004173a0 in **both** targets
  (`Match/MatchS03_004173a0.cpp`, `Float89a360Get`), moved out of the asi-only `Util/PromoLoop_round35.cpp`. The
  asi install `RH_ScopedInstall(Float89a360Get, 0x004173a0)` moves with it.
- Knob `MASHED_RULE10_SEED_DIFF=1`: both `Rule10Seed` call sites pass `Float89a360Get()` instead of `0.f`.
  Knob unset (default): `0.f`, unchanged.
- Log only: `MASHED_RULE10_SEED_LOG=1` (or the knob) writes one `[rule10-seed]` line per call to `mashed_re.log`
  (site, course id, difficulty raw hex, timer, bonus, both as float and raw hex). No other effect.

## Arms

Original (scenario_launch, `--mode 3 --cars 4 --rule 10 --oracle --hold 40`, private root
`D:\MashedWT\MATCH-T13-SEED_root`, lock-wrapped). The oracle's `FUN_004046a0` record is extended (read-only) with:
entry `DAT_00644158` (course id), entry `DAT_0089a360` (raw u32), exit `DAT_007f0fe4` (raw u32 and float), exit
`DAT_005ea0ac`, and the raw 0x90 bytes at 0x00636b80 (checkpoint table) at exit.

| arm | track arg | expected course id |
|---|---|---|
| O_SANDS | `--track 5` | 0xb |
| O_CITY | `--track 12` | 0x1a |
| O_NEU | `--track 2` | 0x27 |
| O_TRAIN (control) | `--track 0` | default branch (not 0xb / 0x1a / 0x27) |

Port (`mashed_re.exe` of this branch, `MASHED_ROOT` = the private root, base env of
`verify/savelink_20261023/run_arms.py` with `MASHED_ROUND_RULE=10`, `MASHED_DET_FRAMES=900`,
`MASHED_RULE10_SEED_LOG=1`):

| arm | `MASHED_TRACK_VIEW` | knob |
|---|---|---|
| P_OFF_SANDS / P_ON_SANDS | `sands` (course 11) | off / `MASHED_RULE10_SEED_DIFF=1` |
| P_OFF_CITY / P_ON_CITY | `City` (course 26) | off / on |
| P_OFF_NEU / P_ON_NEU | `Neustein` (course 39) | off / on |

Default gates on the knob-OFF build: `verify/savelink_20261023/run_arms.py H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.

## Predictions

H1 (factor 0.75, the matched body): in every O arm on 0xb / 0x1a / 0x27, `DAT_0089a360` at `FUN_004046a0` entry
reads `0x40200000` (2.5) and the exit timer is 16.75 / 19.25 / 18.25.
H0 (factor 1.0, the port today): `DAT_0089a360` reads 0 at entry and the timer is 17.5 / 20.0 / 19.0.
A mix (some calls 2.5, some 0, for example the first setup call before the AI reset) is reported per call. It is
neither H1 nor H0.

O_TRAIN: timer 30.0 at every call whatever `DAT_0089a360` holds (control: the default branch ignores `f`).

Port: P_OFF timers 17.5 / 20.0 / 19.0 on every call. P_ON timers equal the original's per call when the exe's
`0x0089a360` holds the original's entry value at that call. A P_ON call that reads a different `DAT_0089a360`
than the original's call of the same index is a call-order residual and is reported, not passed.

Gates: G-CTRL `8284be46 / ba407a16 / 31fe043e`, B `1d5bf4f9` x3 (the `gctrl.ref` / `b.ref` values in
`re/orchestrator/merge_train.json` on this branch's base) on the knob-OFF build. Any moved hash = RED for the step.

## Scoring

- Equality is by raw float bits (`DAT_007f0fe4`, the bonus). A 1-ulp difference between O and P_ON is reported as
  a residual, not counted as agreement.
- The result goes to a user decision row (flip `MASHED_RULE10_SEED_DIFF` to default ON, or keep `0.f`). This step
  never flips the default.
- Piz-collision crashes (`MASHED.exe+0x1509b4`, `+0xe66db`, U-9677) are re-runs.
- If the original arm cannot reach rule 10 on a course (no `FUN_004046a0` exit with that course id), that course is
  reported as not measured. It is not scored from the decomp.
