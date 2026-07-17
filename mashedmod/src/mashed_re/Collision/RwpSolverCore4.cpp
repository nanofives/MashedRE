// Mashed RE — B5e: RwpSolver-island CLUSTER 4 clean-room port (octree query/insert/remove).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-16). VERBATIM transcription of the 3 K4 functions from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm before
// porting. Style/idiom follows RwpSolverLeaves1.cpp (K1) / RwpSolverBroadphase3.cpp (K3):
// extern "C" per-function, // 0x00xxxxxx RVA comments, absolute-address macro bindings,
// RH_ScopedInstall registration block.
//
// NO-GUESSING verifications against live pool0 disasm (2026-07-16):
// 1. All 3 bodies are __cdecl (caller cleanup): FUN_005646c0 ends ADD ESP,0x2ad8 / RET at
//    0x00564c72..0x00564c78; FUN_00564c80 RETs at 0x00564e84 / 0x0056511a; FUN_00565260 RET
//    at 0x0056554a. __chkstk @0x005646c5 is the MSVC stack probe for the 0x2ad8-byte frame —
//    compiler-provided in the port, not transcribed.
// 2. FUN_005646c0 stack contiguity: Ghidra splits the node-box stack into local_2800[5] +
//    afStack_27ec[2554], and the current box into local_2aa4[4] + local_2a94/2a90/2a8c.
//    Both are single contiguous blocks: box-stack entries are 8 floats at [ESP+i*0x20+0x2e8]
//    (LEA @0x0056472a), with the write cursor biased +5 floats ([ESP+i*0x20+0x2fc] LEA
//    @0x005649b1, stores at [ESI-0x14..+0x4] @0x00564b8f..0x00564c49); the current box is
//    8 floats at [ESP+0x44] filled by MOVSD.REP x8 @0x00564735. Ported as one array each
//    with reference aliases (same class as the K3 FUN_0055c2d0 finding).
// 3. FUN_005646c0 comparison idioms, all NaN-rejecting, transcribe as printed:
//    (a < b) != (a == b)  = FCOMP + TEST AH,0x41 + JP   (e.g. 0x0056477c..0x00564783);
//    strict a < b (mask-AND arm) = FCOMP + TEST AH,0x5 + JP (0x00564944) and
//    strict a > b (mask-SET arm) = FCOMP + AND EAX,0x4100 + JNZ (0x0056492d).
// 4. FUN_005646c0 fVar1..fVar6 (0x0056486c..0x00564920) and the FILD select blocks
//    (0x00564b5a..0x00564c49): every stored float is one FMUL/FMUL/FADD(P) chain with a
//    single rounding at FSTP — plain C float expressions are bit-identical on this x87
//    build (no /arch:SSE2). _DAT_005cc318 / _DAT_005e5418 are read as float (FMUL float ptr
//    @0x00564870 / @0x00564892) — the same two hysteresis floats K1's FUN_005641b0 binds.
// 5. FUN_00564c80 returns param_1 in EAX at both exits (MOV EAX,EBP @0x00564e7c and
//    @0x00565112). The decomp's two CONCAT22s are real partial-register artifacts, kept
//    verbatim: uVar17 = MOV AX,word[..] over the live index register (0x00564dc1, high word
//    = (uVar6 + uVar13*10) >> 16, NOT always zero since uVar13*10 can exceed 16 bits);
//    uVar14 = MOV BX,[0xc00e] over live uVar13<<6 (0x00564f68). Callee arg orders confirmed:
//    FUN_005646c0 @0x00564cf0, FUN_00564310 @0x00564d8c/0x00564dde/0x0056504e,
//    FUN_005641b0 @0x00564f1a/0x005650b1, FUN_00565120 @0x0056508c,
//    FUN_005651b0 @0x005650c4, FUN_00565160 @0x005650e3.
// 6. FUN_00565260 recount loop @0x0056546a..0x00565486 is DEGENERATE IN THE ORIGINAL: the
//    chain cursor (EAX = uVar3) is never advanced — the loop re-tests byte 0x881f[uVar3]
//    bit 0x80 of the FIRST chain node only; if set, iVar5 counts up to 0x1f and exits, if
//    clear the loop never terminates. The decomp renders exactly that and it is transcribed
//    verbatim (the EBP reload @0x0056547f is the decomp's `puVar8 = local_4;`). Also:
//    the final `uVar3 >> 10 & 0x3ff` @0x00565516..0x00565519 is AND ECX,EBP with EBP == 0x3ff
//    guaranteed by the 8-slot loop exit — decomp's constant is correct.
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

