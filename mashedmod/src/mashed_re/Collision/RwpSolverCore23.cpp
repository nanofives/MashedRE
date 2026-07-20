// ============================================================================
//  RwpSolverCore23.cpp  —  B5e solver-island cluster K23  (7 of 9 fns)
//  Verbatim RWP-3.7 per-island solver-step DRIVERS/WRAPPERS (the mechanical
//  7 of the K23 cluster). Each is `int __cdecl F(int solverCtx)` returning the
//  context (EAX), mirroring the K21/K22 driver idiom.
//
//    FUN_0055fe50 (0x0055fe50, 77 B)  — 1-line broadphase kick: 10-arg call to
//                                        FUN_0055a1f0 (K3) with fields resolved
//                                        from the solver ctx (+0x70..+0xbc).
//    FUN_0055fea0 (0x0055fea0, 203 B) — two-loop body-array pass: seeds the
//                                        active-body manifold headers (0x8000
//                                        sentinels), clears per-joint slot +0x2c,
//                                        then pumps pairs via FUN_00568990 (K21).
//    FUN_0055ff70 (0x0055ff70, 28 B)  — guard → world sweep FUN_0056bb30 (K21)
//                                        when world+0xc is set.
//    FUN_0055ff90 (0x0055ff90, 603 B) — arena mark + island-active-flag walk:
//                                        primes the union-find arena (K11
//                                        FUN_00567c60) then, per island record
//                                        (stride 0x28), sets the "settled" bit
//                                        (rec[6]|1) unless any member shape's
//                                        active bit (shape+0x24 → +0x5c bitset)
//                                        is set.
//    FUN_00561c50 (0x00561c50, 517 B) — pair-record AABB gate + active-bit set:
//                                        per contact, tests the four squared
//                                        velocity/omega bounds (ctx+0xc..+0x18)
//                                        against the manifold's motion span and
//                                        flips the pair's active flag via
//                                        FUN_0055ac00 (DONE).
//    FUN_00561e60 (0x00561e60, 31 B)  — 1-line wrapper → FUN_00568fd0 (K11).
//    FUN_00561e80 (0x00561e80, 31 B)  — 1-line wrapper → FUN_00568dd0 (K11).
//
//  DEFERRED (2 fns, both to the focused K23 session):
//   1) FUN_00561040 (0x00561040, 566 B) — outer pair-record slicer → K13
//      orchestrator FUN_00560260. The 22-dword descriptor frame is well-
//      understood (values at even slots = puVar1[0,2,4,6,8,0xa,0xc,0x10,0x12,
//      0x14]; odd slots = &block; slots [14]/[15] uninit), BUT the arg8 block
//      (&local_14) is read by K13 FUN_00560260 via param_8+4 (=count) AND
//      param_8+0x10, which in the original reaches [ESP+0x90] — the current
//      pair-record base written at 0x5611a2. So the by-value frame spans past a
//      naive 5-dword block; a 5-dword model CRASHED at spawn (2026-07-20). Needs
//      the exact [ESP] frame-layout reconstruction (map every slot K13's
//      param_1/param_8 read against FUN_00561040's stores).
//   2) FUN_00561390 (0x00561390, 2239 B) — the CCD + impact-
//  resolution monster. All its ABI unknowns are resolved (see
//  re/analysis/b5e/K23_PORT_RECON_2026-07-20.md and the 2026-07-20 session note):
//    * local_40 is the 4th float of FUN_0055bb70's param_3 OUTPUT buffer
//      (disasm: LEA[ESP+0x6c] param_3; both reads resolve to buffer_base+0xc) —
//      model `float local_4c[4]` and read local_4c[3] where the decomp says
//      local_40. [UNCERTAIN CLOSED]
//    * KV +0x24 = RwpVolFn24 float10 (0x005614e8); body +0x1c = RwpBodyFn1c
//      returning EAX (FUN_0057c2b0, no x87 leak).
//    * every param_1[N] is a byte-addressed struct field Ghidra rendered as
//      float* — the loop counters (local_a4/local_9c/fVar19/iVar23) and count
//      slots (param_1[0x1e]/[0x32]/[0x35] = ctx+0x78/+0xc8/+0xd4) are INTEGERS
//      (disasm 0x5613e7 MOV/CMP/JBE), NOT floats.
//    * contiguous buffers to array: local_4c[4], auStack_30[4] (+uStack_24),
//      auStack_20[8].
//  The monster warrants the K12/K17 full-listing transcription discipline in a
//  focused session (its subtle buffer/scratch bugs are only caught by the race,
//  per the K18 lesson) — deferred to keep this port faithful.
//
//  Deps (all externed; definitions live in sibling K-cluster TUs of same .asi):
//    K3 (FUN_0055a1f0), K11 (FUN_00567c60, FUN_00568dd0, FUN_00568fd0),
//    K13 (FUN_00560260), K21 (FUN_00568990, FUN_0056bb30), DONE (FUN_0055ac00).
//  Version anchor: MASHED.exe 2,846,720 B / SHA-256 BDCAE093…3C0E.
//
//  DISASM-RESOLVED — FUN_00561040 → FUN_00560260 (0x0056122d):
//   * 14 args pushed (0x005611d5..0x0056122c), cleaned by ADD ESP,0x38. K13
//     modeled FUN_00560260 with 13 params; the 14th (ctx+0x408, pushed first)
//     is VESTIGIAL (never read — K13 raced GREEN driven by this original
//     14-arg caller). Reproduced verbatim: extern FUN_00560260 with 14 dword
//     params, push all 14. (project_b5e_solver_island / K23 recon.)
//   * The 22-dword descriptor frame (&local_78) is a `{value, &block}`-
//     interleaved array; the odd puVar1[1],[3],… loads in the decomp are DEAD
//     stores (overwritten by &block at the same slot — verified 0x56106f/73
//     etc.). Slots [ESP+0x54]/[0x58] (frame[14]/[15], the puVar1[0xe]/[0xf]
//     value pair) are NEVER written — left uninitialised, matching the original;
//     FUN_00560260 does not read them.
// ============================================================================
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef unsigned char  undefined1;
typedef unsigned short undefined2;
typedef unsigned short ushort;
typedef unsigned char  byte;

