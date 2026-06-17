# WS-PHYS-C4-A6B — A6b (FUN_00468980) verbatim-LUT installed-hook lane (2026-06-17)

Branch `ws-phys-c4-a6b-a3`. Extends the proven A4/A5/A6a `.asi` verbatim-LUT
installed-hook C4 telemetry lane (`mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp`)
to A6b = FUN_00468980, the aerodynamic-stabilization step. Anchored MASHED.exe
SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.

## Verdict: grounded gate C4-GREEN 4/4; airborne aero body UNEXERCISED -> A6b stays C2 (NO overclaim).

A6b's WHOLE body is gated on `+0x9e0 == 0.0` (zero wheels grounded = fully
airborne). In the canonical Arctic Quick-Battle race the player car is
**all-4-grounded for every frame** (`+0x9e0 == 0x40800000 == 4.0`; telemetry
`groundedCnt` = 1082130432 for all 179 samples). So the only canonically-reachable
path is the grounded SHORT-CIRCUIT (`FCOMP 0.0; JP -> ADD ESP,0x20; RET`), which
the in-process A/B verifies **4/4 bit-identical** (`A6b_selftest_grounded_4of4.txt`).
The aerodynamic auto-level body (the function's actual work) was transcribed
verbatim but **NOT exercised** — per [[feedback-no-overclaiming-c-levels]] this is
NOT a C4 datapoint (it would be claiming C4 for a path that does nothing). A6b
stays **C2**.

This is the same coverage class A5 documented (all-4-grounded so the airborne
branch never fired), but for A6b the airborne gate guards the ENTIRE body, so
grounded-only coverage leaves the function's real work unverified.

## ABI — RESOLVED (the wall the decomp couldn't cross)
Disassembled the A4 dispatch site (0x00470943, Mashed_pool12 RO 2026-06-17):
`MOV ESI,[ESP+0x3c]` (xform = A4's param_4 world matrix); `PUSH EBP`(input,UNUSED by
A6b); `PUSH EBX`(dt); `MOV ECX,EDI`(record); `CALL 0x00468980`. So:
- **ECX = record**, **ESI = xform** (the per-vehicle world RwMatrix, NOT the record),
  one float stack arg **[ESP+4] = dt** (param_2). The decomp's `unaff_ESI` = the
  xform matrix; `ESI[9]`=at.y, `ESI[1]`=right.y.
- NOTE: the lane's own `A4_Body` `Call_A6b` set ESI=record — a latent dispatch bug
  there (A4 is C4 via its body-math self-test which stops BEFORE the dispatch tail;
  not used by this A6b self-test, which uses the correct ECX=record/ESI=xform ABI).

## Callee ABIs — RESOLVED
- **FUN_004a3384** = MSVC CRT `acos(double)` (fpatan-based; reads FPU control word).
  Call site: `SUB ESP,8; FSTP double [ESP]; CALL` -> result in ST0 (float10).
  Forwarded to the LIVE original (PCH_Fwd_4a3384) so the CRT FPU-state checks +
  rounding are bit-exact.
- **FUN_004c4d20** = `__cdecl undefined4 FUN_004c4d20(out_mtx, float* axis, float
  angle_deg, int mode)` = RwMatrixRotate-equivalent. Decompiled (0x004c4d20..
  0x004c4dba): internally calls FUN_004c3b90 (inv-sqrt LUT) + fsin/fcos + FUN_004c4a50;
  it ROUTES THROUGH THE RW DEVICE/LUT, so it is forwarded to the LIVE original
  (PCH_Fwd_4c4d20). Preconcats an axis-angle rotation onto the world matrix in place.
- **FUN_004c39b0** = Vec3 normalize (inv-sqrt LUT) — forwarded live (PCH_Fwd_4c39b0).

## Exact-bit constants caught (the lane's value, even pre-coverage)
The standalone `Vehicle/AeroStabilize.cpp` transcription has WRONG constants:
- it uses `kAcosM = -2.0` for `_DAT_005ccae0`; the real .rdata is the **double
  0x404ca5dcc0000000 = 57.29578 = 180/pi** (memory_read).
- it uses `kBias0 = 0.0` for the pitch bias; the real `_DAT_005ccad8` is the
  **double 90.0** (0x4056800000000000) — both passes subtract 90 (pitch via the
  double `_DAT_005ccad8`, roll via the FLOAT `_DAT_005ccad0` = 0x42b40000 = 90.0).
- dt scale `_DAT_005cc558` = 0x3a83126f (0.001); vel-align `_DAT_005cc9a0` = 0.05
  (0x3d4ccccd). Basis `_DAT_006146fc`=(0,1,0) so the dots reduce to at.y / right.y.
The `.asi` hook is a NAKED verbatim transcription of 0x00468980..0x00468b34 using
these exact bit patterns and forwarding the 3 CALLs live, so it is bit-identical by
construction; the only thing missing is an airborne frame to run the body on.

## float10 chain (transcribed verbatim, awaiting airborne coverage)
The angle keeps the acos ST0 result in x87 float10 across `*180/pi` (double FMUL),
`-90` (double/float FSUB), FCHS (pitch only), `*dts` (float FMUL) before the single
FSTP-to-float32. Reproduced exactly in the naked body; built x87 (.asi TU, no /arch:SSE2).

## How to close the gap (deferred)
The available canonical harness (`re/frida/phys_c4_telemetry.py` + nav_agent.js)
only overrides one control (4=accelerate) for player 0 — no steering, so the car
can't be driven off Arctic's ramps. Closing A6b needs a scenario that drives the
car airborne (`+0x9e0 -> 0.0`): a multi-control race recipe or a track with a
guaranteed early jump. Synthetic injection of `+0x9e0=0.0` would be C3 at best
(NOT canonical) and is explicitly avoided.
