// Mashed RE — B5e: RwpSolver-island CLUSTER 5 clean-room port (shape re-index / bounds
// refresh glue + descriptor-dispatch wrappers).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-16). VERBATIM transcription of the 5 K5 functions from
// re/analysis/b5e/decomp/FUN_00xxxxxx.c, every body re-verified against live disasm before
// porting. Style/idiom follows RwpSolverLeaves1.cpp (K1) / RwpSolverBroadphase3.cpp (K3) /
// RwpSolverCore4.cpp (K4): extern "C" per-function, // 0x00xxxxxx RVA comments,
// RH_ScopedInstall registration block.
//
// NO-GUESSING verifications against live pool0 disasm (2026-07-16):
// 1. All 5 bodies are __cdecl (caller cleanup): RETs at 0x0055aba4, 0x0055e07b,
//    0x0055bbb4/0x0055bbca/0x0055bbe2, 0x0055bdd4, 0x0055c2c9. Returns: FUN_0055ab30
//    returns param_1 (MOV EAX,ESI @0x0055ab9f); FUN_0055e050 returns param_1 (MOV EAX,ESI
//    @0x0055e075); FUN_0055c230 returns the dispatch result (MOV EDI,EAX @0x0055c2ab,
//    MOV EAX,EDI @0x0055c2c2); FUN_0055bb70 / FUN_0055bd80 are void.
// 2. FUN_0055ab30 stale-high-half pushes, both PROVABLY masked in the callees:
//    (a) @0x0055ab5d PUSH EDX where only DX was loaded (@0x0055ab40) — high 16 bits are
//        caller garbage; FUN_00565260 masks every param_2 use (`& 0xffff` first use,
//        `(ushort)` casts after — see RwpSolverCore4.cpp body).
//    (b) @0x0055ab74..78 MOV AX,word[EDI+0x20] over live EAX (= param_3 from @0x0055ab69)
//        then PUSH EAX — high 16 bits are param_3's high half; FUN_00564c80 masks
//        (`param_2 & 0xffff` @first use, `(ushort)param_2` for the octree call).
//    Passing the zero-extended ushort is therefore bit-identical; transcribed as the
//    decomp prints it (undefined2 arg).
// 3. FUN_0055e050's 32-byte buffer: LEA ECX,[ESP] @0x0055e053 runs BEFORE PUSH ESI, so
//    ECX = buffer base; LEA EDX,[ESP+0xc] @0x0055e066 (after 3 more pushes) is the SAME
//    address — decomp's single local_20 is correct. The obj[0]+0x18 dispatch
//    (CALL [EAX+0x18] @0x0055e060) is cdecl 2-arg (param_1, buffer); the original
//    batches both calls' cleanup into one ADD ESP,0x14 @0x0055e072 (2+3 dwords) —
//    per-call compiler cleanup in the port is semantically identical for cdecl.
//    The buffer is consumed by FUN_0055ab30's param_3 → FUN_00564c80's float[8] box.
// 4. FUN_0055bb70 / FUN_0055bd80 share the gate ((desc+0x40 byte & 2)==0 &&
//    (param_1+0xc & 0x20000)==0), verified 0x0055bb7b..0x0055bb88 / 0x0055bd8b..0x0055bd98.
//    FUN_004c4600 arg order (local_40, param_1, param_2) verified @0x0055bb92..99 /
//    0x0055bda2..a9 (arg1 = LEA of the 0x40-byte frame slot = E-0x40). FUN_0055bd80's
//    desc+0x10 dispatch is cdecl 4-arg (param_1, iVar1, param_3, param_4), ADD ESP,0x10
//    @0x0055bdcd — the concrete-arity counterpart of K3's FUN_0055bd70 naked forwarder.
// 5. FUN_0055c230 x87 summation order differs from the decomp's printed text (same class
//    as the K2 finding): every row is (m[r+1]*v1 + m[r]*v0) + m[r+2]*v2 —
//    row x FADDP order @0x0055c24f/0x0055c257, row y @0x0055c268/0x0055c270,
//    row z @0x0055c281/0x0055c289 — identical shape to K3's FUN_0055c2d0 rows. Products
//    stay 80-bit into the FADDPs; single rounding per FSTP ([ESP+0x8/0xc/0x10]
//    @0x0055c259/72/8b) → float10 casts + one float store, K3 idiom.
// 6. FUN_0055c230's rotated point local_c/local_8/local_4 is one CONTIGUOUS float[3]
//    block (E-0xc/E-8/E-4, FSTPs above; passed as &local_c and indexed [0..2] by the
//    desc+0x18 callee) — ported as float loc[3] (same class as K3 note 6 / K4 note 2).
//    The desc+0x18 dispatch is cdecl 4-arg returning int (ADD ESP,0x10 @0x0055c2ad,
//    TEST EDI @0x0055c2b0); FUN_0055c0f0 arg order (iVar1, param_2) verified
//    @0x0055c2b8..ba (PUSH ESI, PUSH EDI).
//
// x87 note: 80-bit ST0 chains carry the accepted <=1-ULP floor under MSVC's 64-bit long
// double (project_phys_chain_float10_methodology).
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

