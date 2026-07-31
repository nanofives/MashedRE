# statediff — divergence-driven discovery lane (prototype 2026-07-31)

Top-down complement to per-function promotion: run the same deterministic
scenario stock and hooked, snapshot one state surface every render frame,
diff, and map diverging fields to their **writer functions** — the inferred
missing/wrong ports. Discovery lane only: a GREEN whole-run diff is ensemble
evidence, **not** per-function C4 (re/CONFIDENCE.md still gates promotion).

## Pieces

- Capture: `re/frida/scenario_launch.py --statediff-out <p.msd> [--statediff-car N]`
  — snapshots the 0xd04 vehicle record at `0x008815a0 + N*0xd04` on every
  phase-3 render tick (`FUN_004c1be0`, ~60/s). Frame 0 = first phase-3 tick
  (the only engine-driven, cross-boot-alignable anchor; the python-timed warp
  poke makes the menu anchor useless across boots). Press pulses are
  suppressed during capture (wall-clock-timed input). Format: `FORMAT.md`.
- Diff: `py -3.12 re\tools\statediff\statediff.py A.msd B.msd [--json out]`
  — first divergent frame + per-dword-field first-divergence and raw values
  (hex/i32/f32, no interpretation). Exit 0 GREEN / 2 divergence.

## Protocol (order is load-bearing)

1. **Noise floor first**: stock vs stock (`--hooks` empty on both). Fields
   that diverge here (timers, wall-clock-derived, RNG-seeded) form the
   *noise mask* — record them in `NOISE_MASK.md` and exclude them from any
   stock-vs-hooked claim.
2. **Stock vs hooked**: same scenario, second boot with `--hooks <rvas>`.
   Diverging fields NOT in the noise mask = real behavioral divergence.
3. **Writers mapping** (interactive, Ghidra pool slot): for each diverging
   offset `+X`, xref writers of `0x008815a0+X` (car-0 direct accesses are
   their own DAT symbols) AND of the base/sibling anchors for indexed
   `base + car*0xd04 + X` writers. Join candidates against `hooks.csv`
   status: writers at C0/C1/scaffold are the inferred missing ports.
   Caveat (71s-AV lesson): the diverging field is the *symptom*; the culprit
   may be upstream — chase the writer's inputs when the writer itself is
   already verified.

## Prototype status

- [x] differ + synthetic self-test (RED at seeded frame/field, GREEN on identity)
- [x] stock-vs-stock noise floor on Training/QuickRace/1-car — 32/833 dwords
      noisy (`NOISE_MASK.md`), 801 bit-stable
- [x] stock-vs-hooked run — hook 0x0047eb30 (VehiclePhysicsWorldStep port)
      live (fresh phys_c4_bridge_selftest.log = install witness): GREEN,
      1249 aligned frames bit-identical outside the mask
      (`verify/statediff_proto/hooked_vs_stock.json`)
- [x] writers mapping exercised on a forced RED (2026-07-31). Two forcing
      attempts, both instructive:
      - Bypassing 0x0047eb30 (`--bypass-proxy`, `ret 0` live from +5.4s) gave
        GREEN — in the idle-car Training scenario that path's output never
        reaches the car-0 record. Concrete instance of the coverage caveat:
        a GREEN only covers what the scenario executes.
      - `--boost 0.5` (vel-Y injection) gave RED: 22 fields, first divergent
        frame 912 (~race-GO; physics only integrates after GO). The differ
        surfaced the injected 0.5 at +0x9b4 verbatim.
      Mapping (Ghidra pool slot, `reference_to` on 0x008815a0+off →
      `function_at` → hooks.csv join): +0x9b4 (0x00881f54) has 19 absolute
      xrefs across 11 functions — C4 VehicleWheelForceIntegrator 0x0046ddb0,
      C3 0x0046baa0/0x0046d660/0x0046c570, and 7 still at C2
      (0x0046bf50, 0x00467350 VehicleSlipTimerTick, 0x0046d570,
      0x004809e0/0x00480720 respawn pair, 0x0046cb30, 0x0046bfc0
      collision/impulse solver) = the inferred port-next list for that field.
      Known limitation confirmed: pointer-relative-only fields (+0x218 ring
      +0xb94) have ZERO absolute xrefs — for those, join offsets via
      re/analysis/structs/vehicle.md instead. Evidence:
      verify/statediff_proto/{bypass,boost}_vs_stock.json.

## Discovery pass #1 — driving scenario (2026-07-31)

- Driving noise floor + alignment rules: see NOISE_MASK.md (anchor
  `--anchor-nonzero 0xbf4`, window `--until 314`).
- **Hooked-boot config rules (hard-won):**
  - `MASHED_PHYS_C4_SELFTEST` must be OFF for statediff (the launcher now
    forces this for `--statediff-out` runs): it re-executes hook bodies
    in-process (A3 spawn 3x/call, partial rollback) — both a spawn-breaker
    and a determinism-breaker.
  - The FULL canonical hook set (`--hooks all`) wedged the WARPED race at
    phase 2. BISECTED + FIXED 2026-07-31: registry index 946 =
    `Ring5ab980` @0x005ab980 (audio stream ring-copy, was C3-green via
    early-window synthetic diff) reproduced the wedge ALONE — the port
    was `void` but the original leaves cnt in EAX at RET and the caller
    FUN_005ab710 consumes it (0x005ab7f7 `ADD EBP,EAX` / 0x005ab7f9
    `ADD EDI,EAX`, stream-cursor advance; pre-flagged by U-6701). Same
    class as U-9025. Fixed (return cnt), verified: culprit-alone 533
    frames; full set 5/6 boots healthy (0/19 pre-fix).
    RESIDUAL: ~1/6 full-set boots still wedge at phase 2 —
    a second, INTERMITTENT mechanism (unbisected; needs majority-vote
    bisection ~3 boots/verdict, or may be a warp-timing race rather than
    a specific hook). Full-set statediff runs are usable now but must
    check the capture's frame count and re-boot on a wedged run.
    Bisection driver: scratchpad bisect_wedge.py pattern (MASHED_HOOK_LO/HI
    index bisection + MASHED_HOOK_MANIFEST for the index→name table).
- **Result (physics-chain set 0x0046b540,0x00470670,0x0046ddb0,0x00467650,
  0x00468980,0x0047ea40,0x0047eb30 vs stock, in-window):** RED, first
  divergence at relative frame 117 = the FIRST physics-integration frame
  after GO. First mover `+0x9b8` (vel.z) differs by EXACTLY 1 ULP; 175
  fields follow by end of window, all ULP-scale. This is the documented
  x87/float10 faithfulness residual class (A5/A6a acceptance notes) —
  independently rediscovered and first-write-localized by this lane.
  Divergence inside the bit-stable window is also behavioral proof the
  hook set was live (no selftest log available in statediff runs).
  Writers-map of +0x9b8 (0x00881f58, 19 xrefs): same 11-function
  population as +0x9b4, including installed C4 A5 0x0046ddb0.
  Evidence: verify/statediff_proto/drive_hooked_phys_vs_stock.json.