// --- Ghidra scalar types kept verbatim so each body transcribes character-for-character. ---
typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned short undefined2;
typedef unsigned int   undefined4;

// Ghidra CONCAT22(hi16, lo16) — partial-register reassembly (see header note 5).
static inline uint CONCAT22(ushort hi, ushort lo) { return ((uint)hi << 16) | lo; }

// --- Game globals referenced by the bodies (RwpIntegrator.cpp deref-macro idiom). ---
#define DAT_007dc8c8      (*(uint*)0x007dc8c8u)    // FUN_005646c0 — pair-test counter   (0x0056473d)
#define DAT_007dc8cc      (*(uint*)0x007dc8ccu)    // FUN_005646c0 — pair-accept counter (0x00564822)
#define _DAT_005e5418     (*(float*)0x005e5418u)   // FUN_005646c0 (0x00564892) — same float as K1
#define _DAT_005cc318     (*(float*)0x005cc318u)   // FUN_005646c0 (0x00564870) — same float as K1

// --- K1 (RwpSolverLeaves1.cpp) callees — declarations match the K1 definitions exactly. ---
extern "C" void   __cdecl FUN_00563e70(int param_1, ushort param_2, ushort param_3);
extern "C" void   __cdecl FUN_00563f60(int param_1, ushort param_2);
extern "C" bool   __cdecl FUN_005641b0(float *param_1, float *param_2);
extern "C" uint   __cdecl FUN_00564310(float *param_1, float *param_2, float *param_3);
extern "C" void   __cdecl FUN_00565120(int param_1, uint param_2, uint param_3, ushort param_4);
extern "C" void   __cdecl FUN_00565160(int param_1, uint param_2, uint param_3);
extern "C" void   __cdecl FUN_005651b0(int param_1, uint param_2, uint param_3);

