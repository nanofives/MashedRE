// ============================================================================
//  RwpSolverCore19.cpp  —  B5e solver-island cluster K19
//  3 verbatim RWP-3.7 narrow-phase driver/wrapper helpers:
//    FUN_00578e50 (0x00578e50, 105 B)  — SAT-axis narrow-phase wrapper (calls K18
//                                        FUN_00578610, gate on depth < tol, dispatch)
//    FUN_0057adb0 (0x0057adb0, 105 B)  — TOI narrow-phase wrapper (calls K18
//                                        FUN_0057a9a0, same gate + dispatch)
//    FUN_005752b0 (0x005752b0, 680 B)  — box/box(-ish) narrow-phase driver: builds
//                                        the shape-type indices, runs the K15 SAT
//                                        driver FUN_00576880, writes the manifold
//                                        record, then the K18 clip FUN_00574ad0.
//  Deps: K15 (FUN_00576880), K18 (FUN_00578610, FUN_0057a9a0, FUN_005757d0,
//        FUN_00574ad0). All externed below; definitions live in the sibling
//        RwpSolverCore15/18 TUs of the same .asi.
//  Version anchor: MASHED.exe 2,846,720 B / SHA-256 BDCAE093…3C0E.
//
//  All float math is x87 (FLD/FCOMP/FSTP confirmed by disasm) — plain C float
//  compiles to x87 under /arch:IA32, so this is bit-faithful.
//
//  DISASM-RESOLVED CONVENTIONS (verified against listing, pool2 session):
//   * FUN_00578e50 / FUN_0057adb0 call their K18 callee with SEVEN __cdecl args
//     (ADD ESP,0x1c after CALL @0x578e88 / @0x57ade8). K18's FUN_00578610 body
//     reads only 6 params, but the wrapper pushes a 7th (param_5) which __cdecl
//     caller-cleanup discards — reproduced verbatim via a 7-param extern (C
//     linkage resolves by name, not arity). All 7 args are pushed as their exact
//     value types so no int<->float conversion is introduced (param_4+offset are
//     raw addresses = int; param_5 = raw dword = undefined4).
//   * FUN_005752b0: sVar1/sVar2 are ZERO-extended (XOR EAX,EAX / MOV AX,word @
//     0x5752c0..0x5752d1) → declared `ushort`, so `(uint)sVar` zero-extends to
//     match the pushed dword. The point-count at record+0xac (param_3[0x2b]) is an
//     INTEGER, written 0 (MOV [EDI+0xac],EBP) and compared `!=0` / looped as int
//     (CMP EAX,EBP / CMP ECX,EDX) — Ghidra mistyped it float via param_3 being
//     float*. FUN_00576880 args +0x50/+0xd0/+0x10c are pushed as raw dwords
//     (MOV/PUSH, no FLD) → externed `undefined4` for byte-exact pushes. The three
//     out-ptrs &local_108/&local_114/&local_110 go to NON-adjacent stack slots and
//     each reads back a single scalar — no contiguous-buffer trap. local_100[256]
//     is a proper stack array (LEA + PUSH).
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

// --- extern callees (K15/K18, defined in sibling TUs of same .asi) -----------
// K15 SAT driver — 12-arg __cdecl. +0x50/+0xd0/+0x10c pushed as raw dwords.
extern "C" uint       __cdecl FUN_00576880(undefined4 param_1,uint param_2,undefined4 param_3,uint param_4,float param_5,undefined4 param_6,float *param_7,float *param_8,uint *param_9,uint *param_10,float *param_11,float *param_12); // 0x00576880 (K15)
// K18 polygon clip / manifold fill — 6-arg __cdecl.
extern "C" undefined4 __cdecl FUN_00574ad0(float *param_1,float *param_2,uint param_3,float param_4,float param_5,float *param_6); // 0x00574ad0 (K18)
// K18 SAT-axis test — wrapper pushes 7 args (see header note); K18 body uses 6.
extern "C" undefined4 __cdecl FUN_00578610(undefined4 param_1,undefined4 param_2,float param_3,int param_4,int param_5,float *param_6,undefined4 param_7); // 0x00578610 (K18)
// K18 conservative-advancement TOI — 7-arg __cdecl.
extern "C" undefined4 __cdecl FUN_0057a9a0(undefined4 param_1,undefined4 param_2,float param_3,int param_4,int param_5,float *param_6,undefined4 param_7); // 0x0057a9a0 (K18)
// K18 two-shape narrow-phase dispatch — 2-arg __cdecl.
extern "C" bool       __cdecl FUN_005757d0(undefined4 param_1,int param_2); // 0x005757d0 (K18)

