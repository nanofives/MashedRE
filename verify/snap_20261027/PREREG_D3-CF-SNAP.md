# PREREG D3-CF-SNAP: remove the port-only snapshot / restore that SUBSTEP_OUTER keeps (2026-10-27)

Branch `orch/D3-CF-SNAP` from `orch/integration` `96c032d0b`. Ledger row D3-CF-SNAP ("Port-only snapshot/restore kept by
SUBSTEP_OUTER", evidence `ROADMAP.md:2246` (the row's `:2219` has shifted to `:2246` on this base), `verify/substep_20261021/RESULT_substep.md`
section 5.1). Committed alone, before any code or arm.

## 0. What section 5.1 requires (literal)

`verify/substep_20261021/RESULT_substep.md:88-93` (section 5, item 1) states a gap, not a task:

> A car the opponent loop skips or relocates after the interleaved step is restored from the pre-step snapshot and stepped per
> car. That drops any car-car impulse an earlier car's substep dealt to it in that frame. [...] the original has neither the
> stuck relocation (FUN_00470c70's respawn calls 0x004713b2 / 0x004713d1 are not ported) nor the spin skip. Dead cars never
> enter the interleave (the A4 pre-phase skips them), so only the stuck relocation reaches the restore. How often it fires on
> G-CTRL is not counted [UNCERTAIN: no log on the restore path].

The only remedy named in the record is the decision question at `RESULT_substep.md:110-111` (section 6, bullet 3):

> Keep the snapshot / restore, or move the skip / relocation decisions ahead of the physics step so the restore and its
> impulse loss go away (changes relocation frames against both the fiber and the default)?

So section 5.1 requires two things, both read off the text above: (R1) count how often the restore fires (its own
`[UNCERTAIN]`), and (R2) the alternative to keeping it, "move the skip / relocation decisions ahead of the physics step", which
the record itself says changes relocation frames. No other step is recorded. That decision has not been taken (the row
carries it). This lane builds R1 and R2 as default-OFF knobs and measures them. It does not flip anything.

## 1. Code map (this base)

`mashedmod/src/mashed_re/D3d9Render/TrackRenderer.cpp`:

- Pre-phase `:5366-5398`: under `MASHED_A4_ORDER` (seeded), each AI car that is not dead (`round_mode_ && !alive`) and not
  spun (`a.spin > 0`) gets its A4 and `s_outer[v].have = true` (`:5395`).
- Outer step `:5422-5434`: `VehiclePhysics_OuterSnapshot(v)` for every `have` car, then `VehiclePhysics_StepCarsOuter`.
- `UpdateRace` `:5673` (before the opponent loop by default) updates `race_[v].gate` / `.alive`.
- Opponent loop `:5730-5864`. `outerUndo` (`:5733`) = `VehiclePhysics_OuterRestore(v)` + `have = false`. Called at four
  sites: dead skip `:5737`, spin skip `:5741`, stuck relocation `:5769`, and the io-mismatch safety net `:5801-5817`
  (restore + per-car `StepCar`).
- Snapshot body: `Vehicle/VehiclePhysicsRun.cpp:1051-1081` (`OuterSnap`, record + basis + contact cache + adapter state).

Reachability, read from the code: with `MASHED_DEAD_COAST` default ON (`VehiclePhysicsRun.cpp:945-948`), the dead skip
(`:5736`) excludes `deadCoast` cars, and `deadCoast` requires `!alive`, so on the physics path the dead skip only fires when
dead coast is opted out. The spin and dead skips can reach a `have` car only if `a.spin` / `alive` changed between the
pre-phase and the loop (`UpdateRace` or the AI tick in between). The stuck relocation fires when `stuck_t > 2.5 s`
(`:5762`), decided from `race_[v].gate` after `UpdateRace`. D2 arms run `MASHED_MEASURE_SOLO=1` (`:4784`), so `ai_cars_` is
empty, the outer driver needs `outerN > 1` (`:5429`), and none of these sites runs on D2.

## 2. Plan

Step 1 (this lane, knobs, default OFF):

- **K-LOG `MASHED_SNAP_LOG=1`** (diagnostic, no computed value changes): one `[snap]` line in `mashed_re.log` per restore,
  with frame index, slot and site (`dead`, `spin`, `reloc`, `mismatch`), cap 2000 lines. Closes R1.
- **K-POST `MASHED_SNAP_POST=1`** (behaviour knob, physics path only): the frame's skip / relocation decisions are the ones
  made before its physics step.
  - A car the outer driver stepped (`s_outer[v].have`) is never restored for a dead or spin state that appeared after the
    pre-phase: this frame takes the outer result, and the skip applies from the next frame, whose pre-phase already excludes it.
  - The stuck relocation block runs after the car's commit (end of the loop body), with the same condition (not dead,
    not spun, not dead coast, `ng >= 2`, not `RoundEnd7ExitActive`) and the same `stuck_t` accumulation. The trigger frame
    is unchanged (it reads the same `race_[v].gate`). The relocated position, yaw and zeroed velocity are unchanged. What
    changes: the frame's physics step is kept instead of undone and re-run from the relocated position, so the car starts
    stepping from the relocated position one frame later, with the next frame's AI inputs.
  - The io-mismatch safety net stays (it guards against a pre-phase / loop io drift, measured 0 lines in substep, section 3).
  With K-POST on, the only remaining restore site is that net.

