# WS-PHYS-C4-A6A — A6a (FUN_00467650) verbatim-LUT installed-hook lane (2026-06-17)

Branch `ws-phys-c4-a6a`. Extends the proven A4/A5 `.asi` verbatim-LUT installed-hook
C4 telemetry lane (`mashedmod/src/mashed_re/Vehicle/PhysicsChainHooks.cpp`) to A6a =
FUN_00467650, the velocity / angular-velocity integration step (the big x87 float10
body). Anchored MASHED.exe SHA-256 BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.

## Verdict: RED — 93/96 bit-identical (NOT promoted to C4; NO overclaim).

In-process self-test (`MASHED_PHYS_C4_SELFTEST=1`, A6a inline-JMP LIVE, canonical
Arctic Quick-Battle race, `re/frida/phys_c4_telemetry.py hooked A6a_Entry,0x00467650`):
**93/96 calls bit-identical** (88 drive-force calls in4=255 gm=6 all-grounded + 8 coast).
Evidence: `re/analysis/phys_c4_evidence/A6a_selftest_93of96.txt` (reproduced across 2 races).

The 3 residual calls (24/82/84) diverge by **1-8 ULP in the per-wheel suspension
force X (`piVar12[0x1c]`, telemetry `wNfx`)** and one dependent `+0x9c0` (angVelY).
This is the documented **[U-A6A-FLOAT10]** class (WS-A-VERIFY-3): the original keeps
`local_98`/`local_bc`/`f5`/`f4` in x87 registers as float10 across the suspension-force
computation+store (disasm 0x468127..0x468337), and a C transcription rounds those
intermediates to float32 at different points. The X axis exposes it (the drive-dir X
`piVar12[0x1f]` is the significant component; the near-zero Y always rounded identically).
Resolution path (deferred, high-risk): a full naked-asm shim of the
`if (kSpeedMin < le4 && grounded)` suspension block keeping `local_bc`/`local_98`/`f5`/`f4`
float10 exactly as the original (the same technique the GearCvtCompute/DriveForceAccum/
Accum60/AccumD0Num/FrictionUpdate/LinVelFactor shims already use for the closed chains).

## [U-A6A-ST0] — RESOLVED
A6a calls FUN_004a2c48 (round-ST0-to-i64) at **7 sites** with an implicit ST0 input not
in the decompilation. Disassembled each call's exact x87 sequence (Mashed_pool12, RO) and
reproduced it with naked shims that load ST0 identically then forward to the LIVE
FUN_004a2c48 (so the round is bit-identical):
- #1 0x4679ca `FILD[+0x494];FSUB dt`  -> RoundIntSubDt  (upshift-timer decrement, clamp <0)
- #2 0x4679f8 `FILD[+0x494];FADD dt`  -> RoundIntAddDt  (downshift-timer increment, clamp >0)
- #3 0x467cfe `FILD[+0xbf4];FSUB dt`  -> RoundIntSubDt  (boost ctr, gm==6)
- #4 0x467dc6 `FILD[+0xbf4];FSUB dt`  -> RoundIntSubDt  (boost-state 1)
- #5 0x467e25 `FILD[+0xbf4(spill)];FSUB dt` -> RoundIntSubDt (boost-state 2)
- #6 0x467e7e `(w[0x1f]*velx+w[0x20]*vely+w[0x21]*velz)/(w[-0xa]*1.74533)` -> RoundDriveProj
- #7 0x467f93 `FILD[input[5]];FADD 256.0` -> RoundBrakeInput (wheel>1 brake quantize)
All ST0 inputs verified against the disassembly; the FSUB of the spilled `dt`/`.rdata`
const is a bit-exact float32 subtract.

## Bugs found + fixed by the A/B (the lane's value)
1. **Structural — drive-dir dot association (the big one, 58 RED -> 3 RED):** the
   suspension dot `local_c0` is `fwd.z*(inv*ld8) + fwd.x*s0 + fwd.y*s1` where `s2=inv*ld8`
   is one pre-computed value (ASM 0x4681c1 `FMUL ST2`), reused by `local_a4 = s2 - local_c0`.
   The decompiler rendered it `fwd.z*fVar5*local_d8` (ambiguous), and the first transcription
   used `(fwd.z*fInv)*ld8` (wrong association). Fixed -> `s2` pre-computed and reused.
2. **Exact-bit .rdata constants:** a literal-vs-.rdata sweep of all 38 A6a constants found
   **15 of the decimal literals in the standalone Integrate2.cpp mis-round** (e.g.
   `_DAT_005cea2c` 299488.0 decimal -> 0x48923c00 but the real bits are 0x48927c00;
   `_DAT_005cea34`/`_DAT_005cea38`/`_DAT_005cc990`/`_DAT_005cea1c`/...). The `.asi` hook uses
   `Cf(0x........)` exact bit patterns (memory_read pool12).
