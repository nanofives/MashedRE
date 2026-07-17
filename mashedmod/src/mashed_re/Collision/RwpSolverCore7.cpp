// Mashed RE — B5e: RwpSolver-island CLUSTER 7 clean-room port (the 0x0056c group-B stage:
// per-island body/jacobian transforms + the accumulate/scatter and index-build passes).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool14, read_only, 2026-07-17). VERBATIM transcription of the 5 K7 functions from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm before
// porting. Style/idiom follows RwpSolverLeaves1.cpp (K1) .. RwpSolverIntegrate6.cpp (K6):
// extern "C" per-function, // 0x00xxxxxx RVA comments, cluster-internal forward decls,
// RH_ScopedInstall registration block.
//
// Members (rva:size): 0056c580:861 0056c8e0:434 0056caa0:1262 0056cf90:216 0056d070:724
//   — 5 fns / 3497 B, deps DONE (no cross-cluster deps; within-cluster: caa0->c8e0, d070->cf90),
//   0 indirect-call sites (island_dag ind=0 for K7; confirmed via function_callees — c580/c8e0/
//   cf90 are leaves, caa0 calls only c8e0, d070 calls only cf90).
//
// NO-GUESSING verifications against live pool14 disasm (2026-07-17):
//  1. Calling convention: all 5 end in a plain RET (0xC3, caller cleanup) => __cdecl. RET bytes:
//     0x0056c8dc, 0x0056ca91, 0x0056cf8d, 0x0056d067, 0x0056d343 (each preceded by the
//     POP EBX/ADD ESP,imm epilogue; no RET-imm anywhere).
//  2. FUN_0056cf90 is a pure 24-dword integer copy (all MOV, no x87). Every source offset was
//     checked against disasm 0x0056cf90..0x0056d064 and MATCHES the decomp EXACTLY:
//     param_1[0..3] gather the +0x20-strided rows ((p3+0/2/4/6)*0x20+p2); param_1[4..0x17] read
//     iVar1+{4,0x44,0x84,0xc4 | 8,0x48,0x88,0xc8 | 0x10,0x50,0x90,0xd0 | 0x14,0x54,0x94,0xd4 |
//     0x18,0x58,0x98,0xd8}. 24 dwords -> a 0x60-byte record (matches d070's iVar9*0x60 callee
//     stride). Transcribed verbatim with undefined4 casts (bit copy, no float interpretation).
//  3. FUN_0056d070 is pure integer/pointer index-build (counts, prefix sums, scatter of the
//     0x2c/0x0c pair fields, calls FUN_0056cf90 to gather each 0x60 record). No x87 in the body;
//     transcribed verbatim. Ghidra `param_9 != (int *)(*(uint*)... >> 2)` loop bound and the
//     `param_1 = (int*)((int)param_1 + -1)` down-counter kept literally.
//  4. FUN_0056caa0 float ops are ALL 2-term adds (`local_buf[i] + dst[i]` accumulate) — a single
//     FADD each, no associativity freedom, bit-exact by construction. The four 8-dword gather
//     loops (`for iVar=8`) copy contiguous float[8] stack blocks (Ghidra split each as
//     local_80[4]+local_70+local_6c+local_68 etc — same contiguous-block class as K3
//     FUN_0055c2d0 / K4 FUN_005646c0); ported as float[8] with local_70=buf[4], local_6c=buf[5],
//     local_68=buf[6]. Only [0],[1],[2],[4],[5],[6] are read back (the +0xc / +0x1c holes are the
//     w-lanes, left untouched — matching the decomp's `+0xc/+0x1c = 0` zero-stores at c... and the
//     read-back skipping index 3/7). The two c8e0 call sites keep the decomp's 4-arg __cdecl order.
//  5. FUN_0056c580 / FUN_0056c8e0 are deeply-interleaved x87 (3 parallel accumulators FSTP'd to
//     32-bit float slots per partial — [ESP+0x84/0x88/0x8c] in c580, [ESP+0x10/0x14/0x18] in
//     c8e0), so the printed FADD associativity is NOT trustworthy. Re-traced the accumulator +
//     FADDP chains on pool14 (2026-07-17). Findings APPLIED below:
//       (a) c580 fVar5 = (p[-1]^2 + p[3]*p[0]) + p[7]*p[1]  — matches printed left-assoc
//           (acc initialised with p[-1]^2 @0x0056c5d2, FLD acc/FADD ST(product) @c603..c60f then
//           @c63c..c64b). KEPT.
//       (b) c580 fVar2 = p[8]*p[1] + (p[4]*p[0] + p[-1]*p[0]) and
//           c580 fVar3 = p[9]*p[1] + (p[5]*p[0] + p[1]*p[-1]) — the accumulators [ESP+0x88]/[0x8c]
//           are PRODUCT-initialised (@c5ea/c5f7 with the t3 term), the t2 product FADD'd in
//           (@c61b/c62e giving t2+t3), then the t1 product FADD'd (@c663/c682 giving t1+(t2+t3)).
//           This DIFFERS from the decomp's printed `(t1+t2)+t3`. CORRECTED below.
//       (c) c580 three matrix-row stores *(iVar13+0x10/0x14/0x18+param_1) =
//           A*M10 + (B*M14 + C*M18)  — the FADDP chain (@c7d2..c80f / c7f0..c80f / c813..c832)
//           combines the M14 and M18 products first, then adds the M10 product; DIFFERS from the
//           printed `A*M10 + C*M18 + B*M14`. CORRECTED below.
//       (d) c580 fVar4-scaled stores (*(iVar13+0/4/8+param_1) and pfVar14[-2/-1/0]) are single
//           FMULs — no grouping. KEPT.
//     REMAINING deep-stack sums (5+-deep interleaved ST stack, not cleanly hand-traceable):
//       c580 fVar7/fVar1/fVar6/fVar8/fVar9/fVar10 accumulators and the pfVar14[2/3/4] stores;
//       ALL of c8e0's 5-term row stores. These retain the decomp's printed data-flow + order and
//       carry the accepted <=1-ULP x87 partial-rounding floor (each partial FSTP'd to float32),
//       to be settled at the B5e lane-end whole-loop per-field body-state diff (NOT a C2 gate;
//       project_phys_chain_float10_methodology). Data flow (which value multiplies which, which
//       offset feeds which store) was cross-checked against disasm and matches.
//
// x87 note: build is x87 (no /arch:SSE2); plain-float product/sum expressions below accumulate in
// 80-bit ST and round on store. The original's per-partial float32 FSTP rounding in c580/c8e0 is
// the source of the <=1-ULP floor noted in 5; behavioural (C2) parity is unaffected.
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

