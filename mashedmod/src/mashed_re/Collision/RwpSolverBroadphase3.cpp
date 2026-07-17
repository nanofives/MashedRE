// Mashed RE — B5e: RwpSolver-island CLUSTER 3 clean-room port (broadphase, 9 fns).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool14, read_only, 2026-07-16). VERBATIM transcription of the K3 cluster from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm this
// session (per-function notes below). Style/idiom follows RwpSolverLeaves1.cpp (K1) /
// RwpSolverMath2.cpp (K2).
//
// K3 members (rva:size): 0055a1f0:1957 0055a9a0:397 0055abb0:73 0055ae50:27 0055b750:173
// 0055bae0:133 0055bd70:14 0055c0f0:318 0055c2d0:175. Deps: K1 (FUN_00564040,
// FUN_00565200, FUN_00565550 — all ported in RwpSolverLeaves1.cpp).
//
// NO-GUESSING disasm verifications (pool14, 2026-07-16):
//  1. Calling convention: all 9 end in plain RET (caller cleanup) => __cdecl
//     (body_end 0x0055a994 / 0x0055ab2c / 0x0055abf8 / 0x0055ae6a(JMP) / 0x0055b7fc /
//      0x0055bb64 / 0x0055bd7d(JMP) / 0x0055c22d / 0x0055c37e).
//  2. K1 LATENT DEFECT FIXED THIS CLUSTER: original FUN_00564040 returns the collected
//     pair COUNT in EAX on every path (XOR EAX,EAX @0x00564057; count reloaded into EAX
//     @0x0056409a inside the loop; the param_5 sort block 0x005640c2..0x005640f3 never
//     writes EAX; live at RET 0x005640fa). K1 had ported it `void`; FUN_0055a1f0
//     (call @0x0055a331) and FUN_0055a9a0 (call @0x0055aa08, EAX stored @0x0055aa23)
//     both consume that return. RwpSolverLeaves1.cpp signature changed void -> uint.
//  3. FUN_0055bd70 is an argument-forwarding tail JMP, not a call: MOV EAX,[ESP+4];
//     MOV [ESP+4],EAX; MOV ECX,[EAX+0x5c]; JMP dword ptr [ECX+0x10]
//     (0x0055bd70..0x0055bd7b). The volume-descriptor +0x10 target inherits the CALLER's
//     entire stack frame (unknown arg count) => ported __declspec(naked), byte-equivalent.
//  4. FUN_0055ae50's decomp prints an argless `FUN_00565550()`; the disasm REWRITES both
//     arg slots then tail-JMPs: [ESP+4] <- *param_1, [ESP+8] <- word param_2+0x20
//     (0x0055ae50..0x0055ae66, JMP 0x00565550). Ported as a 2-arg forwarding call; the
//     16-bit arg's garbage-high-bits difference is dead — FUN_00565550 masks & 0xffff
//     (same for FUN_0055abb0's PUSH EDX @0x0055abe6 into FUN_00565200's & 0xffff).
//  5. x87 summation GROUPING (decomp prints left-to-right; ports use the disasm order;
//     operand-swap inside one FADDP is commutative-exact and not annotated):
//       FUN_0055bae0: x=(m8v2+m4v1)+m0v0, y=(m9v2+m1v0)+m5v1, z=(m10v2+m2v0)+m6v1
//         (0x0055bb02..0x0055bb40); z rounds to float via [ESP+8] BEFORE +m14
//         (0x0055bb40/0x0055bb57), x/y stay 80-bit into the +m12/+m13 adds. [X87]
//       FUN_0055c0f0 loop: sx=(p1m4+p0m0)+p2m8, sy=(m5p1+p2m9)+p0m1,
//         sz=(p2m10+p1m6)+m2p0 rounded via [ESP+0x10] (0x0055c14e) before +m14;
//         sx/sy stay 80-bit (FADD ST0,ST2 @0x0055c155, ST1 @0x0055c15f). [X87]
//       FUN_0055c0f0 tail: o2 sum stored with FST (NOT FSTP) @0x0055c1ed — param_1[3]
//         consumes the UNROUNDED 80-bit o2 (FMUL ST2 @0x0055c204/0x0055c221) while
//         param_1[2] gets the rounded float. o0/o1 re-read ROUNDED from [EAX]/[EAX+4]
//         (0x0055c1f8/0x0055c1fd, 0x0055c214/0x0055c21a). [X87]
//       FUN_0055c2d0: all four rows (mDv1+mCv0)+mEv2 shape (0x0055c2e7..0x0055c34a),
//         each rounded to a float local; final *param_4/*param_5 adds are float+float.
//       FUN_0055b750: d0..d2 = (v-m) kept 80-bit into the products; s0=pf5*d2-pf6*d1
//         and s1=pf6*d0-pf4*d2 round via [ESP+8]/[ESP+0xc] (0x0055b793/0x0055b7a3);
//         s2=pf4*d1-pf5*d0 stays 80-bit into the out[2] add (FADD ST0,ST1
//         @0x0055b7f2). [X87]
//  6. FUN_0055c2d0's transformed point is passed as &local_c with local_8/local_4
//     CONTIGUOUSLY ABOVE it ([ESP+4]/[ESP+8]/[ESP+0xc], LEA ECX,[ESP+4] @0x0055c346) —
//     the +0x20 descriptor callee indexes param_2[0..2] through that pointer, so the
//     port uses a float[4] local block, not discrete locals. Dispatch is cdecl 4-arg
//     (ADD ESP,0x10 @0x0055c36c); FUN_0055bae0's +0x08 dispatch is cdecl 2-arg
//     returning float* (ADD ESP,0x8 @0x0055baf3, TEST EAX @0x0055baf6).
//  7. FUN_0055a1f0: both body-object dispatches are cdecl 1-arg — obj[0]+0x14 sleep
//     callback (CALL [EAX+0x14] @0x0055a62b, ADD ESP,4) and obj[0]+0x10 wake gate
//     returning int (CALL [EDX+0x10] @0x0055a754, TEST EAX @0x0055a75a). Targets live
//     in the REAL rwpOBJTYPEBODY table 0x0062403c until KV2 lands (island_vtable_reach
//     §1.2). Return value = EBP = param_1 on both exits (MOV EAX,EBP @0x0055a979/
//     0x0055a98a). DAT_007dc8c0 retry-latch order verified 0x0055a783/92/9b. The
//     5-int pair-record compaction copy is REP MOVSD @0x0055a8bc. Wake conditions
//     (+0x78 set, +0x60 clear, +0x64 set) verified 0x0055a6ec..0x0055a70b; the
//     sleeping-owner path keeps piVar11=piVar12 by skipping the reload (JNZ 0x0055a7a9
//     from 0x0055a745), matching the decomp's comma-expression.
//  8. FUN_0055a9a0: filter select verified 0x0055aa8a..0x0055aabb (NEG/SBB idioms:
//     accept iff param_6==0xf or param_6 & ((2 - in64)<<((1 - in5c)*2)) with in64/in5c
//     the +0x64/+0x5c bitmap hits); pair-record layout +0/+4w/+8/+0xcw/+0x10 verified
//     0x0055aac3..0x0055aae7 (same as FUN_0055a1f0's 0x0055a36e..0x0055a390); returns
//     the emitted-record count (early-out MOV EAX,[ESP+0x10] @0x0055ab21).
//
// x87 note: 80-bit ST0 chains carry the accepted <=1-ULP floor under MSVC's 64-bit long
// double (project_phys_chain_float10_methodology) — [X87]-tagged above.
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

