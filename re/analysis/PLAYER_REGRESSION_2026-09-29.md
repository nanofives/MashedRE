# U-9141 / U-9145 — the PLAYER-car physics regression since D2 closed

Session 2026-09-29, branch `race/first-frame-parity`, starting from `64095a44`.

**This section (§1 and §2) is written and committed BEFORE the first run of this session.**
Nothing in it changes after the first number lands. §3 onward is filled in afterwards.

---

## 1. The inherited facts (not re-derived here)

From `re/analysis/D2_CONTROLLED_ARM_2026-09-29.md` (pre-registration `1d0ca916`, results
`4938adba`) and `re/analysis/D3_DRIVE_FORCE_2026-09-29.md` (`9573f3a3..64095a44`):

- **The controlled D2 arm** `C-A8-1` = the `a8` held-full-lock recipe
  (`re/tools/statediff/a8_run_port.py`) + `MASHED_MEASURE_NOOPP=1` + `--max-lines 1080` on
  both reducers. It is **deterministic**: run-to-run spread on `slip 1500-2000` is **exactly
  0** on 3/3 runs at both ends (note §3.3). One run per commit is therefore decisive.
- On that arm, **`56ad3806` (the D2 close) gives `0.1916 / 0.2669 / 1932.09`** and the
  ORIGINAL's archived capture gives `0.1913 / 0.2498 / 1940.59`. Two of the three plus `av.y`
  reproduce the ROADMAP §D2 row to four decimals.
- **HEAD gives `0.1609 / 0.2296 / 691.01`** — `-16.0% / -14.0% / -64.2%`. Criterion C2 of the
  pre-registered rule FAILED, and the failure is a real HEAD-vs-`56ad3806` difference in the
  **player's own trace** that the uncontrolled recipe was masking.
- The 2026-09-28 conclusion *"no commit in `56ad3806..HEAD` edits the player's solver,
  therefore the drift is the instrument"* does **not** survive a controlled instrument. The
  bisect has to be redone on the controlled arm. That is task 1 here.
- **U-9145 (U-D2-OPPONENT-COUPLING)**: with the opponents present, the same HEAD build gives
  the player a driving-median of **2538** instead of 691 (3.7x), even though
  `VehicleCarCarContact` (`0x00469df0`, `Collision/CarCarContacts.cpp`) has **zero callers**
  anywhere in the port. The coupling is shared mutable state and is unidentified. That is
  task 3 here.

Both directions of the regression matter and they are not the same question: 691 is the
player alone at HEAD, 2538 is the player at HEAD with three opponents, 1932 is the player
alone at `56ad3806`, and 1940.59 is the ORIGINAL (which has three opponents).

## 1.1 The commits in range

`git log --oneline 56ad3806..HEAD -- mashedmod/` = **24** commits (60 in total; 36 touch no
build input). Newest first:

```
1d0ca916 U-9141 step 0: the D2 controlled arm and its PASS RULE          <- the knob itself
d165e6b4 re-classify: U-D3-DRIVE-FORCE
9573f3a3 U-D3-DRIVE-FORCE: the missing A6a start boost                   <- + Fi_GameMode 0->6
b5923d20 U-D3-DRIVE: criterion (e) FAILS, and the settle costs AI (b)
83a7b6ea U-D3-DRIVE: the spawn settle                                    <- candidate
a9da810a standalone: remove the placeholder free-fly camera; MASHED_TITLE
2e7a2b92 D3 powerups: port the dispatcher's BOX-STATE gate
079e2106 U-9138 re-check: PASS
c5bcf71c U-9138 FIXED: Vehicle::Rw_TransformPoints binds to the VECTORS form  <- candidate
e2dce8f1 D3 contact (c): MISSILE ported
bd69b3d8 D3 contact (c): MORTAR's own chain
7f6b44d3 D3 contact (c): port the shared target acquisition
f2afeb8b D3 contact (c): R_FLAME ported
5bb0d5e3 D3 contact (c): DRUM clean
7b1ba43a D3 contact (c): SHOTGUN clean
c20ca0ae D3 contact (c): port the OIL/P_MINE contact chain
f39747af D3 contact: bind the two Collision residuals
286d99a2 D3 AI (b): instrument the step inputs, port the RW RNG          <- candidate
4ff428ad D3 modes: oracle covers all 11 rules, APPEND fires, G-G1, G-G2  <- candidate
09a73dc6 D3 AI: port the FUN_00416250 inputs ... opponents drive         <- candidate
647a5e24 D3 powerups: measure all 9 types against the original
38dfeb17 D3 step 2: AI measured against the original
7f71e36f D3 step 1: audit of what a clean-env race actually runs
```