// ---------------------------------------------------------------------------
// 0x005646c0  Octree overlap query: iterative stack walk from the root node; for every
//             stored primitive whose loose AABB overlaps *param_2 (and passes the optional
//             pair-filter bitmap at param_1+0xc018), inserts the pair (param_3, hit) into
//             the pair list at param_1+0xc010 via FUN_00563e70. 1465 B, __cdecl.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_005646c0(int param_1, float *param_2, ushort param_3)
{
  float fVar1;
  float fVar2;
  float fVar3;
  float fVar4;
  float fVar5;
  float fVar6;
  ushort uVar7;
  uint uVar8;
  uint uVar9;
  int iVar10;
  float *pfVar11;
  float *pfVar12;
  uint local_2ad8;
  uint local_2ad4;
  uint local_2ad0;
  int local_2ac8;
  // Contiguous blocks (header note 2). uStack_4 in the decomp is the __chkstk return-address
  // artifact — not transcribed.
  float local_2aa4[8];                       // current node box; [4..6] aliased below
  float &local_2a94 = local_2aa4[4];
  float &local_2a90 = local_2aa4[5];
  float &local_2a8c = local_2aa4[6];
  uint local_2a84;
  ushort local_2a80[320];                    // node index stack
  float boxStack_[320 * 8];                  // node box stack, one 8-float entry per level
  float *const local_2800 = boxStack_;       // decomp's local_2800 (entry base)
  float *const afStack_27ec = boxStack_ + 5; // decomp's afStack_27ec (+5-float alias)

  pfVar11 = (float *)(param_1 + 0x7fe0);
  pfVar12 = local_2800;
  for (iVar10 = 8; iVar10 != 0; iVar10 = iVar10 + -1) {
    *pfVar12 = *pfVar11;
    pfVar11 = pfVar11 + 1;
    pfVar12 = pfVar12 + 1;
  }
  local_2a80[0] = 0;
  local_2ac8 = 1;
  do {
    local_2ac8 = local_2ac8 + -1;
    local_2a84 = (uint)local_2a80[local_2ac8];
    uVar7 = *(ushort *)(param_1 + (local_2a84 * 5 + 0x2607) * 4) & 0x3ff;
    pfVar11 = local_2800 + local_2ac8 * 8;
    pfVar12 = local_2aa4;
    for (iVar10 = 8; iVar10 != 0; iVar10 = iVar10 + -1) {
      *pfVar12 = *pfVar11;
      pfVar11 = pfVar11 + 1;
      pfVar12 = pfVar12 + 1;
    }
    for (; uVar7 != 0x3ff; uVar7 = *(ushort *)(param_1 + 0x8820 + (uint)uVar7 * 4)) {
      uVar8 = *(ushort *)(param_1 + 0x881e + (uint)uVar7 * 4) & 0x3ff;
      DAT_007dc8c8 = DAT_007dc8c8 + 1;
      pfVar11 = (float *)(uVar8 * 0x20 + param_1);
      if (((((pfVar11[4] < *param_2 != (pfVar11[4] == *param_2)) &&
            (param_2[4] < *pfVar11 != (param_2[4] == *pfVar11))) &&
           (pfVar11[5] < param_2[1] != (pfVar11[5] == param_2[1]))) &&
          ((param_2[5] < pfVar11[1] != (param_2[5] == pfVar11[1]) &&
           (pfVar11[6] < param_2[2] != (pfVar11[6] == param_2[2]))))) &&
         ((param_2[6] < pfVar11[2] != (param_2[6] == pfVar11[2]) &&
          ((iVar10 = *(int *)(param_1 + 0xc018), iVar10 == 0 ||
           (uVar9 = (uint)*(ushort *)(param_1 + 0xb810 + uVar8 * 2) +
                    (uint)*(ushort *)(param_1 + 0xb810 + (uint)param_3 * 2) * *(int *)(iVar10 + 4),
           (*(uint *)(iVar10 + 0xc + (uVar9 >> 5) * 4) & 1 << ((byte)uVar9 & 0x1f)) == 0)))))) {
        DAT_007dc8cc = DAT_007dc8cc + 1;
        FUN_00563e70(*(int *)(param_1 + 0xc010), param_3, (ushort)uVar8);
      }
    }
    local_2ad0 = 0xff;
    fVar1 = local_2aa4[0] * _DAT_005e5418 + local_2a94 * _DAT_005cc318;
    fVar2 = local_2aa4[1] * _DAT_005e5418 + local_2a90 * _DAT_005cc318;
    fVar3 = local_2aa4[2] * _DAT_005e5418 + local_2a8c * _DAT_005cc318;
    fVar4 = local_2a94 * _DAT_005e5418 + local_2aa4[0] * _DAT_005cc318;
    fVar5 = local_2a90 * _DAT_005e5418 + local_2aa4[1] * _DAT_005cc318;
    fVar6 = local_2a8c * _DAT_005e5418 + local_2aa4[2] * _DAT_005cc318;
    if (fVar4 < param_2[4]) {
      local_2ad0 = 0xaa;
    }
    if (*param_2 < fVar1) {
      local_2ad0 = local_2ad0 & 0x55;
    }
    if (fVar5 < param_2[5]) {
      local_2ad0 = local_2ad0 & 0xcc;
    }
    if (param_2[1] < fVar2) {
      local_2ad0 = local_2ad0 & 0x33;
    }
    if (fVar6 < param_2[6]) {
      local_2ad0 = local_2ad0 & 0xf0;
    }
    if (param_2[2] < fVar3) {
      local_2ad0 = local_2ad0 & 0xf;
    }
    local_2ad4 = 0;
    local_2ad8 = 0xffffffff;
    pfVar11 = afStack_27ec + local_2ac8 * 8;
    do {
      if ((local_2ad0 & 1 << ((byte)local_2ad4 & 0x1f)) != 0) {
        uVar7 = *(ushort *)(param_1 + 0x9820 + (local_2a84 * 10 + local_2ad4) * 2);
        if ((uVar7 & 0x8000) == 0) {
          local_2a80[local_2ac8] = uVar7;
          uVar8 = (int)local_2ad4 >> 1 & 1;
          uVar9 = (int)local_2ad4 >> 2 & 1;
          pfVar11[-1] = (float)(local_2ad4 & 1) * fVar1 +
                        (float)(int)(1 - (local_2ad4 & 1)) * local_2a94;
          *pfVar11 = (float)uVar8 * fVar2 + (float)(int)(1 - uVar8) * local_2a90;
          uVar8 = (int)local_2ad8 >> 1 & 1;
          pfVar11[1] = (float)uVar9 * fVar3 + (float)(int)(1 - uVar9) * local_2a8c;
          uVar9 = (int)local_2ad8 >> 2 & 1;
          pfVar11[-5] = (float)(local_2ad8 & 1) * fVar4 +
                        (float)(int)(1 - (local_2ad8 & 1)) * local_2aa4[0];
          local_2ac8 = local_2ac8 + 1;
          pfVar11[-4] = (float)uVar8 * fVar5 + (float)(int)(1 - uVar8) * local_2aa4[1];
          pfVar11[-3] = (float)uVar9 * fVar6 + (float)(int)(1 - uVar9) * local_2aa4[2];
          pfVar11 = pfVar11 + 8;
        }
        else {
          for (uVar7 = *(ushort *)(param_1 + 0x9820 + ((local_2ad4 & 0xffff) + local_2a84 * 10) * 2)
                       & 0x3ff; uVar7 != 0x3ff;
              uVar7 = *(ushort *)(param_1 + 0x8820 + (uint)uVar7 * 4)) {
            uVar8 = *(ushort *)(param_1 + 0x881e + (uint)uVar7 * 4) & 0x3ff;
            DAT_007dc8c8 = DAT_007dc8c8 + 1;
            pfVar12 = (float *)(uVar8 * 0x20 + param_1);
            if ((((pfVar12[4] < *param_2 != (pfVar12[4] == *param_2)) &&
                 (param_2[4] < *pfVar12 != (param_2[4] == *pfVar12))) &&
                (pfVar12[5] < param_2[1] != (pfVar12[5] == param_2[1]))) &&
               (((param_2[5] < pfVar12[1] != (param_2[5] == pfVar12[1]) &&
                 (pfVar12[6] < param_2[2] != (pfVar12[6] == param_2[2]))) &&
                ((param_2[6] < pfVar12[2] != (param_2[6] == pfVar12[2]) &&
                 ((iVar10 = *(int *)(param_1 + 0xc018), iVar10 == 0 ||
                  (uVar9 = (uint)*(ushort *)(param_1 + 0xb810 + uVar8 * 2) +
                           (uint)*(ushort *)(param_1 + 0xb810 + (uint)param_3 * 2) *
                           *(int *)(iVar10 + 4),
                  (*(uint *)(iVar10 + 0xc + (uVar9 >> 5) * 4) & 1 << ((byte)uVar9 & 0x1f)) == 0)))))
                ))) {
              DAT_007dc8cc = DAT_007dc8cc + 1;
              FUN_00563e70(*(int *)(param_1 + 0xc010), param_3, (ushort)uVar8);
            }
          }
        }
      }
      local_2ad4 = local_2ad4 + 1;
      local_2ad8 = local_2ad8 - 1;
    } while (-9 < (int)local_2ad8);
  } while (0 < local_2ac8);
  return;
}