// --- Ghidra scalar types kept verbatim (as RwpSolverLeaves1.cpp). ---
typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned short undefined2;
typedef unsigned int   undefined4;
typedef long double    float10;     // x87 80-bit extended — MSVC = 64-bit double [X87]

// --- Game globals, bound to absolute addresses under the decomp's names. ---
#define DAT_007dc8c0   (*(int*)0x007dc8c0u)   // body-activation retry latch (0x0055a783/92/9b)

// --- K1 leaves this cluster calls (RwpSolverLeaves1.cpp). FUN_00564040's return type
//     was corrected void -> uint this cluster (verification note 2). ---
extern "C" uint __cdecl FUN_00564040(int param_1,ushort param_2,int param_3,int param_4,int param_5);
extern "C" void __cdecl FUN_00565200(int param_1,uint param_2,float *param_3);
extern "C" undefined2 __cdecl FUN_00565550(int param_1,uint param_2);

// --- Indirect-dispatch binding types (targets stay ORIGINAL code until KV lands). ---
typedef float* (__cdecl *RwpVolFnGetPoint)(int, undefined4);                  // desc +0x08 (note 6)
typedef void   (__cdecl *RwpVolFnInterval)(int, float*, float*, float*);      // desc +0x20 (note 6)
typedef int    (__cdecl *RwpBodyFnWake)(int*);                                // body obj +0x10 (note 7)
typedef void   (__cdecl *RwpBodyFnSleep)(int*);                               // body obj +0x14 (note 7)