// --- Ghidra scalar types kept verbatim (as RwpSolverLeaves1.cpp). ---
typedef unsigned int   uint;
typedef unsigned int   undefined4;

// --- Cluster-internal forward decls (caa0 calls c8e0, d070 calls cf90). ---
extern "C" void __cdecl FUN_0056c8e0(int param_1,int param_2,float *param_3,int param_4);
extern "C" void __cdecl FUN_0056cf90(undefined4 *param_1,int param_2,int param_3);

// ---------------------------------------------------------------------------
// 0x0056c580  Per-body jacobian/inertia transform loop: for each of param_7[1] indices, scales
//             the linear block by (pfVar11[2]^2) and applies a 3x3 rotation built from the 12
//             pfVar11 lane values to both the indexed dest block (iVar13+param_1) and the walked
//             pfVar14 block. Header note 5(a)(b)(c)(d).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056c580(int param_1,undefined4 param_2,int param_3,undefined4 param_4,
                 undefined4 param_5,int param_6,int *param_7,int param_8)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float *pfVar11;
  int iVar12;
  int iVar13;
  float *pfVar14;
  float *pfVar15;
  float *pfVar16;
  int local_54;
  // Ghidra split as local_50[4]+local_40+local_3c+local_38 — one contiguous float[8] stack block
  // (note 4 class). local_40=local_50[4], local_3c=local_50[5], local_38=local_50[6].
  float local_50 [8];

  local_54 = 0;
  if (param_7[1] != 0) {
    pfVar11 = (float *)(param_8 + 4);
    pfVar14 = (float *)(param_3 + 8);
    do {
      fVar4 = pfVar11[2] * pfVar11[2];
      fVar1 = pfVar11[-1];
      iVar13 = *(int *)(*param_7 + local_54 * 4);
      fVar2 = pfVar11[-1];
      fVar3 = *pfVar11;
      pfVar15 = pfVar14 + -2;
      pfVar16 = local_50;
      for (iVar12 = 8; iVar12 != 0; iVar12 = iVar12 + -1) {
        *pfVar16 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        pfVar16 = pfVar16 + 1;
      }
      iVar13 = iVar13 * 0x20;
      // note 5(a): fVar5 matches printed left-assoc.
      fVar5 = fVar1 * fVar1 + pfVar11[3] * *pfVar11 + pfVar11[7] * pfVar11[1];
      // note 5(b): fVar2/fVar3 accumulators are t1 + (t2 + t3), NOT the printed (t1+t2)+t3.
      fVar2 = pfVar11[8] * pfVar11[1] + (pfVar11[4] * *pfVar11 + fVar2 * fVar3);
      fVar3 = pfVar11[9] * pfVar11[1] + (pfVar11[5] * *pfVar11 + pfVar11[1] * pfVar11[-1]);
      // remaining accumulators: printed order retained (deep-stack, <=1-ULP x87 floor, note 5).
      fVar7 = pfVar11[7] * pfVar11[5] + pfVar11[4] * pfVar11[3] + pfVar11[3] * pfVar11[-1];
      fVar1 = pfVar11[8] * pfVar11[5] + pfVar11[4] * pfVar11[4] + pfVar11[3] * *pfVar11;
      fVar6 = pfVar11[9] * pfVar11[5] + pfVar11[5] * pfVar11[4] + pfVar11[3] * pfVar11[1];
      fVar8 = pfVar11[9] * pfVar11[7] + pfVar11[8] * pfVar11[3] + pfVar11[7] * pfVar11[-1];
      fVar9 = pfVar11[9] * pfVar11[8] + pfVar11[8] * pfVar11[4] + pfVar11[7] * *pfVar11;
      fVar10 = pfVar11[9] * pfVar11[9] + pfVar11[8] * pfVar11[5] + pfVar11[7] * pfVar11[1];
      // note 5(d): fVar4-scaled linear block — single FMUL each.
      *(float *)(iVar13 + param_1) = fVar4 * *(float *)(iVar13 + param_6);
      *(float *)(iVar13 + 4 + param_1) = fVar4 * *(float *)(iVar13 + 4 + param_6);
      *(float *)(iVar13 + 8 + param_1) = fVar4 * *(float *)(iVar13 + 8 + param_6);
      // note 5(c): matrix-row stores group (M14-product + M18-product) first, then + M10-product.
      *(float *)(iVar13 + 0x10 + param_1) =
           fVar5 * *(float *)(iVar13 + 0x10 + param_6) +
           (fVar8 * *(float *)(iVar13 + 0x18 + param_6) +
            fVar7 * *(float *)(iVar13 + 0x14 + param_6));
      *(float *)(iVar13 + 0x14 + param_1) =
           fVar2 * *(float *)(iVar13 + 0x10 + param_6) +
           (fVar9 * *(float *)(iVar13 + 0x18 + param_6) +
            fVar1 * *(float *)(iVar13 + 0x14 + param_6));
      *(float *)(iVar13 + 0x18 + param_1) =
           fVar3 * *(float *)(iVar13 + 0x10 + param_6) +
           (fVar10 * *(float *)(iVar13 + 0x18 + param_6) +
            fVar6 * *(float *)(iVar13 + 0x14 + param_6));
      local_54 = local_54 + 1;
      pfVar11 = pfVar11 + 0xc;
      pfVar14[-2] = local_50[0] * fVar4;
      pfVar14[-1] = local_50[1] * fVar4;
      *pfVar14 = local_50[2] * fVar4;
      // pfVar14[2/3/4]: printed order retained (deep-stack, <=1-ULP x87 floor, note 5).
      pfVar14[2] = local_50[6] * fVar8 + local_50[4] * fVar5 + local_50[5] * fVar7;
      pfVar14[3] = local_50[6] * fVar9 + local_50[4] * fVar2 + local_50[5] * fVar1;
      pfVar14[4] = local_50[6] * fVar10 + local_50[4] * fVar3 + local_50[5] * fVar6;
      pfVar14 = pfVar14 + 8;
    } while (local_54 != param_7[1]);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0056c8e0  Single-body 3x(mat4 . vec4) accumulate: transforms the 0xd0-strided 3x4 block at
//             (param_4*0x20 + param_2) by the vec4 param_3 and ADDS into the three output rows at
//             (param_4*0x20 + {0,4,8} / +{0x10,0x14,0x18}) + param_1. All row stores are 5-term
//             MAC reductions accumulated into the existing dest (note 5, printed order retained).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056c8e0(int param_1,int param_2,float *param_3,int param_4)
{
  float *pfVar1;
  float *pfVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  float fVar8;
  float fVar9;
  float fVar10;
  float fVar11;
  float fVar12;
  float fVar13;
  float fVar14;
  int iVar15;

  iVar15 = param_4 * 0x20;
  fVar3 = *(float *)(iVar15 + 4 + param_2);
  fVar4 = *param_3;
  fVar5 = *(float *)(iVar15 + 8 + param_2);
  fVar6 = *param_3;
  fVar7 = *(float *)(iVar15 + 0x44 + param_2);
  fVar8 = param_3[1];
  fVar9 = *(float *)(iVar15 + 0x48 + param_2);
  fVar10 = param_3[1];
  pfVar1 = (float *)(iVar15 + 0x14 + param_1);
  fVar11 = *(float *)(iVar15 + 0x84 + param_2);
  fVar12 = param_3[2];
  fVar13 = *(float *)(iVar15 + 0x88 + param_2);
  fVar14 = param_3[2];
  pfVar2 = (float *)(iVar15 + 0x10 + param_1);
  *(float *)(iVar15 + param_1) =
       *(float *)((param_4 + 6) * 0x20 + param_2) * param_3[3] +
       *(float *)((param_4 + 4) * 0x20 + param_2) * param_3[2] +
       *(float *)((param_4 + 2) * 0x20 + param_2) * param_3[1] +
       *(float *)(iVar15 + param_2) * *param_3 + *(float *)(iVar15 + param_1);
  *(float *)(iVar15 + 4 + param_1) =
       *(float *)(iVar15 + 0xc4 + param_2) * param_3[3] +
       fVar11 * fVar12 + fVar7 * fVar8 + fVar3 * fVar4 + *(float *)(iVar15 + 4 + param_1);
  *(float *)(iVar15 + 8 + param_1) =
       *(float *)(iVar15 + 200 + param_2) * param_3[3] +
       fVar13 * fVar14 + fVar9 * fVar10 + fVar5 * fVar6 + *(float *)(iVar15 + 8 + param_1);
  fVar3 = *(float *)(iVar15 + 0x14 + param_2);
  fVar4 = *param_3;
  fVar5 = *(float *)(iVar15 + 0x18 + param_2);
  fVar6 = *param_3;
  fVar7 = *(float *)(iVar15 + 0x54 + param_2);
  fVar8 = param_3[1];
  fVar9 = *(float *)(iVar15 + 0x58 + param_2);
  fVar10 = param_3[1];
  fVar11 = *(float *)(iVar15 + 0x94 + param_2);
  fVar12 = param_3[2];
  fVar13 = *(float *)(iVar15 + 0x98 + param_2);
  fVar14 = param_3[2];
  *pfVar2 = *(float *)(iVar15 + 0xd0 + param_2) * param_3[3] +
            *(float *)(iVar15 + 0x90 + param_2) * param_3[2] +
            *(float *)(iVar15 + 0x50 + param_2) * param_3[1] +
            *(float *)(iVar15 + 0x10 + param_2) * *param_3 + *pfVar2;
  *pfVar1 = *(float *)(iVar15 + 0xd4 + param_2) * param_3[3] +
            fVar11 * fVar12 + fVar7 * fVar8 + fVar3 * fVar4 + *pfVar1;
  *(float *)(iVar15 + 0x18 + param_1) =
       *(float *)(iVar15 + 0xd8 + param_2) * param_3[3] +
       fVar13 * fVar14 + fVar9 * fVar10 + fVar5 * fVar6 + *(float *)(iVar15 + 0x18 + param_1);
  return;
}

// ---------------------------------------------------------------------------
// 0x0056caa0  Constraint accumulate/scatter pass: zeroes the param_4/param_5 impulse rows and the
//             param_3 accumulator rows, runs FUN_0056c8e0 over the param_8 contact stream (twice
//             per contact, dst-side and its 0x40 mirror), copies the linear/angular blocks from
//             param_6 into param_1, then scatters each body's 8-float block back into the two
//             accumulators indexed by the param_12 body-pair table. Header note 4.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056caa0(int param_1,undefined4 param_2,int *param_3,int *param_4,
                 int *param_5,int param_6,int *param_7,int param_8,uint param_9,undefined4 param_10,
                 undefined4 param_11,int param_12,int param_13,undefined4 param_14,int param_15,
                 undefined4 param_16,undefined4 param_17,int param_18)
{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  int iVar4;
  float *pfVar5;
  int iVar6;
  int iVar7;
  float *pfVar8;
  int local_a0;
  int local_9c;
  uint local_90;
  // contiguous float[8] gather buffers (note 4): local_70=[4],local_6c=[5],local_68=[6], etc.
  float local_80 [8];
  float local_60 [8];
  float local_40 [8];
  float local_20 [8];

  iVar1 = *param_7;
  iVar7 = param_7[1];
  iVar3 = 0;
  if (param_13 != 0) {
    iVar4 = 0;
    do {
      iVar3 = iVar3 + 1;
      iVar6 = iVar4 + 0x40;
      **(undefined4 **)(*param_4 + -4 + iVar3 * 4) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 4) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 8) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x10) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x14) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x18) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x20) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x24) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x28) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x30) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x34) = 0;
      *(undefined4 *)(*(int *)(*param_4 + -4 + iVar3 * 4) + 0x38) = 0;
      *(undefined4 *)(iVar4 + *param_5) = 0;
      *(undefined4 *)(*param_5 + -0x3c + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x38 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x30 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x2c + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x28 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x20 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x1c + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x18 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0x10 + iVar6) = 0;
      *(undefined4 *)(*param_5 + -0xc + iVar6) = 0;
      *(undefined4 *)(*param_5 + -8 + iVar6) = 0;
      iVar4 = iVar6;
    } while (iVar3 != param_13);
  }
  local_90 = param_9 >> 2;
  param_4[1] = param_13;
  param_5[1] = param_13;
  if (local_90 != 0) {
    local_a0 = param_18;
    do {
      iVar3 = *(int *)(param_8 + 0x2c);
      iVar4 = *(int *)(param_8 + 0x3c);
      iVar6 = *(int *)(param_8 + 0x1c);
      if (*(int *)(param_8 + 0xc) != -1) {
        FUN_0056c8e0(*(undefined4 *)(*param_4 + iVar6 * 4),param_8,
                     (float *)((param_15 - param_18) + local_a0),0);
        FUN_0056c8e0(iVar6 * 0x40 + *param_5,param_8,(float *)local_a0,0);
      }
      if (iVar3 != -1) {
        FUN_0056c8e0(*(undefined4 *)(*param_4 + iVar4 * 4),param_8,
                     (float *)((param_15 - param_18) + local_a0),1);
        FUN_0056c8e0(iVar4 * 0x40 + *param_5,param_8,(float *)local_a0,1);
      }
      local_a0 = local_a0 + 0x10;
      param_8 = param_8 + 0x100;
      local_90 = local_90 - 1;
    } while (local_90 != 0);
  }
  local_a0 = 0;
  if (iVar7 != 0) {
    iVar3 = 0;
    do {
      iVar4 = *(int *)(iVar1 + local_a0 * 4) * 0x20;
      *(undefined4 *)(iVar4 + param_1) = *(undefined4 *)(iVar4 + param_6);
      *(undefined4 *)(iVar4 + 4 + param_1) = *(undefined4 *)(iVar4 + 4 + param_6);
      uVar2 = *(undefined4 *)(iVar4 + 8 + param_6);
      *(undefined4 *)(iVar4 + 0xc + param_1) = 0;
      *(undefined4 *)(iVar4 + 8 + param_1) = uVar2;
      *(undefined4 *)(iVar4 + 0x10 + param_1) = *(undefined4 *)(iVar4 + 0x10 + param_6);
      *(undefined4 *)(iVar4 + 0x14 + param_1) = *(undefined4 *)(iVar4 + 0x14 + param_6);
      uVar2 = *(undefined4 *)(iVar4 + 0x18 + param_6);
      *(undefined4 *)(iVar4 + 0x1c + param_1) = 0;
      *(undefined4 *)(iVar4 + 0x18 + param_1) = uVar2;
      *(undefined4 *)(iVar3 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 4 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 8 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0xc + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0x10 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0x14 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0x18 + *param_3) = 0;
      *(undefined4 *)(iVar3 + 0x1c + *param_3) = 0;
      local_a0 = local_a0 + 1;
      iVar3 = iVar3 + 0x20;
    } while (local_a0 != iVar7);
  }
  param_3[1] = iVar7;
  local_a0 = 0;
  if (param_13 != 0) {
    local_9c = 0;
    do {
      iVar7 = *(int *)(param_12 + local_a0 * 8);
      iVar3 = *(int *)(param_12 + 4 + local_a0 * 8);
      if (iVar7 != -1) {
        pfVar5 = *(float **)(*param_4 + local_a0 * 4);
        pfVar8 = local_80;
        for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
          *pfVar8 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar8 = pfVar8 + 1;
        }
        pfVar5 = (float *)(*(int *)(iVar1 + iVar7 * 4) * 0x20 + param_1);
        *pfVar5 = local_80[0] + *pfVar5;
        pfVar5[1] = local_80[1] + pfVar5[1];
        pfVar5[2] = local_80[2] + pfVar5[2];
        pfVar5[4] = local_80[4] + pfVar5[4];
        pfVar5[5] = local_80[5] + pfVar5[5];
        pfVar5[6] = local_80[6] + pfVar5[6];
        pfVar5 = (float *)(*param_5 + local_9c);
        pfVar8 = local_40;
        for (iVar4 = 8; iVar4 != 0; iVar4 = iVar4 + -1) {
          *pfVar8 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar8 = pfVar8 + 1;
        }
        pfVar5 = (float *)(iVar7 * 0x20 + *param_3);
        *pfVar5 = local_40[0] + *pfVar5;
        pfVar5[1] = local_40[1] + pfVar5[1];
        pfVar5[2] = local_40[2] + pfVar5[2];
        pfVar5[4] = local_40[4] + pfVar5[4];
        pfVar5[5] = local_40[5] + pfVar5[5];
        pfVar5[6] = local_40[6] + pfVar5[6];
      }
      if (iVar3 != -1) {
        pfVar5 = (float *)(*(int *)(*param_4 + local_a0 * 4) + 0x20);
        pfVar8 = local_60;
        for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
          *pfVar8 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar8 = pfVar8 + 1;
        }
        pfVar5 = (float *)(*(int *)(iVar1 + iVar3 * 4) * 0x20 + param_1);
        *pfVar5 = local_60[0] + *pfVar5;
        pfVar5[1] = local_60[1] + pfVar5[1];
        pfVar5[2] = local_60[2] + pfVar5[2];
        pfVar5[4] = local_60[4] + pfVar5[4];
        pfVar5[5] = local_60[5] + pfVar5[5];
        pfVar5[6] = local_60[6] + pfVar5[6];
        pfVar5 = (float *)(*param_5 + 0x20 + local_9c);
        pfVar8 = local_20;
        for (iVar7 = 8; iVar7 != 0; iVar7 = iVar7 + -1) {
          *pfVar8 = *pfVar5;
          pfVar5 = pfVar5 + 1;
          pfVar8 = pfVar8 + 1;
        }
        pfVar5 = (float *)(iVar3 * 0x20 + *param_3);
        *pfVar5 = local_20[0] + *pfVar5;
        pfVar5[1] = local_20[1] + pfVar5[1];
        pfVar5[2] = local_20[2] + pfVar5[2];
        pfVar5[4] = local_20[4] + pfVar5[4];
        pfVar5[5] = local_20[5] + pfVar5[5];
        pfVar5[6] = local_20[6] + pfVar5[6];
      }
      local_a0 = local_a0 + 1;
      local_9c = local_9c + 0x40;
    } while (local_a0 != param_13);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x0056cf90  Gather one 0x60-byte (24-dword) record: transposes the +0x20-strided 3x4 rows at
