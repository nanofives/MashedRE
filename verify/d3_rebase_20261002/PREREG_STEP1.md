# STEP 1 PRE-REGISTRATION — D3 re-baseline on the post-attempt-20 build. **UNRUN.**

Written and committed **before** any capture in this directory is taken.
Branch `race/first-frame-parity`, HEAD at write time `1cbd4678` (STEP 0).

## Why this step exists

D2 attempt 20 (`015537a2`) changed two things that apply to **every** car:

1. `ProduceTerrainBatch`'s admission test went **plane-distance → spatial**
   (`mashedmod/src/mashed_re/Collision/ContactProducer.cpp:67`,
   `MASHED_D2_BATCHMODE` A/B at `:106`).
2. The substep budget became **integer**, so the port runs **2** substeps per frame
   instead of 3 (`mashedmod/src/mashed_re/Vehicle/VehiclePhysicsRun.cpp:918-922` and
   `:1076`, commit `015537a2`).

`ProduceTerrainBatch` is called **per car** at `VehiclePhysicsRun.cpp:1007`, so AI slots
1..3 are inside the blast radius. **Every attempt-20 run was `participants=1`** — the
AI-side magnitude of both changes is `[UNCERTAIN]` and has never been measured.
(`re/NEXT_SESSION.md` item 7 as of `015537a2`.)

## Attribution limit, stated before the numbers exist

D3's last (b)/(e) scores were taken **2026-09-29** at `9573f3a3` / `d165e6b4`
(`verify/d3_force_20260929/e_b_check.txt`). Since then **30 commits touching
`mashedmod/src`** have landed (`git log d165e6b4..HEAD -- mashedmod/src`), spanning D2
attempts 12-20 **and** unrelated render work (car brightness, car grey chassis, Arctic sea
tile, pickups). Therefore:

> **A HEAD-minus-2026-09-29 delta is a 3-day, 30-commit delta. It is NOT "what attempt 20
> moved".** It will be labelled as such in the RESULT, and no part of it is attributed to
> attempt 20 except through gate **G1-ATTRIB** below.

## Bands: UNCHANGED

- (b) bands live in `re/tools/ai_ctrl_window.py`.
- (e) `REFERENCE` / `BAND_PCT` / `GATED` live in `re/tools/ai_speed_env.py`.

**Neither file is edited in this step.** `git diff --stat` on both is printed in the
RESULT as proof.

## Arms

Standalone recipe, identical across arms except the named knob
(`verify/d3_force_20260929/PROVENANCE.txt`, with `MASHED_WIN_POS` updated to the current
`primary-bl` per memory `game-window-on-left-monitor`):

```
py -3.12 re/tools/sa_capture.py verify/d3_rebase_20261002/<tag> 8,30,60 \
    MASHED_MUTE=1 MASHED_TRACK_VIEW=Training MASHED_CAR=1 MASHED_ROUND=1 \
    MASHED_WIN_POS=primary-bl MASHED_TITLE="D3 rebase <tag>" \
    MASHED_AI_STEPDUMP=verify/d3_rebase_20261002/<tag>.csv
```

| tag | arm |
|---|---|
| `r1` `r2` `r3` | default build at HEAD, three repeats |
| `p1` | HEAD + `MASHED_D2_BATCHMODE=plane` |

`p1` reverts attempt 20's **admission test only**. **The substep change has no revert
knob** — `015537a2` landed it unconditionally — so its share is **not separable in this
step**, and the RESULT will say so rather than splitting it.

## Gates, registered before running

- **G1-DET** — `r1`/`r2`/`r3` must agree to **every printed digit** on all (e) and (b)
  lines. If they do not, the recipe is non-deterministic at HEAD, every number below is a
  sample and not a measurement, and I **STOP** and report that instead of scoring.
- **G1-BOOT** — a run whose capture is missing, or reports `speed=0.00`, or is short, is
  **retried once** before being called a failure (attempt 20's registered boot-retry rule,
  `verify/d2_wheelstate_20261002/PREREG_STEP34.md` §3). A second failure is reported as a
  failure, not retried again.
- **G1-E** — (e) is **MET** iff both gated stats (`launch`, `ft_median_m0`) are inside
  their band on **all three** cars **and** `regime0=1` on every car. A car with
  `regime0=0` is re-taken, not scored (§D3's regime condition).
- **G1-B** — (b) is **MET** iff all 10 bands pass on **all three** cars.
- **G1-DELTA** — report, per car, HEAD minus the 2026-09-29 `sa_b2` value for every gated
  (e) value and every (b) metric, labelled **"3-day / 30-commit delta"**.
- **G1-ATTRIB** — attempt 20's admission test is credited with moving a number **only**
  where `p1` differs from `r1` on that same number. Where `p1 == r1`, the admission test
  moved it by **0** for the AI cars and that is what gets written. `p1` is **not** expected
  to reproduce 2026-09-29 (it still carries the substep change and 29 other commits); if it
  does, that is reported as an observation, not assumed in advance.

## Guards (recipes unchanged from `verify/d3_force_20260929/guards.txt`)

- **Powerups:** `pwsh re/tools/pu_replay/sweep.ps1`. Expect **11/11 decision CLEAN**,
  contact CLEAN on 10 of 11 with `g3` the known R_FLAME 2-of-546 residue.
- **Modes:** `py -3.12 re/frida/scenario_launch.py --track 0 --mode 10 --cars 4 --car 0
  --poke-ctrl-slots --oracle --rule 3 --hold 50`. Expect **ORACLE VERDICT GREEN**,
  MISMATCH 0.

## D2 WATCH

`MASHED_D2_BATCHMODE`, the contact collector and the substep/chunk loop are all **D2
re-pickup triggers** (`DEFERRED.md` D-11071). Any `p1`-vs-`r1` difference is reported as a
**"D2 REOPEN CANDIDATE"** row with evidence in the RESULT, and **no D2 code is changed in
this session**.

## Out of scope for this step

- `VehiclePhysicsRun.cpp:702`'s fitted AI start seed (`+0xbf8 = 1`, `+0xbf4 = 1300`) is
  **not touched**. Replacing it is a separately pre-registered step and only if the AI's
  own pre-race input is proven to drive it on the **running original**.
- No port. No C-level moves. No band edits.

## PID hygiene

Every `mashed_re.exe` is spawned and killed **by PID** by `sa_capture.py`; every
`MASHED.exe` by `scenario_launch.py`. No blanket kill by name.
