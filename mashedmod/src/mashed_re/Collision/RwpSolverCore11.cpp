// Mashed RE — B5e: RwpSolver-island CLUSTER 11 clean-room port (the contact-island union-find /
// partition builder + the two per-pair contact-cache scatter passes).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool14, read_only, 2026-07-17). VERBATIM transcription of the 5 K11 functions from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm before
// porting. Style/idiom follows RwpSolverCore7..10.cpp.
//
// Members (rva:size): 00567f00:1460 00567c60:670 005685f0:431 00568dd0:505 00568fd0:354 —
//   5 fns / 3420 B, deps none (island leaves within the cluster; 67c60 -> 67f00 -> {005684c0,
//   00568560 (K1), 005685f0 (this cluster)}).
//
// NO-GUESSING verifications against live pool14 disasm (2026-07-17):
//  1. Calling convention: all 5 end in a plain RET (0xC3) => __cdecl. RET bytes read at body_end:
//     0x005684b3 (67f00), 0x00567efd (67c60), 0x0056879e (5685f0), 0x00568fc8 (68dd0),
//     0x00569131 (68fd0).
//  2. FUN_00567f00 is a union-find (disjoint-set path-compression) island merge with a 9-arg stack
//     ABI (Ghidra rendered only param_1/param_2 + in_stack_00000018/1c/20/24; the caller 67c60
//     pushes 9 args). Ported with a positional 9-slot signature: param_1@0x04, param_2@0x08,
//     a3@0x0c, a4@0x10, a5@0x14, in_stack_18@0x18, in_stack_1c@0x1c, in_stack_20@0x20,
//     in_stack_24@0x24. Its calls to FUN_005684c0/FUN_00568560 (K1) pass Ghidra-loose int/ptr
//     types; bound here through all-int ABI-shim externs (identical 32-bit __cdecl stack ABI to the
//     real RwpSolverLeaves1.cpp signatures) so every call arg is a plain (int) cast.
//  3. FUN_00567c60 / FUN_005685f0 are pure integer/pointer partition bookkeeping (list splice /
//     merge / scatter); transcribed verbatim.
//  4. FUN_00568dd0 / FUN_00568fd0 walk a contact list whose node struct Ghidra typed `float*`, so
//     the POINTER fields were mis-rendered as float ops (K10 class): `pfVar[0x2c]` (a shape ptr;
//     MOV @0x00568f4e), `pfVar[0x2d]/[0x2f]` (body ptrs), and `pfVar[0x37]` (list-next) are RAW
//     pointer reads -> `*(T**)(pfVar + off)`; `*pfVar1 = (float)pfVar12` and `param_1 = (int*)fVar4`
//     are BIT stores of a pointer/float into the other kind of slot. Fixed accordingly. The genuine
//     float math (`pfVar[0/1/2]` dot products) stays float; the 3-term dot @0x00568f38 is FADDP-
//     grouped `(v1*p1 + v0*p0) + v2*p2` (reverse-pair, <=1-ULP x87 floor -> U-9020); the LAB path
//     negates via FCHS (@0x00568f69). DAT_005d757c = 0.0f (clamp), sentinel 0xff7fffff = -FLT_MAX.
#include "../Core/HookSystem.h"
#include <cstring>                  // memcpy — pointer/float bit reinterpret

