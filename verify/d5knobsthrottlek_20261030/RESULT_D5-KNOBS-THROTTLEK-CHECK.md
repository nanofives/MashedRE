# RESULT D5-KNOBS-THROTTLEK-CHECK

Verdict: GREEN. MASHED_THROTTLE_K stays DELETE-CANDIDATE (row 124 of RESULT_D5-KNOBS-D-TRIAGE-2.md), with the
[UNCERTAIN] resolved: the read executes on the default path, but its only consumer is the scaffold else-body.
Static only. No source edit, no game run (the verdict does not need a hash confirmation).
All paths are relative to mashedmod/src/mashed_re/.

## Control-flow path (D3d9Render/TrackRenderer.cpp unless noted)

1. `g_track.UpdateCar(di)` at exe_main.cpp:3862 (MASHED_SIM_HZ=0 hatch) and :3876 (sim-step loop). UpdateCar entry: :5176.
2. Early return only when `!car_ready_ || in.dt <= 0.f` (:5177). Otherwise execution reaches :5219-5221.
3. :5219-5221 `static const float s_throttleK = []{ ... GetEnvironmentVariableA("MASHED_THROTTLE_K", ...) ...}()`
   runs once, on the first UpdateCar call that passes :5177. No branch on MASHED_REAL_PHYSICS precedes it, so the
   env read happens on the default build.
4. :5222 `const float kThrottle = kTop * s_throttleK;` also runs unconditionally.
5. Only consumer of kThrottle in the whole tree (grep over mashedmod/src): :5899 and :5900
   (`car_vel_[0] += fwd[0] * accel * kThrottle * in.dt;` and the [2] twin).
6. :5899-5900 sit inside the `else` opened at :5896 (`} else {`). That else pairs with
   `if (Vehicle::VehiclePhysics_Enabled()) {` at :5674 and closes at :5932
   (`}  // end MASHED_REAL_PHYSICS else (scaffold body)`).
7. Branch condition: VehiclePhysics_Enabled() (Vehicle/VehiclePhysicsRun.cpp:295-307) returns
   `!(getenv("MASHED_REAL_PHYSICS") && v[0]=='0')`. Default (unset) is true, so the if-branch (ported chain) runs
   and :5899 is not reached. The else-body runs only with MASHED_REAL_PHYSICS=0.

## Reclassification

- Default player path: the knob has no effect. Setting MASHED_THROTTLE_K changes kThrottle but nothing reads it
  when MASHED_REAL_PHYSICS is unset. The live path takes accel through io.input[4]/[5] (:5686-5687) into the
  ported chain.
- Only reachable consumer: the MASHED_REAL_PHYSICS=0 A/B revert scaffold (:5896-5932). Class: DEAD on the default
  build, DEV-only on the revert.
- Verdict for row 124: DELETE-CANDIDATE confirmed. Coupling: it is deletable on its own (remove :5213-5222 and
  use a literal 0.58f in :5899), but the clean deletion is with the scaffold else-body (:5896-5932), which also
  removes kDrag, kGrip, kSteer and kGravity users. That decision belongs with the MASHED_REAL_PHYSICS=0 revert
  retirement, not with this knob alone. Do not delete THROTTLE_K ahead of that unless the revert stays.
- Side effect to note for any deleter: the unconditional static init at :5219 is a per-process GetEnvironmentVariableA
  call, harmless, and no state depends on it.

## Not run

No mashed_re.exe set/unset hash run. Static evidence is complete: the sole consumer is in the not-taken branch,
so a hash comparison would be identical by construction.

## Changed files

Only this RESULT file. No mashedmod/src or build input touched.
