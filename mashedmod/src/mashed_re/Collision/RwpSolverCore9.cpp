// Mashed RE — B5e: RwpSolver-island CLUSTER 9 clean-room port (the 0x0056e/f contact-batch
// bookkeeping leaves: per-batch counter reset/scatter/advance + the one Jacobian-row emit and its
// cross-product basis builder).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool14, read_only, 2026-07-17). VERBATIM transcription of the 7 K9 functions from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm before
// porting. Style/idiom follows RwpSolverCore7/8.cpp: extern "C" per-function, // 0x00xxxxxx RVA
// comments, cluster-internal forward decls, RH_ScopedInstall block.
//
// Members (rva:size): 0056ef30:133 0056efc0:83 0056f020:113 0056f0a0:328 0056f1f0:338 0056fad0:186
//   00567c00:49  — 7 fns / 1230 B. Within-cluster deps: f0a0->f1f0, 67c00->ef30 (confirmed via
//   function_callees). Zero indirect calls.
//
// NO-GUESSING verifications against live pool14 disasm (2026-07-17):
//  1. Calling convention: all 7 end in a plain RET (0xC3) => __cdecl. RET bytes read at each
//     body_end: 0x0056efb4, 0x0056f012, 0x0056f090, 0x0056f1e7 (f0a0: POP EDI/ESI/EBP/EBX @f1e3
//     then RET), 0x0056f341, 0x0056fb89, 0x00567c30.
//  2. FUN_0056ef30 / FUN_0056efc0 / FUN_0056f020 / FUN_0056f1f0 / FUN_0056f0a0 / FUN_00567c00 are
//     pure integer/pointer bookkeeping (counter zeroing / scatter / +1 advance / 4-align) — no
//     x87, no ambiguity; transcribed verbatim.
//  3. FUN_0056f0a0 passes &DAT_005e5738 to FUN_0056f1f0 (memory_read 0x005e5738 = a constant
//     descriptor table: 00 00 00 00 | 00 00 00 00 | 00 00 00 00 | FF FF FF FF | ... — the "unset"
//     row template). Bound by absolute address (standalone rebind = KV/lane-end, like K1's rand).
//  4. FUN_0056fad0 is the only float body: two optional blocks (param_5 / param_6 flags) each
//     build a 3-vector copy + a 3x3 cross-product basis. Every cross term is a single
//     FLD/FMUL/FLD/FMUL/FSUBP (minuend = the FIRST product) — verified against the FSUBP stream
//     (@0x0056fb08/fb17/fb26 block1, @0x0056fb66/fb75/fb84 block2); no multi-term associativity.
//     _DAT_005cc33c = -1.0f (memory_read 0x005cc33c = 000080bf) => param_1[8..10] = -param_2[0..2].
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

typedef unsigned int   uint;
typedef unsigned int   undefined4;

#define _DAT_005cc33c  (*(const float*)0x005cc33cu)   // -1.0f
#define DAT_005e5738   ((undefined4*)0x005e5738u)     // constant "unset" row-template table (note 3)

// --- Cluster-internal forward decls. ---
extern "C" void __cdecl FUN_0056ef30(int param_1);
extern "C" void __cdecl FUN_0056f1f0(int *param_1,undefined4 *param_2);

// ---------------------------------------------------------------------------
// 0x0056ef30  Zero the 26 per-batch running counters/offsets of the contact-batch header.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056ef30(int param_1)
{
  *(undefined4 *)(param_1 + 4) = 0;
  *(undefined4 *)(param_1 + 0x14) = 0;
  *(undefined4 *)(param_1 + 0x20) = 0;
  *(undefined4 *)(param_1 + 0x2c) = 0;
  *(undefined4 *)(param_1 + 0x38) = 0;
  *(undefined4 *)(param_1 + 0x44) = 0;
  *(undefined4 *)(param_1 + 0x50) = 0;
  *(undefined4 *)(param_1 + 0x5c) = 0;
  *(undefined4 *)(param_1 + 0x68) = 0;
  *(undefined4 *)(param_1 + 0x8c) = 0;
  *(undefined4 *)(param_1 + 0xb0) = 0;
  *(undefined4 *)(param_1 + 0xbc) = 0;
  *(undefined4 *)(param_1 + 0xec) = 0;
  *(undefined4 *)(param_1 + 0xe0) = 0;
  *(undefined4 *)(param_1 + 0xd4) = 0;
  *(undefined4 *)(param_1 + 0xc8) = 0;
  *(undefined4 *)(param_1 + 0xa4) = 0;
  *(undefined4 *)(param_1 + 0x98) = 0;
  *(undefined4 *)(param_1 + 0x78) = 0;
  *(undefined4 *)(param_1 + 0x84) = 0;
  *(undefined4 *)(param_1 + 0xf4) = 0;
  *(undefined4 *)(param_1 + 0xfc) = 0;
  *(undefined4 *)(param_1 + 0xf8) = 0;
  *(undefined4 *)(param_1 + 0x100) = 0;
  *(undefined4 *)(param_1 + 0x104) = 0;
  *(undefined4 *)(param_1 + 0x108) = 0;
  return;
}

