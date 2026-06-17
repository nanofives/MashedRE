# Session prompts — wave 10 (extend physics C4 + steer/AI runtime). Paste one per session.

Generated 2026-06-17 after wave-9 landed the FIRST bit-identical physics C4 (A4 96/96) + real
steering. Verify+runtime pivot continues; renderer = Track::World spike (ratified); physics C4 =
the full .asi verbatim-LUT lane (ratified). COMMON PREAMBLE: re/SESSION_PROMPTS.md. Rules: Frida/
boot/standalone-runtime = MAIN tree only; analyzed Ghidra clones (pool11/12/6; NOT pool4);
subagent type = `general-purpose` (fork is gone) with self-contained prompts; NO-GUESSING; commit
as nanofives + trailer; guard `original/`.

STATE: `mashed_re.exe` drives a real-physics race (speed 0->112, suspended, stable) + steers via
physics (input[0]/[1]->A4 wheel-angle->yaw). **A4 FUN_00470670 is C4** (96/96, .asi verbatim-LUT
installed-hook lane = Vehicle/PhysicsChainHooks.cpp + re/frida/phys_c4_telemetry.py; the A4
trampoline+forwarder+self-test template is REUSABLE). A5/A6a/A6b/A3 are C2 (lane-ready). Steer
gain kYawScale=1.0 needs in-race calibration. Standalone VehicleControl.cpp kGripMul is a ~1-2
ULP-off decimal literal (exact = 0x392ec33e).

---

## WS-PHYS-C4-A5 — A5 FUN_0046ddb0 to C4 (MAIN TREE). Pool: dev .asi + pool11
> Extend the PhysicsChainHooks lane to A5 (EDI=record). Harder than A4: A5 writes SHARED globals
> (DAT_008815xx contact arrays) + calls RNG FUN_00472650 — so the in-process self-test needs a
> shared-global snapshot/restore + RNG seed control (capture+replay the RNG sequence) so the
> original-body vs transcription run on identical state. Live-LUT forwarders already exist. Per-
> frame bit-diff the record fields A5 writes; resolve any divergence. Promote C4 via re-classify
> (inline-JMP live + canonical race; no overclaim). Reuse the A4 template.

## WS-PHYS-C4-A6A — A6a FUN_00467650 to C4 (MAIN TREE). Pool: dev .asi + pool11
> A6a (the big x87 float10 body). Self-test it through the lane; resolve [U-A6A-ST0] (the implicit
> ST0 input to FUN_004a2c48=ROUND) by capturing the live ST0 at the call. The TU is x87 (no
> /arch:SSE2). Bit-diff; C4 via re-classify.

## WS-PHYS-C4-A6B + A3 — A6b FUN_00468980 + A3 FUN_0046b540 to C4 (MAIN TREE). Pool: dev .asi + pool11
> A6b: disassemble the FUN_004a3384 (asin) + FUN_004c4d20 (device matrix, no-visible-args) ABI;
> forward to the live originals; bit-diff the rotation-apply. A3: spawn-time (off hot path) — self-
> test the record init (mass +0x50/+0x54/+0x58, wheel geometry, handling table 0x00613140). C4 each.

## WS-PHYS-SMOKE-STEER — calibrate steering + fix the grip const (MAIN TREE). Pool: standalone run
> Run mashed_re.exe MASHED_REAL_PHYSICS=1 with steer input: confirm heading follows input (turns
> left/right, not a fixed circle), straight=no drift, radius tightens with magnitude; verify steer
> SIGN (flip the input[0]/[1] mapping if reversed); calibrate kYawScale so the turn radius matches
> the original (capture an original-side turn-rate reference if feasible). ALSO fix the standalone
> grip const: VehicleControl.cpp kGripMul `1.66677e-4f` -> the exact 0x392ec33e bit pattern (A4 has
> no transcendental, so the exact constant makes the STANDALONE A4 bit-identical too). Build + commit.

## WS-AI-VERIFY — AI runtime (MAIN TREE). Pool: standalone run
> mashed_re.exe MASHED_REAL_AI=1 + a race (now LUT-safe + steering real): do opponents drive the
> AI%d.AI race line (not the gate-ribbon)? Resolve [U-C-STEER-MAG]/[U-C-RATE0/1]; fix ctrl->motion.
> Screenshot/log opponent paths vs the spline.

## WS-D-VISUAL / WS-J2 / WS-E-POLISH — tails
> D-VISUAL: powerup visuals on the spike. J2: collision FX from live contacts. E-POLISH: vehicle
> lighting (ledger #9) + HUD on the spike. After the physics+AI race is verified.

---
## Recommended wave-10
**10a (MAIN tree, the C4 push — serialized on the booted .asi):** WS-PHYS-C4-A5 -> A6A -> A6B+A3
(each reuses the A4 lane template; this closes the physics C4 debt fn-by-fn).
**10b (MAIN tree, standalone runtime — interleave):** WS-PHYS-SMOKE-STEER (+ grip-const fix) +
WS-AI-VERIFY. Then tails. GOAL: the whole physics chain C4 + a controllable real-physics race
with live AI opponents — physics provably bit-identical to the original, fn-by-fn.