// --- extern callees (defined in sibling K-cluster TUs of same .asi) ----------
// K3 broadphase entry (10 raw-dword args — pointers/counts in slots).
extern "C" int __cdecl FUN_0055a1f0(undefined4,undefined4,undefined4,undefined4,undefined4,
                                    undefined4,undefined4,undefined4,undefined4,undefined4);   // 0x0055a1f0
// K11 union-find arena primer (10 raw-dword args).
extern "C" void __cdecl FUN_00567c60(undefined4,undefined4,undefined4,undefined4,undefined4,
                                     undefined4,undefined4,undefined4,undefined4,undefined4);   // 0x00567c60
extern "C" undefined4 __cdecl FUN_00568dd0(undefined4,undefined4);                              // 0x00568dd0 (K11)
extern "C" undefined4 __cdecl FUN_00568fd0(undefined4,undefined4);                              // 0x00568fd0 (K11)
extern "C" uint __cdecl FUN_00568990(int,int,uint,int,uint);                                    // 0x00568990 (K21)
extern "C" int  __cdecl FUN_0056bb30(int);                                                      // 0x0056bb30 (K21)

// FUN_0055ac00 (0x0055ac00) is NOT ported — it stays original; call it through a raw
// function pointer to its RVA (same idiom as VehicleCouplingBridge::rwpActiveBit), so no
// unresolved-external symbol is introduced. (.asi: valid in MASHED.exe address space.)
static inline void FUN_0055ac00(int owner,int body,int set)
{
  reinterpret_cast<void(__cdecl *)(int,int,int)>(0x0055ac00u)(owner,body,set);   // 0x0055ac00
}

// forward decls for RH_ScopedInstall
extern "C" int __cdecl FUN_0055fe50(int param_1);
extern "C" int __cdecl FUN_0055fea0(int param_1);
extern "C" int __cdecl FUN_0055ff70(int param_1);
extern "C" int __cdecl FUN_0055ff90(int param_1);
extern "C" int __cdecl FUN_00561c50(int param_1);
extern "C" int __cdecl FUN_00561e60(int param_1);
extern "C" int __cdecl FUN_00561e80(int param_1);