// ---------------------------------------------------------------------------
// 0x0056efc0  Scatter one param_2 value into the [+0xe8]/[+0xec] indexed slot and zero the three
//             mirror slots ([+0xc4]/[+0xc8], [+0xdc]/[+0xe0], [+0xd0]/[+0xe0]).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056efc0(int param_1,undefined4 param_2)
{
  *(undefined4 *)(*(int *)(param_1 + 0xe8) + *(int *)(param_1 + 0xec) * 4) = param_2;
  *(undefined4 *)(*(int *)(param_1 + 0xc4) + *(int *)(param_1 + 0xc8) * 4) = 0;
  *(undefined4 *)(*(int *)(param_1 + 0xdc) + *(int *)(param_1 + 0xe0) * 4) = 0;
  *(undefined4 *)(*(int *)(param_1 + 0xd0) + *(int *)(param_1 + 0xe0) * 4) = 0;
  return;
}

// ---------------------------------------------------------------------------
// 0x0056f020  Advance the batch: 4-align [+0x100], accumulate the [+0xfc]-indexed count into
//             [+0xf4], then +1 the six per-batch cursors ([+0xec]/[+0xc8]/[+0xe0]/[+0xd4]/[+0xfc]).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056f020(int param_1)
{
  *(uint *)(param_1 + 0x100) = *(int *)(param_1 + 0x100) + 3U & 0xfffffffc;
  *(int *)(param_1 + 0xf4) =
       *(int *)(param_1 + 0xf4) + *(int *)(*(int *)(param_1 + 0xe8) + *(int *)(param_1 + 0xfc) * 4);
  *(int *)(param_1 + 0xec) = *(int *)(param_1 + 0xec) + 1;
  *(int *)(param_1 + 0xc8) = *(int *)(param_1 + 0xc8) + 1;
  *(int *)(param_1 + 0xe0) = *(int *)(param_1 + 0xe0) + 1;
  *(int *)(param_1 + 0xd4) = *(int *)(param_1 + 0xd4) + 1;
  *(int *)(param_1 + 0xfc) = *(int *)(param_1 + 0xfc) + 1;
  return;
}

