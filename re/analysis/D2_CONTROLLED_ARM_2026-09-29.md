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

*(filled in after the runs; nothing above this line changes)*
