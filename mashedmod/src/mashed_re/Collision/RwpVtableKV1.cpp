// Mashed RE — B5e lane-end KV, BATCH B (PARTIAL): KV1 scene-callback group.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-25). Style/idiom follows RwpSolverGlue5.cpp (K5) and
// RwpVtableKV2.cpp (KV Batch A).
//
// SCOPE — this TU ports ONE of the four Batch-B candidates. Read the deferral note below
// before adding the other three; they are NOT safe to transcribe from decomp as-is.
//
// ---------------------------------------------------------------------------------------
// WHY ONLY 0x0056a450 (deferral rationale — do not "finish" this TU without disasm work)
// ---------------------------------------------------------------------------------------
// 0x0056adb0, 0x0056aae0 and 0x0056ac40 all build their OUTGOING call frames on the stack,
// and Ghidra mis-models them:
//   * FUN_0056adb0's decomp calls `FUN_0056ac40()` with NO arguments at both call sites,
//     while assigning `uStack_e0 = 0x56af69` / `0x56b16d` — those are RETURN ADDRESSES, i.e.
//     the "locals" local_14..local_40 / piStack_dc / piStack_d8 ARE the outgoing frame.
//   * FUN_0056aae0 REP MOVSDs 0x22 words from `&stack0x0000003c` into `auStack_a0` before
//     `FUN_0056a7a0(param_3,param_4,param_5)` — again an outgoing frame, so that call's real
//     arity is 3 + 34 words, not 3.
//   * FUN_0056ac40 itself reads `in_stack_00000024/30/3c`, so its true arity exceeds the 4
//     params Ghidra recovered.
// This is the documented `feedback_ghidra_prebranch_args` pitfall. Transcribing these from
// the decomp would emit calls with garbage arguments that still COMPILE. Each needs its
// call-site pushes recovered from disassembly first. Deferred deliberately.
//
// 0x0056a450 has NO callees at all (plate `callees_depth1: []`, confirmed: the decomp
// contains no CALL), so it carries none of that risk.
//
// ---------------------------------------------------------------------------------------
// FRAME (NO-GUESSING, cross-checked two independent ways)
// ---------------------------------------------------------------------------------------
// 0x0056a450 is the scene `+0xf4` callback (installed at 0x0055fd9c when `[scene+0x58]==2`;
// the else-branch twin is 0x00569140 @0x0055fdb5 — see re/analysis/b5e/island_vtable_reach.md
// §1.3). It is invoked ONCE, at 0x560a9c, via `CALL [ESP+0x14c]` with a 53-dword by-value
// frame fully specified in re/analysis/b5e/K13_PORT_RECON_2026-07-18.md:115-124.
//
// Cross-check that pins the mapping: for a __cdecl callee with no entry pushes, the arg at
// `in_stack_000000NN` is frame word `NN/4`, i.e. zero-based index `NN/4 - 1`. The DEEPEST
// slot this function reads is `in_stack_000000d4` -> index 52 -> word 53 of 53 — exactly the
// K13 frame length, and word 53 is `param_3[0x49]`, the value used as the reciprocal
// `1.0 / x` (the mass-matrix diagonal). Two independent sources agree, so the frame binding
// is not an inference.
//
//   frame word (K13)                      | in_stack_ | index | use here
//   --------------------------------------|-----------|-------|------------------------
//   w1  param_5[0x25]                     | 0x04      |  0    | param_1 (out accumulator)
//   w2  param_5[0x26]                     | 0x08      |  1    | param_2 (body count)
//   w3  param_5[0x27]                     | 0x0c      |  2    | param_3 (UNUSED by body)
//   w4  param_5[0]                        | 0x10      |  3    | param_4 (jacobian base)
//   w14 param_5[0xa]                      | 0x38      | 13    | index table
//   w20 param_5[0x10]                     | 0x50      | 19    | per-group count (int*)
//   w35 param_5[0x1f]                     | 0x8c      | 34    | group count
//   w38 param_3[4]                        | 0x98      | 37    | LDL source A
//   w41 param_3[7]                        | 0xa4      | 40    | LDL source B
//   w44 param_3[0xa]                      | 0xb0      | 43    | LDL cursor (float*)
//   w47 param_3[0x46]                     | 0xbc      | 46    | inertia block cursor
//   w50 param_3[0x43]                     | 0xc8      | 49    | jacobian block (float*)
//   w53 param_3[0x49]                     | 0xd4      | 52    | diagonal -> reciprocal
//
// K13 already calls this slot as `((void(__cdecl*)(Kv11Frame))param_11)(f)` with
// `struct Kv11Frame { int w[53]; }` (RwpSolverPartition13.cpp:52,309-318), so taking the
// frame by value here is ABI-identical to the original's stack layout AND matches the
// existing ported call site exactly.
//
// NO-GUESSING notes:
//   1. The original mutates its own incoming stack slots (`in_stack_000000d4` becomes the
//      reciprocal; `bc`/`c8`/`50`/`38` are advanced as cursors). Those are callee-owned
//      stack copies, so they are reproduced here as LOCALS seeded from the frame — same
//      observable behavior, and it avoids writing through the caller's frame.
//   2. `_DAT_005cc320` is the float 1.0 constant (cited in the 0x0056a450 plate); bound by
//      absolute address rather than hard-coded so the bit pattern comes from the binary.
//   3. param_3 (w3) is read into the signature for ABI completeness but never used by the
//      body — matching the decomp exactly. Do not "optimize" it away.
//   4. Every float expression below preserves the decomp's operand and summation order.
//      The 6-term row sums are left in printed order because this function is a candidate
//      for veccap Unicorn verification (see re/tools/veccap/) — that harness, NOT a build,
//      is what will prove x87 faithfulness. Until it passes, treat this port as UNVERIFIED
//      (the VECCAP-2 lesson: building clean proves nothing about bit-identity).
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

