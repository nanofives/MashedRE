# U-9141 — a CONTROLLED arm for the D2 `a8` gate recipe

Session 2026-09-29, branch `race/first-frame-parity`. **User decision (2) of 2026-09-29:**
give the D2 gate recipe a controlled version, after the criterion (e) work, with the
opponent removal as a *measurement-harness* knob and the pass rule **pre-registered before
running**. §1 and §2 below are written and committed in that order and **before the first
controlled-arm run**, so neither the arm nor its pass rule can be chosen after seeing a
number. §3 onward is filled in afterwards.

---

## 1. The problem this arm exists to fix (inherited, not re-derived)

`D3_DRIVE_2026-09-28.md` §3 bisected the "D2 drift" and found the D2 table cannot be
reproduced at HEAD **for a reason that is not a drive-law change**: no commit in
`56ad3806..HEAD` edits the player's solver. Two things moved the table, and both are the
port becoming *more* like the original:

- `09a73dc6` — three RNG-driven opponents began sharing the player's world during a 50 s
  full-lock donut taken at the start line. §3.4 CONFIRMED this is the variance source:
  with the opponents not updated, HEAD is stable to four decimals on 3/3 runs
  (slip 1500-2000 = `0.1427`) and `09a73dc6` gives the identical `0.1427` on 3/3; with them
  updated, HEAD spans 0.128..0.177.
- `4ff428ad` — the race got longer. §3.5: the logged frame count is deterministic per
  configuration and is 1083 at `56ad3806`, 1623 at `09a73dc6`, even though the recipe is
  wall-clock-bounded at 50 s. A longer race repopulates the reducer's speed bands, and the
  bands are what the D2 table is made of.

§3.6 therefore proposed exactly two changes, in this order: pin the frame count, then
measure with the opponents absent. §3.7 stated the finding but did not action it, because
changing a closed phase's gate recipe is above a session's remit. It is now a user decision.

**What this arm does NOT do.** It does not revert either commit, it does not move a D2 band,
and it does not touch the default path. `MASHED_MEASURE_NOOPP` is default-OFF and with it
unset the built image is byte-identical in behaviour to before.

---

## 2. PRE-REGISTERED — the arm, and its pass rule, fixed before the first run

### 2.1 The arm

**C-A8-1** = the existing `a8` held-full-lock recipe (`re/tools/statediff/a8_run_port.py`,
unchanged) plus exactly two controls:

1. **Opponents absent**, via the new harness knob `MASHED_MEASURE_NOOPP=1`
   (`D3d9Render/TrackRenderer.cpp`, at the per-opponent update loop). It sets that loop's
   bound to 0 and does nothing else, so the three AI cars stay parked on the grid and never
   enter the shared physics/collision world. The AI clock advance, `AiBridgeSnapshot`,
   `Ai_Standalone_Tick`, `AiStepDump` and the player's entire solver are untouched — this is
   §3.4's `NOOPP` configuration, which was the deterministic one (3/3 to four decimals at
   both `09a73dc6` and HEAD), and deliberately **not** `NOOPP+NOTICK`, which §3.4 measured as
   no different.

   It follows the `MASHED_STEER_HOLD` precedent verbatim (`exe_main.cpp:2933-2935`:
   *"MEASUREMENT HARNESS ONLY; it commands input, it changes no computed value"*): it commands
   the scenario, it is default-OFF, and it is not a default-path change.

   At **`56ad3806` the knob is not needed and is not used** — the opponents never entered the
   chain at that commit at all (§3.4's last table row: *"no knob needed - opponents never
   entered the chain"*). Both ends of the comparison therefore reach the same scenario state
   by different routes, which is stated here rather than glossed.

2. **Fixed frame count**, via the new `--max-lines N` arm on `a8_slip_axis.py` and
   `a8_momentum.py`. Truncation is applied to the raw frames **before** the regime filter and
   **before** the spike median is computed, so the spike exclusion is also computed over the
   same span at both ends.

   **N = 1080**, fixed here. Chosen as the largest round number at or below the shortest
   complete run ever observed at `56ad3806` (1083 lines, §3.1's four runs are 1083/1084/1083/1084).

   Frame count is a fixed *duration* here, not merely a fixed row count: the standalone's
   chain dt is pinned at `frameMs = 50` (1/60 s), measured as a **single distinct**
   `linTerm=1.66667e-05` over all 3596 samples of
   `verify/d3_force_20260929/cad1/friction_diag.log`. So truncating to 1080 frames truncates
   both ends to the same 18.0 s of simulated time.

   **The ORIGINAL side is NOT truncated.** Its capture is fixed and archived
   (`verify/a8_steer_20260824/orig_steerR.msd`, 2335 frames) and it is the reference the D2
   row was measured against; what varies across commits is the PORT length, and that is what
   is controlled.

3. **Discard rule**, carried from §3.8: any run producing fewer than 1080 parsed frames is a
   truncated boot, not a sample of the recipe. It is discarded and re-run, and the discard is
   reported rather than silent.

**Runs:** 3 per commit, at HEAD and at `56ad3806`, built in place
(`git checkout 56ad3806 -- mashedmod/`, build, measure, `git checkout HEAD -- mashedmod/`),
no worktree, `original/` never touched, PIDs spawned and killed by `a8_run_port.py`.

### 2.2 The three statistics

Exactly the ones the ROADMAP §D2 row is made of, reduced by the same two scripts:
`slip 1500-2000` and `slip 2000-2600` (both `slip vs fwd`, `a8_slip_axis.py`), and
`driving-only horiz median` (`a8_momentum.py --orig-steer-min 33.0 --port-steer-min 0.9`).
`av.y` per band is recorded alongside as a fourth, reported-not-gated witness.

### 2.3 The pass rule — three criteria, all three required

| # | criterion | bound, and where the bound comes from |
|---|---|---|
| **C1** | **instrument determinism.** Within each commit, the 3 runs' `slip 1500-2000` must agree with each other. | **±0.005 absolute.** §3.4 measured the `NOOPP` configuration at `0.1427` on 3/3 at HEAD and `0.1427` on 3/3 at `09a73dc6` — agreement to four decimals, i.e. a spread of 0. ±0.005 is a deliberately generous bound stated up front, and it is the same order as the 0.005 slip bound `U9138_FIX_2026-09-28.md` §6.1 used. |
| **C2** | **cross-commit agreement.** HEAD's and `56ad3806`'s controlled-arm medians must agree on all three statistics of §2.2. | **±2%.** The project's standing bound for a speed/median statistic (`U9138_FIX_2026-09-28.md` §5: "driving-median speed: within 2%", "max speed: within 2%"), and the same bound criterion (e)'s band inherits. |
| **C3** | **D2 reproduction.** `56ad3806`'s controlled arm must reproduce the ROADMAP §D2 row it originally produced: slip 1500-2000 `0.1916`, slip 2000-2600 `0.2668`, driving-median `1887`. | **±2%**, same provenance as C2. |

**U-9141 CLOSES only if C1, C2 and C3 all hold.** The three outcomes are pre-committed:

- **All three hold** → the recipe is a controlled instrument again and the D2 verdict stands
  on it. Close U-9141; record the recipe change in ROADMAP §D2 as a dated amendment.
- **C1 and C2 hold, C3 fails** → the recipe is controlled but the D2 row's numbers belong to
  the *uncontrolled* recipe and must be re-baselined against the controlled arm. That is an
  amendment and a new open row, **not** a closure of U-9141, and the re-baselined numbers are
  a user decision (they change a closed phase's recorded values).
- **C1 or C2 fails** → the arm is not controlled either, the two §3.6 controls are not
  sufficient, and U-9141 stays open with the failing criterion named and the next control
  proposed.

No fourth outcome is available, and none of these bounds moves after the first number lands.

---

## 3. MEASURED

Nothing above this line was changed after the first run. Evidence `verify/d2_ctrl_20260929/`.

### 3.1 The knob is inert when unset — the default-path proof, run first

One default run (knob unset) on the build that **contains** the knob, against `g_d2_2` from
earlier today on the build that **did not**:

| | lines | slip 1500-2000 | slip 2000-2600 | driving-median |
|---|---:|---:|---:|---:|
| `g_d2_2`, pre-knob build | 1497 | 0.1323 | 0.2992 | 2538.13 |
| `knoboff_1`, knob compiled in, unset | 1497 | **0.1323** | **0.2992** | **2538.13** |

Identical to every printed digit, same line count. `MASHED_MEASURE_NOOPP` unset changes
nothing, which is what "measurement-harness knob, not a default-path change" has to mean.

### 3.2 The controlled arm, 3 runs per commit

All six runs exceeded the 1080-frame floor (1268/1266/1268 at HEAD, 1085/1086/1084 at
`56ad3806`), so **nothing was discarded**.

| arm | run | slip 1500-2000 (n) | slip 2000-2600 (n) | av.y | driving-median (n) |
|---|---|---:|---:|---|---:|
| **`56ad3806`** controlled | g56_1 | **0.1916** (171) | **0.2669** (380) | +1.123 / +1.591 | **1932.09** (943) |
| | g56_2 | **0.1916** (171) | **0.2669** (381) | +1.123 / +1.591 | **1932.37** (943) |
| | g56_3 | **0.1916** (171) | **0.2669** (379) | +1.123 / +1.591 | **1931.36** (943) |
| **HEAD** controlled | head_1 | **0.1609** (48) | **0.2296** (24) | +1.019 / +0.792 | **691.01** (626) |
| | head_2 | **0.1609** (48) | **0.2296** (24) | +1.019 / +0.792 | **691.01** (626) |
| | head_3 | **0.1609** (48) | **0.2296** (24) | +1.019 / +0.792 | **691.01** (626) |
| ORIGINAL (untruncated, archived) | — | 0.1913 (312) | 0.2498 (541) | +1.143 / +1.464 | 1940.59 (1153) |
| ROADMAP §D2 row (2026-09-14) | — | 0.1916 | 0.2668 | 1.12 / 1.59 | 1887 |

### 3.3 The 2×2 — which control did what, measured rather than assumed

The two controls were applied together, so each was also measured alone. `--max-lines 1080`
costs nothing to apply retroactively, so the opponents-ON cell is the same logs reduced twice.

| arm | slip 1500-2000, 3-4 runs | slip 2000-2600 | driving-median | spread on slip |
|---|---|---:|---:|---:|
| HEAD, opponents ON, **untruncated** | 0.1411 / 0.1323 / 0.1411 | 0.2993 / 0.2992 / 0.2993 | 2539.4 / 2538.2 / 2539.4 | 0.0088 |
| HEAD, opponents ON, **trunc 1080** | 0.1411 / 0.1323 / 0.1411 / 0.1323 | 0.2993 / 0.2992 / 0.2993 / 0.2992 | 2516.4 / 2538.1 / 2516.4 / 2538.1 | 0.0088 |
| HEAD, **opponents OFF**, trunc 1080 | 0.1609 / 0.1609 / 0.1609 | 0.2296 ×3 | 691.0 ×3 | **0.0000** |
| `56ad3806`, trunc 1080 (this session) | 0.1916 / 0.1916 / 0.1916 | 0.2669 ×3 | 1932.1 / 1932.4 / 1931.4 | **0.0000** |
| `56ad3806`, trunc 1080 (2026-09-28 logs) | 0.1916 / 0.1916 | 0.2668 / 0.2627 | — | 0.0000 |

Read off it:

1. **Truncation alone is not the confound.** The opponents-ON arm gives the same three slip
   values truncated and untruncated, and its median moves only 2539 → 2516-2538. `--max-lines`
   is a legitimate control (it pins a variable that provably moved, §3.5 of the 2026-09-28
   note) but it was not what was wrong.
2. **Opponent removal is the whole of it**, and it is large: slip 1500-2000 0.1411 → 0.1609,
   slip 2000-2600 0.2993 → 0.2296, driving-median 2538 → **691**.
3. **The control works.** With the opponents removed the instrument's run-to-run spread on
   `slip 1500-2000` is **exactly 0** at both commits (3/3 identical to four decimals). With
   them present it is 0.0088 — larger than C1's own ±0.005 bound. So the arm does what it was
   built to do: it turns a non-deterministic recipe into a deterministic one.

### 3.4 Verdict against the pre-registered rule

| # | result | detail |
|---|---|---|
| **C1** | **PASS** | spread 0.0000 on `slip 1500-2000` at both commits, against a ±0.005 bound; and the uncontrolled arm's 0.0088 spread fails the same bound, which is the control's own positive. |
| **C2** | **FAIL** | HEAD vs `56ad3806`: slip 1500-2000 `-16.02%`, slip 2000-2600 `-13.98%`, driving-median `-64.23%`. Bound ±2%. Not marginal. |
| **C3** | **FAIL on 1 of 3** | `56ad3806` vs the D2 row: slip 1500-2000 `0.1916` vs `0.1916` = **0.00%** PASS; slip 2000-2600 `0.2669` vs `0.2668` = **+0.04%** PASS; driving-median `1932.09` vs `1887` = **+2.39%** FAIL (band 1849.3..1924.7). `av.y` `1.123`/`1.591` vs `1.12`/`1.59` matches exactly (reported, not gated). |

**U-9141 does NOT close.** That is the pre-registered consequence of a C2 failure and it is
honoured as written.

**Two corrections to my own §2.3, stated rather than quietly reinterpreted.**

- The outcome text attached to a C2 failure said *"the arm is not controlled either, the two
  §3.6 controls are not sufficient"*. **That label is wrong, and §3.3 is why**: C1 passed with
  a spread of exactly 0 and the arm demonstrably removes the non-determinism the uncontrolled
  recipe still shows. The arm IS controlled. What C2's failure actually means is that the
  controlled instrument has **exposed a real HEAD-vs-`56ad3806` difference in the player's own
  trace** that the uncontrolled recipe was masking — its 0.128..0.177 spread brackets both
  0.1609 and 0.1916, so it could not have told them apart. The pre-registered *decision*
  (U-9141 stays open) stands; the *reason* is the opposite of what I wrote.
- C3's driving-median miss is **0.4 percentage points** over a bound I will not move after the
  fact. Worth recording that the same overshoot is already in the 2026-09-28 note: its §3.1
  measured today's `56ad3806` build at `1928.48 / 1914.91 / 1931.36 / 1928.48` and called that
  a reproduction of the D2 table. So the D2 row's `1887` is itself ~2% below what the recipe
  produces at its own commit, independent of any control, and that is a question about the D2
  row's recorded value — not about this arm.

### 3.5 What the failure of C2 opens — TWO separate questions, both [UNCERTAIN]

**[UNCERTAIN] U-D2-OPPONENT-COUPLING — how do the opponents change the PLAYER's trace at
all?** Car-car contact is **not** in the standalone's player path: `VehicleCarCarContact`
(`0x00469df0`, `Collision/CarCarContacts.cpp`) is compiled into `mashed_re.exe` but has
**zero callers** anywhere in the port (`grep -rn VehicleCarCarContact mashedmod/src/mashed_re`
finds only its definition and its `ContactSolvers.h` declaration), and `TrackRenderer` has no
car-car collision call. So three parked or driving opponents should not be able to move the
player by 3.7x in median speed — and they do. The coupling must be shared mutable state.
Named candidates, each with its citation, **none of them assumed**:
`g_torqueRingPhase` (`DAT_007f101c`), which `VehiclePhysics_StepCar` advances
`(phase + 1) & 0xf` once per A4 call — four times per frame with opponents, once without —
and which A4 uses to index the steer ring `+0x1ac + phase*4` / `+0x270 + phase*4`
(`0x00470670`); `Collision::g_suspScratch` (`DAT_00881560`), documented in
`ForceIntegratorStubs.cpp:32-33` as *"shared with the wheel solver"*; and the powerup
dispatcher / pickup field the opponents drive. Next command:

```
# per-global A/B at HEAD, controlled arm, one temporary diag at a time, removed after:
#   1. advance g_torqueRingPhase 4x/frame with the opponents OFF. If the player's trace
#      returns to the opponents-ON values, the ring is the coupling and A4's ring write
#      needs a reader audit (findoffset.py --writes 0x1ac / 0x270 and the read side).
#   2. same shape for Collision::g_suspScratch.
# The instrument is now deterministic (spread 0.0000), so ONE run per configuration decides
# -- unlike the refuted one-run premise of D3_DRIVE_2026-09-28.md section 1.3, which was
# refuted on the UNCONTROLLED arm.
```

**[UNCERTAIN] U-9141 itself, restated — the residual HEAD-vs-`56ad3806` player difference.**
With the opponents absent at both ends, the player is `-16% / -14% / -64%` off its own
`56ad3806` values. The 2026-09-28 bisect concluded *"no commit in `56ad3806..HEAD` edits the
player's solver"* and therefore that the drift was purely the instrument; **on a controlled
instrument that conclusion does not hold**, and the bisect has to be redone on the controlled
arm. It is now cheap and sound to do so, because C1 makes one run per commit decisive:

```
# bisect 56ad3806..HEAD on the CONTROLLED arm, one run per commit, over the 17 commits of
# D3_DRIVE_2026-09-28.md section 1.2 that touch mashedmod/:
py -3.12 re/tools/statediff/a8_run_port.py verify/<tag> 50 -MASHED_REAL_PHYSICS \
    MASHED_MEASURE_NOOPP=1 MASHED_TITLE="U-9141 controlled bisect <sha>"
py -3.12 re/tools/statediff/a8_slip_axis.py --orig verify/a8_steer_20260824/orig_steerR.msd \
    --port verify/<tag>/motion_diag.log --max-lines 1080
# classification, fixed here: slip 1500-2000 >= 0.185 is GOOD, <= 0.170 is BAD, between is
# INDETERMINATE and gets a second run. Commits before 09a73dc6 have no opponent loop to
# disable, so the knob is a no-op there and the arm is the same measurement either way.
```

One asymmetry in the arm to carry into that bisect, stated now rather than discovered later:
at `56ad3806` the opponents are not merely un-stepped, they are moved by the pre-D3 kinematic
Option B model, whereas `MASHED_MEASURE_NOOPP=1` at HEAD leaves them **parked**. If
U-D2-OPPONENT-COUPLING turns out to be real, that difference is itself a confound and the
HEAD end should instead use the Option B treatment (§3.4's `NOAIPHYS` configuration, which
that session measured at 0.1736 / 0.1561 / 0.1774 — closer to 0.1916, and not stable).

### 3.6 Reproduce

```
# knob-off inertness control
py -3.12 re/tools/statediff/a8_run_port.py verify/d2_ctrl_20260929/knoboff_1 50 \
    -MASHED_REAL_PHYSICS MASHED_TITLE="D2 knob-off inertness control"
# HEAD controlled arm, i = 1..3
py -3.12 re/tools/statediff/a8_run_port.py verify/d2_ctrl_20260929/head_$i 50 \
    -MASHED_REAL_PHYSICS MASHED_MEASURE_NOOPP=1 MASHED_TITLE="D2 controlled arm HEAD $i"
# 56ad3806 controlled arm (no knob: the opponents never enter the chain there), i = 1..3
git checkout 56ad3806 -- mashedmod/ && mashedmod\build.bat
py -3.12 re/tools/statediff/a8_run_port.py verify/d2_ctrl_20260929/g56_$i 50 \
    -MASHED_REAL_PHYSICS MASHED_TITLE="D2 controlled arm 56ad3806 $i"
git checkout HEAD -- mashedmod/ && mashedmod\build.bat
# reduce, both ends identically
py -3.12 re/tools/statediff/a8_slip_axis.py --orig verify/a8_steer_20260824/orig_steerR.msd \
    --port verify/d2_ctrl_20260929/<tag>/motion_diag.log --max-lines 1080
py -3.12 re/tools/statediff/a8_momentum.py  --orig verify/a8_steer_20260824/orig_steerR.msd \
    --port verify/d2_ctrl_20260929/<tag>/motion_diag.log --max-lines 1080 \
    --orig-steer-min 33.0 --port-steer-min 0.9
```

`mashedmod/` was restored to HEAD after the probe and `git status --short -- mashedmod/` is
empty; no worktree was created; `original/` was never touched; every `mashed_re.exe` PID was
spawned and killed by `a8_run_port.py`, by PID.