// --- Ghidra scalar types kept verbatim (as RwpSolverLeaves1.cpp). ---
typedef unsigned char  byte;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned short undefined2;
typedef unsigned int   undefined4;
typedef unsigned char  undefined1;
typedef long double    float10;     // x87 80-bit extended — MSVC = 64-bit double [X87]

// --- K2 (RwpSolverMath2.cpp) / K3 (RwpSolverBroadphase3.cpp) / K4 (RwpSolverCore4.cpp)
//     callees — declarations match the definitions exactly. ---
extern "C" undefined4 * __cdecl FUN_004c4600(undefined4 *param_1,undefined4 *param_2,undefined4 *param_3);
extern "C" void __cdecl FUN_0055bae0(int param_1,float *param_2,undefined4 param_3);
extern "C" void __cdecl FUN_0055c0f0(float *param_1,float *param_2);
extern "C" int  __cdecl FUN_00564c80(int param_1, uint param_2, float *param_3);
extern "C" void __cdecl FUN_00565260(int param_1, uint param_2);

// --- Indirect-dispatch binding types (targets stay ORIGINAL code until KV lands:
//     body-object table 0x0062403c / volume-descriptor tables 0x5e4f50 band,
//     island_vtable_reach §1.2/§1.1). ---
typedef void (__cdecl *RwpBodyFnBounds)(int*, undefined1*);                   // body obj +0x18 (note 3)
typedef void (__cdecl *RwpVolFnApply)(int, int, undefined4, undefined4);      // desc +0x10 (note 4)
typedef int  (__cdecl *RwpVolFnQuery)(int, undefined4, float*, undefined4);   // desc +0x18 (note 6)