Named as **candidates, not assumptions**: the start-boost port (`9573f3a3`, including its
`Fi_GameMode()` 0→6, which is global and is NOT gated on `slot != 0`), the spawn settle
(`83a7b6ea`), the U-9138 `Rw_TransformPoints` binding (`c5bcf71c`, which is on the player's
own transform path), the modes / race-length change (`4ff428ad`), and the AI tick / clock
globals (`09a73dc6`, `286d99a2`). Every one is *tested*, none is presumed.

---

## 2. PRE-REGISTERED — method, classification rule and pass rule

### 2.1 The measurement, identical at every commit

```
py -3.12 re/tools/statediff/a8_run_port.py verify/player_reg_20260929/<tag> 50 \
    -MASHED_REAL_PHYSICS MASHED_MEASURE_NOOPP=1 MASHED_TITLE="player regression <tag>"
py -3.12 re/tools/statediff/a8_slip_axis.py --orig verify/a8_steer_20260824/orig_steerR.msd \
    --port verify/player_reg_20260929/<tag>/motion_diag.log --max-lines 1080
py -3.12 re/tools/statediff/a8_momentum.py  --orig verify/a8_steer_20260824/orig_steerR.msd \
    --port verify/player_reg_20260929/<tag>/motion_diag.log --max-lines 1080 \
    --orig-steer-min 33.0 --port-steer-min 0.9
```

`MASHED_MUTE=1` is in the recipe already, so every run is muted. Builds are made **in place**:
`git checkout <sha> -- mashedmod/` → `mashedmod\build.bat` → measure →
`git checkout HEAD -- mashedmod/`. **No worktree.** `original/` is never touched. Every
`mashed_re.exe` PID is spawned and killed by `a8_run_port.py`, by PID.

### 2.2 The knob at commits that predate it — stated now, not discovered later

`MASHED_MEASURE_NOOPP` was added at `1d0ca916` (`D3d9Render/TrackRenderer.cpp`, 30 lines
around the per-opponent update loop). At any commit older than that the checked-out tree has
no knob, so:

- **At `09a73dc6` and newer** the knob is re-applied as a **temporary local patch** to the
  checked-out `TrackRenderer.cpp` — the identical two-line substitution `1d0ca916` makes
  (`const int aiCarN = s_measureNoOpp ? 0 : (int)ai_cars_.size();` and the loop bound), with
  no other edit. The patch is reverted by the `git checkout HEAD -- mashedmod/` restore. Each
  commit where this was done is recorded in §3.