namespace mashed_re {
namespace Collision {

typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned int   undefined4;

#define DAT_005d757c   (*(const float*)0x005d757cu)   // 0.0f

static inline float asf(uint i) { float f; memcpy(&f,&i,4); return f; }

// --- extern callees. K1 pair bound as all-int ABI shims (identical 32-bit __cdecl stack ABI to
//     the real RwpSolverLeaves1.cpp sigs; extern "C" links by name). ---
extern "C" void __cdecl FUN_005684c0(int,int,int,int,int);                          // K1 (real: int,int*,int*,undefined4*,undefined4*)
extern "C" void __cdecl FUN_00568560(int,int,int,int,int);                          // K1 (real: int*,int,int*,undefined4*,undefined4*)
extern "C" void __cdecl FUN_00567f00(int param_1,int param_2,int a3,int a4,int a5,
                 int in_stack_00000018,uint in_stack_0000001c,int in_stack_00000020,
                 int in_stack_00000024);
extern "C" void __cdecl FUN_005685f0(int param_1,uint *param_2,uint *param_3,undefined4 *param_4,
                                     undefined4 *param_5);

// ---------------------------------------------------------------------------
// 0x00567f00  Contact-island union-find merge: for each broadphase pair (two loops — the
//             in_stack_18[0..1c) primary set and the in_stack_20 secondary list), classifies each
//             body as active/static, then path-compresses + unions the two bodies' set roots,
//             dispatching to FUN_005684c0 (new set) / FUN_005685f0 (merge two) / FUN_00568560
//             (append to one). Header note 2. Verbatim union-find; call args (int)-cast to the
//             all-int ABI shims.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00567f00(int param_1,int param_2,int a3,int a4,int a5,
                 int in_stack_00000018,uint in_stack_0000001c,int in_stack_00000020,
                 int in_stack_00000024)
{
  ushort uVar1;
  int iVar2;
  int iVar3;
  int *piVar4;
  int *piVar5;
  int *piVar6;
  int *piVar7;
  int iVar8;
  int *piVar9;
  int *piVar10;
  int iVar11;
  int *piVar12;
  int *piVar13;
  bool bVar14;
  uint local_4;
  (void)a3;(void)a4;(void)a5;

  local_4 = 0;
  if (in_stack_0000001c != 0) {
    do {
      iVar8 = *(int *)(in_stack_00000018 + local_4 * 4);
      if ((*(short *)(iVar8 + 0x54) == -1) ||
         (uVar1 = *(ushort *)(*(int *)(iVar8 + 0x50) + 0x20),
         (*(uint *)(*(int *)(*(int *)(*(int *)(iVar8 + 0x50) + 0x24) + 0x60) +
                   (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) == 0)) {
        iVar11 = 0;
      }
      else {
        iVar11 = 1;
      }
      if ((*(short *)(iVar8 + 0x5c) == -1) ||
         (uVar1 = *(ushort *)(*(int *)(iVar8 + 0x58) + 0x20),
         (*(uint *)(*(int *)(*(int *)(*(int *)(iVar8 + 0x58) + 0x24) + 0x60) +
                   (uint)(uVar1 >> 5) * 4) & 1 << ((byte)uVar1 & 0x1f)) == 0)) {
        iVar3 = 0;
      }
      else {
        iVar3 = 1;
      }
      if (iVar11 == iVar3) {
        iVar3 = *(int *)(iVar8 + 0x58);
        iVar11 = iVar8 + 8;
        piVar5 = (int *)((uint)*(ushort *)(*(int *)(iVar8 + 0x50) + 0x20) * 0x10 + param_2);
        piVar4 = (int *)((uint)*(ushort *)(iVar3 + 0x20) * 0x10 + param_2);
        bVar14 = *piVar5 == 0;
        iVar2 = *piVar4;
        if (bVar14) {
          piVar5[3] = *(int *)(iVar8 + 0x50);
        }
        if (iVar2 == 0) {
          piVar4[3] = iVar3;
        }
        if (bVar14 == (iVar2 == 0)) {
          if (bVar14) {
            FUN_005684c0(param_1,(int)piVar5,(int)piVar4,iVar11,0);
          }
          else {
            piVar7 = (int *)*piVar5;
            if (piVar7 == (int *)0x0) {
              iVar8 = 0;
            }
            else if (piVar7 == piVar5) {
              iVar8 = piVar5[1];
            }
            else {
              piVar6 = piVar7;
              piVar10 = (int *)*piVar7;
              if (piVar7 != (int *)*piVar7) {
                do {
                  piVar6 = piVar10;
                  piVar10 = (int *)*piVar6;
                } while (piVar6 != (int *)*piVar6);
              }
              if (piVar5 != piVar7) {
                do {
                  *piVar5 = (int)piVar6;
                  bVar14 = piVar7 != (int *)*piVar7;
                  piVar5 = piVar7;
                  piVar7 = (int *)*piVar7;
                } while (bVar14);
              }
              iVar8 = piVar6[1];
            }
            piVar5 = (int *)*piVar4;
            if (piVar5 == (int *)0x0) {
              iVar3 = 0;
            }
            else if (piVar5 == piVar4) {
              iVar3 = piVar4[1];
            }
            else {
              piVar7 = piVar5;
              piVar6 = (int *)*piVar5;
              if (piVar5 != (int *)*piVar5) {
                do {
                  piVar7 = piVar6;
                  piVar6 = (int *)*piVar7;
                } while (piVar7 != (int *)*piVar7);
              }
              if (piVar4 != piVar5) {
                do {
                  *piVar4 = (int)piVar7;
                  bVar14 = piVar5 != (int *)*piVar5;
                  piVar4 = piVar5;
                  piVar5 = (int *)*piVar5;
                } while (bVar14);
              }
              iVar3 = piVar7[1];
            }
            FUN_005685f0((int)param_1,(uint *)iVar8,(uint *)iVar3,(undefined4 *)iVar11,
                         (undefined4 *)0);
          }
        }
        else if (bVar14) {
          piVar7 = (int *)*piVar4;
          if (piVar7 == (int *)0x0) {
            FUN_00568560(0,(int)piVar4,(int)piVar5,iVar11,0);
          }
          else if (piVar7 == piVar4) {
            FUN_00568560(piVar4[1],(int)piVar4,(int)piVar5,iVar11,0);
          }
          else {
            piVar6 = piVar7;
            piVar10 = (int *)*piVar7;
            if (piVar7 != (int *)*piVar7) {
              do {
                piVar6 = piVar10;
                piVar10 = (int *)*piVar6;
              } while (piVar6 != (int *)*piVar6);
            }
            piVar10 = piVar4;
            if (piVar4 != piVar7) {
              do {
                piVar12 = piVar7;
                *piVar10 = (int)piVar6;
                piVar7 = (int *)*piVar12;
                piVar10 = piVar12;
              } while (piVar12 != (int *)*piVar12);
            }
            FUN_00568560(piVar6[1],(int)piVar4,(int)piVar5,iVar11,0);
          }
        }
        else {
          piVar7 = (int *)*piVar5;
          if (piVar7 == (int *)0x0) {
            FUN_00568560(0,(int)piVar5,(int)piVar4,iVar11,0);
          }
          else if (piVar7 == piVar5) {
            FUN_00568560(piVar5[1],(int)piVar5,(int)piVar4,iVar11,0);
          }
          else {
            piVar6 = piVar7;
            piVar10 = (int *)*piVar7;
            if (piVar7 != (int *)*piVar7) {
              do {
                piVar6 = piVar10;
                piVar10 = (int *)*piVar6;
              } while (piVar6 != (int *)*piVar6);
            }
            piVar10 = piVar5;
            if (piVar5 != piVar7) {
              do {
                piVar12 = piVar7;
                *piVar10 = (int)piVar6;
                piVar7 = (int *)*piVar12;
                piVar10 = piVar12;
              } while (piVar12 != (int *)*piVar12);
            }
            FUN_00568560(piVar6[1],(int)piVar5,(int)piVar4,iVar11,0);
          }
        }
      }
      else {
        if (iVar11 == 0) {
          iVar11 = *(int *)(iVar8 + 0x58);
        }
        else {
          iVar11 = *(int *)(iVar8 + 0x50);
        }
        piVar4 = (int *)((uint)*(ushort *)(iVar11 + 0x20) * 0x10 + param_2);
        iVar8 = iVar8 + 8;
        piVar5 = (int *)*piVar4;
        if (piVar5 == (int *)0x0) {
          piVar4[3] = iVar11;
          FUN_005684c0(param_1,(int)piVar4,0,iVar8,0);
        }
        else if (piVar5 == piVar4) {
          FUN_00568560(piVar4[1],0,(int)piVar4,iVar8,0);
        }
        else {
          piVar7 = piVar5;
          piVar6 = (int *)*piVar5;
          if (piVar5 != (int *)*piVar5) {
            do {
              piVar7 = piVar6;
              piVar6 = (int *)*piVar7;
            } while (piVar7 != (int *)*piVar7);
          }
          piVar6 = piVar4;
          if (piVar4 != piVar5) {
            do {
              piVar10 = piVar5;
              *piVar6 = (int)piVar7;
              piVar5 = (int *)*piVar10;
              piVar6 = piVar10;
            } while (piVar10 != (int *)*piVar10);
          }
          FUN_00568560(piVar7[1],0,(int)piVar4,iVar8,0);
        }
      }
      local_4 = local_4 + 1;
    } while (local_4 < in_stack_0000001c);
  }
  if (in_stack_00000024 != 0) {
    in_stack_00000018 = in_stack_00000024;
    piVar5 = (int *)(in_stack_00000020 + 0xb4);
    do {
      if (piVar5[-2] != 0) {
        if (((short)piVar5[1] == -1) ||
           (uVar1 = *(ushort *)(*piVar5 + 0x20),
           (*(uint *)(*(int *)(*(int *)(*piVar5 + 0x24) + 0x60) + (uint)(uVar1 >> 5) * 4) &
           1 << ((byte)uVar1 & 0x1f)) == 0)) {
          iVar8 = 0;
        }
        else {
          iVar8 = 1;
        }
        if (((short)piVar5[3] == -1) ||
           (uVar1 = *(ushort *)(piVar5[2] + 0x20),
           (*(uint *)(*(int *)(*(int *)(piVar5[2] + 0x24) + 0x60) + (uint)(uVar1 >> 5) * 4) &
           1 << ((byte)uVar1 & 0x1f)) == 0)) {
          iVar11 = 0;
        }
        else {
          iVar11 = 1;
        }
        if (iVar8 == iVar11) {
          iVar8 = piVar5[2];
          piVar6 = piVar5 + 8;
          piVar4 = (int *)((uint)*(ushort *)(*piVar5 + 0x20) * 0x10 + param_2);
          piVar7 = (int *)((uint)*(ushort *)(iVar8 + 0x20) * 0x10 + param_2);
          bVar14 = *piVar4 == 0;
          iVar11 = *piVar7;
          if (bVar14) {
            piVar4[3] = *piVar5;
          }
          if (iVar11 == 0) {
            piVar7[3] = iVar8;
          }
          if (bVar14 == (iVar11 == 0)) {
            if (bVar14) {
              FUN_005684c0(param_1,(int)piVar4,(int)piVar7,0,(int)piVar6);
            }
            else {
              piVar10 = (int *)*piVar4;
              if (piVar10 == (int *)0x0) {
                iVar8 = 0;
              }
              else if (piVar10 == piVar4) {
                iVar8 = piVar4[1];
              }
              else {
                piVar12 = piVar10;
                piVar13 = (int *)*piVar10;
                if (piVar10 != (int *)*piVar10) {
                  do {
                    piVar12 = piVar13;
                    piVar13 = (int *)*piVar12;
                  } while (piVar12 != (int *)*piVar12);
                }
                if (piVar4 != piVar10) {
                  do {
                    *piVar4 = (int)piVar12;
                    bVar14 = piVar10 != (int *)*piVar10;
                    piVar4 = piVar10;
                    piVar10 = (int *)*piVar10;
                  } while (bVar14);
                }
                iVar8 = piVar12[1];
              }
              piVar4 = (int *)*piVar7;
              if (piVar4 == (int *)0x0) {
                iVar11 = 0;
              }
              else if (piVar4 == piVar7) {
                iVar11 = piVar7[1];
              }
              else {
                piVar10 = piVar4;
                piVar12 = (int *)*piVar4;
                if (piVar4 != (int *)*piVar4) {
                  do {
                    piVar10 = piVar12;
                    piVar12 = (int *)*piVar10;
                  } while (piVar10 != (int *)*piVar10);
                }
                if (piVar7 != piVar4) {
                  do {
                    *piVar7 = (int)piVar10;
                    bVar14 = piVar4 != (int *)*piVar4;
                    piVar7 = piVar4;
                    piVar4 = (int *)*piVar4;
                  } while (bVar14);
                }
                iVar11 = piVar10[1];
              }
              FUN_005685f0((int)param_1,(uint *)iVar8,(uint *)iVar11,(undefined4 *)0,
                           (undefined4 *)piVar6);
            }
          }
          else if (bVar14) {
            piVar10 = (int *)*piVar7;
            if (piVar10 == (int *)0x0) {
              FUN_00568560(0,(int)piVar7,(int)piVar4,0,(int)piVar6);
            }
            else if (piVar10 == piVar7) {
              FUN_00568560(piVar7[1],(int)piVar7,(int)piVar4,0,(int)piVar6);
            }
            else {
              piVar12 = piVar10;
              piVar13 = (int *)*piVar10;
              if (piVar10 != (int *)*piVar10) {
                do {
                  piVar12 = piVar13;
                  piVar13 = (int *)*piVar12;
                } while (piVar12 != (int *)*piVar12);
              }
              piVar13 = piVar7;
              if (piVar7 != piVar10) {
                do {
                  piVar9 = piVar10;
                  *piVar13 = (int)piVar12;
                  piVar10 = (int *)*piVar9;
                  piVar13 = piVar9;
                } while (piVar9 != (int *)*piVar9);
              }
              FUN_00568560(piVar12[1],(int)piVar7,(int)piVar4,0,(int)piVar6);
            }
          }
          else {
            piVar10 = (int *)*piVar4;
            if (piVar10 == (int *)0x0) {
              FUN_00568560(0,(int)piVar4,(int)piVar7,0,(int)piVar6);
            }
            else if (piVar10 == piVar4) {
              FUN_00568560(piVar4[1],(int)piVar4,(int)piVar7,0,(int)piVar6);
            }
            else {
              piVar12 = piVar10;
              piVar13 = (int *)*piVar10;
              if (piVar10 != (int *)*piVar10) {
                do {
                  piVar12 = piVar13;
                  piVar13 = (int *)*piVar12;
                } while (piVar12 != (int *)*piVar12);
              }
              piVar13 = piVar4;
              if (piVar4 != piVar10) {
                do {
                  piVar9 = piVar10;
                  *piVar13 = (int)piVar12;
                  piVar10 = (int *)*piVar9;
                  piVar13 = piVar9;
                } while (piVar9 != (int *)*piVar9);
              }
              FUN_00568560(piVar12[1],(int)piVar4,(int)piVar7,0,(int)piVar6);
            }
          }
        }
        else {
          if (iVar8 == 0) {
            iVar8 = piVar5[2];
          }
          else {
            iVar8 = *piVar5;
          }
          piVar6 = (int *)((uint)*(ushort *)(iVar8 + 0x20) * 0x10 + param_2);
          piVar4 = piVar5 + 8;
          piVar7 = (int *)*piVar6;
          if (piVar7 == (int *)0x0) {
            piVar6[3] = iVar8;
            FUN_005684c0(param_1,(int)piVar6,0,0,(int)piVar4);
          }
          else if (piVar7 == piVar6) {
            FUN_00568560(piVar6[1],0,(int)piVar6,0,(int)piVar4);
          }
          else {
            piVar10 = piVar7;
            piVar12 = (int *)*piVar7;
            if (piVar7 != (int *)*piVar7) {
              do {
                piVar10 = piVar12;
                piVar12 = (int *)*piVar10;
              } while (piVar10 != (int *)*piVar10);
            }
            piVar12 = piVar6;
            if (piVar6 != piVar7) {
              do {
                piVar13 = piVar7;
                *piVar12 = (int)piVar10;
                piVar7 = (int *)*piVar13;
                piVar12 = piVar13;
              } while (piVar13 != (int *)*piVar13);
            }
            FUN_00568560(piVar10[1],0,(int)piVar6,0,(int)piVar4);
          }
        }
      }
      piVar5 = piVar5 + 0x38;
      in_stack_00000018 = in_stack_00000018 + -1;
    } while (in_stack_00000018 != 0);
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00567c60  Island partition finalize: zeroes the bucket heads, runs FUN_00567f00 to union the
//             pairs, then walks each root set writing the compacted body/edge/contact arrays into
//             the param_1 pool, and appends the leftover un-merged constraints. Header note 3.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00567c60(int param_1,int param_2,undefined4 *param_3,int param_4,
                 int param_5,uint param_6,undefined4 param_7,int param_8,int param_9,uint param_10)
{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  undefined4 *puVar6;
  uint uVar7;
  uint *puVar8;
  uint *puVar9;
  int iVar10;
  uint uVar11;
  uint *puVar12;
  int iVar13;
  uint uVar14;
  uint *puVar15;
  int local_10;

  iVar4 = param_1;
  *(undefined4 *)(param_2 + 4) = 0;
  puVar6 = param_3;
  for (iVar10 = param_4; iVar10 != 0; iVar10 = iVar10 + -1) {
    *puVar6 = 0;
    puVar6 = puVar6 + 4;
  }
  FUN_00567f00(param_2,(int)param_3,param_4,param_5,(int)param_6,param_7,param_8,param_9,
               (int)param_10);
  iVar10 = *(int *)(param_2 + 4);
  param_4 = 0;
  param_9 = 0;
  if (iVar10 == 0) {
    param_2 = 0;
  }
  else {
    param_2 = param_2 + 8;
  }
  iVar13 = 0;
  if (iVar10 != 0) {
    param_8 = 0;
    puVar12 = (uint *)(param_2 + 0x10);
    local_10 = iVar10;
    do {
      uVar2 = puVar12[3];
      uVar11 = puVar12[-3];
      uVar3 = *puVar12;
      piVar5 = (int *)(*(int *)(param_1 + 0x10) + param_8);
      param_10 = 0;
      iVar13 = *(int *)(param_1 + 8) + param_4 * 4;
      *piVar5 = iVar13;
      uVar14 = puVar12[-2];
      uVar7 = 0;
      if (uVar11 != 0) {
        do {
          *(undefined4 *)(iVar13 + uVar7 * 4) = *(undefined4 *)(uVar14 + 0xc);
          iVar13 = *piVar5;
          uVar14 = *(uint *)(uVar14 + 8);
          param_10 = param_10 + *(ushort *)(*(int *)(iVar13 + uVar7 * 4) + 0xc);
          uVar7 = uVar7 + 1;
        } while (uVar7 < uVar11);
      }
      piVar5[3] = uVar11;
      param_4 = param_4 + uVar11;
      piVar5[5] = param_10;
      uVar11 = 0;
      piVar5[2] = *(int *)(param_1 + 0xc) + param_9 * 4;
      puVar8 = (uint *)puVar12[1];
      if (uVar3 != 0) {
        do {
          *(uint **)(piVar5[2] + uVar11 * 4) = puVar8 + -2;
          puVar8 = (uint *)*puVar8;
          uVar11 = uVar11 + 1;
        } while (uVar11 < uVar3);
      }
      piVar5[4] = uVar3;
      param_9 = param_9 + uVar3;
      puVar15 = (uint *)0x0;
      uVar11 = 0;
      piVar5[1] = 0;
      puVar8 = (uint *)puVar12[4];
      if (uVar2 != 0) {
        do {
          puVar9 = puVar8;
          if (uVar11 == 0) {
            piVar5[1] = (int)(puVar9 + -0x35);
          }
          else {
            puVar15[0x37] = (uint)(puVar9 + -0x35);
          }
          puVar15 = puVar9 + -0x35;
          uVar11 = uVar11 + 1;
          puVar8 = (uint *)*puVar9;
        } while (uVar11 < uVar2);
        if (puVar15 != (uint *)0x0) {
          puVar9[2] = 0;
        }
      }
      piVar5[6] = 0;
      puVar12 = puVar12 + 10;
      param_8 = param_8 + 0x28;
      local_10 = local_10 + -1;
      iVar13 = iVar10;
    } while (local_10 != 0);
  }
  *(int *)(param_1 + 4) = iVar13;
  param_10 = 0;
  if (param_6 != 0) {
    param_1 = param_4 << 2;
    iVar13 = iVar10 * 0x28;
    param_2 = iVar10;
    do {
      iVar10 = *(int *)(param_5 + param_10 * 4);
      uVar1 = *(ushort *)(iVar10 + 0x20);
      if (param_3[(uint)uVar1 * 4] == 0) {
        if ((*(uint *)(*(int *)(*(int *)(iVar10 + 0x24) + 0x60) + (uint)(uVar1 >> 5) * 4) &
            1 << ((byte)uVar1 & 0x1f)) != 0) {
          puVar6 = (undefined4 *)(*(int *)(iVar4 + 0x10) + iVar13);
          param_2 = param_2 + 1;
          iVar13 = iVar13 + 0x28;
          piVar5 = (int *)(*(int *)(iVar4 + 8) + param_1);
          param_1 = param_1 + 4;
          *puVar6 = (undefined4)(int)piVar5;
          *piVar5 = iVar10;
          puVar6[3] = 1;
          puVar6[4] = 0;
          puVar6[1] = 0;
          uVar1 = *(ushort *)(*(int *)*puVar6 + 0xc);
          puVar6[6] = 0;
          puVar6[5] = (uint)uVar1;
        }
      }
      else {
        param_3[(uint)uVar1 * 4] = 0;
      }
      param_10 = param_10 + 1;
    } while (param_10 < param_6);
    *(int *)(iVar4 + 4) = param_2;
    return;
  }
  *(int *)(param_1 + 4) = iVar10;
  return;
}

// ---------------------------------------------------------------------------
// 0x005685f0  Disjoint-set list merge helper: appends param_4/param_5 records to the set-header
//             lists at param_2 (the same-set fast path) or splices two set headers (param_2 !=
//             param_3), keeping the two singly-linked run lists and count fields. Header note 3.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_005685f0(int param_1,uint *param_2,uint *param_3,undefined4 *param_4,
                 undefined4 *param_5)
{
  uint uVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  uint *puVar5;
  uint *puVar6;

  if (param_2 == param_3) {
    if (param_4 != (undefined4 *)0x0) {
      *param_4 = 0;
      if (param_2[4] == 0) {
        param_2[6] = (uint)param_4;
        param_2[5] = (uint)param_4;
        param_2[4] = 1;
      }
      else {
        *(undefined4 **)param_2[6] = param_4;
        param_2[6] = (uint)param_4;
        param_2[4] = param_2[4] + 1;
      }
    }
    if (param_5 != (undefined4 *)0x0) {
      *param_5 = 0;
      if (param_2[7] != 0) {
        *(undefined4 **)param_2[9] = param_5;
        param_2[9] = (uint)param_5;
        param_2[7] = param_2[7] + 1;
        return;
      }
      param_2[7] = 1;
      param_2[9] = (uint)param_5;
      param_2[8] = (uint)param_5;
      return;
    }
  }
  else {
    uVar1 = *param_3;
    puVar2 = param_2;
    puVar5 = param_3;
    if ((*param_2 <= uVar1) && (puVar2 = param_3, puVar5 = param_2, uVar1 == *param_2)) {
      *param_3 = uVar1 + 1;
    }
    *(uint *)puVar5[2] = puVar2[2];
    *(undefined4 *)(puVar5[2] + 4) = 0;
    *(uint *)(puVar2[3] + 8) = puVar5[2];
    puVar2[3] = puVar5[3];
    puVar2[1] = puVar2[1] + puVar5[1];
    if (puVar5[4] != 0) {
      if (puVar2[4] == 0) {
        puVar2[5] = puVar5[5];
        puVar2[6] = puVar5[6];
        puVar2[4] = puVar5[4];
      }
      else {
        *(uint *)puVar2[6] = puVar5[5];
        puVar2[6] = puVar5[6];
        puVar2[4] = puVar2[4] + puVar5[4];
      }
    }
    if (puVar5[7] != 0) {
      if (puVar2[7] == 0) {
        puVar2[8] = puVar5[8];
        puVar2[9] = puVar5[9];
        puVar2[7] = puVar5[7];
      }
      else {
        *(uint *)puVar2[9] = puVar5[8];
        puVar2[9] = puVar5[9];
        puVar2[7] = puVar2[7] + puVar5[7];
      }
    }
    if (param_4 != (undefined4 *)0x0) {
      *param_4 = 0;
      if (puVar2[4] == 0) {
        puVar2[6] = (uint)param_4;
        puVar2[5] = (uint)param_4;
        puVar2[4] = 1;
      }
      else {
        *(undefined4 **)puVar2[6] = param_4;
        puVar2[6] = (uint)param_4;
        puVar2[4] = puVar2[4] + 1;
      }
    }
    if (param_5 != (undefined4 *)0x0) {
      *param_5 = 0;
      if (puVar2[7] == 0) {
        puVar2[9] = (uint)param_5;
        puVar2[8] = (uint)param_5;
        puVar2[7] = 1;
      }
      else {
        *(undefined4 **)puVar2[9] = param_5;
        puVar2[9] = (uint)param_5;
        puVar2[7] = puVar2[7] + 1;
      }
    }
    iVar3 = *(int *)(param_1 + 4) + -1;
    *(int *)(param_1 + 4) = iVar3;
    if (iVar3 != 0) {
      puVar2 = (uint *)(param_1 + 8 + iVar3 * 0x28);
      puVar6 = puVar5;
      for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1) {
        *puVar6 = *puVar2;
        puVar2 = puVar2 + 1;
        puVar6 = puVar6 + 1;
      }
      *(uint **)(puVar5[2] + 4) = puVar5;
    }
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00568dd0  Contact-cache scatter (build pass): for each active contact node, hashes the two
//             body/shape ids into the param_1 open-addressing table, writes the packed record
//             (ids + the two contact points + the relative-normal-velocity dot), clamps the dot
//             at 0, and links it into the bucket. Header note 4 (pointer fields de-confused).
// ---------------------------------------------------------------------------
extern "C" int * __cdecl FUN_00568dd0(int *param_1,int param_2)
{
  float *pfVar1;
  ushort uVar2;
  ushort uVar3;
  int iVar4;
  int iVar5;
  void *pShape;
  float fVar7;
  uint uVar8;
  int iVar9;
  uint uVar10;
  float *pfVar11;
  float *pfVar12;
  int local_10;
  int local_c;
  uint local_8;

  uVar8 = 0;
  local_10 = 0;
  if (param_1[1] != 0) {
    do {
      uVar8 = uVar8 + 1;
      *(undefined4 *)(*param_1 + -4 + uVar8 * 4) = 0;
    } while (uVar8 < (uint)param_1[1]);
  }
  local_8 = 0;
  if (*(int *)(param_2 + 4) != 0) {
    local_c = 0;
    do {
      iVar9 = *(int *)(param_2 + 0x10) + local_c;
      if (((*(byte *)(iVar9 + 0x18) & 3) == 0) &&
         (pfVar11 = *(float **)(iVar9 + 4), pfVar11 != (float *)0x0)) {
        iVar9 = local_10 * 0x28;
        do {
          uVar2 = *(ushort *)(pfVar11 + 0x2e);
          pfVar12 = (float *)(param_1[3] + iVar9);
          local_10 = local_10 + 1;
          iVar9 = iVar9 + 0x28;
          if (uVar2 == 0xffff) {
            uVar8 = 0xffff;
          }
          else {
            uVar8 = (uint)*(ushort *)(*(int *)(pfVar11 + 0x2d) + 0x20);
          }
          uVar3 = *(ushort *)(pfVar11 + 0x30);
          if (uVar3 == 0xffff) {
            uVar10 = 0xffff;
          }
          else {
            uVar10 = (uint)*(ushort *)(*(int *)(pfVar11 + 0x2f) + 0x20);
          }
          iVar4 = *param_1;
          iVar5 = param_1[1];
          *(undefined4 *)pfVar12 = *(undefined4 *)(pfVar11 + 0x2d);   // copy shape-ptr bits
          pfVar1 = (float *)(iVar4 + ((uVar8 * 0x20 + (uint)uVar2) * 0x3ffd + uVar10 * 0x20 +
                                      (uint)uVar3 & iVar5 - 1U) * 4);
          pfVar12[1] = pfVar11[0x2e];
          pfVar12[2] = pfVar11[0x2f];
          pfVar12[3] = pfVar11[0x30];
          pfVar12[5] = *pfVar11;
          pfVar12[6] = pfVar11[1];
          pfVar12[7] = pfVar11[2];
          if (*(short *)(pfVar11 + 0x2e) == -1) {
LAB_00568f4e:
            pShape = *(void **)(pfVar11 + 0x2c);
            fVar7 = -(*(float *)((int)pShape + 0x28) * pfVar11[2] +
                     *(float *)((int)pShape + 0x20) * *pfVar11 +
                     *(float *)((int)pShape + 0x24) * pfVar11[1]);
          }
          else {
            iVar4 = *(int *)(*(int *)(pfVar11 + 0x2d) + 0x24);
            if ((iVar4 == 0) ||
               (uVar2 = *(ushort *)(*(int *)(pfVar11 + 0x2d) + 0x20),
               (*(uint *)(*(int *)(iVar4 + 0x60) + (uint)(uVar2 >> 5) * 4) &
               1 << ((byte)uVar2 & 0x1f)) == 0)) goto LAB_00568f4e;
            {
              float *pfVar6 = *(float **)(pfVar11 + 0x2c);
              fVar7 = pfVar6[2] * pfVar11[2] + *pfVar6 * *pfVar11 + pfVar6[1] * pfVar11[1];
            }
          }
          pfVar12[8] = fVar7;
          if (pfVar12[8] < DAT_005d757c) {
            pfVar12[8] = 0.0;
          }
          pfVar12[9] = *pfVar1;
          *(uint *)pfVar1 = (uint)pfVar12;                 // link: store node ptr bits into bucket
          pfVar11 = *(float **)(pfVar11 + 0x37);           // list-next
        } while (pfVar11 != (float *)0x0);
      }
      local_8 = local_8 + 1;
      local_c = local_c + 0x28;
    } while (local_8 < *(uint *)(param_2 + 4));
  }
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x00568fd0  Contact-cache query (max pass): for each active contact node, probes the param_1
//             hash table for the matching body pair and keeps the largest relative-normal-velocity
//             dot into node+0x34 (starting from -FLT_MAX). Header note 4 (pointer fields
//             de-confused; the -FLT_MAX/float accumulator ported as a plain float local).
// ---------------------------------------------------------------------------
extern "C" int * __cdecl FUN_00568fd0(int *param_1,int param_2)
{
  ushort uVar1;
  ushort uVar2;
  float *pfVar3;
  float fVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  uint uVar8;
  int local_8;
  uint local_4;

  piVar5 = param_1;
  local_4 = 0;
  if (*(int *)(param_2 + 4) == 0) {
    return param_1;
  }
  local_8 = 0;
  do {
    iVar6 = *(int *)(param_2 + 0x10) + local_8;
    if ((*(byte *)(iVar6 + 0x18) & 3) == 0) {
      for (pfVar3 = *(float **)(iVar6 + 4); pfVar3 != (float *)0x0;
           pfVar3 = *(float **)(pfVar3 + 0x37)) {
        float best;
        uVar1 = *(ushort *)(pfVar3 + 0x2e);
        best = asf(0xff7fffff);                    // -FLT_MAX
        pfVar3[0x34] = 0.0;
        if (uVar1 == 0xffff) {
          uVar7 = 0xffff;
        }
        else {
          uVar7 = (uint)*(ushort *)(*(int *)(pfVar3 + 0x2d) + 0x20);
        }
        uVar2 = *(ushort *)(pfVar3 + 0x30);
        if (uVar2 == 0xffff) {
          uVar8 = 0xffff;
        }
        else {
          uVar8 = (uint)*(ushort *)(*(int *)(pfVar3 + 0x2f) + 0x20);
        }
        for (iVar6 = *(int *)(*piVar5 +
                             ((uVar7 * 0x20 + (uint)uVar1) * 0x3ffd + uVar8 * 0x20 + (uint)uVar2 &
                             piVar5[1] - 1U) * 4); iVar6 != 0; iVar6 = *(int *)(iVar6 + 0x24)) {
          if ((((*(ushort *)(iVar6 + 4) == uVar1) && (*(ushort *)(iVar6 + 0xc) == uVar2)) ||
              ((*(ushort *)(iVar6 + 4) == uVar2 && (*(ushort *)(iVar6 + 0xc) == uVar1)))) &&
             (fVar4 = *(float *)(iVar6 + 0x1c) * pfVar3[2] +
                      *(float *)(iVar6 + 0x14) * *pfVar3 + *(float *)(iVar6 + 0x18) * pfVar3[1],
             best < fVar4)) {
            pfVar3[0x34] = *(float *)(iVar6 + 0x20);
            best = fVar4;
          }
        }
      }
    }
    local_4 = local_4 + 1;
    local_8 = local_8 + 0x28;
  } while (local_4 < *(uint *)(param_2 + 4));
  return piVar5;
}

// --- gta-reversed-style hook registration — CLUSTER 11. ---
RH_ScopedInstall(FUN_00567f00, 0x00567f00);
RH_ScopedInstall(FUN_00567c60, 0x00567c60);
RH_ScopedInstall(FUN_005685f0, 0x005685f0);
RH_ScopedInstall(FUN_00568dd0, 0x00568dd0);
RH_ScopedInstall(FUN_00568fd0, 0x00568fd0);

}  // namespace Collision
}  // namespace mashed_re