3. **float10 accumulator collapse [U-A6A-FLOAT10] (closed for 4 of 5 chains):** the original
   keeps several `a*b+c` accumulators in float10 (ST) and rounds to float32 only on the FSTP.
   Naked shims now reproduce this exactly for: the gear/CVT `local_cc` precompute
   (GearCvtCompute — keeps fVar5/fVar3 float10 across the 5-candidate loop; this alone took
   79->93), the drive-force product (DriveForceAccum — `local_e4*local_cc` float10 across XYZ),
   the boost-state-1 force (reuses DriveForceAccum), the friction-torque divide+update
   (FrictionUpdate), the `local_60`/`local_d0` magnitude accumulators (Accum60/AccumD0Num —
   keep the LUT magnitude's ST0 float10), and the linear-vel redistribution (LinVelFactor).
   The ONE remaining un-shimmed float10 chain = the suspension force block (residual above).

## Lane mechanics (reused A4/A5 template)
- `A6a_Entry` naked trampoline at 0x00467650: ESI=record + 4 cdecl stack args
  (param_1,dt,param_3,input); preserves EBX/EBP/EDI/ESI; caller-cleans.
- `OrigA6aTrampoline`: re-executes `SUB ESP,0xe4` then `push 0x00467656; ret` (clobber-free,
  preserves ESI=record) to run the LIVE original body bypassing our inline-JMP for the A/B.
- All LUT callees forwarded to live originals (FUN_004c3ac0/39b0/3df0/4c4d20, FUN_0040e350/
  0040e340) so the running MASHED's RW fast-sqrt LUT executes -> bit-identity reachable.
- In-process A/B (zero Frida overhead on this >1000/s hot path): snapshot 0xd04 record, run
  original, capture writes, restore, run mine, bit-compare. No RNG / no shared-global writes
  in A6a -> no snapshot/exclusion needed beyond the record.

## Coverage notes (honest)
- gm=6 Quick Battle, all-4-grounded (`+0x9e0`==0x40800000) every sampled call; coast (in4=0)
  8 calls. NOT exercised in-race (transcribed verbatim, unverified): the airborne path
  (`+0x9e0 != 0x40800000` -> the divide-by-dt suspension branch is absent here since A6a's
  suspension is grounded-gated), the helicopter type-C fwd-align drive branch (Arctic cars
  are not type-C `-0x69e1a6`/`-0xe17f4c`), the brake branch (input[5]!=0 && input[4]==0 — the
  throttle-only demo never brakes; BrakeForceAccum + call#7 transcribed, unhit), the spinning
  power-up vel-damp (`tId==-1`/`-0x373738`), and the boost-state machine (`+0xbf8`==1/2).

## [U-A6A-FLOAT10] — RESOLUTION FULLY SPECIFIED (2026-07-01)
Disassembled the wheel-force store block (0x004682a7..0x00468337, branch1 local_94&0x100==0)
+ the high-load else (0x00468240..0x004682a5). The SOLE residual: the original keeps **f5 in
an x87 register (float10) across the three products** local_ac*f5 (0x4682fc), local_a8*f5
(0x46830a), f5*local_a4 (0x468318), rounding to f32 only on the FSTP into wheel[0x1c/1d/1e]
(0x468327/0x468331/0x468337). The C transcription (PhysicsChainHooks.cpp:1753/1760/1764)
stores f5 to a `float` first → the products use f32 f5 → 1-8 ULP on wNfx (local_ac path is the
significant one; the near-zero Y rounds identically).

Key stack slots (branch1): [ESP+0x28]=local_c8 [0x2c]=local_c4 [0x30]=local_c0 [0x34]=local_bc
[0x1c]=local_d4 [0x44]=local_ac [0x48]=local_a8 [0x4c]=local_a4 [0x50/54/58]=local_a0/9c/98.
Exact op order (branch1, local_94==0 sub-path):
  local_98 = pv[0x16]*pv[0x1b]*SuspScale   (ST, 0x4682ab-b6)
  local_a0 = local_c8*local_98 (FSTP f32); local_9c = local_c4*local_98 (FSTP f32);
  local_98 = local_98*local_c0 (FSTP f32)  (0x4682b7-d2)
  f4 = local_d4*speed (ST); if (f4 < 0x005cd120) f5 = f4*0x005ce18c*local_bc  ELSE f5 = local_bc
       -- f5 stays in ST (0x4682d3-fb)
  local_a0 = local_ac*f5 + local_a0  (FSTP f32, f5 reused from ST1)   0x4682fc
  local_9c = local_a8*f5 + local_9c  (FSTP f32, f5 from ST1)          0x46830a
  ST0 = f5*local_a4 + local_98        (kept in ST, NOT stored)         0x468318
  wheel[0x1c]=local_a0+wheel[0x1c]; wheel[0x1d]=local_9c+wheel[0x1d];
  wheel[0x1e]=ST0+wheel[0x1e]         0x468320-337

FIX = one naked-asm shim (pattern: the existing GearCvtCompute/DriveForceAccum/Accum60 shims in
PhysicsChainHooks.cpp) reproducing 0x4682a7..0x468337 with f5 in ST across the 3 products, then
re-run `re/frida/phys_c4_telemetry.py hooked A6a_Entry,0x00467650` (self-test) → expect 96/96
GREEN → re-classify A6a C2->C4. All other A6a divergence classes already closed (this is the
last one). A3/A4/A5 already C4; A6b C3 (needs a natural-airborne track for canonical C4).