- **Before `09a73dc6`** the opponents never enter the physics/collision chain at all
  (`D3_DRIVE_2026-09-28.md` §3.4, last row: *"no knob needed — opponents never entered the
  chain"*), so the knob is a no-op there and is **not** applied. This is the same asymmetry
  the `56ad3806` end of `D2_CONTROLLED_ARM_2026-09-29.md` §2.1 already carries.

At `09a73dc6` itself both treatments are available, so **that commit is measured BOTH ways**
(patched knob ON, and unpatched) as the control that the patch is equivalent to the knob.

### 2.3 Classification rule — fixed here, before any run

Per commit, one run. Two statistics decide, and they must agree:

| verdict | `slip 1500-2000` | driving-median |
|---|---|---|
| **GOOD** (reproduces `56ad3806`) | ≥ 0.185 | ≥ 1700 |
| **BAD** (reproduces HEAD) | ≤ 0.170 | ≤ 1000 |
| **INDETERMINATE** | anything else, or the two disagree | |

The slip thresholds are `D2_CONTROLLED_ARM_2026-09-29.md` §3.5's own pre-registered
`≥ 0.185 / ≤ 0.170`, carried verbatim. The median thresholds are the matching split of the
measured 1932 vs 691 with the same proportional margin. An INDETERMINATE commit gets a second
run and, if still INDETERMINATE, is reported as a **partial** mover with its numbers rather
than forced into a bucket.

**Discard rule**, carried from the arm: any run producing fewer than 1080 parsed frames is a
truncated boot, not a sample. It is discarded, re-run, and the discard is reported.

**Search**: binary over the 24 commits. Because the regression may be the sum of more than one
commit, after the first-bad is found the bisect **continues** over `[first-bad .. HEAD]`
whenever the first-bad's numbers differ from HEAD's by more than the C1 bound, until every
transition is attributed.

### 2.4 The PASS RULE for the fix — the criteria of `1d0ca916`, unchanged

The fix is accepted only if the controlled arm at HEAD+fix satisfies the three criteria
pre-registered at `1d0ca916` §2.3, with the **same bounds**, none moved:

| # | criterion | bound |
|---|---|---|
| **C1** | instrument determinism — 3 runs at HEAD+fix agree on `slip 1500-2000` | ±0.005 absolute |
| **C2** | cross-commit agreement — HEAD+fix vs `56ad3806`'s controlled arm (`0.1916 / 0.2669 / 1932.09`) on all three statistics | ±2% |
| **C3** | D2 reproduction — `56ad3806`'s controlled arm vs the ROADMAP §D2 row (`0.1916 / 0.2668 / 1887`) | ±2%, **already measured at `4938adba` and NOT re-run**: PASS / PASS / +2.39% FAIL. Unchanged by this session. |

**C3 is inherited, not re-litigated.** Its one miss (driving-median +2.39%) is a recorded-value
question about the D2 row's `1887`, already flagged in the ROADMAP §D2 amendment, and
re-baselining it is a user decision. This session does not move it.

**And the overriding rule, stated above C2: the fix must make the port match the ORIGINAL, not
merely `56ad3806`.** For every commit the bisect indicts, §4 establishes from the original
binary (RVAs) and from the original's own archived D2 capture
(`verify/a8_steer_20260824/orig_steerR.msd`, `0.1913 / 0.2498 / 1940.59`) which of the two
behaviours is faithful. **If `56ad3806` is itself wrong somewhere, that is said in §4 and the
port is moved to the original's behaviour, not back to `56ad3806`'s** — in which case C2 is
reported as measured and the deviation is named, rather than the bound being bent.

### 2.5 U-9145 — the coupling hunt, method fixed here

Per-global A/B at HEAD on the controlled arm, one temporary diag at a time, removed after,
following `D2_CONTROLLED_ARM_2026-09-29.md` §3.5's next-command list. A candidate is the
coupling only if forcing its opponents-present value with the opponents OFF returns the
player's trace to the opponents-ON values. Named candidates, each already cited:
`g_torqueRingPhase` (`DAT_007f101c`), advanced `(phase + 1) & 0xf` once per A4 call and used
to index the steer ring `+0x1ac + phase*4` / `+0x270 + phase*4` (`0x00470670`);
`Collision::g_suspScratch` (`DAT_00881560`); the powerup dispatcher / pickup field. Any
further candidate found by search is added with its RVA before being tested.

### 2.6 Guards that must be re-run after the fix

Fixed here so they cannot be chosen afterwards:

| guard | requirement |
|---|---|
| criterion (e) — `re/tools/ai_speed_env.py --check` | must stay **PASS 3/3** (it is at 0.05% today) |
| AI (b) — `re/tools/ai_ctrl_window.py --check` | **reported as-is**, not gated |
| power-ups — `re/tools/pu_replay/sweep.ps1` | decision **CLEAN**; contact CLEAN except the known `g3` R_FLAME residue |
| modes oracle — one rule (rule 3) | **GREEN**, MISMATCH 0 |
| `mashedmod\build.bat` | both targets clean |

---

## 3. MEASURED — the bisect, and the correction it forces

Nothing above this line changed after the first run. Evidence `verify/player_reg_20260929/`.

### 3.1 The bisect as pre-registered — first-bad is `09a73dc6`

Reproduction control first: HEAD, controlled arm as §2.1, on the build this session started
from. `0.1609 / 0.2296 / 682.24` against `4938adba`'s `0.1609 / 0.2296 / 691.01` — the two
slip statistics to four decimals, the median -1.3%. The arm reproduces.

| # | commit | knob | lines | slip 1500-2000 | slip 2000-2600 | driving-median | verdict |
|---|---|---|---:|---:|---:|---:|---|
| — | `56ad3806` (D2 close) | none, as §2.2 | 1085 | 0.1916 | 0.2668 | 1932.09 | GOOD (inherited) |
| 2 | `38dfeb17` | none | 1085 | **0.1916** | **0.2669** | **1932.37** | **GOOD** |
| 3 | `647a5e24` | none | 1086 | **0.1916** | **0.2668** | **1931.36** | **GOOD** |
| 4 | `09a73dc6` | patched | 1266 | **0.1718** | **0.2372** | **697.36** | **BAD** |
| 6 | `286d99a2` | patched | 1266 | 0.1726 | 0.2372 | 692.07 | BAD |
| 12 | `7f6b44d3` | patched | 1265 | 0.1726 | 0.2372 | 692.07 | BAD |
| 23 | HEAD | committed knob | 1268 | 0.1609 | 0.2296 | 682.24 | BAD |

Read literally, that names `09a73dc6` — the commit where the opponents begin to drive from
the ctrl bytes — as the first bad commit. **It is wrong**, and §3.2 is why.

One discard, reported per the rule: `286d99a2`'s first two boots produced no
`motion_diag.log` at all (the run stalled in the frontend at `NAV_DEMO phase=0
00_challengeselect`, `mashed_re.log` tail). The third boot, given 90 s instead of 50 s,
exited on its own with 1266 lines. Every probe from there on uses 90 s with one automatic
retry.

### 3.2 THE CONTROL THAT INVERTS IT — the arm is not symmetric, and that is the whole -64%

§2.2 carried forward, from `D2_CONTROLLED_ARM_2026-09-29.md` §2.1/§3.5, that the knob is
applied at HEAD and **not** at `56ad3806`, where the opponents "never entered the chain".
That note also flagged the residual asymmetry — *"at `56ad3806` the opponents are not merely
un-stepped, they are moved by the pre-D3 kinematic Option B model, whereas
`MASHED_MEASURE_NOOPP=1` at HEAD leaves them parked"* — and carried it into the bisect as
something to watch. **It is not a detail. It is the entire finding.**

Two controls, each one run, each decisive because the arm's spread is 0:

| control | slip 1500-2000 | slip 2000-2600 | driving-median | lines |
|---|---:|---:|---:|---:|
| `647a5e24` (i3), **no knob** — opponents move | 0.1916 | 0.2668 | 1931.36 | 1086 |
| `647a5e24` (i3), **knob patched in** — opponents parked | **0.1726** | **0.2372** | **692.07** | 1265 |
| `56ad3806`, **no knob** (the arm as run at `4938adba`) | 0.1916 | 0.2669 | 1932.09 | 1085 |
| `56ad3806`, **knob patched in** — opponents parked | **0.1713** | **0.2372** | **705.03** | 1266 |

`647a5e24` is *the same commit* measured both ways and it moves `1931.36 → 692.07`. The knob,
not any commit, produces the collapse. And `56ad3806` — the D2 close itself, three months of
commits before any D3 work — collapses the same way, `1932.09 → 705.03`.

**So the symmetric controlled arm, knob at BOTH ends:**

| | slip 1500-2000 | slip 2000-2600 | driving-median |
|---|---:|---:|---:|
| `56ad3806`, knob | 0.1713 | 0.2372 | 705.03 |
| HEAD, knob | 0.1609 | 0.2296 | 691.01 |
| **HEAD vs `56ad3806`, symmetric** | **-6.1%** | **-3.2%** | **-2.0%** |

against the asymmetric arm's `-16.0% / -14.0% / -64.2%`.

**Corrections to the inherited record, stated rather than glossed:**

1. **`4938adba`'s C2 failure is dominated by the arm's own asymmetry, not by a player
   regression.** The driving-median's `-64.2%` is **0%** of it: `56ad3806` measured the same
   way is 705, not 1932. The `-64%` is the knob.
2. **`09a73dc6` is NOT the first bad commit.** It is only the oldest commit in the range at
   which the plan's §2.2 rule *applies the knob*. The bisect as designed could not have
   found anything else, because the treatment changes at exactly that boundary. This is the
   batch/group-attribution hazard in a different costume: the probe's own configuration
   changed with the independent variable.
3. **What survives is small and real**: on a symmetric arm, HEAD is `-6.1% / -3.2% / -2.0%`
   off `56ad3806`. Only `slip 1500-2000` is outside the ±2% C2 bound, and the driving-median
   — the statistic that looked like a 3.7x catastrophe — is inside it.
4. **U-9145 is older than D3.** The opponents move the player's median `1932 → 705` at
   `56ad3806`, where they are kinematic scaffold cars that never call
   `VehiclePhysics_StepCar` at all. So the coupling cannot be `g_torqueRingPhase`
   (`DAT_007f101c`) or `Collision::g_suspScratch` (`DAT_00881560`) — both of those need the
   opponents to run the physics chain, and at `56ad3806` they do not. Nor is it the
   power-ups: `MASHED_NO_PICKUPS=1` at HEAD gives `0.1609 / 0.2296 / 689.82`, unchanged
   (§3.3), and `56ad3806`'s demo race has no orbs at all
   (`g_track.InitPickups()` was added to that path at `09a73dc6`, `exe_main.cpp`).

### 3.3 The A/B matrix at HEAD — what the opponents actually do to the player

One run each, same build (`235e964a`), controlled arm unless stated:

| arm | opponents | AI tick | slip 1500-2000 | slip 2000-2600 | driving-median | player reseeds in 1080 |
|---|---|---|---:|---:|---:|---:|
| `MASHED_MEASURE_NOOPP=1` | **parked** | runs | 0.1609 | 0.2296 | 682.24 | **72** |
| `MASHED_MEASURE_NOOPP=1 MASHED_NO_PICKUPS=1` | parked | runs | 0.1609 | 0.2296 | 689.82 | — |
| `MASHED_GATE_RIBBON_AI=1` | **gate-ribbon scaffold** | **off** | 0.1354 | 0.2993 | **2539.80** | 23 |
| default (`4938adba` `g_d2_*`) | **ported chain** | runs | 0.1323-0.1411 | 0.2993 | **2538** | — |
| `647a5e24`, no knob | Option B scaffold | off | 0.1916 | 0.2668 | 1931.36 | **11** |
| ORIGINAL (archived) | real | real | 0.1913 | 0.2498 | 1940.59 | n/a |

The discriminator for the median is **whether the opponents move at all**, not how they are
driven: ribbon-scaffold (2539.80) and ported-chain (2538) agree, and both are 3.7x the parked
arm. And the mechanism is visible in the logs: the count of player body-basis reseeds
(`reseed=1`, set only by `VehiclePhysics_ResetOrientation(0, …)`, i.e. the player's own
off-mesh recovery at `TrackRenderer.cpp:2805-2829`) is **11** on the good arm, **23** on the
ribbon arm and **71-72** on the parked arms. With the opponents parked the player falls into a
repeated off-mesh relocate loop during the full-lock donut and never gets up to speed. The
first divergence between `647a5e24` no-knob and `09a73dc6` knob is at frame **149**, which is
a `reseed=1` frame: the 148 frames before it are byte-identical, and at 149 the speed is
identical (`sp=595.13`) while the velocity heading differs (`velH` 1.7416 vs 1.6825).

### 3.4 Where that leaves the question the session was asked

The `-64%` is an instrument artefact. But a player-side fidelity gap **does** exist and the
symmetric arm is not where to read it, because parking the opponents is not what the original
does — the original's `orig_steerR.msd` was captured with three opponents driving. On the arm
that matches the original's scenario:

| | driving-median | vs ORIGINAL 1940.59 |
|---|---:|---|
| ORIGINAL | 1940.59 | — |
| `56ad3806`, opponents driving | 1932.09 / 1931.36 / 1932.37 | **-0.4%** |
| HEAD, opponents driving | 2538 | **+31%** |
| HEAD, `MASHED_NO_START_BOOST=1` (`4938adba`) | 2488.6 / 2507.9 | +28% |
| HEAD, `MASHED_GATE_RIBBON_AI=1` | 2539.80 | +31% |

**That** is the player regression: on the original's own scenario the port's player has gone
from -0.4% to +31% on the donut median. It is not the start boost (the `NO_START_BOOST`
control is still +28%) and it is not the opponents' new drive model (the gate-ribbon arm,
which has no AI tick and no ported opponent physics, is +31% as well). §4 bisects it.

