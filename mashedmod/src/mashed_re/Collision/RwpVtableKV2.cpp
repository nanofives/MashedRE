// Mashed RE — B5e lane-end KV, BATCH A: body-object table 0x0062403c slot functions,
// the two shared volume-descriptor leaves, and the Null-table constructor.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-25). Style/idiom follows RwpSolverGlue5.cpp (K5).
//
// TABLE LAYOUT — read live from the slot at 0x00624030..0x0062406f (memory_read, not decomp):
//   0x0062403c + 0x00 = 0x00000000
//   0x0062403c + 0x04 = 0x0062405c  -> "rwpOBJTYPEBODY"
//   0x0062403c + 0x08 = 0xffffffff
//   0x0062403c + 0x0c = 0x00000000
//   0x0062403c + 0x10 = 0x0057c3a0
//   0x0062403c + 0x14 = 0x0057c3f0
//   0x0062403c + 0x18 = 0x0057c590
//   0x0062403c + 0x1c = 0x0057c5a0   (runtime-overridden to 0x0057c2b0 by FUN_0057c270)
//
// PREP-PACK CORRECTIONS (re/analysis/plans/kv_port_prep_2026-07-24.md listed these as
// "Ghidra-undefined, discovery-first"; live disasm says otherwise):
//   1. 0x0057c590 is a 5-byte `JMP 0x0057c420` thunk (bytes e9 8b fe ff ff @0x0057c590).
//      0x0057c5a0 is a 5-byte `JMP 0x0057c440` thunk (bytes e9 9b fe ff ff @0x0057c5a0).
//      Both targets ARE Ghidra-defined functions with existing hooks.csv rows (C1) —
//      so 2 of the 7 "discovery-first" gaps were not gaps. We port and hook the BODIES
//      (0x0057c420 / 0x0057c440) and leave the 5-byte thunks unpatched: a 5-byte inline
//      JMP over a 5-byte thunk is exactly-fitting but pointless, and hooking the body
//      also covers non-table callers. Dispatch still lands in our code either way.
//   2. 0x0057b9a0 and 0x0057c1d0 both RETURN A FLOAT IN ST0: each does
//      `FLD dword ptr [0x005d757c]` and never pops it. [0x005d757c] reads 0x00000000 = 0.0f
//      (memory_read 2026-07-25). Declaring these `void` would leak the x87 stack in any
//      caller compiled SSE2 — the hazard recorded in memory feedback_x87_st0_float10_return_fnptr.
//      They are declared returning `float` here.
//
// NO-GUESSING verifications against live pool0 disasm (2026-07-25):
//   A. All bodies are __cdecl (caller cleanup): ADD ESP,0xc @0x0057c3cc; ADD ESP,4
//      @0x0057c404; ADD ESP,0x10 @0x0057c43a; ADD ESP,0x10 @0x0057c49a.
//   B. FUN_0057c3a0's gate is UNSIGNED: CMP EDI,EBX @0x0057c3b4 / JAE 0x0057c3e5 — body runs
//      only when (uint)ctx[3] < (uint)ctx[2] (ctx+0xc vs ctx+0x8).
//   C. FUN_0057c3a0's return is the NEG/SBB/AND idiom @0x0057c3e5..0x0057c3ea:
//      `NEG EAX; SBB EAX,EAX; AND EAX,ESI` => param_1 when the FUN_0055b030 result is
//      non-zero, else 0. On the JAE-taken path EAX was zeroed @0x0057c3a7, so it returns 0.
//   D. FUN_0057c3a0 call arg order (cdecl, left-to-right = last push first):
//      PUSH ECX @0x0057c3c1 / PUSH EAX @0x0057c3c2 / PUSH EDX @0x0057c3c3 =>
//      FUN_0055b030([p1+0x24], [p1+4], [[p1+0x10]+8]).
//   E. FUN_0057c440 negates BEFORE the matrix copy: FLD/FCHS/FSTP triples @0x0057c453..0x0057c46a
//      write [ESP+0x14]/[ESP+0x18]/[ESP+0x1c] — one CONTIGUOUS float[3], passed by LEA
//      @0x0057c489 to FUN_004c51a0 (K5/K3 idiom). The copy is REP MOVSD ECX=0x10 @0x0057c487
//      (16 dwords = one 64-byte RwMatrix) from [[*piVar1+0x10]] + piVar1[1]*0x40 (SHL EAX,6
//      @0x0057c480).
//   F. FUN_0057c1d0 writes the raw dword 0x3f800000 (MOV EDX,0x3f800000 @0x0057c1d4) to
//      p2[0],[1],[2],[6] and 0 to p2[3],[4],[5]; then zeroes p3[2],[1],[0]. param_1 is
//      never read. Transcribed as raw dword stores so the bit pattern is exact.
//   G. FUN_00562a10 is `MOV EAX,[ESP+0xc]; RET` — returns its THIRD cdecl argument. Body is
//      exactly 5 bytes, so a 5-byte inline JMP fits with no overrun into the next function
//      (contrast 0x005c9d00's 2-byte body — see its hooks.csv caveat; that one is NOT
//      touched by this TU).
//   H. FUN_0057b9a0's body is 7 bytes (FLD 6 + RET 1) — 5-byte JMP fits.
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