// ---------------------------------------------------------------------------
// 0x0056f1f0  Emit one Jacobian/constraint row: scatter the param_2[0x10..0x1a] block into the 9
//             per-body SoA arrays at the packed index iVar1, copy the two 0x40-byte body sub-blocks
//             (param_2[0..2]/[4..6] and [8..10]/[0xc..0xe]) into the [*param_1] pool with the
//             batch tags, then +1 the [+0x2e]-indexed cursor.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056f1f0(int *param_1,undefined4 *param_2)
{
  int iVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  uint uVar4;
  int iVar5;
  undefined4 *puVar6;

  puVar6 = (undefined4 *)(param_1[0x2b] + param_1[0x3e] * 8);
  uVar2 = *puVar6;
  uVar3 = puVar6[1];
  uVar4 = param_1[0x42] + *(int *)(param_1[0x2e] + param_1[0x3e] * 4);
  iVar1 = (uVar4 & 3) + ((int)uVar4 >> 2) * 4;
  *(undefined4 *)(param_1[10] + iVar1 * 4) = param_2[0x10];
  *(undefined4 *)(param_1[7] + iVar1 * 4) = param_2[0x11];
  *(undefined4 *)(param_1[0x19] + iVar1 * 4) = param_2[0x12];
  *(undefined4 *)(param_1[4] + iVar1 * 4) = param_2[0x13];
  *(undefined4 *)(param_1[0xd] + iVar1 * 4) = param_2[0x14];
  *(undefined4 *)(param_1[0x10] + iVar1 * 4) = param_2[0x15];
  *(undefined4 *)(param_1[0x13] + iVar1 * 4) = param_2[0x16];
  *(undefined4 *)(param_1[0x16] + iVar1 * 4) = param_2[0x17];
  *(undefined4 *)(param_1[0x22] + iVar1 * 4) = param_2[0x18];
  iVar5 = iVar1 * 0x40;
  puVar6 = (undefined4 *)(*param_1 + iVar5);
  *puVar6 = *param_2;
  puVar6[1] = param_2[1];
  puVar6[2] = param_2[2];
  puVar6 = (undefined4 *)(iVar5 + 0x10 + *param_1);
  *puVar6 = param_2[4];
  puVar6[1] = param_2[5];
  puVar6[2] = param_2[6];
  *(undefined4 *)(iVar5 + 0xc + *param_1) = uVar2;
  *(int *)(iVar5 + 0x1c + *param_1) = param_1[0x3e];
  puVar6 = (undefined4 *)(iVar5 + 0x20 + *param_1);
  *puVar6 = param_2[8];
  puVar6[1] = param_2[9];
  puVar6[2] = param_2[10];
  puVar6 = (undefined4 *)(iVar5 + 0x30 + *param_1);
  *puVar6 = param_2[0xc];
  puVar6[1] = param_2[0xd];
  puVar6[2] = param_2[0xe];
  *(undefined4 *)(iVar5 + 0x2c + *param_1) = uVar3;
  *(int *)(iVar5 + 0x3c + *param_1) = param_1[0x3e];
  *(undefined4 *)(param_1[0x25] + iVar1 * 4) = param_2[0x1a];
  *(int *)(param_1[0x2e] + param_1[0x3e] * 4) = *(int *)(param_1[0x2e] + param_1[0x3e] * 4) + 1;
  return;
}