## 4. U-9145 — the coupling, found: TWO channels, neither of them a physics global

The §2.5 candidates (`g_torqueRingPhase` `DAT_007f101c`, `Collision::g_suspScratch`
`DAT_00881560`, the power-up dispatcher) are all **refuted** by §3.2/§3.3 before any of them
was A/B'd: the coupling exists at `56ad3806`, where the opponents never call
`VehiclePhysics_StepCar`, and `MASHED_NO_PICKUPS=1` changes nothing. So the search was run
on the trace instead — a temporary per-sim-step player dump (`MASHED_PLAYERTRACE=1`,
`TrackRenderer.cpp`, `%.17g` on every field) A/B'd knob-ON vs knob-OFF at the same build.

### 4.1 Channel A — the harness's steer-hold onset was on the REAL clock

`verify/player_reg_20260929/pt2_*`: the two runs are **bit-identical for 58 sim steps** and
then run **exactly one step out of phase** — the held full lock begins at sim step **60**
with the opponents updated and at sim step **59** with them parked. Everything else
(`pos`, `sp`, `vel`, `b14`, `b1c`, `+0x9e4`) matches to the last bit up to that point.

Mechanism, in the source: `di.steer` was decided **once per RENDER frame** from the real
clock (`td = t - s_drive_t0` against `MASHED_STEER_HOLD_AFTER`, `exe_main.cpp`), while the
car sim runs on a **real-time fixed-timestep accumulator** (`s_simAccum += sim_real_dt`,
`1/s_simHz` per step, 0..6 steps per render frame). So the *sim step* at which the hold began
was a function of the *render frame rate*, i.e. of CPU load — and updating three opponents is
CPU load. That one step is enough, because the donut is chaotic through the off-mesh recovery
at `TrackRenderer.cpp:2805-2829`, which **halves `car_speed_` on every trigger**: 11 vs 71
recoveries over the same 1080 frames (§3.3).

