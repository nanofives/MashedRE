// ============================================================================
//  RwpSolverCore22.cpp  —  B5e solver-island cluster K22
//  2 verbatim RWP-3.7 swept-contact drivers:
//    FUN_00573670 (0x00573670, 544 B)  — swept-pair edge-list pump: for each edge pair,
//                                         resolves the two shapes' sub-volume LUT slots
//                                         (param_4/param_5/param_6 tables), fetches both
//                                         transforms (FUN_0055abb0 K3 / FUN_0055bd80 K5),
//                                         runs the two-sided closest-feature query
//                                         FUN_00575c60 (K18) twice, generates the manifold
//                                         set with FUN_00576640 (K16), then drives each
//                                         manifold through the K21 swept-pair TOI loop
//                                         FUN_005729a0, accumulating the hit count.
//    FUN_0056b9d0 (0x0056b9d0, 95 B)   — per-body swept-contact entry: primes the world
//                                         scratch (FUN_0056b7a0 K18) then calls
//                                         FUN_00573670 (within) with the body's edge list.
//  Deps: K3 (FUN_0055abb0), K5 (FUN_0055bd80), K16 (FUN_00576640), K18 (FUN_00575c60,
//        FUN_0056b7a0), K21 (FUN_005729a0). All externed below; definitions live in
//        sibling TUs of the same .asi.
//  Version anchor: MASHED.exe 2,846,720 B / SHA-256 BDCAE093…3C0E.
//
//  Pure control/pointer code — no local float math (all arithmetic is index/address
//  computation); the float work is entirely inside the K18/K16/K21 callees.
//
//  DISASM-RESOLVED CONVENTIONS (mirror FUN_00568990 in K21):
//   * FUN_00573670 reuses the incoming `param_2` (edge-list base) as a scratch int after
//     piVar5 is derived from it (piVar5 = param_2+8 computed ONCE before the loop; the
//     later `param_2 = …` writes are shape-B's sub-buffer address, consumed only by the
//     FUN_005729a0 call). Verbatim per the Ghidra decomp (the K20 param-reuse idiom).
//   * FUN_00575c60 (K18) param_3 (iVar7/iVar6) is a raw sub-volume address (or 0), a
//     `float*` in K18's signature → cast (float*)int. param_6 gets the raw int* pointer as
//     an `undefined4` dword (same as K21's FUN_00568990).
//   * local_40/local_20 are 32-byte transform scratch, passed both as `undefined4` (the
//     buffer address, to FUN_0055abb0/FUN_0055bd80) and as `float*` (to FUN_00575c60).
// ============================================================================
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;
typedef unsigned char  undefined1;
typedef unsigned short ushort;

// --- extern callees (defined in sibling K-cluster TUs of same .asi) ----------
extern "C" undefined4 __cdecl FUN_0055abb0(undefined4 *param_1,int param_2,undefined4 param_3);       // 0x0055abb0 (K3)
extern "C" void       __cdecl FUN_0055bd80(int param_1,int param_2,undefined4 param_3,undefined4 param_4); // 0x0055bd80 (K5)
extern "C" uint       __cdecl FUN_00576640(int *param_1,uint param_2,uint param_3,int param_4,int param_5); // 0x00576640 (K16)
extern "C" int        __cdecl FUN_00575c60(undefined4 *param_1,uint *param_2,float *param_3,float *param_4,float *param_5,undefined4 param_6); // 0x00575c60 (K18)
extern "C" int        __cdecl FUN_0056b7a0(undefined4 *param_1,int param_2,uint param_3,int param_4); // 0x0056b7a0 (K18)
extern "C" undefined4 __cdecl FUN_005729a0(float *param_1,int *param_2,float *param_3,float *param_4); // 0x005729a0 (K21)

// forward decls for RH_ScopedInstall / mutual reference
extern "C" int        __cdecl FUN_00573670(int param_1,int param_2,uint param_3,int param_4,int param_5,int param_6,int param_7);
extern "C" undefined4 __cdecl FUN_0056b9d0(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,undefined4 param_6,undefined4 param_7);