// ---------------------------------------------------------------------------
// 0x00564c80  Octree insert: inflates param_3's AABB by the margin float at param_1+0xc014
//             into the primitive-box slot param_2, runs the overlap query (FUN_005646c0),
//             then descends octants (FUN_00564310) to the deepest fitting node, links the
//             primitive there, and splits the node when its count field saturates.
//             Returns param_1. 1179 B, __cdecl.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_00564c80(int param_1, uint param_2, float *param_3)
{
  uint *puVar1;
  ushort *puVar2;
  byte *pbVar3;
  short sVar4;
  ushort uVar5;
  ushort uVar6;
  ushort uVar7;
  ushort uVar8;
  uint uVar9;
  float *pfVar10;
  int iVar11;
  int iVar12;
  uint uVar13;
  undefined4 uVar14;
  float *pfVar15;
  float *pfVar16;
  uint uVar17;
  // Contiguous block (header note 2): local_40[4] + local_30/2c/28 is one 8-float box.
  float local_40[8];
  float &local_30 = local_40[4];
  float &local_2c = local_40[5];
  float &local_28 = local_40[6];
  float local_20[8];

  uVar9 = param_2 & 0xffff;
  pfVar10 = (float *)(uVar9 * 0x20 + param_1);
  pfVar10[4] = param_3[4] - *(float *)(param_1 + 0xc014);
  pfVar10[5] = param_3[5] - *(float *)(param_1 + 0xc014);
  pfVar10[6] = param_3[6] - *(float *)(param_1 + 0xc014);
  *pfVar10 = *(float *)(param_1 + 0xc014) + *param_3;
  pfVar10[1] = param_3[1] + *(float *)(param_1 + 0xc014);
  pfVar10[2] = param_3[2] + *(float *)(param_1 + 0xc014);
  FUN_005646c0(param_1, pfVar10, (ushort)param_2);
  pfVar15 = (float *)(param_1 + 0x7fe0);
  pfVar16 = local_40;
  for (iVar12 = 8; iVar12 != 0; iVar12 = iVar12 + -1) {
    *pfVar16 = *pfVar15;
    pfVar15 = pfVar15 + 1;
    pfVar16 = pfVar16 + 1;
  }
  uVar13 = 0;
  uVar6 = 0xffff;
  if ((((local_30 <= pfVar10[4]) && (*pfVar10 < local_40[0] != (*pfVar10 == local_40[0]))) &&
      (local_2c <= pfVar10[5])) &&
     (((pfVar10[1] < local_40[1] != (pfVar10[1] == local_40[1]) && (local_28 <= pfVar10[6])) &&
      ((pfVar10[2] < local_40[2] != (pfVar10[2] == local_40[2]) &&
       (uVar6 = (ushort)FUN_00564310(local_20, pfVar10, local_40), uVar6 != 0xffff)))))) {
    do {
      pfVar15 = local_20;
      pfVar16 = local_40;
      for (iVar12 = 8; iVar12 != 0; iVar12 = iVar12 + -1) {
        *pfVar16 = *pfVar15;
        pfVar15 = pfVar15 + 1;
        pfVar16 = pfVar16 + 1;
      }
      iVar12 = (uint)uVar6 + (uVar13 & 0xffff) * 10;
      sVar4 = *(short *)(param_1 + 0x9820 + iVar12 * 2);
      uVar17 = CONCAT22((ushort)((uint)iVar12 >> 0x10), (ushort)sVar4);
      if (sVar4 < 0) {
        if (uVar6 != 0xffff) {
          uVar5 = *(ushort *)(param_1 + 0x981a);
          uVar17 = (uint)uVar5;
          *(undefined2 *)(param_1 + 0x981a) = *(undefined2 *)(param_1 + 0x8820 + uVar17 * 4);
          *(ushort *)(param_1 + 0x881e + uVar17 * 4) = (ushort)param_2;
          puVar2 = (ushort *)(param_1 + 0x9820 + ((uint)uVar6 + (uVar13 & 0xffff) * 10) * 2);
          *(ushort *)(param_1 + 0x8820 + uVar17 * 4) = *puVar2 & 0x3ff;
          *puVar2 = *puVar2 & 0xfc00 | uVar5;
          *(ushort *)(param_1 + 0x8020 + uVar9 * 2) = uVar6 & 0x3f | (ushort)(uVar13 << 6);
          iVar12 = FUN_005641b0(pfVar10, local_40);
          if (iVar12 != 0) {
            pbVar3 = (byte *)(param_1 + 0x881f + uVar17 * 4);
            *pbVar3 = *pbVar3 | 0x80;
            if ((*puVar2 & 0x7c00) != 0x7c00) {
              *puVar2 = *puVar2 + 0x400;
            }
            if (0xc00 < (*puVar2 & 0x7c00)) {
              uVar5 = *(ushort *)(param_1 + 0xc00e);
              uVar14 = CONCAT22((ushort)((uVar13 << 6) >> 0x10), uVar5);
              if (uVar5 != 0x3ff) {
                uVar7 = *puVar2 & 0x3ff;
                iVar12 = (uint)uVar5 * 5 + 0x2607;
                *(undefined2 *)(param_1 + 0xc00e) = *(undefined2 *)(param_1 + iVar12 * 4);
                *puVar2 = uVar5;
                *(uint *)(param_1 + iVar12 * 4) =
                     ((uint)uVar6 << 10 | uVar13 & 0xffff) << 10 | 0x3ff;
                uVar9 = 0;
                iVar12 = (uint)uVar5 * 10;
                do {
                  uVar13 = uVar9 & 0xffff;
                  uVar9 = uVar9 + 1;
                  *(undefined2 *)(param_1 + 0x9820 + (uVar13 + iVar12) * 2) = 0x83ff;
                } while ((int)uVar9 < 8);
                if (uVar7 != 0x3ff) {
                  do {
                    uVar13 = (uint)uVar7;
                    uVar6 = *(ushort *)(param_1 + 0x8820 + uVar13 * 4);
                    uVar8 = *(ushort *)(param_1 + 0x881e + uVar13 * 4);
                    uVar9 = uVar8 & 0x3ff;
                    if ((short)uVar8 < 0) {
                      iVar11 = uVar9 * 0x20 + param_1;
                      uVar8 = (ushort)FUN_00564310(local_20, (float *)iVar11, local_40);
                      uVar17 = (uint)uVar8;
                      *(ushort *)(param_1 + 0x8820 + uVar13 * 4) =
                           *(ushort *)(param_1 + 0x9820 + (uVar17 + iVar12) * 2) & 0x3ff;
                      FUN_00565120(param_1, uVar14, uVar17, uVar7);
                      *(ushort *)(param_1 + 0x8020 + uVar9 * 2) = uVar8 & 0x3f | uVar5 << 6;
                      iVar11 = FUN_005641b0((float *)iVar11, local_20);
                      if (iVar11 == 0) {
                        pbVar3 = (byte *)(param_1 + 0x881f + uVar13 * 4);
                        *pbVar3 = *pbVar3 & 0x7f;
                      }
                      else {
                        FUN_005651b0(param_1, uVar14, uVar17);
                      }
                    }
                    else {
                      FUN_00565160(param_1, uVar14, uVar7);
                      *(ushort *)(param_1 + 0x8020 + uVar9 * 2) = uVar5 << 6 | 0x3f;
                    }
                    uVar7 = uVar6;
                  } while (uVar6 != 0x3ff);
                }
              }
            }
          }
          return param_1;
        }
        goto LAB_00564dfc;
      }
      uVar6 = (ushort)FUN_00564310(local_20, pfVar10, local_40);
      uVar13 = uVar17;
    } while (uVar6 != 0xffff);
    uVar6 = 0xffff;
  }
LAB_00564dfc:
  uVar17 = (uint)*(ushort *)(param_1 + 0x981a);
  *(undefined2 *)(param_1 + 0x981a) = *(undefined2 *)(param_1 + 0x8820 + uVar17 * 4);
  *(ushort *)(param_1 + 0x881e + uVar17 * 4) =
       *(ushort *)(param_1 + 0x881e + uVar17 * 4) & 0xfc00 | (ushort)param_2;
  iVar12 = (uVar13 & 0xffff) + 0x79b;
  puVar1 = (uint *)(param_1 + iVar12 * 0x14);
  *(ushort *)(param_1 + 0x8820 + uVar17 * 4) = *(ushort *)(param_1 + iVar12 * 0x14) & 0x3ff;
  *puVar1 = *puVar1 & 0xfffffc00 | uVar17;
  *(ushort *)(param_1 + 0x8020 + uVar9 * 2) = uVar6 & 0x3f | (ushort)(uVar13 << 6);
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x00565260  Octree remove (counterpart to FUN_00564c80): clears the primitive's pair
//             bucket (FUN_00563f60), unlinks it from its node's chain (root list when the
//             locator's low 6 bits are 0x3f, octant chain otherwise), returns the link node
//             to the free list, and frees the node itself when it empties. Contains the
//             original's degenerate recount loop (header note 6). 747 B, __cdecl.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00565260(int param_1, uint param_2)
{
  uint *puVar1;
  ushort uVar2;
  uint uVar3;
  ushort uVar4;
  int iVar5;
  ushort uVar6;
  uint uVar7;
  ushort *puVar8;
  ushort *puVar9;
  uint local_8;
  ushort *local_4;

  uVar3 = param_2;
  uVar7 = param_2 & 0xffff;
  FUN_00563f60(*(int *)(param_1 + 0xc010), (ushort)uVar7);
  uVar6 = *(ushort *)(param_1 + 0x8020 + uVar7 * 2);
  uVar2 = uVar6 & 0x3f;
  uVar6 = uVar6 >> 6;
  uVar7 = (uint)uVar6;
  param_2 = uVar7;
  if ((uVar2 == 0x3f) || (uVar2 == 0xffff)) {
    puVar1 = (uint *)(param_1 + (uVar7 * 5 + 0x2607) * 4);
    puVar8 = (ushort *)&param_2;
    param_2 = (ushort)*puVar1 & 0x3ff;
    uVar2 = *(ushort *)(param_1 + 0x881e + param_2 * 4);
    while (((uVar2 & 0x3ff) != (ushort)uVar3 && ((short)param_2 != 0x3ff))) {
      puVar8 = (ushort *)(param_1 + 0x8820 + param_2 * 4);
      param_2 = (uint)*(ushort *)(param_1 + 0x8820 + param_2 * 4);
      uVar2 = *(ushort *)(param_1 + 0x881e + param_2 * 4);
    }
    uVar2 = *puVar8;
    uVar3 = (uint)uVar2;
    if ((uint *)puVar8 == &param_2) {
      puVar9 = (ushort *)(param_1 + 0x8820 + uVar3 * 4);
      *puVar1 = *puVar1 & 0xfffffc00 | (uint)*(ushort *)(param_1 + 0x8820 + uVar3 * 4);
    }
    else {
      puVar9 = (ushort *)(param_1 + 0x8820 + uVar3 * 4);
      *puVar8 = *(ushort *)(param_1 + 0x8820 + uVar3 * 4);
    }
  }
  else {
    puVar1 = &local_8;
    puVar8 = (ushort *)(param_1 + 0x9820 + ((uint)uVar2 + uVar7 * 10) * 2);
    local_4 = puVar8;
    local_8 = *puVar8 & 0x3ff;
    uVar2 = *(ushort *)(param_1 + 0x881e + local_8 * 4);
    while (((uVar2 & 0x3ff) != (ushort)uVar3 && ((short)local_8 != 0x3ff))) {
      puVar1 = (uint *)(param_1 + 0x8820 + local_8 * 4);
      local_8 = (uint)*(ushort *)(param_1 + 0x8820 + local_8 * 4);
      uVar2 = *(ushort *)(param_1 + 0x881e + local_8 * 4);
    }
    uVar2 = (ushort)*puVar1;
    puVar9 = (ushort *)(param_1 + 0x8820 + (uint)uVar2 * 4);
    if (puVar1 == &local_8) {
      *puVar8 = *puVar8 & 0xfc00 | *puVar9;
    }
    else {
      *(ushort *)puVar1 = *puVar9;
    }
    if ((*(byte *)(param_1 + 0x881f + (uint)uVar2 * 4) & 0x80) != 0) {
      uVar4 = *puVar8;
      if ((uVar4 & 0x7c00) == 0x7c00) {
        iVar5 = 0;
        uVar3 = uVar4 & 0x3ff;
        if ((short)uVar3 != 0x3ff) {
          // DEGENERATE IN THE ORIGINAL (header note 6, 0x0056546a..0x00565486): the chain
          // cursor uVar3 is never advanced. Transcribed verbatim — do not "fix".
          while (iVar5 < 0x1f) {
            puVar8 = local_4;
            if ((*(byte *)(param_1 + 0x881f + uVar3 * 4) & 0x80) != 0) {
              iVar5 = iVar5 + 1;
            }
          }
        }
        uVar4 = uVar4 & 0x83ff | (ushort)(iVar5 << 10);
      }
      else {
        uVar4 = uVar4 - 0x400;
      }
      *puVar8 = uVar4;
    }
  }
  *puVar9 = *(ushort *)(param_1 + 0x981a);
  *(ushort *)(param_1 + 0x981a) = uVar2;
  if ((uVar6 != 0) &&
     (iVar5 = uVar7 * 5 + 0x2607, puVar1 = (uint *)(param_1 + iVar5 * 4),
     (*(ushort *)(param_1 + iVar5 * 4) & 0x3ff) == 0x3ff)) {
    uVar3 = 0;
    iVar5 = param_1 + 0x9820 + uVar7 * 0x14;
    do {
      if ((*(byte *)(iVar5 + 1) & 0x80) == 0) {
        return;
      }
      if ((*(ushort *)(param_1 + 0x9820 + ((uVar3 & 0xffff) + uVar7 * 10) * 2) & 0x3ff) != 0x3ff) {
        return;
      }
      uVar3 = uVar3 + 1;
      iVar5 = iVar5 + 2;
    } while ((int)uVar3 < 8);
    uVar3 = *puVar1;
    *(undefined2 *)(param_1 + 0x9820 + ((uVar3 >> 0x14 & 7) + (uVar3 >> 10 & 0x3ff) * 10) * 2) =
         0x83ff;
    *puVar1 = (uint)*(ushort *)(param_1 + 0xc00e);
    *(ushort *)(param_1 + 0xc00e) = uVar6;
  }
  return;
}

// --- gta-reversed-style hook registration (inert on the exe via HookSystemNoOp; installs
//     the inline-JMP under the .asi for the diff-original A/B acceptance) — CLUSTER 4. ---
RH_ScopedInstall(FUN_005646c0, 0x005646c0);
RH_ScopedInstall(FUN_00564c80, 0x00564c80);
RH_ScopedInstall(FUN_00565260, 0x00565260);

}  // namespace Collision
}  // namespace mashed_re
