# WS-PHYS-COVERAGE-SCENARIO — richer canonical scenario for the physics C4 lane (2026-06-17)

Branch `ws-phys-coverage-scenario`. Goal: drive a RICHER canonical scenario through
the `.asi` verbatim-LUT installed-hook physics lane (`PhysicsChainHooks.cpp`) so the
transcribed-but-unexercised branches (A4 brake, A5/A6a/A6b airborne, A5 random-surface)
actually run and get bit-confirmed in-process. Anchored MASHED.exe SHA-256
BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.

## Scenario driver used: AI-opponent multi-car coverage + per-branch sampling quotas

The per-frame physics dispatcher (`FUN_00470c70`) invokes A4/A5/A6a/A6b for **every**
car record (`DAT_008815a0 + slot*0xd04`, slot 0 = player, slots 1-3 = AI opponents),
not just the player. The AI opponents **brake into corners, steer, and race on their
own**, so their records exercise the brake / grip-else branches without any input
injection — driver option (a) from the prompt.

Two changes made the AI-driven branches land in the in-process self-test:
1. **Per-branch sampling quotas** (replacing the single "first 96 calls" cap that filled
   up on the early all-grounded throttle-only frames before any branch-novel call):
   each self-test now keeps independent quotas for its target branches and admits a call
   only if it advances a not-yet-full quota. A4: idle/accel/**brake**/**hiIn5(input[5]>128)**.
   A5: grounded / **airborne(grounded!=4.0)**. A6a: idle/accel/**brake**/**airborne**.
   A6b: grounded-gate / **airborne(grounded==0.0)**. Each log row now also prints the
   record's car `slot=` (proves AI cars, slots 1-3, drove the branch).
2. **Harness**: `re/frida/phys_c4_telemetry.py` race window default 12 -> 45 s (env
   `PHYS_C4_RACE_SECS`); continuous accelerate (ctrl 4) + optional steer (`PHYS_C4_STEER`)
   so the player car also corners hard. Authoritative run: 75 s, steer=0.
   Control map learned by `re/frida/phys_c4_inrace_probe.py` (in-race FUN_00497310
   override sweep): **ctrl 4 = accelerate** (player reached speed ~2888 with accel+steer).

Run (all 4 per-frame hooks installed live, self-test on):
`MASHED_PHYS_C4_SELFTEST=1 PHYS_C4_RACE_SECS=75 PHYS_C4_STEER=0`
`py -3.12 re/frida/phys_c4_telemetry.py hooked A4_Entry,A5_Entry,A6a_Entry,A6b_Entry,0x00470670,0x0046ddb0,0x00467650,0x00468980`
A4 inline-JMP LIVE confirmed (first byte 0xE9). All hooks' self-tests fired across
slots 0-3. Reproduced across two runs (60 s no-steer + 75 s steer); deterministic.
Process hygiene: harness spawns its own MASHED via Popen and kills ONLY that PID.

## Per-branch bit-confirm results

| Fn | Newly-reached branch | Result | Evidence |
|----|----------------------|--------|----------|
| A4 0x00470670 | **brake** (input[1]!=0, FCHS-negated force) | **GREEN** 53 calls (AI slots 2/3, in1=76..155), 0 mismatch | A4_coverage_brake_GREEN.txt |
| A4 0x00470670 | **input[5]>128** grip-else branch | **GREEN** 16 calls (in5=255), 0 mismatch | A4_coverage_brake_GREEN.txt |
| A6a 0x00467650 | **brake** (input[5]!=0 && input[4]==0) | exercised (32 calls); divergence is ONLY the pre-known susp-force residual (see below) | A6a_coverage_brake_93pct.txt |
| A5 0x0046ddb0 | grounded (all 4 slots) | GREEN 48/48 | A5_coverage_grounded_allslots.txt |
| A5 0x0046ddb0 | **airborne** (grounded!=4.0) | **NOT REACHED** (0 calls) | — |
| A5 0x0046ddb0 | **random-surface** (key 0xffff32ff RNG) | **NOT REACHED** (rng=0 all) | — |
| A6a 0x00467650 | **airborne** | **NOT REACHED** (0 calls) | — |
| A6b 0x00468980 | grounded gate (all 4 slots) | GREEN 8/8 | A6b_coverage_grounded.txt |
| A6b 0x00468980 | **airborne aero body** (grounded==0.0) | **NOT REACHED** (0 calls) | — |

## Outcomes

### A4 — coverage caveats REMOVED.
The A4 brake branch and the input[5]>128 grip-else branch (previously transcribed
verbatim but unhit in the throttle-only player demo) are now exercised by the braking
AI opponents and are **bit-identical, 0 mismatches** across 104 self-test calls / all 4
car slots. A4 was already C4 on its body-math self-test; this CLOSES its honest coverage
gap (the brake branch is the FCHS-negated mirror of the accel branch + the same
grip/boost sub-branches). hooks.csv A4 note updated: caveat dropped.

### A6a — brake branch exercised; A6a STAYS C2 (residual unchanged, no regression).
A6a's brake branch now runs (32 brake calls, AI slots). The 11/83 non-OK calls diverge
ONLY in the per-wheel suspension force X/Z (`wNfx`/`wNfz` = `piVar12[0x1c..0x1e]`) plus
3 downstream linear/angular-velocity fields — i.e. the documented **[U-A6A-FLOAT10]**
susp-force float10 residual (A6a_FINDINGS_2026-06-17.md), which runs on the grounded
contact path regardless of throttle vs brake. The brake-branch logic itself
(BrakeForceAccum / call#7 RoundBrakeInput) introduces **no new divergence class**. So
A6a's brake branch is now covered, confirming the residual is the only blocker; A6a
remains C2 (NOT promoted; no overclaim).

### A5 / A6a airborne + A5 random-surface + A6b aero body — STILL UNEXERCISED.
**The canonical Quick-Battle Arctic arena is geometrically flat.** Probed exhaustively
(`phys_c4_inrace_probe.py`): the player car driven at full throttle (speed up to ~2888)
with every steer/ram combo NEVER lifted a wheel (`grounded` stayed 0x40800000 = 4.0
every sample); the racing AI opponents likewise stayed all-4-grounded for 75 s. With
zero airborne frames for any of the 4 cars, A5's airborne suspension branch, A6a's
airborne path, and A6b's entire aero body (gated `+0x9e0==0.0`) cannot be reached on
this scenario, and A5's random-surface RNG branch (a surface-key the Arctic tiles don't
carry) never fired. Reaching them needs a DIFFERENT track with a ramp/jump driven via a
circuit-race menu path — that requires reverse-engineering the original's race-mode/
track-select menu navigation (the nav recipe is hardcoded to Quick-Battle/Arctic) plus
a per-track jump map. Per NO-GUESSING this session does not fabricate a menu path or a
synthetic `+0x9e0=0.0` write (synthetic injection would be C3 at best, not canonical).
A5 stays C4 with its airborne/random caveat retained; A6a/A6b stay C2.

## Physics-chain C4 tally (unchanged this session): 3/5 (A3, A4, A5).
A4's coverage is now broader (brake + hi-input verified); A6a RED 93/96 (susp float10);
A6b grounded-only. No promotion this session (A6b's aero body remains unexercised).

## Files touched
- `mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp` — per-branch sampling quotas +
  `slot=` in every self-test log header (no body/transcription changes; the verbatim
  bodies are untouched).
- `re/frida/phys_c4_telemetry.py` — longer default window + continuous accel/steer.
- `re/frida/phys_c4_inrace_probe.py` (new) — in-race control-code probe (ctrl 4 = accel;
  no control lifts a wheel on flat Arctic).