**Fixed** by counting sim steps instead of real seconds (`steerHoldApply()` called inside
both sim-step paths). Verified: bit-identity between the two arms extends from 58 steps to
**157** (`verify/player_reg_20260929/fix_*`), and the solo arm below is now deterministic to
every printed digit on 2/2 runs. Harness-only; the threshold is the same 4 s, now measured on
the clock the car is actually integrated on.

### 4.2 Channel B — the opponents get the PLAYER ELIMINATED at race time 2.6 s

With channel A closed, the remaining first divergence (`verify/player_reg_20260929/pt3_*`,
the post-`UpdateRace` dump) is a single field:

```
A (opponents parked)  post gate=2 lap=0 alive=1 prog=2.7822690 ng=94 rt=2.5999982
B (opponents driving) post gate=2 lap=0 alive=0 prog=2.7822690 ng=94 rt=2.5999982
```

`race_[0].alive` goes **false at `race_time_ = 2.60 s`** when the opponents drive. The writer
is `TrackRenderer.cpp:4349`/`:4404`, `race_cam_.EliminationCheck(cc)` — the ported
`0x00410d10` standard path (`Race/RaceCamera.cpp:499`), which fires only when the camera's
required zoom has saturated at exactly 10.0 (`0x00410ee3`, `fcomp [0x005cc55c]`) and then
kills the least-progressed of the most separated pair (`0x0040e180` at `0x00410efb`,
`0x00410fe2..0x00411014`). Three opponents racing away from a player doing a stationary donut
saturate that zoom in 2.6 s; three parked opponents never do.