// ---------------------------------------------------------------------------
// 0x0056f0a0  Pad a contact batch up to a 4-multiple: emit (align - count) filler rows via
//             FUN_0056f1f0(&DAT_005e5738), then advance all the per-batch running offsets/counters
//             by the padded count / row count. Header note 3.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056f0a0(int param_1)
{
  int *piVar1;
  uint uVar2;
  int iVar3;
  uint uVar4;

  uVar2 = *(uint *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4);
  uVar4 = uVar2 + 3 & 0xfffffffc;
  if (uVar2 != uVar4) {
    iVar3 = uVar4 - uVar2;
    do {
      FUN_0056f1f0((int *)param_1,DAT_005e5738);
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  *(uint *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) =
       *(int *)(*(int *)(param_1 + 0xb8) + *(int *)(param_1 + 0xf8) * 4) + (uVar2 - uVar4);
  *(uint *)(param_1 + 4) = *(int *)(param_1 + 4) + uVar4;
  iVar3 = (int)(uVar4 + ((int)(uVar2 + 3) >> 0x1f & 3U)) >> 2;
  *(int *)(param_1 + 0x14) = *(int *)(param_1 + 0x14) + iVar3;
  *(int *)(param_1 + 0x20) = *(int *)(param_1 + 0x20) + iVar3;
  *(int *)(param_1 + 0x2c) = *(int *)(param_1 + 0x2c) + iVar3;
  *(int *)(param_1 + 0x38) = *(int *)(param_1 + 0x38) + iVar3;
  *(int *)(param_1 + 0x44) = *(int *)(param_1 + 0x44) + iVar3;
  *(int *)(param_1 + 0x50) = *(int *)(param_1 + 0x50) + iVar3;
  *(int *)(param_1 + 0x5c) = *(int *)(param_1 + 0x5c) + iVar3;
  *(int *)(param_1 + 0x8c) = *(int *)(param_1 + 0x8c) + iVar3;
  *(int *)(param_1 + 0xb0) = *(int *)(param_1 + 0xb0) + 1;
  *(int *)(param_1 + 0x68) = *(int *)(param_1 + 0x68) + iVar3;
  *(uint *)(param_1 + 0x98) = *(int *)(param_1 + 0x98) + uVar4;
  *(uint *)(*(int *)(param_1 + 0xdc) + *(int *)(param_1 + 0xe0) * 4) =
       *(int *)(*(int *)(param_1 + 0xdc) + *(int *)(param_1 + 0xe0) * 4) + uVar4;
  *(uint *)(*(int *)(param_1 + 0xd0) + *(int *)(param_1 + 0xd4) * 4) =
       *(int *)(*(int *)(param_1 + 0xd0) + *(int *)(param_1 + 0xd4) * 4) + uVar2;
  piVar1 = (int *)(*(int *)(param_1 + 0xc4) + *(int *)(param_1 + 0xd4) * 4);
  *piVar1 = *piVar1 + 1;
  *(uint *)(param_1 + 0x100) = *(int *)(param_1 + 0x100) + uVar2;
  *(uint *)(param_1 + 0x104) = *(int *)(param_1 + 0x104) + uVar2;
  *(uint *)(param_1 + 0x108) = *(int *)(param_1 + 0x108) + uVar4;
  *(int *)(param_1 + 0xf8) = *(int *)(param_1 + 0xf8) + 1;
  return;
}

// ---------------------------------------------------------------------------
// 0x0056fad0  Cross-product basis builder: block 1 (param_5!=0) copies param_2 as the row axis and
//             builds axis x param_3; block 2 (param_6!=0) writes -param_2 (*_DAT_005cc33c) and
//             param_2 x param_4. Every basis term is a single a*b - c*d (note 4).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0056fad0(float *param_1,float *param_2,float *param_3,float *param_4,
                                     int param_5,int param_6)
{
  if (param_5 != 0) {
    *param_1 = *param_2;
    param_1[1] = param_2[1];
    param_1[2] = param_2[2];
    param_1[4] = param_2[2] * param_3[1] - param_2[1] * param_3[2];
    param_1[5] = param_3[2] * *param_2 - param_2[2] * *param_3;
    param_1[6] = param_2[1] * *param_3 - param_3[1] * *param_2;
  }
  if (param_6 != 0) {
    param_1[8] = *param_2 * _DAT_005cc33c;
    param_1[9] = param_2[1] * _DAT_005cc33c;
    param_1[10] = param_2[2] * _DAT_005cc33c;
    param_1[0xc] = param_2[1] * param_4[2] - param_4[1] * param_2[2];
    param_1[0xd] = param_2[2] * *param_4 - param_4[2] * *param_2;
    param_1[0xe] = param_4[1] * *param_2 - param_2[1] * *param_4;
  }
  return;
}

// ---------------------------------------------------------------------------
// 0x00567c00  Reset a contact-batch header pair: zero the two [+0x11c]/[+0x110] tallies, run
//             FUN_0056ef30 on param_1, then zero five param_2 fields.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_00567c00(int param_1,int param_2)
{
  *(undefined4 *)(param_1 + 0x11c) = 0;
  *(undefined4 *)(param_1 + 0x110) = 0;
  FUN_0056ef30(param_1);
  *(undefined4 *)(param_2 + 0x40) = 0;
  *(undefined4 *)(param_2 + 0x4c) = 0;
  *(undefined4 *)(param_2 + 0x34) = 0;
  *(undefined4 *)(param_2 + 4) = 0;
  *(undefined4 *)(param_2 + 0x10) = 0;
  return;
}

// --- gta-reversed-style hook registration — CLUSTER 9. ---
RH_ScopedInstall(FUN_0056ef30, 0x0056ef30);
RH_ScopedInstall(FUN_0056efc0, 0x0056efc0);
RH_ScopedInstall(FUN_0056f020, 0x0056f020);
RH_ScopedInstall(FUN_0056f0a0, 0x0056f0a0);
RH_ScopedInstall(FUN_0056f1f0, 0x0056f1f0);
RH_ScopedInstall(FUN_0056fad0, 0x0056fad0);
RH_ScopedInstall(FUN_00567c00, 0x00567c00);

}  // namespace Collision
}  // namespace mashed_re