// ---------------------------------------------------------------------------
// 0x0055ab30  Octree re-index: if the shape's bit (index word at param_2+0x20) is set in
//             the bitmap at param_1[0x1a], remove it (FUN_00565260); if param_3 (a new
//             float[8] box) is non-null, re-insert (FUN_00564c80) and set the bit.
//             Returns param_1. Verification note 2 (stale-high-half pushes, masked).
// ---------------------------------------------------------------------------
extern "C" undefined4 * __cdecl FUN_0055ab30(undefined4 *param_1,int param_2,int param_3)
{
  uint *puVar1;
  ushort uVar2;

  uVar2 = *(ushort *)(param_2 + 0x20);
  if ((*(uint *)(param_1[0x1a] + (uint)(uVar2 >> 5) * 4) & 1 << ((byte)uVar2 & 0x1f)) != 0) {
    FUN_00565260((int)*param_1,uVar2);                                       // 0x0055ab61
  }
  if (param_3 != 0) {
    FUN_00564c80((int)*param_1,*(undefined2 *)(param_2 + 0x20),(float *)param_3);  // 0x0055ab7a
    puVar1 = (uint *)(param_1[0x1a] + (uint)(*(ushort *)(param_2 + 0x20) >> 5) * 4);
    *puVar1 = *puVar1 | 1 << ((byte)*(ushort *)(param_2 + 0x20) & 0x1f);
  }
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x0055e050  Shape bounds refresh: asks the body object for its bounds box (obj[0]+0x18
//             into a 32-byte stack buffer, note 3), then re-indexes the shape in the
//             octree owner at param_1[9] via FUN_0055ab30. Returns param_1.
// ---------------------------------------------------------------------------
extern "C" int * __cdecl FUN_0055e050(int *param_1)
{
  undefined1 local_20 [32];

  ((RwpBodyFnBounds)(*(void **)(*param_1 + 0x18)))(param_1,local_20);        // 0x0055e060
  FUN_0055ab30((undefined4 *)param_1[9],(int)param_1,(int)local_20);         // 0x0055e06d
  return param_1;
}

// ---------------------------------------------------------------------------
// 0x0055bb70  Gated matrix-combine + point-transform: when the shape is "live" (gate in
//             note 4) and param_2 is non-null, composes param_1's frame with param_2 into
//             a 0x40-byte local (FUN_004c4600) and transforms through FUN_0055bae0;
//             otherwise forwards param_2 (or param_1 when param_2==0) unmodified.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0055bb70(int param_1,int param_2,undefined4 param_3)
{
  undefined4 *uVar1;
  undefined1 local_40 [64];

  if (((*(byte *)(*(int *)(param_1 + 0x5c) + 0x40) & 2) == 0) &&
     ((*(uint *)(param_1 + 0xc) & 0x20000) == 0)) {
    if (param_2 != 0) {
      uVar1 = FUN_004c4600((undefined4 *)local_40,(undefined4 *)param_1,(undefined4 *)param_2);  // 0x0055bb99
      FUN_0055bae0(param_1,(float *)uVar1,param_3);                          // 0x0055bba8
      return;
    }
    FUN_0055bae0(param_1,(float *)param_1,param_3);                          // 0x0055bbbe
    return;
  }
  FUN_0055bae0(param_1,(float *)param_2,param_3);                            // 0x0055bbd6
  return;
}

// ---------------------------------------------------------------------------
// 0x0055bd80  Same gate/combine as FUN_0055bb70, but the transformed frame (or the
//             passthrough pointer) feeds the shape's volume-descriptor +0x10 slot
//             (cdecl 4-arg, note 4) instead of FUN_0055bae0.
// ---------------------------------------------------------------------------
extern "C" void __cdecl FUN_0055bd80(int param_1,int param_2,undefined4 param_3,undefined4 param_4)
{
  int iVar1;
  undefined1 local_40 [64];

  iVar1 = param_2;
  if ((((*(byte *)(*(int *)(param_1 + 0x5c) + 0x40) & 2) == 0) &&
      ((*(uint *)(param_1 + 0xc) & 0x20000) == 0)) && (iVar1 = param_1, param_2 != 0)) {
    iVar1 = (int)FUN_004c4600((undefined4 *)local_40,(undefined4 *)param_1,(undefined4 *)param_2);  // 0x0055bda9
  }
  ((RwpVolFnApply)(*(void **)(*(int *)(param_1 + 0x5c) + 0x10)))(param_1,iVar1,param_3,param_4);    // 0x0055bdca
  return;
}

// ---------------------------------------------------------------------------
// 0x0055c230  Rotated descriptor query: if param_2 (a 4x4 RW frame) is non-null, rotates
//             the direction param_4 by its 3x3 into a contiguous float[3] (notes 5, 6),
//             queries the descriptor +0x18 slot with it, and on a non-null result
//             transforms the returned point set back via FUN_0055c0f0. Returns the
//             dispatch result.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_0055c230(int param_1,float *param_2,undefined4 param_3,float *param_4,undefined4 param_5)
{
  int iVar1;
  float loc [3];                     // local_c/local_8/local_4 — contiguous (note 6)

  if (param_2 != (float *)0x0) {
    loc[0] = (float)(((float10)param_2[1] * param_4[1] + (float10)*param_2 * *param_4) +
             (float10)param_2[2] * param_4[2]);                              // FSTP 0x0055c259
    loc[1] = (float)(((float10)param_2[5] * param_4[1] + (float10)param_2[4] * *param_4) +
             (float10)param_2[6] * param_4[2]);                              // FSTP 0x0055c272
    loc[2] = (float)(((float10)param_2[9] * param_4[1] + (float10)param_2[8] * *param_4) +
             (float10)param_2[10] * param_4[2]);                             // FSTP 0x0055c28b
    param_4 = loc;
  }
  iVar1 = ((RwpVolFnQuery)(*(void **)(*(int *)(param_1 + 0x5c) + 0x18)))(param_1,param_3,param_4,param_5);  // 0x0055c2a8
  if ((iVar1 != 0) && (param_2 != (float *)0x0)) {
    FUN_0055c0f0((float *)iVar1,param_2);                                    // 0x0055c2ba
  }
  return iVar1;
}

// --- gta-reversed-style hook registration (inert on the exe via HookSystemNoOp; installs
//     the inline-JMP under the .asi for the diff-original A/B acceptance) — CLUSTER 5. ---
RH_ScopedInstall(FUN_0055ab30, 0x0055ab30);
RH_ScopedInstall(FUN_0055e050, 0x0055e050);
RH_ScopedInstall(FUN_0055bb70, 0x0055bb70);
RH_ScopedInstall(FUN_0055bd80, 0x0055bd80);
RH_ScopedInstall(FUN_0055c230, 0x0055c230);

}  // namespace Collision
}  // namespace mashed_re