// ---------------------------------------------------------------------------
// 0x00573670  Swept-pair edge-list pump. param_1 = solver context, param_2 = edge-pair
//   array (0x14 stride), param_3 = pair count, param_4 = sub-volume id LUT, param_5 =
//   sub-volume descriptor table, param_6 = sub-volume swept-fraction table, param_7 =
//   enable flag. Returns the accumulated TOI-hit count.
// ---------------------------------------------------------------------------
extern "C" int __cdecl
FUN_00573670(int param_1,int param_2,uint param_3,int param_4,int param_5,int param_6,int param_7)
{
  ushort uVar1;
  int *piVar2;
  undefined4 uVar3;
  uint uVar4;
  int *piVar5;
  int iVar6;
  int iVar7;
  uint uVar8;
  int *piVar9;
  int local_54;
  int local_50;
  uint local_4c;
  undefined1 local_40 [32];
  undefined1 local_20 [32];

  local_54 = 0;
  if ((param_3 != 0) && (param_7 != 0)) {
    local_4c = 0;
    piVar9 = (int *)(param_1 + 0x37c);
    piVar2 = (int *)**(int **)(param_1 + 0x70);
    *(undefined4 *)(param_1 + 0x3ec) = *(undefined4 *)(*piVar2 + 0xc014);
    *(int *)(param_1 + 0x3f0) = piVar2[3];
    uVar3 = *(undefined4 *)(*piVar2 + 0xc018);
    if (param_3 != 0) {
      piVar5 = (int *)(param_2 + 8);
      do {
        iVar6 = 0;
        if (*(int *)(param_1 + 0x3f4) != 0) {
          return local_54;
        }
        iVar7 = 0;
        local_50 = 0;
        if (((short)piVar5[-1] != -1) &&
           (uVar1 = *(ushort *)(param_4 + (uint)*(ushort *)(piVar5[-2] + 0x20) * 2), uVar1 != 0)) {
          iVar7 = (uint)uVar1 * 0x20 + param_5;
          local_50 = param_6 + (uint)uVar1 * 4;
        }
        param_2 = 0;
        if (((short)piVar5[1] != -1) &&
           (uVar1 = *(ushort *)(param_4 + (uint)*(ushort *)(*piVar5 + 0x20) * 2), uVar1 != 0)) {
          iVar6 = (uint)uVar1 * 0x20 + param_5;
          param_2 = param_6 + (uint)uVar1 * 4;
        }
        if ((short)piVar5[-1] == -1) {
          FUN_0055bd80(piVar5[-2],0,0,(undefined4)(int)local_40);
        }
        else {
          FUN_0055abb0((undefined4 *)piVar2,piVar5[-2],(undefined4)(int)local_40);
        }
        if ((short)piVar5[1] == -1) {
          FUN_0055bd80(*piVar5,0,0,(undefined4)(int)local_20);
        }
        else {
          FUN_0055abb0((undefined4 *)piVar2,*piVar5,(undefined4)(int)local_20);
        }
        *(undefined4 *)(param_1 + 0x380) = 0;
        *(undefined4 *)(param_1 + 0x398) = 0;
        *(undefined4 *)(param_1 + 0x38c) = 0;
        *(undefined4 *)(param_1 + 0x3d8) = 0;
        iVar7 = FUN_00575c60((undefined4 *)piVar9,(uint *)(piVar5 + -2),(float *)iVar7,(float *)local_40,
                             (float *)local_20,(undefined4)(int)piVar5);
        if ((iVar7 != 0) &&
           (iVar6 = FUN_00575c60((undefined4 *)piVar9,(uint *)piVar5,(float *)iVar6,(float *)local_20,
                                 (float *)local_40,(undefined4)(int)(piVar5 + -2)), iVar6 != 0)) {
          uVar4 = FUN_00576640(piVar9,iVar7,iVar6,piVar5[2],uVar3);
          uVar8 = 0;
          if (uVar4 != 0) {
            do {
              iVar6 = (int)FUN_005729a0((float *)param_1,(int *)(*piVar9 + uVar8 * 8),
                                        (float *)local_50,(float *)param_2);
              local_54 = local_54 + iVar6;
              uVar8 = uVar8 + 1;
            } while (uVar8 < uVar4);
          }
        }
        local_4c = local_4c + 1;
        piVar5 = piVar5 + 5;
      } while (local_4c < param_3);
    }
    return local_54;
  }
  return 0;
}

// ---------------------------------------------------------------------------
// 0x0056b9d0  Per-body swept-contact entry. param_1 = body/world handle, param_2..param_7
//   forwarded to the scratch primer (FUN_0056b7a0 K18) and the edge pump (FUN_00573670).
//   Always returns 1.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl
FUN_0056b9d0(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,undefined4 param_5,
             undefined4 param_6,undefined4 param_7)
{
  *(undefined4 *)(param_1 + 0x84) = 0;
  FUN_0056b7a0(*(undefined4 **)(param_1 + 0x70),param_2,param_3,
               *(undefined4 *)(*(int *)**(undefined4 **)(param_1 + 0x70) + 0xc018));
  FUN_00573670(param_1,*(undefined4 *)(param_1 + 0x80),*(undefined4 *)(param_1 + 0x84),param_4,
               param_5,param_6,param_7);
  return 1;
}

// --- gta-reversed-style hook registration — CLUSTER 22. ---
RH_ScopedInstall(FUN_00573670, 0x00573670);
RH_ScopedInstall(FUN_0056b9d0, 0x0056b9d0);

}  // namespace Collision
}  // namespace mashed_re