Step 2 (decision row, not this lane): flip K-POST if Mariano decides to, with the hashes this lane measures.

Step 3 (after a flip): if the mismatch counter stays 0 across the gate set, replace the net's restore with a logged
`StepCar` refusal and delete `OuterSnapshot` / `OuterRestore` / `OuterSnap`. Not this lane.

## 3. Arms

Exe: worktree build, `MASHED_NO_DEPLOY=1`. Runner: `verify/snap_20261027/run_arms.py` (copy of
`verify/savelink_20261023/run_arms.py`, paths changed, adds stacks `SL` = K-LOG, `SP` = K-POST, `SPSL` = both). Private root
`D:/MashedWT/D3-CF-SNAP_root`, data `D:/MashedWT/D3-CF-SNAP_data`. Every boot under
`mashed_lock.py run --label D3-CF-SNAP`.

- A0 default: `H_DEF B_DEF_K0 B_DEF_K1 B_DEF_K26`.
- A1 log: `H_SL B_SL_K0 B_SL_K1 B_SL_K26` (restore counts per site).
- A2 knob: `H_SPSL B_SPSL_K0 B_SPSL_K1 B_SPSL_K26`.
- D2: not run. Structurally unreachable (section 1, `MEASURE_SOLO`). Stated, not measured.

## 4. Predictions and pass rules

- P0 (no-op build): A0 equals the `merge_train.json` refs: G-CTRL `5943c5f1 / 364350ae / 52052890`, B
  `3a03670c / e85ea6fb / 53778664` (K0 / K1 / K26). Fail = the knob code moved the default: RED.
- P1 (log is inert): A1 hashes equal A0 on every arm. Fail = RED.
- P2 (restore count): reported per arm and site as measured. No predicted value. Substep's reading ("only the stuck
  relocation reaches the restore") is checked against it: any `dead` or `spin` line on a default arm refutes that sentence.
- P3 (knob scope): on any arm where A1 logs 0 restores and 0 relocations would have fired, A2 equals A0 exactly. Where A1
  logs one or more `reloc`, A2 may differ, and the differing arms' hashes are reported as the expected post-flip refs.
- P4 (knob removes the restore): A2 logs 0 `dead`, 0 `spin`, 0 `reloc` lines. Fail = RED.

Verdict: GREEN if P0, P1, P4 hold and A2 equals A0 on every arm (nothing to decide). DECISION if P0, P1, P4 hold and A2
differs on an arm (the flip question goes to Mariano with the A2 hashes). RED if P0, P1 or P4 fails.