**How a race-bookkeeping flag reaches the player's motion:** `UpdateRace`'s per-car `step()`
returns immediately when `!r.alive` (`TrackRenderer.cpp:4127`), so `race_[0].gate` **freezes**
— and `race_[0].gate` is the input to the player's own off-mesh re-aim,
`gates_[(race_[0].gate + 3) % n]` at `TrackRenderer.cpp:2808`. Alive, the gate tracks the car
and the re-aim churns, so the recovery loop re-fires (71 recoveries, median 691); eliminated,
the gate is pinned and the re-aim is constant, so the car escapes it (median 2520).

So the answer to U-9145 is: **the opponents do not couple into the player through any shared
physics state. They couple through (A) the frame rate into the harness's own trigger, and (B)
the camera-zoom elimination into `race_[0].alive` and from there into the player's off-mesh
re-aim.** Both are cited above; neither is `DAT_007f101c` or `DAT_00881560`.

## 5. WHICH SIDE IS FAITHFUL — measured on the ORIGINAL, and NEITHER port arm is

`verify/a8_steer_20260824/orig_steerR.msd.provenance.json` records the reference's argv:

```
re/frida/scenario_launch.py --statediff-out … --statediff-drive --statediff-drive-late
                            --statediff-steer 1 --hold 38
```