// ---------------------------------------------------------------------------
// 0x0055a1f0  Broad-phase tick / collision-pair generator. Walks the dirty-shape bitmap,
//             expands hash-pair candidates via FUN_00564040, emits shape pairs
//             (0x14-byte records) + constraint ptrs, then reconciles the enabled bitmap
//             (+0x60) against wake/sleep via the body-object +0x10/+0x14 callbacks.
//             Returns param_1. Verification notes 2, 5, 7.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0055a1f0(uint param_1,int param_2,uint param_3,uint *param_4,int *param_5,
                                    uint param_6,uint *param_7,int param_8,uint param_9,uint *param_10)
{
  ushort uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  uint uVar5;
  ushort uVar6;
  uint uVar7;
  int iVar8;
  uint *puVar9;
  uint uVar10;
  int *piVar11;
  int *piVar12;
  int *piVar13;
  uint local_1c;
  uint local_18;
  uint local_14;
  uint local_10;
  uint local_c;
  int *local_8;

  iVar2 = param_1;
  uVar10 = 0;
  uVar3 = 0;
  local_1c = 0;
  local_14 = 0;
  local_18 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    do {
      uVar3 = uVar3 + 1;
      *(undefined4 *)(*(int *)(param_1 + 0x70) + -4 + uVar3 * 4) = 0;
    } while (uVar3 < *(uint *)(param_1 + 4));
  }
  uVar3 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    do {
      if (*(int *)(*(int *)(param_1 + 0x5c) + uVar3 * 4) != 0) {
        uVar5 = 0;
        uVar7 = 1;
        do {
          if ((*(uint *)(*(int *)(param_1 + 0x5c) + uVar3 * 4) & uVar7) != 0) {
            *(short *)(*(int *)(param_1 + 0x7c) + uVar10 * 2) = (short)uVar3 * 0x20 + (short)uVar5;
            uVar10 = uVar10 + 1;
          }
          uVar5 = uVar5 + 1;
          uVar7 = uVar7 * 2;
        } while (uVar5 < 0x20);
      }
      *(undefined4 *)(*(int *)(param_1 + 0x74) + uVar3 * 4) =
           *(undefined4 *)(*(int *)(param_1 + 0x5c) + uVar3 * 4);
      uVar3 = uVar3 + 1;
      local_1c = uVar10;
    } while (uVar3 < *(uint *)(param_1 + 4));
  }
  local_10 = 0;
  if (local_1c != 0) {
    do {
      uVar1 = *(ushort *)(*(int *)(param_1 + 0x7c) + local_10 * 2);
      uVar3 = (uint)uVar1;
      iVar8 = *(int *)(*(int *)(param_1 + 0x50) + uVar3 * 4);
      iVar4 = *(int *)(iVar8 + 0x24);
      if ((iVar4 != 0) &&
         (((*(uint *)(*(int *)(iVar4 + 0x64) + (uint)(*(ushort *)(iVar8 + 0x20) >> 5) * 4) &
           1 << ((byte)*(ushort *)(iVar8 + 0x20) & 0x1f)) != 0 ||
          ((iVar4 != 0 &&
           ((*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(*(ushort *)(iVar8 + 0x20) >> 5) * 4) &
            1 << ((byte)*(ushort *)(iVar8 + 0x20) & 0x1f)) != 0)))))) {
        puVar9 = (uint *)(*(int *)(param_1 + 0x70) + (uint)(uVar1 >> 5) * 4);
        *puVar9 = *puVar9 | 1 << ((byte)uVar1 & 0x1f);
        uVar10 = FUN_00564040(*(int *)(param_1 + 0x54),uVar1,*(int *)(param_1 + 0x80),
                              *(int *)(param_1 + 0x70),*(uint *)(param_1 + 8) & 1);
        local_8 = (int *)0x0;
        if (uVar10 != 0) {
          piVar12 = param_5 + local_14 * 5 + 3;
          do {
            uVar6 = *(ushort *)(*(int *)(param_1 + 0x80) + (int)local_8 * 2);
            if (local_14 < param_6) {
              *(undefined2 *)(piVar12 + -2) = 0x8000;
              piVar12[-3] = *(int *)(*(int *)(param_1 + 0x50) + uVar3 * 4);
              *(undefined2 *)piVar12 = 0x8000;
              iVar8 = *(int *)(*(int *)(param_1 + 0x50) + (uint)uVar6 * 4);
              piVar12[1] = 0;
              piVar12[-1] = iVar8;
              local_14 = local_14 + 1;
              piVar12 = piVar12 + 5;
            }
            if (local_1c < *(uint *)(param_1 + 0x44)) {
              uVar7 = 1 << ((byte)uVar6 & 0x1f);
              iVar8 = (uint)(uVar6 >> 5) * 4;
              if ((*(uint *)(iVar8 + *(int *)(param_1 + 0x74)) & uVar7) == 0) {
                *(ushort *)(*(int *)(param_1 + 0x7c) + local_1c * 2) = uVar6;
                puVar9 = (uint *)(iVar8 + *(int *)(param_1 + 0x74));
                local_1c = local_1c + 1;
                *puVar9 = *puVar9 | uVar7;
              }
            }
            local_8 = (int *)((int)local_8 + 1);
          } while ((uint)local_8 < uVar10);
        }
        if (*(int *)(param_1 + 0x20) != 0) {
          for (piVar12 = *(int **)(*(int *)(param_1 + 0x20) + uVar3 * 4); piVar12 != (int *)0x0;
              piVar12 = (int *)piVar12[1]) {
            iVar8 = *piVar12;
            if (*(short *)(iVar8 + 0x54) == -1) {
LAB_0055a502:
              if (local_18 < param_9) {
                *(int *)(param_8 + local_18 * 4) = iVar8;
LAB_0055a515:
                local_18 = local_18 + 1;
              }
            }
            else {
              iVar4 = *(int *)(*(int *)(iVar8 + 0x50) + 0x24);
              if (((iVar4 == 0) ||
                  (uVar6 = *(ushort *)(*(int *)(iVar8 + 0x50) + 0x20),
                  (*(uint *)(*(int *)(iVar4 + 0x64) + (uint)(uVar6 >> 5) * 4) &
                  1 << ((byte)uVar6 & 0x1f)) == 0)) || (*(short *)(iVar8 + 0x5c) == -1))
              goto LAB_0055a502;
              iVar4 = *(int *)(*(int *)(iVar8 + 0x58) + 0x24);
              if ((iVar4 == 0) ||
                 (uVar6 = *(ushort *)(*(int *)(iVar8 + 0x58) + 0x20),
                 (*(uint *)(*(int *)(iVar4 + 0x64) + (uint)(uVar6 >> 5) * 4) &
                 1 << ((byte)uVar6 & 0x1f)) == 0)) goto LAB_0055a502;
              uVar6 = *(ushort *)(*(int *)(iVar8 + 0x58) + 0x20) ^
                      *(ushort *)(*(int *)(iVar8 + 0x50) + 0x20) ^ uVar1;
              if (local_1c < *(uint *)(param_1 + 0x44)) {
                uVar3 = 1 << ((byte)uVar6 & 0x1f);
                iVar4 = (uint)(uVar6 >> 5) * 4;
                if ((*(uint *)(iVar4 + *(int *)(param_1 + 0x74)) & uVar3) == 0) {
                  local_1c = local_1c + 1;
                  *(ushort *)(*(int *)(param_1 + 0x7c) + -2 + local_1c * 2) = uVar6;
                  puVar9 = (uint *)(iVar4 + *(int *)(param_1 + 0x74));
                  *puVar9 = *puVar9 | uVar3;
                }
              }
              if ((uVar1 < uVar6) && (local_18 < param_9)) {
                *(int *)(param_8 + local_18 * 4) = iVar8;
                goto LAB_0055a515;
              }
            }
          }
        }
      }
      local_10 = local_10 + 1;
    } while (local_10 < local_1c);
  }
  param_1 = local_18;
  uVar3 = 0;
  param_6 = local_14;
  if (*(int *)(iVar2 + 4) != 0) {
    do {
      uVar3 = uVar3 + 1;
      *(undefined4 *)(*(int *)(iVar2 + 0x78) + -4 + uVar3 * 4) = 0;
    } while (uVar3 < *(uint *)(iVar2 + 4));
  }
  uVar3 = 0;
  if (local_1c != 0) {
    do {
      uVar1 = *(ushort *)(*(int *)(iVar2 + 0x7c) + uVar3 * 2);
      uVar10 = (uint)(uVar1 >> 5);
      uVar3 = uVar3 + 1;
      *(uint *)(*(int *)(iVar2 + 0x78) + uVar10 * 4) =
           *(uint *)(*(int *)(iVar2 + 0x78) + uVar10 * 4) | 1 << ((byte)uVar1 & 0x1f);
    } while (uVar3 < local_1c);
  }
  param_9 = 0;
  if (*(int *)(iVar2 + 4) != 0) {
    local_8 = (int *)0x0;
    do {
      uVar3 = ~*(uint *)(*(int *)(iVar2 + 0x78) + param_9 * 4) &
              *(uint *)(*(int *)(iVar2 + 0x60) + param_9 * 4);
      if (uVar3 != 0) {
        local_c = 1;
        local_10 = 0;
        do {
          if ((local_c & uVar3) != 0) {
            piVar12 = *(int **)(*(int *)(iVar2 + 0x50) + ((int)local_8 + local_10) * 4);
            if ((*(byte *)(piVar12 + 2) & 2) != 0) {
              if ((piVar12[9] != 0) &&
                 ((*(uint *)(*(int *)(piVar12[9] + 0x60) + (uint)(*(ushort *)(piVar12 + 8) >> 5) * 4
                            ) & 1 << ((byte)*(ushort *)(piVar12 + 8) & 0x1f)) != 0)) {
                ((RwpBodyFnSleep)(*(void **)(*piVar12 + 0x14)))(piVar12);   // 0x0055a62b (note 7)
                puVar9 = (uint *)(*(int *)(iVar2 + 0x60) + (uint)(*(ushort *)(piVar12 + 8) >> 5) * 4
                                 );
                *puVar9 = *puVar9 & ~(1 << ((byte)*(ushort *)(piVar12 + 8) & 0x1f));
                *(int *)(iVar2 + 0x4c) = *(int *)(iVar2 + 0x4c) + -1;
              }
              puVar9 = (uint *)(*(int *)(iVar2 + 0x60) + (((int)local_8 + local_10) >> 5) * 4);
              *puVar9 = *puVar9 & ~(1 << ((byte)local_10 & 0x1f));
            }
          }
          local_10 = local_10 + 1;
          local_c = local_c * 2;
        } while (local_10 < 0x20);
      }
      param_9 = param_9 + 1;
      local_8 = (int *)((int)local_8 + 0x20);
    } while (param_9 < *(uint *)(iVar2 + 4));
  }
  param_9 = 0;
  if (local_1c != 0) {
    do {
      uVar1 = *(ushort *)(*(int *)(iVar2 + 0x7c) + param_9 * 2);
      iVar8 = (uint)(uVar1 >> 5) * 4;
      uVar3 = 1 << ((byte)uVar1 & 0x1f);
      if ((((*(uint *)(iVar8 + *(int *)(iVar2 + 0x78)) & uVar3) != 0) &&
          ((*(uint *)(iVar8 + *(int *)(iVar2 + 0x60)) & uVar3) == 0)) &&
         ((*(uint *)(iVar8 + *(int *)(iVar2 + 0x64)) & uVar3) != 0)) {
        local_8 = (int *)0x0;
        piVar12 = *(int **)(*(int *)(iVar2 + 0x50) + (uint)uVar1 * 4);
        if (((piVar12[9] == 0) ||
            (piVar11 = piVar12,
            (*(uint *)(*(int *)(piVar12[9] + 0x60) + (uint)(*(ushort *)(piVar12 + 8) >> 5) * 4) &
            1 << ((byte)*(ushort *)(piVar12 + 8) & 0x1f)) == 0)) &&
           (piVar11 = local_8, *(uint *)(iVar2 + 0x4c) < *(uint *)(iVar2 + 0x48))) {
          iVar4 = ((RwpBodyFnWake)(*(void **)(*piVar12 + 0x10)))(piVar12);  // 0x0055a754 (note 7)
          if (iVar4 == 0) {
            if (DAT_007dc8c0 == 0) {
              DAT_007dc8c0 = 1;
            }
          }
          else {
            puVar9 = (uint *)(*(int *)(iVar2 + 0x60) + (uint)(*(ushort *)(piVar12 + 8) >> 5) * 4);
            *puVar9 = *puVar9 | 1 << ((byte)*(ushort *)(piVar12 + 8) & 0x1f);
            DAT_007dc8c0 = 0;
            *(int *)(iVar2 + 0x4c) = *(int *)(iVar2 + 0x4c) + 1;
            piVar11 = piVar12;
          }
        }
        if (piVar11 != (int *)0x0) {
          puVar9 = (uint *)(iVar8 + *(int *)(iVar2 + 0x60));
          *puVar9 = *puVar9 | uVar3;
        }
      }
      param_9 = param_9 + 1;
    } while (param_9 < local_1c);
  }
  uVar3 = 0;
  param_9 = 0;
  if (local_1c != 0) {
    do {
      if (param_3 <= uVar3) break;
      uVar1 = *(ushort *)(*(int *)(iVar2 + 0x7c) + param_9 * 2);
      if ((*(uint *)(*(int *)(iVar2 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
          != 0) {
        uVar3 = uVar3 + 1;
        *(undefined4 *)(param_2 + -4 + uVar3 * 4) =
             *(undefined4 *)(*(int *)(iVar2 + 0x50) + (uint)uVar1 * 4);
      }
      param_9 = param_9 + 1;
    } while (param_9 < local_1c);
  }
  param_9 = 0;
  *param_4 = uVar3;
  if (local_14 != 0) {
    piVar12 = param_5 + local_14 * 5;
    do {
      iVar8 = *(int *)(*param_5 + 0x24);
      if ((iVar8 == 0) ||
         (uVar1 = *(ushort *)(*param_5 + 0x20),
         (*(uint *)(*(int *)(iVar8 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) ==
         0)) {
        iVar8 = *(int *)(param_5[2] + 0x24);
        if ((iVar8 != 0) &&
           (uVar1 = *(ushort *)(param_5[2] + 0x20),
           (*(uint *)(*(int *)(iVar8 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
           != 0)) goto LAB_0055a897;
        param_6 = param_6 - 1;
        piVar12 = piVar12 + -5;
        piVar11 = piVar12;
        piVar13 = param_5;
        for (iVar8 = 5; iVar8 != 0; iVar8 = iVar8 + -1) {   // REP MOVSD @0x0055a8bc
          *piVar13 = *piVar11;
          piVar11 = piVar11 + 1;
          piVar13 = piVar13 + 1;
        }
      }
      else {
LAB_0055a897:
        param_9 = param_9 + 1;
        param_5 = param_5 + 5;
      }
    } while (param_9 < param_6);
  }
  uVar3 = 0;
  *param_7 = param_6;
  if (local_18 == 0) {
    *param_10 = 0;
    return iVar2;
  }
  do {
    iVar8 = *(int *)(param_8 + uVar3 * 4);
    if (*(short *)(iVar8 + 0x54) == -1) {
LAB_0055a920:
      if (*(short *)(iVar8 + 0x5c) != -1) {
        iVar4 = *(int *)(*(int *)(iVar8 + 0x58) + 0x24);
        if ((iVar4 != 0) &&
           (uVar1 = *(ushort *)(*(int *)(iVar8 + 0x58) + 0x20),
           (*(uint *)(*(int *)(iVar4 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
           != 0)) goto LAB_0055a94f;
      }
      param_1 = param_1 - 1;
      *(undefined4 *)(param_8 + uVar3 * 4) = *(undefined4 *)(param_8 + param_1 * 4);
    }
    else {
      iVar4 = *(int *)(*(int *)(iVar8 + 0x50) + 0x24);
      if ((iVar4 == 0) ||
         (uVar1 = *(ushort *)(*(int *)(iVar8 + 0x50) + 0x20),
         (*(uint *)(*(int *)(iVar4 + 0x60) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) ==
         0)) goto LAB_0055a920;
LAB_0055a94f:
      uVar3 = uVar3 + 1;
    }
    if (param_1 <= uVar3) {
      *param_10 = param_1;
      return iVar2;
    }
  } while( true );
}

// ---------------------------------------------------------------------------
// 0x0055a9a0  Broad-phase pair generation for an explicit shape list (param_2 array of
//             param_3 shape ptrs): expands hash-pair candidates per shape via
//             FUN_00564040, filters by the param_6 mask against the +0x5c/+0x64 bitmaps
//             (verification note 8), emits 0x14-byte pair records into param_4 capped at
//             param_5. Returns the emitted-record count.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0055a9a0(uint param_1,int param_2,uint param_3,int param_4,int param_5,
                                    uint param_6)
{
  uint *puVar1;
  ushort uVar2;
  ushort uVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  uint uVar7;
  undefined4 *puVar8;
  undefined4 *puVar9;
  int iVar10;
  int local_10;
  uint local_8;

  iVar5 = param_1;
  local_10 = 0;
  if (param_6 == 0) {
    return 0;
  }
  uVar6 = 0;
  if (*(int *)(param_1 + 4) != 0) {
    do {
      uVar6 = uVar6 + 1;
      *(undefined4 *)(*(int *)(param_1 + 0x70) + -4 + uVar6 * 4) = 0;
    } while (uVar6 < *(uint *)(param_1 + 4));
  }
  local_8 = 0;
  if (param_3 != 0) {
    do {
      uVar2 = *(ushort *)(*(int *)(param_2 + local_8 * 4) + 0x20);
      uVar6 = FUN_00564040(*(int *)(iVar5 + 0x54),uVar2,*(int *)(iVar5 + 0x80),
                           *(int *)(iVar5 + 0x70),*(uint *)(iVar5 + 8) & 1);
      puVar1 = (uint *)(*(int *)(iVar5 + 0x70) + (uint)(uVar2 >> 5) * 4);
      param_1 = 0;
      *puVar1 = *puVar1 | 1 << ((byte)uVar2 & 0x1f);
      if (uVar6 != 0) {
        puVar8 = (undefined4 *)(param_4 + 8 + local_10 * 0x14);
        do {
          uVar3 = *(ushort *)(*(int *)(iVar5 + 0x80) + param_1 * 2);
          uVar7 = 1 << ((byte)uVar3 & 0x1f);
          iVar10 = (uint)(uVar3 >> 5) * 4;
          puVar9 = puVar8;
          if (((*(uint *)(iVar10 + *(int *)(iVar5 + 0x70)) & uVar7) == 0) &&
             ((param_6 == 0xf ||
              ((param_6 &
               (2u - (uint)((*(uint *)(iVar10 + *(int *)(iVar5 + 0x64)) & uVar7) != 0)) <<
               (((1 - (int)((*(uint *)(iVar10 + *(int *)(iVar5 + 0x5c)) & uVar7) != 0)) * 2) &
                0x1f)) != 0)))) {
            puVar9 = puVar8 + 5;
            puVar8[-2] = *(undefined4 *)(*(int *)(iVar5 + 0x50) + (uint)uVar2 * 4);
            *(undefined2 *)(puVar8 + -1) = 0x8000;
            uVar4 = *(undefined4 *)(*(int *)(iVar5 + 0x50) + (uint)uVar3 * 4);
            *(undefined2 *)(puVar8 + 1) = 0x8000;
            *puVar8 = uVar4;
            puVar8[2] = 0;
            local_10 = local_10 + 1;
            if (local_10 == param_5) {
              return local_10;
            }
          }
          param_1 = param_1 + 1;
          puVar8 = puVar9;
        } while (param_1 < uVar6);
      }
      local_8 = local_8 + 1;
    } while (local_8 < param_3);
  }
  return local_10;
}

// ---------------------------------------------------------------------------
// 0x0055abb0  If shape param_2's index bit is set in the bitmap at param_1[0x1a], fill
//             the AABB buffer param_3 via FUN_00565200 and return param_3; else 0.
//             (Original passes the index word with garbage high bits — note 4.)
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl FUN_0055abb0(undefined4 *param_1,int param_2,undefined4 param_3)
{
  ushort uVar1;
  undefined4 uVar2;

  uVar1 = *(ushort *)(param_2 + 0x20);
  uVar2 = 0;
  if ((*(uint *)(param_1[0x1a] + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) != 0) {
    FUN_00565200(*param_1,uVar1,(float *)param_3);
    uVar2 = param_3;
  }
  return uVar2;
}

// ---------------------------------------------------------------------------
// 0x0055ae50  Arg-rewriting tail JMP into FUN_00565550: forwards (*param_1,
//             *(ushort*)(param_2+0x20)) — verification note 4. The decomp printed an
//             argless call; the disasm rewrites both arg slots (0x0055ae50..0x0055ae66).
// ---------------------------------------------------------------------------
extern "C" undefined2 __cdecl FUN_0055ae50(int *param_1,int param_2)
{
  return FUN_00565550(*param_1,*(ushort *)(param_2 + 0x20));
}

// ---------------------------------------------------------------------------
// 0x0055b750  World-space point param_2 -> body-relative velocity-style vector param_3:
//             d = param_2 - bodyMatrix.pos (matrix iVar9 = body idx*0x40 into table
//             **(*param_1+0x10), pos at +0x30), crossed with the 8-float row at
//             idx*0x20 into (*(*param_1+0x10))[2], plus that row's +0/+4/+8 base.
//             x87 rounding map in verification note 5.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0055b750(int *param_1,float *param_2,float *param_3)
{
  float *pfVar8;
  int iVar9;

  iVar9 = param_1[1] * 0x40 + **(int **)(*param_1 + 0x10);
  {
    float10 d0 = (float10)*param_2 - (float10)*(float *)(iVar9 + 0x30);      // FSUBR 0x0055b774
    float10 d1 = (float10)param_2[1] - (float10)*(float *)(iVar9 + 0x34);
    float10 d2 = (float10)param_2[2] - (float10)*(float *)(iVar9 + 0x38);
    pfVar8 = (float *)(param_1[1] * 0x20 + (*(int **)(*param_1 + 0x10))[2]);
    float s0 = (float)((float10)pfVar8[5] * d2 - (float10)pfVar8[6] * d1);   // FSTP [ESP+8]
    float s1 = (float)((float10)pfVar8[6] * d0 - (float10)pfVar8[4] * d2);   // FSTP [ESP+0xc]
    float10 s2 = (float10)pfVar8[4] * d1 - (float10)pfVar8[5] * d0;          // stays 80-bit [X87]
    *param_3 = *pfVar8 + s0;
    param_3[1] = *(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 8) + 4 + param_1[1] * 0x20) + s1;
    param_3[2] = (float)((float10)*(float *)(*(int *)(*(int *)(*param_1 + 0x10) + 8) + 8 +
                                             param_1[1] * 0x20) + s2);       // FADD ST0,ST1
  }
}

// ---------------------------------------------------------------------------
// 0x0055bae0  Fetch a mutable point from the shape's volume descriptor (+0x08 slot,
//             cdecl 2-arg returning float*) and transform it in place by the 4x4
//             param_2 (RW row layout, translation at [0xc..0xe]). Disasm summation
//             grouping + z-row rounding in verification note 5. Dispatch lands in the
//             REAL descriptor tables (0x5e4f50 band) until KV3.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0055bae0(int param_1,float *param_2,undefined4 param_3)
{
  float *pfVar10;

  pfVar10 = ((RwpVolFnGetPoint)(*(void **)(*(int *)(param_1 + 0x5c) + 8)))(param_1,param_3);  // 0x0055baf0
  if ((pfVar10 != (float *)0x0) && (param_2 != (float *)0x0)) {
    float10 sx = ((float10)param_2[8] * pfVar10[2] + (float10)param_2[4] * pfVar10[1]) +
                 (float10)*param_2 * *pfVar10;                               // 0x0055bb02..0x0055bb14
    float10 sy = ((float10)param_2[9] * pfVar10[2] + (float10)param_2[1] * *pfVar10) +
                 (float10)param_2[5] * pfVar10[1];                           // 0x0055bb16..0x0055bb29
    float  sz = (float)(((float10)param_2[10] * pfVar10[2] + (float10)param_2[2] * *pfVar10) +
                 (float10)param_2[6] * pfVar10[1]);                          // FSTP [ESP+8] 0x0055bb40
    *pfVar10 = (float)((float10)param_2[0xc] + sx);                          // FADD ST0,ST2
    pfVar10[1] = (float)((float10)param_2[0xd] + sy);                        // FADD ST0,ST1
    pfVar10[2] = param_2[0xe] + sz;                                          // float + float
  }
}

// ---------------------------------------------------------------------------
// 0x0055bd70  Argument-forwarding tail JMP through the shape's volume descriptor +0x10
//             slot (verification note 3): the target inherits the CALLER's whole stack
//             frame, so this must stay a naked byte-equivalent forwarder — a C call
//             would pin the arg count.
// ---------------------------------------------------------------------------
extern "C" __declspec(naked) void FUN_0055bd70(void)
{
  __asm {
    mov eax, dword ptr [esp + 4]
    mov dword ptr [esp + 4], eax
    mov ecx, dword ptr [eax + 0x5c]
    jmp dword ptr [ecx + 0x10]
  }
}

// ---------------------------------------------------------------------------
// 0x0055c0f0  Transform a support-point set in place by 4x4 param_2 (point array at
//             param_1[0x14], count word at +0x54), then rotate the direction vector
//             param_1[0..2] and recompute the plane offset param_1[3] against either
//             the first transformed point (count!=0) or param_2's translation row.
//             FST-not-FSTP unrounded o2 + rounding map in verification note 5.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0055c0f0(float *param_1,float *param_2)
{
  float *pfVar11;
  int iVar12;
  uint uVar13;

  uVar13 = 0;
  if (*(ushort *)(param_1 + 0x15) != 0) {
    iVar12 = 0;
    do {
      pfVar11 = (float *)(*(int *)(param_1 + 0x14) + iVar12);
      uVar13 = uVar13 + 1;
      {
        float10 sx = ((float10)pfVar11[1] * param_2[4] + (float10)*pfVar11 * *param_2) +
                     (float10)pfVar11[2] * param_2[8];                       // 0x0055c110..0x0055c122
        float10 sy = ((float10)param_2[5] * pfVar11[1] + (float10)pfVar11[2] * param_2[9]) +
                     (float10)*pfVar11 * param_2[1];                         // 0x0055c124..0x0055c137
        float  sz = (float)(((float10)pfVar11[2] * param_2[10] + (float10)pfVar11[1] * param_2[6]) +
                     (float10)param_2[2] * *pfVar11);                        // FSTP [ESP+0x10]
        *pfVar11 = (float)((float10)param_2[0xc] + sx);                      // FADD ST0,ST2
        *(float *)(iVar12 + 4 + *(int *)(param_1 + 0x14)) =
             (float)((float10)param_2[0xd] + sy);                            // FADD ST0,ST1
        *(float *)(iVar12 + 8 + *(int *)(param_1 + 0x14)) = param_2[0xe] + sz;
      }
      iVar12 = iVar12 + 0x10;
    } while (uVar13 < *(ushort *)(param_1 + 0x15));
  }
  {
    float fVar1 = *param_1;      // copies staged through [ESP]/[ESP+4]/[ESP+8] (0x0055c189..0x0055c19a)
    float fVar2 = param_1[1];
    float fVar3 = param_1[2];
    *param_1 = (float)(((float10)fVar3 * param_2[8] + (float10)fVar2 * param_2[4]) +
               (float10)fVar1 * *param_2);                                   // 0x0055c19e..0x0055c1b6
    param_1[1] = (float)(((float10)fVar3 * param_2[9] + (float10)fVar2 * param_2[5]) +
                 (float10)fVar1 * param_2[1]);                               // 0x0055c1b8..0x0055c1d1
    float10 o2 = ((float10)fVar3 * param_2[10] + (float10)fVar2 * param_2[6]) +
                 (float10)fVar1 * param_2[2];
    param_1[2] = (float)o2;                                                  // FST (keeps o2) 0x0055c1ed
    if (*(ushort *)(param_1 + 0x15) != 0) {
      pfVar11 = (float *)*(int *)(param_1 + 0x14);
      param_1[3] = (float)(((float10)pfVar11[1] * param_1[1] + (float10)*pfVar11 * *param_1) +
                   (float10)pfVar11[2] * o2);                                // FMUL ST2 0x0055c204
      return;
    }
    param_1[3] = (float)(((float10)param_2[0xd] * param_1[1] + (float10)param_2[0xc] * *param_1) +
                 (float10)param_2[0xe] * o2);                                // FMUL ST2 0x0055c221
  }
}

// ---------------------------------------------------------------------------
// 0x0055c2d0  Support-interval query: optionally transform direction param_3 by the
//             transposed rotation of param_2 (plus a translation dot in loc[0]), call
//             the volume descriptor +0x20 slot (cdecl 4-arg), then offset both interval
//             ends by loc[0]. The three rotated components MUST sit contiguously above
//             loc[0] — the callee indexes them through &loc[1] (verification note 6).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0055c2d0(int param_1,float *param_2,float *param_3,float *param_4,
                                     float *param_5)
{
  float loc[4];   // [0]=local_10 [1]=local_c [2]=local_8 [3]=local_4 ([ESP]..[ESP+0xc])

  loc[0] = 0.0f;
  if (param_2 != (float *)0x0) {
    loc[0] = (float)(((float10)param_2[0xd] * param_3[1] + (float10)param_2[0xc] * *param_3) +
             (float10)param_2[0xe] * param_3[2]);                            // 0x0055c2e7..0x0055c2fc
    loc[1] = (float)(((float10)param_2[1] * param_3[1] + (float10)*param_2 * *param_3) +
             (float10)param_2[2] * param_3[2]);
    loc[2] = (float)(((float10)param_2[5] * param_3[1] + (float10)param_2[4] * *param_3) +
             (float10)param_2[6] * param_3[2]);
    loc[3] = (float)(((float10)param_2[9] * param_3[1] + (float10)param_2[8] * *param_3) +
             (float10)param_2[10] * param_3[2]);
    param_3 = &loc[1];                                                       // LEA ECX,[ESP+4]
  }
  ((RwpVolFnInterval)(*(void **)(*(int *)(param_1 + 0x5c) + 0x20)))(param_1,param_3,param_4,param_5);  // 0x0055c363
  *param_4 = loc[0] + *param_4;
  *param_5 = loc[0] + *param_5;
}

// --- gta-reversed-style hook registration (inert on the exe via HookSystemNoOp; installs
//     the inline-JMP under the .asi for the diff-original A/B acceptance) — CLUSTER 3. ---
RH_ScopedInstall(FUN_0055a1f0, 0x0055a1f0);
RH_ScopedInstall(FUN_0055a9a0, 0x0055a9a0);
RH_ScopedInstall(FUN_0055abb0, 0x0055abb0);
RH_ScopedInstall(FUN_0055ae50, 0x0055ae50);
RH_ScopedInstall(FUN_0055b750, 0x0055b750);
RH_ScopedInstall(FUN_0055bae0, 0x0055bae0);
RH_ScopedInstall(FUN_0055bd70, 0x0055bd70);
RH_ScopedInstall(FUN_0055c0f0, 0x0055c0f0);
RH_ScopedInstall(FUN_0055c2d0, 0x0055c2d0);

}  // namespace Collision
}  // namespace mashed_re