typedef unsigned int undefined4;

// The 53-dword p11 callback frame (K13_PORT_RECON:115-124). Layout must stay in lockstep
// with RwpSolverPartition13.cpp's Kv11Frame.
struct Kv11Frame { int w[53]; };

// 0x005cc320 = float 1.0 (0x0056a450 plate, "Constants").
#define MASHED_DAT_005cc320 (*reinterpret_cast<const float*>(0x005cc320u))

// ---------------------------------------------------------------------------
// 0x0056a450  scene +0xf4 callback (scene+0x58 == 2 branch). Modified Cholesky / LDL^T
//             apply: inverts the mass-matrix diagonal, zeroes the output accumulator, then
//             for each island group accumulates M^-1 * J^T * impulse into the per-body
//             4-vectors, rescales the jacobian rows by the inverse diagonal, and applies
//             the 3x3 inertia block to the angular half. Closes with the LDL^T forward
//             elimination / back-substitution sweep over param_2 bodies.
//             Leaf: no callees.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056a450(Kv11Frame f)
{
  float *pfVar1;
  float *pfVar2;
  float  fVar3,fVar4,fVar5,fVar6,fVar7,fVar8,fVar9,fVar10,fVar11,fVar12,fVar13,fVar14,fVar15;
  float  fVar16,fVar17,fVar18,fVar19,fVar20,fVar21,fVar22,fVar23,fVar24,fVar25,fVar26,fVar27;
  float  fVar28,fVar29,fVar30,fVar31,fVar32,fVar33,fVar34,fVar35,fVar36,fVar37,fVar38,fVar39;
  undefined4 *puVar40;
  float *pfVar41;
  float *pfVar42;
  int    iVar43;
  int    local_150;
  int    local_14c;

  // --- incoming frame -> named slots (note 1; index = in_stack_offset/4 - 1) ---
  undefined4 *param_1            = reinterpret_cast<undefined4 *>(f.w[0]);   // 0x04
  int         param_2            = f.w[1];                                    // 0x08
  int         param_4            = f.w[3];                                    // 0x10
  int         in_stack_00000038  = f.w[13];
  int        *in_stack_00000050  = reinterpret_cast<int *>(f.w[19]);
  int         in_stack_0000008c  = f.w[34];
  int         in_stack_00000098  = f.w[37];
  int         in_stack_000000a4  = f.w[40];
  float      *in_stack_000000b0  = reinterpret_cast<float *>(f.w[43]);
  int         in_stack_000000bc  = f.w[46];
  float      *in_stack_000000c8  = reinterpret_cast<float *>(f.w[49]);
  // raw bits reinterpreted, NOT an int->float conversion (the slot holds a float).
  float       in_stack_000000d4  = *reinterpret_cast<const float *>(&f.w[52]);

  in_stack_000000d4 = MASHED_DAT_005cc320 / in_stack_000000d4;               // 1.0 / diagonal

  puVar40 = param_1;
  for (iVar43 = param_2; iVar43 != 0; iVar43 = iVar43 + -1) {
    *puVar40 = 0;
    puVar40[1] = 0;
    puVar40[2] = 0;
    puVar40[3] = 0;
    puVar40 = puVar40 + 4;
  }

  local_14c = 0;
  if (in_stack_0000008c != 0) {
    in_stack_000000bc = in_stack_000000bc + 0x20;
    local_150 = in_stack_0000008c;
    do {
      iVar43 = 0;
      if (*in_stack_00000050 != 0) {
        fVar3  = *in_stack_000000c8;
        fVar4  = in_stack_000000c8[1];
        fVar5  = in_stack_000000c8[2];
        fVar6  = in_stack_000000c8[4];
        fVar7  = in_stack_000000c8[5];
        fVar8  = in_stack_000000c8[6];
        fVar9  = *(float *)(in_stack_000000bc + -0x20);
        fVar10 = *(float *)(in_stack_000000bc + -0x1c);
        fVar11 = *(float *)(in_stack_000000bc + -0x18);
        fVar12 = *(float *)(in_stack_000000bc + -0x14);
        fVar13 = *(float *)(in_stack_000000bc + -0xc);
        fVar14 = *(float *)(in_stack_000000bc + -8);
        fVar15 = *(float *)(in_stack_000000bc + 8);
        pfVar41 = (float *)(local_14c * 0x60 + 0x20 + param_4);
        do {
          fVar16 = pfVar41[-8];   fVar17 = pfVar41[-7];   fVar18 = pfVar41[-6];
          fVar19 = pfVar41[-5];   fVar20 = pfVar41[-4];   fVar21 = pfVar41[-3];
          fVar22 = pfVar41[-2];   fVar23 = pfVar41[-1];   fVar24 = *pfVar41;
          fVar25 = pfVar41[1];    fVar26 = pfVar41[2];    fVar27 = pfVar41[3];
          fVar28 = pfVar41[4];    fVar29 = pfVar41[5];    fVar30 = pfVar41[6];
          fVar31 = pfVar41[7];    fVar32 = pfVar41[8];    fVar33 = pfVar41[9];
          fVar34 = pfVar41[10];   fVar35 = pfVar41[0xb];  fVar36 = pfVar41[0xc];
          fVar37 = pfVar41[0xd];  fVar38 = pfVar41[0xe];  fVar39 = pfVar41[0xf];

          pfVar42 = (float *)(param_1 + *(int *)(in_stack_00000038 + iVar43 * 4) * 4);
          *pfVar42     = fVar16 * fVar3 + fVar20 * fVar4 + fVar24 * fVar5 + fVar28 * fVar6 +
                         fVar32 * fVar7 + fVar36 * fVar8 + *pfVar42;
          pfVar42[1]   = fVar17 * fVar3 + fVar21 * fVar4 + fVar25 * fVar5 + fVar29 * fVar6 +
                         fVar33 * fVar7 + fVar37 * fVar8 + pfVar42[1];
          pfVar42[2]   = fVar18 * fVar3 + fVar22 * fVar4 + fVar26 * fVar5 + fVar30 * fVar6 +
                         fVar34 * fVar7 + fVar38 * fVar8 + pfVar42[2];
          pfVar42[3]   = fVar19 * fVar3 + fVar23 * fVar4 + fVar27 * fVar5 + fVar31 * fVar6 +
                         fVar35 * fVar7 + fVar39 * fVar8 + pfVar42[3];

          pfVar41[-8]  = fVar16 * fVar12;
          pfVar41[-7]  = fVar17 * fVar12;
          pfVar41[-6]  = fVar18 * fVar12;
          pfVar41[-5]  = fVar19 * fVar12;
          pfVar41[-4]  = fVar20 * fVar12;
          pfVar41[-3]  = fVar21 * fVar12;
          pfVar41[-2]  = fVar22 * fVar12;
          pfVar41[-1]  = fVar23 * fVar12;
          *pfVar41     = fVar24 * fVar12;
          pfVar41[1]   = fVar25 * fVar12;
          pfVar41[2]   = fVar26 * fVar12;
          pfVar41[3]   = fVar27 * fVar12;

          pfVar41[4]   = fVar28 * fVar9  + fVar32 * fVar10 + fVar36 * fVar11;
          pfVar41[5]   = fVar29 * fVar9  + fVar33 * fVar10 + fVar37 * fVar11;
          pfVar41[6]   = fVar30 * fVar9  + fVar34 * fVar10 + fVar38 * fVar11;
          pfVar41[7]   = fVar31 * fVar9  + fVar35 * fVar10 + fVar39 * fVar11;
          pfVar41[8]   = fVar28 * fVar10 + fVar32 * fVar13 + fVar36 * fVar14;
          pfVar41[9]   = fVar29 * fVar10 + fVar33 * fVar13 + fVar37 * fVar14;
          pfVar41[10]  = fVar30 * fVar10 + fVar34 * fVar13 + fVar38 * fVar14;
          pfVar41[0xb] = fVar31 * fVar10 + fVar35 * fVar13 + fVar39 * fVar14;

          local_14c = local_14c + 1;                      // ordered as the decomp prints it

          pfVar41[0xc] = fVar28 * fVar11 + fVar32 * fVar14 + fVar36 * fVar15;
          pfVar41[0xd] = fVar29 * fVar11 + fVar33 * fVar14 + fVar37 * fVar15;
          pfVar41[0xe] = fVar30 * fVar11 + fVar34 * fVar14 + fVar38 * fVar15;
          pfVar41[0xf] = fVar31 * fVar11 + fVar35 * fVar14 + fVar39 * fVar15;

          pfVar41 = pfVar41 + 0x18;
          iVar43 = iVar43 + 1;
        } while (iVar43 != *in_stack_00000050);
      }
      iVar43 = *in_stack_00000050;
      in_stack_000000c8 = in_stack_000000c8 + 8;
      in_stack_000000bc = in_stack_000000bc + 0x30;
      in_stack_00000050 = in_stack_00000050 + 1;
      in_stack_00000038 = in_stack_00000038 + iVar43 * 4;
      local_150 = local_150 + -1;
    } while (local_150 != 0);
  }

  if (param_2 != 0) {
    in_stack_00000098 = in_stack_00000098 - (int)in_stack_000000b0;
    in_stack_000000a4 = in_stack_000000a4 - (int)in_stack_000000b0;
    iVar43 = (int)param_1 - (int)in_stack_000000b0;
    local_150 = param_2;
    do {
      pfVar41 = (float *)(in_stack_00000098 + (int)in_stack_000000b0);
      fVar3  = pfVar41[1];
      fVar4  = pfVar41[2];
      fVar5  = pfVar41[3];
      fVar6  = in_stack_000000b0[1];
      fVar7  = in_stack_000000b0[2];
      fVar8  = in_stack_000000b0[3];
      pfVar42 = (float *)(in_stack_000000a4 + (int)in_stack_000000b0);
      fVar9  = pfVar42[1];
      fVar10 = pfVar42[2];
      fVar11 = pfVar42[3];
      pfVar1 = (float *)(iVar43 + (int)in_stack_000000b0);
      fVar12 = pfVar1[1];
      fVar13 = pfVar1[2];
      fVar14 = pfVar1[3];
      pfVar2 = (float *)(iVar43 + (int)in_stack_000000b0);
      *pfVar2   = *pfVar1 - (*pfVar42 - *in_stack_000000b0 * *pfVar41 * in_stack_000000d4) *
                            in_stack_000000d4;
      pfVar2[1] = fVar12 - (fVar9  - fVar6 * fVar3 * in_stack_000000d4) * in_stack_000000d4;
      pfVar2[2] = fVar13 - (fVar10 - fVar7 * fVar4 * in_stack_000000d4) * in_stack_000000d4;
      pfVar2[3] = fVar14 - (fVar11 - fVar8 * fVar5 * in_stack_000000d4) * in_stack_000000d4;
      in_stack_000000b0 = in_stack_000000b0 + 4;
      local_150 = local_150 + -1;
    } while (local_150 != 0);
  }
  return;
}

// --- gta-reversed-style hook registration — KV BATCH B (partial). ---
RH_ScopedInstall(FUN_0056a450, 0x0056a450);

}  // namespace Collision
}  // namespace mashed_re