typedef unsigned char  byte;
typedef unsigned int   uint;
typedef unsigned int   undefined4;

// --- Absolute-address bindings (same idiom as RwpIntegrator.cpp's DAT_007dc8d8). ---
#define MASHED_DAT_007dc8dc (*reinterpret_cast<int*>(0x007dc8dcu))     // cleared @0x0057c3db
#define MASHED_DAT_007dc8d8 (*reinterpret_cast<int*>(0x007dc8d8u))     // plugin offset
#define MASHED_PTR_00624058 (*reinterpret_cast<void**>(0x00624058u))   // table 0x0062403c + 0x1c

// --- Ported elsewhere in the island: declaration matches the definition exactly. ---
extern "C" void __cdecl FUN_0055bd80(int param_1,int param_2,undefined4 param_3,undefined4 param_4); // K5
extern "C" void __cdecl FUN_004c51a0(float *param_1,float *param_2,int param_3);                     // K2 RwMatrixTranslate

// --- Un-ported callees -> RVA-forwarding thunks (RwpSolverCore23.cpp idiom). ---
// FUN_0055b030 / FUN_0055b060 are C1 RenderWare-Physics-3.7 primitives (hooks.csv).
static inline int  Fwd_0055b030(undefined4 a, undefined4 b, undefined4 c)
{ return reinterpret_cast<int(__cdecl *)(undefined4,undefined4,undefined4)>(0x0055b030u)(a,b,c); }
static inline void Fwd_0055b060(undefined4 a)
{ reinterpret_cast<void(__cdecl *)(undefined4)>(0x0055b060u)(a); }
static inline void Fwd_004c45f0(void *m)
{ reinterpret_cast<void(__cdecl *)(void*)>(0x004c45f0u)(m); }          // C3 orthonormal-flags clearer
static inline void Fwd_004c0e50(undefined4 a)
{ reinterpret_cast<void(__cdecl *)(undefined4)>(0x004c0e50u)(a); }
static inline int  Fwd_RpAtomicRegisterPlugin(int size,int id,void *ctor,void *dtor,void *copy)
{ return reinterpret_cast<int(__cdecl *)(int,int,void*,void*,void*)>(0x004e7d40u)(size,id,ctor,dtor,copy); }

// ---------------------------------------------------------------------------
// 0x0057c3a0  table 0x0062403c slot +0x10 — "add": when the owner context still has room
//             ((uint)ctx[3] < (uint)ctx[2], note B), builds the entry via FUN_0055b030,
//             stores the result at [p1+0x10]+4, bumps ctx[3], clears DAT_007dc8dc, and
//             returns p1 on success / 0 on failure (note C).
// ---------------------------------------------------------------------------
extern "C" int * __cdecl FUN_0057c3a0(int *param_1)
{
  int *ctx;
  int  iVar1;

  iVar1 = 0;                                                            // 0x0057c3a7 XOR EAX,EAX
  ctx = *(int **)param_1;                                               // 0x0057c3a9
  if ((uint)ctx[3] < (uint)ctx[2]) {                                    // 0x0057c3b4 / JAE 0x0057c3e5
    iVar1 = Fwd_0055b030(*(undefined4 *)((int)param_1 + 0x24),
                         *(undefined4 *)((int)param_1 + 4),
                         *(undefined4 *)(*(int *)((int)param_1 + 0x10) + 8));   // 0x0057c3c4 (note D)
    *(int *)(*(int *)((int)param_1 + 0x10) + 4) = iVar1;                // 0x0057c3cf
    ctx = *(int **)param_1;                                             // 0x0057c3d2
    ctx[3] = ctx[3] + 1;                                                // 0x0057c3d7/0x0057c3d8
    MASHED_DAT_007dc8dc = 0;                                            // 0x0057c3db
  }
  return (iVar1 != 0) ? param_1 : (int *)0;                             // 0x0057c3e5..0x0057c3ea
}