//             (param_3 rows)*0x20 + param_2 and the +0x40-strided lanes into the flat param_1
//             layout. Pure integer bit-copy (note 2) — offsets verified verbatim against disasm.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056cf90(undefined4 *param_1,int param_2,int param_3)
{
  int iVar1;

  iVar1 = param_3 * 0x20 + param_2;
  *param_1 = *(undefined4 *)(param_3 * 0x20 + param_2);
  param_1[1] = *(undefined4 *)((param_3 + 2) * 0x20 + param_2);
  param_1[2] = *(undefined4 *)((param_3 + 4) * 0x20 + param_2);
  param_1[3] = *(undefined4 *)((param_3 + 6) * 0x20 + param_2);
  param_1[4] = *(undefined4 *)(iVar1 + 4);
  param_1[5] = *(undefined4 *)(iVar1 + 0x44);
  param_1[6] = *(undefined4 *)(iVar1 + 0x84);
  param_1[7] = *(undefined4 *)(iVar1 + 0xc4);
  param_1[8] = *(undefined4 *)(iVar1 + 8);
  param_1[9] = *(undefined4 *)(iVar1 + 0x48);
  param_1[10] = *(undefined4 *)(iVar1 + 0x88);
  param_1[0xb] = *(undefined4 *)(iVar1 + 200);
  param_1[0xc] = *(undefined4 *)(iVar1 + 0x10);
  param_1[0xd] = *(undefined4 *)(iVar1 + 0x50);
  param_1[0xe] = *(undefined4 *)(iVar1 + 0x90);
  param_1[0xf] = *(undefined4 *)(iVar1 + 0xd0);
  param_1[0x10] = *(undefined4 *)(iVar1 + 0x14);
  param_1[0x11] = *(undefined4 *)(iVar1 + 0x54);
  param_1[0x12] = *(undefined4 *)(iVar1 + 0x94);
  param_1[0x13] = *(undefined4 *)(iVar1 + 0xd4);
  param_1[0x14] = *(undefined4 *)(iVar1 + 0x18);
  param_1[0x15] = *(undefined4 *)(iVar1 + 0x58);
  param_1[0x16] = *(undefined4 *)(iVar1 + 0x98);
  param_1[0x17] = *(undefined4 *)(iVar1 + 0xd8);
  return;
}