// ---------------------------------------------------------------------------
// 0x0055fe50  Broadphase kick — 10-arg call to FUN_0055a1f0 (K3).
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0055fe50(int param_1)
{
  FUN_0055a1f0(**(undefined4 **)(param_1 + 0x70),*(undefined4 *)(param_1 + 0x74),
               *(undefined4 *)(param_1 + 0x7c),(undefined4)(param_1 + 0x78),
               *(undefined4 *)(param_1 + 0x80),*(undefined4 *)(param_1 + 0x88),
               (undefined4)(param_1 + 0x84),*(undefined4 *)(param_1 + 0xb4),
               *(undefined4 *)(param_1 + 0xbc),(undefined4)(param_1 + 0xb8));
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x0055fea0  Two-loop body-array pass — seed active-body manifold headers,
//   clear per-joint +0x2c, then pump pairs via FUN_00568990 (K21).
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0055fea0(int param_1)
{
  int *piVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;

  uVar6 = 0;
  if (*(int *)(param_1 + 0x78) != 0) {
    do {
      iVar5 = *(int *)(*(int *)(param_1 + 0x74) + uVar6 * 4);
      if (((1 < *(ushort *)(iVar5 + 0xc)) && ((*(byte *)(iVar5 + 8) & 0x10) != 0)) &&
         (uVar2 = *(uint *)(param_1 + 0x84), uVar2 < *(uint *)(param_1 + 0x88))) {
        piVar1 = (int *)(*(int *)(param_1 + 0x80) + uVar2 * 0x14);
        *(uint *)(param_1 + 0x84) = uVar2 + 1;
        *(undefined2 *)(piVar1 + 1) = 0x8000;
        *piVar1 = iVar5;
        *(undefined2 *)(piVar1 + 3) = 0x8000;
        piVar1[2] = iVar5;
        piVar1[4] = *(int *)(iVar5 + 0x1c);
      }
      uVar6 = uVar6 + 1;
    } while (uVar6 < *(uint *)(param_1 + 0x78));
  }
  uVar6 = 0;
  if (*(int *)(param_1 + 0x3e4) != 0) {
    iVar5 = 0;
    do {
      iVar3 = *(int *)(iVar5 + *(int *)(param_1 + 0x3e0));
      if (iVar3 != 0) {
        *(undefined4 *)(iVar3 + 0x2c) = 0;
      }
      uVar6 = uVar6 + 1;
      iVar5 = iVar5 + 0x14;
    } while (uVar6 < *(uint *)(param_1 + 0x3e4));
  }
  *(undefined4 *)(param_1 + 0x3e4) = 0;
  uVar4 = FUN_00568990(param_1,*(int *)(param_1 + 0x80),*(uint *)(param_1 + 0x84),
                       *(int *)(param_1 + 0x8c),*(uint *)(param_1 + 0x94));
  *(undefined4 *)(param_1 + 0x90) = uVar4;
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x0055ff70  Guard → world sweep FUN_0056bb30 (K21) when world+0xc set.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0055ff70(int param_1)
{
  if (*(int *)(*(int *)(param_1 + 0x70) + 0xc) != 0) {
    FUN_0056bb30(param_1);
  }
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x0055ff90  Arena mark + island-active-flag walk.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0055ff90(int param_1)
{
  ushort uVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int *piVar5;
  undefined4 *puVar6;
  uint uVar7;
  uint local_10;
  uint local_c;
  int local_8;

  **(undefined4 **)(param_1 + 0xd8) = *(undefined4 *)(param_1 + 0x7c);
  *(undefined4 *)(*(int *)(param_1 + 0xd8) + 4) = 0;
  FUN_00567c60((undefined4)(param_1 + 0xc4),*(undefined4 *)(param_1 + 0xd8),
               *(undefined4 *)(param_1 + 0xdc),*(undefined4 *)(param_1 + 0xe0),
               *(undefined4 *)(param_1 + 0x74),*(undefined4 *)(param_1 + 0x78),
               *(undefined4 *)(param_1 + 0xb4),*(undefined4 *)(param_1 + 0xb8),
               *(undefined4 *)(param_1 + 0x8c),*(undefined4 *)(param_1 + 0x90));
  local_c = 0;
  if (*(int *)(param_1 + 200) == 0) {
    return param_1;
  }
  local_8 = 0;
  do {
    uVar7 = 0;
    *(uint *)(local_8 + 0x18 + *(int *)(param_1 + 0xd4)) =
         *(uint *)(local_8 + 0x18 + *(int *)(param_1 + 0xd4)) & 0xfffffffe;
    uVar2 = *(uint *)(local_8 + 0xc + *(int *)(param_1 + 0xd4));
    puVar6 = (undefined4 *)(local_8 + *(int *)(param_1 + 0xd4));
    if (uVar2 != 0) {
      piVar5 = (int *)*puVar6;
      do {
        uVar1 = *(ushort *)(*piVar5 + 0x20);
        if ((*(uint *)(*(int *)(*(int *)(*piVar5 + 0x24) + 0x5c) + (uint)(uVar1 >> 5) * 4) &
            1 << ((byte)uVar1 & 0x1f)) != 0) goto LAB_005601b3;
        uVar7 = uVar7 + 1;
        piVar5 = piVar5 + 1;
      } while (uVar7 < uVar2);
    }
    for (iVar3 = puVar6[1]; iVar3 != 0; iVar3 = *(int *)(iVar3 + 0xdc)) {
      if (*(short *)(iVar3 + 0xb8) != -1) {
        iVar4 = *(int *)(*(int *)(iVar3 + 0xb4) + 0x24);
        if ((iVar4 != 0) &&
           (uVar1 = *(ushort *)(*(int *)(iVar3 + 0xb4) + 0x20),
           (*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
           != 0)) goto LAB_005601b3;
      }
      if (*(short *)(iVar3 + 0xc0) != -1) {
        iVar4 = *(int *)(*(int *)(iVar3 + 0xbc) + 0x24);
        if ((iVar4 != 0) &&
           (uVar1 = *(ushort *)(*(int *)(iVar3 + 0xbc) + 0x20),
           (*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f))
           != 0)) goto LAB_005601b3;
      }
    }
    local_10 = 0;
    if (puVar6[4] != 0) {
      piVar5 = (int *)puVar6[2];
      do {
        iVar3 = *piVar5;
        if (*(short *)(iVar3 + 0x54) != -1) {
          iVar4 = *(int *)(*(int *)(iVar3 + 0x50) + 0x24);
          if ((iVar4 != 0) &&
             (uVar1 = *(ushort *)(*(int *)(iVar3 + 0x50) + 0x20),
             (*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)
             ) != 0)) goto LAB_005601b3;
        }
        if (*(short *)(iVar3 + 0x5c) != -1) {
          iVar4 = *(int *)(*(int *)(iVar3 + 0x58) + 0x24);
          if ((iVar4 != 0) &&
             (uVar1 = *(ushort *)(*(int *)(iVar3 + 0x58) + 0x20),
             (*(uint *)(*(int *)(iVar4 + 0x5c) + (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)
             ) != 0)) goto LAB_005601b3;
        }
        piVar5 = piVar5 + 1;
        local_10 = local_10 + 1;
      } while (local_10 < (uint)puVar6[4]);
    }
    puVar6[6] = puVar6[6] | 1;
LAB_005601b3:
    local_c = local_c + 1;
    local_8 = local_8 + 0x28;
    if (*(uint *)(param_1 + 200) <= local_c) {
      return param_1;
    }
  } while( true );
}

// ---------------------------------------------------------------------------
// 0x00561c50  Pair-record AABB gate + active-bit set (FUN_0055ac00, DONE).
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_00561c50(int param_1)
{
  int iVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  float fVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  float *pfVar11;
  int iVar12;
  int *piVar13;
  int *piVar14;
  uint local_28;
  uint local_24;
  int local_20;
  uint local_1c;
  uint local_14;

  fVar2 = *(float *)(param_1 + 0xc);
  local_14 = 0;
  fVar3 = *(float *)(param_1 + 0x10);
  fVar4 = *(float *)(param_1 + 0x14);
  fVar5 = *(float *)(param_1 + 0x18);
  if (*(int *)(param_1 + 200) != 0) {
    local_20 = 0;
    do {
      piVar13 = (int *)(*(int *)(param_1 + 0xd4) + local_20);
      if (((*(byte *)(piVar13 + 6) & 3) == 0) && (local_24 = 0, piVar13[3] != 0)) {
        do {
          iVar8 = *(int *)(*piVar13 + local_24 * 4);
          local_28 = *(uint *)(iVar8 + 8) & 1;
          if (local_28 != 0) {
            local_1c = 0;
            if (*(ushort *)(iVar8 + 0xc) != 0) {
              piVar14 = (int *)(*(int *)(iVar8 + 0x10) + 4);
              do {
                iVar9 = *(int *)(*(int *)*piVar14 + 0x10);
                iVar12 = ((int *)*piVar14)[1] * 0x20;
                iVar10 = *(int *)(iVar9 + 8);
                fVar6 = *(float *)(iVar10 + 8 + iVar12);
                fVar7 = *(float *)(iVar10 + 4 + iVar12);
                iVar1 = iVar10 + iVar12;
                if ((((fVar2 * fVar2 <
                       fVar6 * fVar6 +
                       fVar7 * fVar7 + *(float *)(iVar10 + iVar12) * *(float *)(iVar10 + iVar12)) ||
                     (fVar3 * fVar3 <
                      *(float *)(iVar1 + 0x18) * *(float *)(iVar1 + 0x18) +
                      *(float *)(iVar1 + 0x14) * *(float *)(iVar1 + 0x14) +
                      *(float *)(iVar1 + 0x10) * *(float *)(iVar1 + 0x10))) ||
                    (pfVar11 = (float *)(*(int *)(iVar9 + 0x40) + iVar12),
                    fVar4 * fVar4 <
                    pfVar11[2] * pfVar11[2] + pfVar11[1] * pfVar11[1] + *pfVar11 * *pfVar11)) ||
                   (fVar5 * fVar5 <
                    pfVar11[6] * pfVar11[6] + pfVar11[5] * pfVar11[5] + pfVar11[4] * pfVar11[4])) {
                  local_28 = 0;
                  break;
                }
                piVar14 = piVar14 + 3;
                local_1c = local_1c + 1;
              } while (local_1c < *(ushort *)(iVar8 + 0xc));
            }
          }
          FUN_0055ac00(*(undefined4 *)(iVar8 + 0x24),iVar8,local_28 == 0);
          local_24 = local_24 + 1;
        } while (local_24 < (uint)piVar13[3]);
      }
      local_14 = local_14 + 1;
      local_20 = local_20 + 0x28;
    } while (local_14 < *(uint *)(param_1 + 200));
  }
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x00561e60 / 0x00561e80  1-line wrappers → K11 FUN_00568fd0 / FUN_00568dd0.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_00561e60(int param_1)
{
  FUN_00568fd0(*(undefined4 *)(param_1 + 0xc0),(undefined4)(param_1 + 0xc4));
  return param_1;
}

extern "C" int __cdecl FUN_00561e80(int param_1)
{
  FUN_00568dd0(*(undefined4 *)(param_1 + 0xc0),(undefined4)(param_1 + 0xc4));
  return param_1;
}

// --- gta-reversed-style hook registration — CLUSTER 23 (7 of 9). ---
//   FUN_00561040 DEFERRED with the monster (see header): its FUN_00560260 arg8
//   (&local_14) is read by K13 as a structured block via param_8+4 / param_8+0x10
//   (the latter reaches the pair-record base the original writes at 0x5611a2), so
//   the by-value frame is larger than a 5-dword block — a first-pass 5-dword model
//   crashed at spawn. Needs the K12/K17 full-listing frame reconstruction.
RH_ScopedInstall(FUN_0055fe50, 0x0055fe50);
RH_ScopedInstall(FUN_0055fea0, 0x0055fea0);
RH_ScopedInstall(FUN_0055ff70, 0x0055ff70);
RH_ScopedInstall(FUN_0055ff90, 0x0055ff90);
RH_ScopedInstall(FUN_00561c50, 0x00561c50);
RH_ScopedInstall(FUN_00561e60, 0x00561e60);
RH_ScopedInstall(FUN_00561e80, 0x00561e80);

}  // namespace Collision
}  // namespace mashed_re