// ---------------------------------------------------------------------------
// 0x0057c3f0  slot +0x14 — "remove": releases the entry at [p1+0x10]+4 via FUN_0055b060,
//             nulls the slot, decrements ctx[3]. Returns p1 (MOV EAX,ESI @0x0057c417).
// ---------------------------------------------------------------------------
extern "C" int * __cdecl FUN_0057c3f0(int *param_1)
{
  int *ctx;

  Fwd_0055b060(*(undefined4 *)(*(int *)((int)param_1 + 0x10) + 4));     // 0x0057c3fc
  *(int *)(*(int *)((int)param_1 + 0x10) + 4) = 0;                      // 0x0057c407
  ctx = *(int **)param_1;                                               // 0x0057c40e
  ctx[3] = ctx[3] + -1;                                                 // 0x0057c413/0x0057c414
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x0057c420  body behind the slot-+0x18 thunk at 0x0057c590 (correction 1). Forwards to
//             the K5 gated matrix-combine FUN_0055bd80 with the shape's owner and its
//             descriptor, combineOp literal 1 (PUSH 1 @0x0057c429).
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0057c420(int param_1,undefined4 param_2)
{
  FUN_0055bd80(**(int **)(param_1 + 0x10),
               (*(int **)(param_1 + 0x10))[2], 1, param_2);             // 0x0057c435
  return;
}

// ---------------------------------------------------------------------------
// 0x0057c440  body behind the slot-+0x1c thunk at 0x0057c5a0 (correction 1). Copies the
//             indexed 64-byte RwMatrix into the shape's matrix slot, then translates it by
//             the NEGATED offset triple and clears the orthonormal flags (note E).
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0057c440(int param_1)
{
  int        *piVar1;
  undefined4 *puVar2;
  undefined4 *puVar4;
  undefined4 *puVar5;
  int         iVar3;
  float       loc [3];                    // [ESP+0x14/0x18/0x1c] — contiguous (note E)

  piVar1 = *(int **)(*(int *)(param_1 + 0x10) + 4);                     // 0x0057c44d/0x0057c450
  loc[0] = -*(float *)(piVar1 + 2);                                     // FLD/FCHS/FSTP 0x0057c453..58
  loc[1] = -*(float *)(piVar1 + 3);                                     // 0x0057c45c..61
  loc[2] = -*(float *)(piVar1 + 4);                                     // 0x0057c465..6a
  puVar2 = *(undefined4 **)(*(int *)(param_1 + 0x10) + 8);              // 0x0057c46e
  puVar4 = (undefined4 *)(**(int **)(*piVar1 + 0x10) + piVar1[1] * 0x40);  // 0x0057c478..85
  puVar5 = puVar2;
  for (iVar3 = 0x10; iVar3 != 0; iVar3 = iVar3 + -1) {                  // REP MOVSD 0x0057c487
    *puVar5 = *puVar4;
    puVar4 = puVar4 + 1;
    puVar5 = puVar5 + 1;
  }
  FUN_004c51a0((float *)puVar2,loc,1);                                  // 0x0057c48f
  Fwd_004c45f0(puVar2);                                                 // 0x0057c495
  return param_1;                                                       // MOV EAX,EBP 0x0057c49d
}

// ---------------------------------------------------------------------------
// 0x0057c2b0  runtime override installed into slot +0x1c by FUN_0057c270. Runs the
//             0x0057c440 body, then — when [p1+0x30] is non-null — flags the atomic at
//             [[p1+0x30]+4] via FUN_004c0e50. Returns p1.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0057c2b0(int param_1)
{
  FUN_0057c440(param_1);
  if (*(int *)(param_1 + 0x30) != 0) {
    Fwd_004c0e50(*(undefined4 *)(*(int *)(param_1 + 0x30) + 4));
  }
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x0057c270  plugin registrar (short init ID 0x901): registers the RpAtomic plugin and
//             overrides table slot +0x1c (PTR_LAB_00624058) with 0x0057c2b0.
//
// [UNCERTAIN] the stored pointer is written as the ORIGINAL RVA literal 0x0057c2b0, not
// &FUN_0057c2b0, so the table's observable dword stays bit-identical to the original. Under
// the .asi that address is JMP-patched to our body, so behavior is ours either way. For the
// standalone target this literal is meaningless; the standalone path must bind the table to
// the local FUN_0057c2b0 instead. Not resolved here — flagged for the KV lane-end review.
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl FUN_0057c270(void)
{
  MASHED_DAT_007dc8d8 = Fwd_RpAtomicRegisterPlugin(4,0x901,
                            reinterpret_cast<void *>(0x0057c2e0u),      // LAB_0057c2e0
                            reinterpret_cast<void *>(0x004d7ff0u),
                            reinterpret_cast<void *>(0x004d7ff0u));
  MASHED_PTR_00624058 = reinterpret_cast<void *>(0x0057c2b0u);
  return 1;
}

// ---------------------------------------------------------------------------
// 0x0057c1d0  Null volume-descriptor table (0x5e5e50) slot +0x0c — writes the identity
//             triple/flags block into param_2 and zeroes param_3's float[3]. param_1 is
//             never read. RETURNS 0.0f IN ST0 (correction 2, note F).
// ---------------------------------------------------------------------------
extern "C" float __cdecl FUN_0057c1d0(undefined4 param_1,undefined4 *param_2,undefined4 *param_3)
{
  (void)param_1;                                                        // never read
  param_2[0] = 0x3f800000;                                              // 0x0057c1e1  (1.0f raw)
  param_2[1] = 0x3f800000;                                              // 0x0057c1e3
  param_2[2] = 0x3f800000;                                              // 0x0057c1e6
  param_2[3] = 0;                                                       // 0x0057c1e9
  param_2[4] = 0;                                                       // 0x0057c1ec
  param_2[5] = 0;                                                       // 0x0057c1ef
  param_2[6] = 0x3f800000;                                              // 0x0057c1f2
  param_3[2] = 0;                                                       // 0x0057c1f9
  param_3[1] = 0;                                                       // 0x0057c1fc
  param_3[0] = 0;                                                       // 0x0057c1ff
  return 0.0f;                                                          // FLD [0x005d757c] 0x0057c1d9
}

// ---------------------------------------------------------------------------
// 0x0057b9a0  shared volume-descriptor slot +0x24 (Sphere/Capsule/Triangle/Null) —
//             `FLD dword ptr [0x005d757c]; RET`. [0x005d757c] = 0x00000000 = 0.0f.
//             Takes no stack arguments; cdecl, so a 0-arg definition is ABI-safe for any
//             caller arity (correction 2, note H).
// ---------------------------------------------------------------------------
extern "C" float __cdecl FUN_0057b9a0(void)
{
  return 0.0f;                                                          // 0x0057b9a0
}

// ---------------------------------------------------------------------------
// 0x00562a10  shared volume-descriptor slot +0x14 (Trilist + the dead Grid table) —
//             `MOV EAX,[ESP+0xc]; RET` = returns the third cdecl argument (note G).
// ---------------------------------------------------------------------------
extern "C" undefined4 __cdecl FUN_00562a10(undefined4 param_1,undefined4 param_2,undefined4 param_3)
{
  (void)param_1; (void)param_2;
  return param_3;                                                       // 0x00562a10
}

// --- gta-reversed-style hook registration — KV BATCH A. The two 5-byte thunks
//     (0x0057c590 / 0x0057c5a0) are deliberately NOT installed; their bodies are. ---
RH_ScopedInstall(FUN_0057c3a0, 0x0057c3a0);
RH_ScopedInstall(FUN_0057c3f0, 0x0057c3f0);
RH_ScopedInstall(FUN_0057c420, 0x0057c420);
RH_ScopedInstall(FUN_0057c440, 0x0057c440);
RH_ScopedInstall(FUN_0057c2b0, 0x0057c2b0);
RH_ScopedInstall(FUN_0057c270, 0x0057c270);
RH_ScopedInstall(FUN_0057c1d0, 0x0057c1d0);
RH_ScopedInstall(FUN_0057b9a0, 0x0057b9a0);
RH_ScopedInstall(FUN_00562a10, 0x00562a10);

}  // namespace Collision
}  // namespace mashed_re