**No `--cars`.** `scenario_launch.py:1739` defaults `--cars` to **1**. So the D2 gate's
original-side reference is a **SOLO race** — one car, no opponents.

Re-run live on the original today with the identical recipe plus `--oracle --rule 0`
(`verify/player_reg_20260929/orig_elim/`, `log/rules_oracle_rule0.json`, 2336 frames,
`MASHED.exe` spawned and killed by the harness):

```
=== scenario_launch  pid=…  track=0 mode=10 cars=1 ===
SegmentCheck  0x00410d10: calls=1448 agree=1448 MISMATCH=0 segment-end(ret!=0)=0
FinishOrder   0x004177b0: calls=2446 agree=2446 MISMATCH=0 appends=0 round-resets=0
inputs seen: {'0': {'n': 1448, …, 'm0Max': 0.0244…, 'm1Max': -1, …}}
ORACLE VERDICT: GREEN
```

`m1Max = -1` — car 1 has no metric, i.e. **no second car exists** — and
`segment-end = 0` over 1448 calls, i.e. **no elimination ever runs**. The original's D2
reference was a one-car race from start to finish.

**Consequence, stated plainly.** The port side of that same gate spawns **three** opponents
(`TrackRenderer.cpp:2441-2473`, hard-coded). So:

- the ROADMAP §D2 row was measured against an original capture **with a different scenario**;
- `MASHED_MEASURE_NOOPP=1` does not fix that — it only *parks* the three, leaving them in
  `ai_cars_`, in `UpdateRace`, in `ParticipantCount()` and in the camera framing;
- **neither existing arm is the reference's scenario.**

A third harness knob, `MASHED_MEASURE_SOLO=1` (`TrackRenderer.cpp`, at the AI spawn), spawns
none, which is the reference's scenario. It is deterministic: two runs at HEAD agree to every
printed digit.

| arm | slip 1500-2000 | slip 2000-2600 | driving-median |
|---|---:|---:|---:|
| **ORIGINAL, solo (the reference)** | **0.1913** | **0.2498** | **1940.59** |
| HEAD, **solo** (2 runs, identical) | 0.1424 | 0.2993 | 2519.82 |
| HEAD, 3 opponents driving | 0.1323-0.1670 | 0.2992 | 2137-2540 |
| HEAD, 3 opponents parked | 0.1609 | 0.2296 | 691 |
| `56ad3806`, 3 opponents driving (the D2 row's arm) | 0.1916 | 0.2669 | 1932 |
| `56ad3806`, 3 opponents parked | 0.1713 | 0.2372 | 705 |
| ROADMAP §D2 row | 0.1916 | 0.2668 | 1887 |

On the scenario the original actually ran, HEAD is **-25.6% / +19.8% / +29.8%**.

## 6. The fix, and the re-measurement

*(filled in last)*