// ---------------------------------------------------------------------------
// 0x0056d070  Island partition index-build: counts contacts per body into the param_1[0x10]/[1]
//             histograms, builds the prefix-sum offset tables (param_1[0xd]/[4]), then scatters
//             each contact's 0x2c/0x0c body-pair into the compacted arrays (calling FUN_0056cf90
//             to gather each 0x60 record). Pure integer/pointer (note 3) — transcribed verbatim.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056d070(int *param_1,int param_2,uint param_3,int param_4,
                 undefined4 param_5,int param_6,int param_7,undefined4 param_8,int *param_9,
                 int param_10,int param_11)
{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  int iVar8;
  int iVar9;
  int local_c;

  piVar2 = param_1;
  param_1[0x16] = (int)param_9;
  param_1[0x17] = param_10;
  param_1[0x18] = param_11;
  iVar3 = 0;
  if (param_4 != 0) {
    do {
      iVar3 = iVar3 + 1;
      *(undefined4 *)(param_1[0x10] + -4 + iVar3 * 4) = 0;
      *(undefined4 *)(param_1[1] + -4 + iVar3 * 4) = 0;
    } while (iVar3 != param_4);
  }
  param_1[0x11] = param_4;
  param_3 = param_3 >> 2;
  param_1[2] = param_4;
  if (param_3 != 0) {
    piVar5 = (int *)(param_2 + 0x2c);
    uVar7 = param_3;
    do {
      iVar3 = piVar5[-8];
      iVar6 = *piVar5;
      if (iVar3 != -1) {
        piVar1 = (int *)(param_1[0x10] + iVar3 * 4);
        *piVar1 = *piVar1 + 1;
        piVar1 = (int *)(param_1[1] + iVar3 * 4);
        *piVar1 = *piVar1 + 1;
      }
      if (iVar6 != -1) {
        *(int *)(param_1[0x10] + iVar6 * 4) = *(int *)(param_1[0x10] + iVar6 * 4) + 1;
        *(int *)(param_1[1] + iVar6 * 4) = *(int *)(param_1[1] + iVar6 * 4) + 1;
      }
      piVar5 = piVar5 + 0x40;
      uVar7 = uVar7 - 1;
    } while (uVar7 != 0);
  }
  iVar6 = 0;
  iVar3 = 0;
  if (param_10 != 0) {
    param_1 = (int *)param_10;
    do {
      iVar8 = 0;
      iVar4 = 0;
      if (*param_9 != 0) {
        do {
          *(int *)(piVar2[0xd] + iVar3 * 4) = iVar6;
          *(int *)(piVar2[4] + iVar3 * 4) = iVar4;
          iVar9 = *(int *)(piVar2[0x10] + iVar3 * 4);
          iVar6 = iVar6 + iVar9;
          iVar4 = iVar4 + iVar9;
          iVar3 = iVar3 + 1;
          iVar8 = iVar8 + 1;
        } while (iVar8 != *param_9);
      }
      param_9 = param_9 + 1;
      param_1 = (int *)((int)param_1 + -1);
    } while (param_1 != (int *)0x0);
  }
  iVar3 = 0;
  piVar2[0x21] = param_3;
  piVar2[0xe] = param_4;
  piVar2[5] = param_4;
  piVar2[0x20] = iVar6;
  piVar2[0x1f] = param_4;
  if (param_4 != 0) {
    do {
      iVar3 = iVar3 + 1;
      *(undefined4 *)(piVar2[0x10] + -4 + iVar3 * 4) = 0;
    } while (iVar3 != param_4);
  }
  iVar3 = 0;
  piVar2[0xb] = piVar2[0x20];
  piVar2[0x1d] = piVar2[0x21];
  piVar2[0x1a] = piVar2[0x21];
  local_c = 0;
  param_1 = (int *)0x0;
  if (param_7 != 0) {
    do {
      param_9 = (int *)0x0;
      *(undefined4 *)(piVar2[0x13] + (int)param_1 * 4) = 0;
      if ((*(uint *)(param_6 + (int)param_1 * 4) & 0xfffffffc) != 0) {
        iVar6 = iVar3 * 0x100 + param_2;
        do {
          iVar4 = *(int *)(iVar6 + 0xc);
          iVar8 = *(int *)(iVar6 + 0x2c);
          *(int *)(piVar2[0x19] + iVar3 * 8) = iVar4;
          *(int *)(piVar2[0x19] + 4 + iVar3 * 8) = iVar8;
          if (iVar4 != -1) {
            iVar9 = *(int *)(piVar2[0xd] + iVar4 * 4) + *(int *)(piVar2[0x10] + iVar4 * 4);
            FUN_0056cf90((undefined4 *)(iVar9 * 0x60 + *piVar2),iVar6,0);
            *(int *)(piVar2[0x1c] + iVar3 * 8) = iVar9 - local_c;
            *(int *)(piVar2[0x10] + iVar4 * 4) = *(int *)(piVar2[0x10] + iVar4 * 4) + 1;
            *(int *)(piVar2[10] + iVar9 * 4) = iVar3;
            *(int **)(piVar2[7] + iVar9 * 4) = param_9;
            *(int *)(piVar2[0x13] + (int)param_1 * 4) =
                 *(int *)(piVar2[0x13] + (int)param_1 * 4) + 1;
            piVar2[8] = piVar2[8] + 1;
          }
          if (iVar8 != -1) {
            iVar4 = *(int *)(piVar2[0xd] + iVar8 * 4) + *(int *)(piVar2[0x10] + iVar8 * 4);
            FUN_0056cf90((undefined4 *)(iVar4 * 0x60 + *piVar2),iVar6,1);
            *(int *)(piVar2[0x1c] + 4 + iVar3 * 8) = iVar4 - local_c;
            *(int *)(piVar2[0x10] + iVar8 * 4) = *(int *)(piVar2[0x10] + iVar8 * 4) + 1;
            *(int *)(piVar2[10] + iVar4 * 4) = iVar3;
            *(int **)(piVar2[7] + iVar4 * 4) = param_9;
            *(int *)(piVar2[0x13] + (int)param_1 * 4) =
                 *(int *)(piVar2[0x13] + (int)param_1 * 4) + 1;
            piVar2[8] = piVar2[8] + 1;
          }
          iVar3 = iVar3 + 1;
          iVar6 = iVar6 + 0x100;
          param_9 = (int *)((int)param_9 + 1);
        } while (param_9 != (int *)(*(uint *)(param_6 + (int)param_1 * 4) >> 2));
      }
      piVar2[0x14] = piVar2[0x14] + 1;
      local_c = local_c + *(int *)(piVar2[0x13] + (int)param_1 * 4);
      param_1 = (int *)((int)param_1 + 1);
    } while (param_1 != (int *)param_7);
  }
  return;
}

// --- gta-reversed-style hook registration — CLUSTER 7. ---
RH_ScopedInstall(FUN_0056c580, 0x0056c580);
RH_ScopedInstall(FUN_0056c8e0, 0x0056c8e0);
RH_ScopedInstall(FUN_0056caa0, 0x0056caa0);
RH_ScopedInstall(FUN_0056cf90, 0x0056cf90);
RH_ScopedInstall(FUN_0056d070, 0x0056d070);

}  // namespace Collision
}  // namespace mashed_re