// forward decls for RH_ScopedInstall
extern "C" undefined4 __cdecl FUN_00578e50(undefined4 param_1,undefined4 param_2,float param_3,int param_4,undefined4 param_5);
extern "C" undefined4 __cdecl FUN_0057adb0(undefined4 param_1,undefined4 param_2,float param_3,int param_4,undefined4 param_5);
extern "C" undefined4 __cdecl FUN_005752b0(int param_1,float param_2,float *param_3);

// ---------------------------------------------------------------------------
// 0x00578e50  SAT-axis narrow-phase wrapper: run the K18 SAT separating-axis
//   test (FUN_00578610) over the shape pair, writing the axis to record+0x100,
//   scalar to +0x10c, depth to +0x110. If it separated AND the depth is below
//   the tolerance param_3, run the two-shape support dispatch (FUN_005757d0).
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_00578e50(undefined4 param_1,undefined4 param_2,float param_3,int param_4,undefined4 param_5)
{
  int iVar1;

  iVar1 = FUN_00578610(param_1,param_2,param_3,param_4 + 0x100,param_4 + 0x10c,
                       (float *)(param_4 + 0x110),param_5);
  if ((iVar1 != 0) && (*(float *)(param_4 + 0x110) < param_3)) {
    FUN_005757d0(param_2,param_4);
    return 1;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// 0x0057adb0  TOI narrow-phase wrapper: identical control flow to 0x00578e50
//   but the axis test is the conservative-advancement TOI iterate
//   (FUN_0057a9a0) instead of the static SAT axis test.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_0057adb0(undefined4 param_1,undefined4 param_2,float param_3,int param_4,undefined4 param_5)
{
  int iVar1;

  iVar1 = FUN_0057a9a0(param_1,param_2,param_3,param_4 + 0x100,param_4 + 0x10c,
                       (float *)(param_4 + 0x110),param_5);
  if ((iVar1 != 0) && (*(float *)(param_4 + 0x110) < param_3)) {
    FUN_005757d0(param_2,param_4);
    return 1;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// 0x005752b0  Box/box narrow-phase driver. param_1 = pair descriptor (two
//   0x88-stride shape blocks: shape A at +0x50.., shape B at +0xd0..), param_2 =
//   penetration tolerance, param_3 = manifold record out (float*). Builds the
//   per-shape type indices (0/1/2) from the shape-kind shorts at +0x54/+0xd4,
//   runs the K15 SAT driver FUN_00576880, then — if the separation is within
//   tolerance — writes the manifold header and runs the K18 clip FUN_00574ad0,
//   filling per-point friction/restitution and picking the reference normal.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_005752b0(int param_1,float param_2,float *param_3)
{
  ushort sVar1;
  ushort sVar2;
  ushort uVar3;
  int iVar4;
  float *pfVar5;
  float fVar6;
  int local_114;
  int local_110;
  float local_10c;
  float local_108;
  float local_104;
  undefined1 local_100 [256];

  local_10c = *(float *)(param_1 + 0x78) + *(float *)(param_1 + 0xf8);
  sVar1 = *(ushort *)(param_1 + 0x54);
  sVar2 = *(ushort *)(param_1 + 0xd4);
  local_104 = *(float *)(param_1 + 0xf8) - *(float *)(param_1 + 0x78);
  local_108 = *(float *)(param_1 + 0x110) + local_10c;
  if (sVar1 == 1) {
    local_114 = 0;
  }
  else {
    local_114 = (sVar1 != 2) + 1;
  }
  if (sVar2 == 1) {
    local_110 = 0;
  }
  else {
    local_110 = (sVar2 != 2) + 1;
  }
  iVar4 = FUN_00576880(*(undefined4 *)(param_1 + 0x50),sVar1,*(undefined4 *)(param_1 + 0xd0),sVar2,
                       local_104,*(undefined4 *)(param_1 + 0x10c),(float *)(param_1 + 0x100),
                       &local_108,(uint *)&local_114,(uint *)&local_110,(float *)param_1,
                       (float *)local_100);
  if ((local_108 - local_10c <= param_2) && (iVar4 != 0)) {
    *param_3 = *(float *)(param_1 + 0x100);
    param_3[1] = *(float *)(param_1 + 0x104);
    param_3[2] = *(float *)(param_1 + 0x108);
    param_3[0x2d] = *(float *)(param_1 + 0x58);
    param_3[0x2e] = *(float *)(param_1 + 0x5c);
    param_3[0x2f] = *(float *)(param_1 + 0xd8);
    param_3[0x30] = *(float *)(param_1 + 0xdc);
    if (*(float *)(param_1 + 0xf0) < *(float *)(param_1 + 0x70)) {
      fVar6 = *(float *)(param_1 + 0xf0);
    }
    else {
      fVar6 = *(float *)(param_1 + 0x70);
    }
    param_3[0x31] = fVar6;
    if (*(float *)(param_1 + 0xf4) < *(float *)(param_1 + 0x74)) {
      fVar6 = *(float *)(param_1 + 0xf4);
    }
    else {
      fVar6 = *(float *)(param_1 + 0x74);
    }
    *(undefined1 *)(param_3 + 0x33) = (undefined1)local_114;
    *(undefined1 *)((int)param_3 + 0xcd) = (undefined1)local_110;
    param_3[0x32] = fVar6;
    *(undefined2 *)((int)param_3 + 0xce) = 0;
    *(int *)((int)param_3 + 0xac) = 0;
    FUN_00574ad0((float *)param_1,(float *)local_100,iVar4,param_2,local_10c,param_3);
    if (*(int *)((int)param_3 + 0xac) != 0) {
      iVar4 = 0;
      if (*(int *)((int)param_3 + 0xac) != 0) {
        pfVar5 = param_3 + 0xc;
        do {
          iVar4 = iVar4 + 1;
          pfVar5[-1] = *(float *)(param_1 + 0x6c);
          *pfVar5 = *(float *)(param_1 + 0xec);
          pfVar5[-2] = 0.0;
          pfVar5[-3] = 0.0;
          pfVar5[-4] = 0.0;
          pfVar5 = pfVar5 + 10;
        } while ((uint)iVar4 < (uint)*(int *)((int)param_3 + 0xac));
      }
      if (((*(byte *)(param_1 + 0x7c) & 0x40) == 0) ||
         (uVar3 = *(ushort *)(param_1 + 0x7e), uVar3 == 0xe)) {
        if ((((*(byte *)(param_1 + 0xfc) & 0x40) != 0) &&
            (uVar3 = *(ushort *)(param_1 + 0xfe), uVar3 != 0xe)) &&
           (((~uVar3 & *(ushort *)(param_1 + 0xd6)) != 0 || ((local_110 == 2 && ((uVar3 & 1) != 0)))
            ))) {
          *param_3 = *(float *)(param_1 + 0xe0);
          param_3[1] = *(float *)(param_1 + 0xe4);
          param_3[2] = *(float *)(param_1 + 0xe8);
        }
      }
      else if (((~uVar3 & *(ushort *)(param_1 + 0x56)) != 0) ||
              ((local_114 == 2 && ((uVar3 & 1) != 0)))) {
        *param_3 = -*(float *)(param_1 + 0x60);
        param_3[1] = -*(float *)(param_1 + 100);
        param_3[2] = -*(float *)(param_1 + 0x68);
        return 1;
      }
      return 1;
    }
  }
  return 0;
}

// --- gta-reversed-style hook registration — CLUSTER 19. ---
RH_ScopedInstall(FUN_00578e50, 0x00578e50);
RH_ScopedInstall(FUN_0057adb0, 0x0057adb0);
RH_ScopedInstall(FUN_005752b0, 0x005752b0);

}  // namespace Collision
}  // namespace mashed_re
