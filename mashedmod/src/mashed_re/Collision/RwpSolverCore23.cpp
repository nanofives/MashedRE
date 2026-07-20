// ============================================================================
//  RwpSolverCore23.cpp  —  B5e solver-island cluster K23  (9 of 9 fns — COMPLETE)
//  Verbatim RWP-3.7 per-island solver-step DRIVERS/WRAPPERS. Each is
//  `int __cdecl F(int solverCtx)` returning the context (EAX), mirroring the
//  K21/K22 driver idiom.
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
//    FUN_00561390 (0x00561390, 2239 B) — the CCD (continuous collision detection)
//                                        + impact-resolution monster. Ported with
//                                        full-listing discipline (2026-07-20):
//    * param_1 typed `int` (Ghidra rendered the solver ctx as float* with word
//      indices); every field accessed by explicit byte-offset cast.
//    * DECISIVE disasm fact: the whole 0x00561390..0x00561c4e body has NO
//      FILD/FIST/CVT — so every Ghidra `(float)((int)x+1)` / `== 0.0` counter
//      pattern is a float-TYPED stack slot over pure-integer ops; ALL such
//      counters (local_a4, the three reused `fVar19` slots, count/flag/index
//      fields) are ported as INT. FUN_0055a9a0's int result likewise stores to
//      ctx+0x84 and forwards to FUN_00573670 as an integer (no int->float).
//    * local_40 == local_4c[3] (4th float of FUN_0055bb70's out buffer);
//      uStack_24 == auStack_30[3]; buffers arrayed for adjacency (K18 lesson):
//      local_4c[4], auStack_30[4], auStack_20[8].
//    * KV +0x24 = RwpVolFn24 (float10 ST0, site 0x005614e8); body +0x1c =
//      RwpBodyFn1c (returns EAX, ignored — FUN_0057c2b0, no x87 leak).
//    * the one cross-type raw copy (contact point -> body record +0x30) is
//      bit-copied via undefined4 (a float=uint assign would truncate).
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
// K13 island solve-step orchestrator — 14 dword params (14th vestigial, see note).
extern "C" int __cdecl FUN_00560260(undefined4,undefined4,undefined4,undefined4,undefined4,
                                    undefined4,undefined4,undefined4,undefined4,undefined4,
                                    undefined4,undefined4,undefined4,undefined4);                // 0x00560260 (K13)
extern "C" uint __cdecl FUN_00568990(int,int,uint,int,uint);                                    // 0x00568990 (K21)
extern "C" int  __cdecl FUN_0056bb30(int);                                                      // 0x0056bb30 (K21)

// --- FUN_00561390 (the monster) extra callees — all ported hooks in sibling K-cluster
//     TUs of the same .asi; extern "C" + __cdecl means the linker matches by name, so the
//     call-site-shaped signatures below push the identical dword/by-value-float sequence
//     (same pattern already used for FUN_00560260 above). ---
typedef long double float10;                                                                     // x87 80-bit ST0 (MSVC 64-bit)
extern "C" float *    __cdecl FUN_0055bb70(int,int,float *);                                     // 0x0055bb70 (K5)  out-buf param_3
extern "C" int   *    __cdecl FUN_0055e050(int *);                                               // 0x0055e050 (K5)
extern "C" undefined4* __cdecl FUN_0055ab30(undefined4,int,undefined4 *);                        // 0x0055ab30 (K5)  owner,body,buf
extern "C" void       __cdecl FUN_0056c0a0(undefined4,undefined4,undefined4,undefined4,undefined4,
                                           undefined4,undefined4,undefined4,undefined4,undefined4,
                                           float);                                               // 0x0056c0a0 (K6) 11-arg, last float
extern "C" void       __cdecl FUN_0056be80(undefined4,undefined4,float);                         // 0x0056be80 (K6)  ptr,val,float
extern "C" float *    __cdecl FUN_00561280(int,uint,float *);                                    // 0x00561280 (K21) out-buf param_3
extern "C" void       __cdecl FUN_00565fa0(float *,float *,float *,float);                       // 0x00565fa0 (K1)
extern "C" void       __cdecl FUN_00565ef0(float *,float *,float *);                             // 0x00565ef0 (K1)
extern "C" int        __cdecl FUN_0055a9a0(undefined4,int *,int,undefined4,undefined4,int);      // 0x0055a9a0 (K3) 6-arg
extern "C" int        __cdecl FUN_00573670(int,undefined4,int,undefined4,int,int,int);           // 0x00573670 (K22)
extern "C" undefined4 __cdecl FUN_0056b9d0(int,int *,int,undefined4,int,int,int);                // 0x0056b9d0 (K22)

// FUN_004c3ac0 (0x004c3ac0, Vec3 magnitude) is NOT ported — original stays; call it via a raw
// RVA fn-ptr typed to return float10 so the x87 ST0 result is POPPED (a void/int fn-ptr would
// leak the FPU stack → NaN corruption; see memory x87_st0_float10_return_fnptr).
static inline float10 FUN_004c3ac0(int p)                                                        // 0x004c3ac0
{
  return reinterpret_cast<float10(__cdecl *)(int)>(0x004c3ac0u)(p);
}

// KV frame types in FUN_00561390 (disasm-verified):
//   shape +0x24 volume-descriptor: __cdecl(shapePtr) -> float in ST0   (site 0x005614e8)
//   body  +0x1c obj-table entry:   __cdecl(bodyPtr)  -> EAX (ignored; FUN_0057c2b0, no x87 leak)
typedef float10 (__cdecl *RwpVolFn24)(int);
typedef int     (__cdecl *RwpBodyFn1c)(int *);

// m32 float constants (verified): 0.5 bbox scale / inertia numerator / 1.0
#define K23_DAT_005cc32c (*(const float *)0x005cc32cu)
#define K23_DAT_005cc574 (*(const float *)0x005cc574u)
#define K23_DAT_005cc320 (*(const float *)0x005cc320u)

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
extern "C" int __cdecl FUN_00561040(int param_1);
extern "C" int __cdecl FUN_00561c50(int param_1);
extern "C" int __cdecl FUN_00561e60(int param_1);
extern "C" int __cdecl FUN_00561e80(int param_1);
extern "C" int __cdecl FUN_00561390(int param_1);

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
// 0x00561040  Outer pair-record slicer → K13 island orchestrator FUN_00560260.
//   Builds the exact by-value stack frame the original passes as arg1/arg8.
//   CRITICAL: the descriptor and the block are ONE CONTIGUOUS array — K13's
//   FUN_00560260 (`piVar8 = param_1`) reads piVar8[0x16] (=block[0], used as a
//   write BASE at line 172 `*(piVar8[0x16]+piVar8[0x17]*4)=…`) and piVar8[0x17]
//   (=block[1], the out counter). In the original those land at [ESP+0x74]/
//   [ESP+0x78] — i.e. block[0]/[1] immediately follow descriptor[21]. A SEPARATE
//   block array makes piVar8[0x16] read past the descriptor → garbage base →
//   write crash (observed intermittently at spawn once real contacts existed).
//   So `frame[30]`:
//     [0..21]  = descriptor: even slots = puVar1[0,2,4,6,8,0xa,0xc,0x10,0x12,
//                0x14]; odd slots = &frame[22] (block base — K13 reads these
//                pointer slots too); [14]/[15] uninit (original skips them).
//     [22]=block[0]=local_20  [23]=block[1]=local_1c(OUT: FUN_00560260 sets 0,++)
//     [24]=block[2]=local_18  [25]=block[3]=local_14  [26]=block[4]=local_10
//     [27]/[28]=block[5]/[6] uninit  [29]=block[7]=pair-record base
//   arg1 = frame; arg8 = &frame[25] (=&local_14): K13 reads param_8+4=frame[26]
//   (count) and param_8+0x10=frame[29] (base), then derefs base+0x14.
//   14-arg __cdecl call (arg14 = ctx+0x408 vestigial — FUN_00560260 reads 13).
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_00561040(int param_1)
{
  undefined4 *puVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int *piVar5;
  int local_84;                 // pairs in this slice
  int local_80;                 // running pair index
  undefined4 frame[30];         // descriptor[0..21] + block[22..29] — ONE contiguous frame

  local_80 = 0;
  puVar1 = *(undefined4 **)(**(int **)(param_1 + 0x70) + 0x10);
  frame[0]  = puVar1[0];
  puVar1[0x17] = 0;
  frame[1]  = (undefined4)(frame + 22);
  frame[2]  = puVar1[2];
  frame[3]  = (undefined4)(frame + 22);
  frame[4]  = puVar1[4];
  frame[5]  = (undefined4)(frame + 22);
  frame[6]  = puVar1[6];
  frame[7]  = (undefined4)(frame + 22);
  frame[8]  = puVar1[8];
  frame[9]  = (undefined4)(frame + 22);
  frame[10] = puVar1[10];
  frame[11] = (undefined4)(frame + 22);
  frame[12] = puVar1[0xc];
  frame[13] = (undefined4)(frame + 22);
  // frame[14], frame[15] left UNINITIALISED (original never writes [ESP+0x54]/[0x58]).
  frame[16] = puVar1[0x10];
  frame[17] = (undefined4)(frame + 22);
  frame[18] = puVar1[0x12];
  frame[19] = (undefined4)(frame + 22);
  frame[20] = puVar1[0x14];
  frame[21] = (undefined4)(frame + 22);
  iVar4 = *(int *)(param_1 + 200);
  if (iVar4 != 0) {
    do {
      uVar3 = 0;
      local_84 = 0;
      if (local_80 == iVar4) {
        return 0;
      }
      piVar5 = (int *)(*(int *)(param_1 + 0xd4) + local_80 * 0x28 + 0x1c);
      iVar2 = local_80;
      do {
        uVar3 = uVar3 + *piVar5;
        if (*(uint *)(param_1 + 0x48) < uVar3) break;
        piVar5 = piVar5 + 10;
        local_84 = local_84 + 1;
        iVar2 = iVar2 + 1;
      } while (iVar2 != iVar4);
      if (local_84 == 0) {
        return 0;
      }
      frame[29] = (undefined4)(*(int *)(param_1 + 0xd4) + local_80 * 0x28);  // block[7] pair-record base
      frame[25] = *(undefined4 *)(param_1 + 0xc4);                          // block[3] local_14
      frame[26] = (undefined4)local_84;                                     // block[4] local_10 (count)
      frame[22] = (undefined4)(puVar1[0x16] + puVar1[0x17] * 4);            // block[0] local_20
      frame[24] = (undefined4)(puVar1[0x18] - puVar1[0x17]);               // block[2] local_18
      FUN_00560260((undefined4)frame,(undefined4)(param_1 + 0x9c),(undefined4)(param_1 + 0x168),
                   (undefined4)(param_1 + 0xfc),(undefined4)(param_1 + 0x2c4),
                   (undefined4)(param_1 + 0x364),(undefined4)(param_1 + 0xa8),(undefined4)(frame + 25),
                   (undefined4)param_1,*(undefined4 *)(**(int **)(param_1 + 0x70) + 0xc),
                   *(undefined4 *)(param_1 + 0xf4),*(undefined4 *)(param_1 + 0xf8),
                   *(undefined4 *)(param_1 + 0x404),(undefined4)(param_1 + 0x408));
      puVar1[0x17] = puVar1[0x17] + frame[23];                              // += block[1] local_1c (OUT)
      iVar4 = *(int *)(param_1 + 200);
      local_80 = local_80 + local_84;
    } while (local_80 != iVar4);
  }
  return param_1;
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

// ---------------------------------------------------------------------------
// 0x00561390  CCD + impact-resolution pass. param_1 = solver context. Phases:
//   (1) per-manifold CCD sweep: for each active member shape, FUN_0055bb70 +
//       3x FUN_004c3ac0 (Vec3 mag, ST0) + the shape's +0x24 volume fn form a
//       time-of-impact estimate; qualifying manifolds get a slot index in puVar9.
//   (2) FUN_0056c0a0 (K6) integrate + a body +0x1c refresh loop over records.
//   (3) impact resolution: gather tagged bodies, copy contact frames into the
//       solve arrays, per contact FUN_00561280 + FUN_00565fa0 build the point,
//       write a skew-symmetric inertia matrix block to the body's +0x40-stride
//       record, accumulate into auStack_20, FUN_0055ab30 finalise; then the
//       FUN_0055a9a0 / FUN_00573670 / FUN_0056b9d0 solver pass and a final
//       FUN_0056be80 (K6) + body +0x1c apply loop.
//   See the header note for the INT-counter / buffer-array / KV-frame decisions.
// ---------------------------------------------------------------------------
extern "C" int __cdecl FUN_00561390(int param_1)
{
  float fVar1; float fVar2; float fVar3; float fVar4; float fVar5; float fVar6; float fVar7;
  ushort uVar8;
  undefined4 *puVar9;
  int iVar10;
  float fVar11; float fVar12;
  float *pfVar13;
  int *piVar15;
  float *pfVar16;
  uint uVar17; uint uVar18;
  int *piVar20;
  int iVar21;
  undefined4 *puVar22;
  int iVar23;
  undefined4 *puVar24; undefined4 *puVar25;
  float10 fVar26; float10 fVar27; float10 fVar28;
  int *local_a8;
  int local_a4;                    // (decomp float) outer manifold counter / phase-3 contact accum — INT
  float *local_a0;
  uint local_9c;
  uint local_98;
  int *local_94;
  uint local_90;
  int local_8c;
  int local_84;                    // count of tagged manifolds
  int local_80;                    // (decomp float) ctx+0x3c base pointer
  int local_78;                    // running slot index (starts at 1)
  int local_74;                    // (decomp float) ctx+0x38 base pointer
  int local_70;                    // (decomp float) ctx+0x34 base pointer
  int local_6c;                    // (decomp float) ctx+0x30 base pointer
  float local_4c[4];               // FUN_0055bb70 out; local_4c[3] == decomp local_40
  float fStack_38;
  float auStack_30[4];             // FUN_00561280 out; auStack_30[3] == decomp uStack_24
  undefined4 auStack_20[8];        // 32-byte contact accumulator
  // per-scope splits of the decomp's reused float `fVar19`:
  float fToi;                      // phase-1 time-of-impact estimate
  int   iGather;                   // phase-3 gather counter
  float fVar19m;                   // phase-3 matrix scratch (pfVar13[2])
  int   iRes;                      // FUN_0055a9a0 int result
  int   recBase;                   // phase-2 record-array base

  puVar9 = *(undefined4 **)(param_1 + 0x40);
  local_6c = 0;
  uVar18 = *(int *)(**(int **)(param_1 + 0x70) + 0x44) * 2;
  local_70 = 0;
  local_74 = 0;
  puVar24 = puVar9;
  for (uVar17 = uVar18 >> 2; uVar17 != 0; uVar17 = uVar17 - 1) {   // memset puVar9 buffer to 0 (dwords)
    *puVar24 = 0;
    puVar24 = puVar24 + 1;
  }
  local_80 = 0;
  local_78 = 1;
  for (uVar18 = uVar18 & 3; uVar18 != 0; uVar18 = uVar18 - 1) {    // ...tail bytes
    *(undefined1 *)puVar24 = 0;
    puVar24 = (undefined4 *)((int)puVar24 + 1);
  }
  local_94 = *(int **)(param_1 + 0x74);            // dead store (overwritten before use)
  iVar23 = 0;
  local_84 = 0;
  local_a4 = 0;
  // ---- phase 1: per-manifold CCD sweep -------------------------------------
  if (*(int *)(param_1 + 0xc8) != 0) {
    local_9c = 0;
    do {
      pfVar13 = (float *)(*(int *)(param_1 + 0xd4) + local_9c);      // record base (stride 0x28)
      if ((*(uint *)((int)pfVar13 + 0x18) & 3) == 0) {              // rec+0x18 flags & 3 == 0
        local_94 = *(int **)pfVar13;                                 // rec+0x00 member-list base
        local_a8 = (int *)0x0;                                       // inner member counter
        if (*(int *)((int)pfVar13 + 0xc) != 0) {                     // rec+0x0c member count
          do {
            iVar23 = *local_94;
            if ((*(uint *)(iVar23 + 8) & 4) == 0) {
              if ((*(uint *)(iVar23 + 8) & 8) != 0) {
                iVar21 = 0;
                uVar8 = *(ushort *)(iVar23 + 0xc);
                local_98 = 0;
                if (uVar8 != 0) {
                  do {
                    iVar10 = *(int *)(iVar21 + *(int *)(iVar23 + 0x10));
                    piVar15 = *(int **)(iVar21 + 4 + *(int *)(iVar23 + 0x10));
                    if (FUN_0055bb70(iVar10,0,local_4c) != 0) {
                      fVar26 = FUN_004c3ac0(*(int *)(*(int *)(*piVar15 + 0x10) + 8) + 0x10 +
                                            piVar15[1] * 0x20);
                      fVar27 = FUN_004c3ac0(piVar15[1] * 0x20 +
                                            *(int *)(*(int *)(*piVar15 + 0x10) + 8));
                      fVar28 = FUN_004c3ac0((int)(piVar15 + 2));
                      fToi = (float)(((fVar28 + (float10)local_4c[3]) * (float10)(float)fVar26 +
                                      (float10)(float)fVar27) * (float10)*(float *)param_1);
                      fVar26 = (*(RwpVolFn24 *)(*(int *)(iVar10 + 0x5c) + 0x24))(iVar10);
                      if ((((float10)*(float *)(iVar10 + 0x4c) + (float10)*(float *)(iVar10 + 0x4c)
                            + fVar26) * (float10)K23_DAT_005cc32c < (float10)fToi) &&
                         (local_4c[3] * *(float *)(param_1 + 0x1c) < fToi)) goto LAB_00561536;
                    }
                    local_98 = local_98 + 1;
                    iVar21 = iVar21 + 0xc;
                  } while (local_98 < uVar8);
                }
              }
            }
            else {
LAB_00561536:
              uVar18 = (uint)*(ushort *)(*local_94 + 0xc);
              if ((uVar18 - 1) + local_78 <= *(uint *)(param_1 + 0x20)) {
                *(short *)((int)puVar9 + (uint)*(ushort *)(*local_94 + 0x20) * 2) = (short)local_78;
                local_84 = local_84 + 1;
                local_78 = uVar18 + local_78;
              }
            }
            local_a8 = (int *)((int)local_a8 + 1);
            local_94 = local_94 + 1;
            iVar23 = local_84;
          } while ((uint)local_a8 < *(uint *)(local_9c + 0xc + *(int *)(param_1 + 0xd4)));
        }
      }
      local_a4 = local_a4 + 1;
      local_9c = local_9c + 0x28;
    } while ((uint)local_a4 < (uint)*(int *)(param_1 + 0xc8));
    if (iVar23 != 0) {
      // ---- block A: gather tagged bodies + copy their contact frames --------
      local_6c = *(int *)(param_1 + 0x30);
      local_70 = *(int *)(param_1 + 0x34);
      local_94 = *(int **)(param_1 + 0x2c);
      local_74 = *(int *)(param_1 + 0x38);
      local_80 = *(int *)(param_1 + 0x3c);
      iGather = 0;
      piVar20 = local_94;
      piVar15 = local_94;
      if (*(int *)(param_1 + 0x78) != 0) {
        do {
          iVar21 = *(int *)(*(int *)(param_1 + 0x74) + iGather * 4);
          if (*(short *)((int)puVar9 + (uint)*(ushort *)(iVar21 + 0x20) * 2) != 0) {
            *piVar20 = iVar21;
            piVar20 = piVar20 + 1;
          }
          iGather = iGather + 1;
        } while ((uint)iGather < (uint)*(int *)(param_1 + 0x78));
      }
      for (; iVar23 != 0; iVar23 = iVar23 + -1) {
        local_98 = (uint)*(ushort *)(*piVar15 + 0xc);
        uVar18 = (uint)*(ushort *)((int)puVar9 + (uint)*(ushort *)(*piVar15 + 0x20) * 2);
        if (local_98 != 0) {
          iVar21 = 0;
          local_a0 = (float *)((int)local_80 + uVar18 * 4);
          puVar24 = (undefined4 *)(uVar18 * 0x10 + (int)local_70);
          local_a8 = (int *)((int)local_6c + uVar18 * 0xc);
          do {
            iVar21 = iVar21 + 0xc;
            piVar20 = *(int **)(*(int *)(*piVar15 + 0x10) + -8 + iVar21);
            puVar22 = (undefined4 *)(**(int **)(*piVar20 + 0x10) + 0x30 + piVar20[1] * 0x40);
            *local_a8 = *puVar22;
            local_a8[1] = puVar22[1];
            local_a8[2] = puVar22[2];
            puVar22 = (undefined4 *)(*(int *)(*(int *)(*piVar20 + 0x10) + 0x10) + piVar20[1] * 0x10);
            *puVar24 = *puVar22;
            puVar24[1] = puVar22[1];
            puVar24[2] = puVar22[2];
            puVar24[3] = puVar22[3];
            *local_a0 = *(float *)param_1;
            local_a0 = local_a0 + 1;
            local_98 = local_98 - 1;
            puVar24 = puVar24 + 4;
            local_a8 = local_a8 + 3;
          } while (local_98 != 0);
        }
        piVar15 = piVar15 + 1;
      }
    }
  }
  // ---- phase 2: integrate + body +0x1c refresh loop --------------------------
  puVar24 = *(undefined4 **)(**(int **)(param_1 + 0x70) + 0x10);
  FUN_0056c0a0(*puVar24,puVar24[1],puVar24[4],puVar24[5],puVar24[2],puVar24[3],puVar24[0x14],
               puVar24[0x15],puVar24[6],puVar24[7],*(float *)param_1);
  iVar23 = 0;
  local_a8 = (int *)0x0;
  if (*(int *)(param_1 + 0xc8) != 0) {
    recBase = *(int *)(param_1 + 0xd4);
    do {
      piVar15 = (int *)(iVar23 + recBase);
      if ((*(byte *)(iVar23 + 0x18 + recBase) & 1) == 0) {
        uVar18 = 0;
        if (piVar15[3] != 0) {
          do {
            piVar15 = *(int **)(*piVar15 + uVar18 * 4);
            (*(RwpBodyFn1c *)(*piVar15 + 0x1c))(piVar15);
            FUN_0055e050(piVar15);
            recBase = *(int *)(param_1 + 0xd4);
            uVar18 = uVar18 + 1;
            piVar15 = (int *)(iVar23 + recBase);
          } while (uVar18 < *(uint *)(iVar23 + 0xc + recBase));
        }
      }
      local_a8 = (int *)((int)local_a8 + 1);
      iVar23 = iVar23 + 0x28;
    } while ((uint)local_a8 < (uint)*(int *)(param_1 + 0xc8));
  }
  // ---- phase 3: impact resolution -------------------------------------------
  if (local_84 != 0) {
    local_90 = local_84;
    local_a8 = local_94;
    do {
      uVar17 = (uint)*(ushort *)(*local_a8 + 0xc);
      uVar18 = (uint)*(ushort *)((int)puVar9 + (uint)*(ushort *)(*local_a8 + 0x20) * 2);
      local_a4 = 0;
      local_9c = 0;
      if (uVar17 != 0) {
        local_98 = 0;
        puVar24 = (undefined4 *)(uVar18 * 0x20 + (int)local_74);
        local_a0 = (float *)((int)local_6c + uVar18 * 0xc);
        pfVar13 = (float *)((int)local_70 + 4 + uVar18 * 0x10);
        do {
          if (FUN_00561280(*local_a8,local_9c,auStack_30) != 0) {
            FUN_00565fa0((float *)puVar24,local_a0,auStack_30,auStack_30[3]);
            piVar15 = *(int **)(*(int *)(*local_a8 + 0x10) + 4 + local_98);
            fVar11 = K23_DAT_005cc574 /
                     (pfVar13[2] * pfVar13[2] +
                      pfVar13[1] * pfVar13[1] + *pfVar13 * *pfVar13 + pfVar13[-1] * pfVar13[-1]);
            fVar12 = pfVar13[-1] * fVar11;
            fStack_38 = *pfVar13 * fVar11;
            fVar11 = pfVar13[1] * fVar11;
            fVar19m = pfVar13[2];
            fVar1 = pfVar13[2];
            fVar2 = pfVar13[2];
            fVar3 = pfVar13[-1];
            fVar4 = *pfVar13;
            fVar5 = pfVar13[1];
            local_4c[0] = fVar11 * *pfVar13;
            fVar6 = pfVar13[1];
            fVar7 = pfVar13[-1];
            *(float *)(piVar15[1] * 0x40 + **(int **)(*piVar15 + 0x10)) =
                 K23_DAT_005cc320 - (fVar11 * fVar5 + fStack_38 * fVar4);
            *(float *)(**(int **)(*piVar15 + 0x10) + 4 + piVar15[1] * 0x40) =
                 fVar2 * fVar11 + fStack_38 * fVar7;
            *(float *)(**(int **)(*piVar15 + 0x10) + 8 + piVar15[1] * 0x40) =
                 fVar12 * fVar6 - fVar1 * fStack_38;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x10 + piVar15[1] * 0x40) =
                 fStack_38 * fVar7 - fVar2 * fVar11;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x14 + piVar15[1] * 0x40) =
                 K23_DAT_005cc320 - (fVar11 * fVar5 + fVar12 * fVar3);
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x18 + piVar15[1] * 0x40) =
                 local_4c[0] + fVar19m * fVar12;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x20 + piVar15[1] * 0x40) =
                 fVar12 * fVar6 + fVar1 * fStack_38;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x24 + piVar15[1] * 0x40) =
                 local_4c[0] - fVar19m * fVar12;
            *(float *)(**(int **)(*piVar15 + 0x10) + 0x28 + piVar15[1] * 0x40) =
                 K23_DAT_005cc320 - (fStack_38 * fVar4 + fVar12 * fVar3);
            *(undefined4 *)(**(int **)(*piVar15 + 0x10) + 0x30 + piVar15[1] * 0x40) = 0;
            *(undefined4 *)(**(int **)(*piVar15 + 0x10) + 0x34 + piVar15[1] * 0x40) = 0;
            *(undefined4 *)(**(int **)(*piVar15 + 0x10) + 0x38 + piVar15[1] * 0x40) = 0;
            *(undefined4 *)(**(int **)(*piVar15 + 0x10) + 0xc + piVar15[1] * 0x40) = 3;
            puVar22 = (undefined4 *)(**(int **)(*piVar15 + 0x10) + 0x30 + piVar15[1] * 0x40);
            *puVar22 = *(undefined4 *)local_a0;                 // raw bit-copy (float bits, not convert)
            puVar22[1] = *(undefined4 *)(local_a0 + 1);
            puVar22[2] = *(undefined4 *)(local_a0 + 2);
            pfVar16 = (float *)(*(int *)(*(int *)(*piVar15 + 0x10) + 0x10) + piVar15[1] * 0x10);
            *pfVar16 = pfVar13[-1];
            pfVar16[1] = *pfVar13;
            pfVar16[2] = pfVar13[1];
            pfVar16[3] = pfVar13[2];
            iVar23 = local_a4 + 1;
            if (local_a4 == 0) {
              puVar22 = puVar24;
              puVar25 = auStack_20;
              for (iVar21 = 8; iVar21 != 0; iVar21 = iVar21 + -1) {
                *puVar25 = *puVar22;
                puVar22 = puVar22 + 1;
                puVar25 = puVar25 + 1;
              }
              local_a4 = iVar23;
            }
            else {
              FUN_00565ef0((float *)auStack_20,(float *)auStack_20,(float *)puVar24);
              local_a4 = iVar23;
            }
          }
          local_9c = local_9c + 1;
          local_98 = local_98 + 0xc;
          pfVar13 = pfVar13 + 4;
          local_a0 = local_a0 + 3;
          puVar24 = puVar24 + 8;
        } while (local_9c < uVar17);
      }
      (*(RwpBodyFn1c *)(*(int *)*local_a8 + 0x1c))((int *)*local_a8);
      FUN_0055ab30(*(undefined4 *)(*local_a8 + 0x24),*local_a8,auStack_20);
      local_a8 = local_a8 + 1;
      local_90 = local_90 + -1;
    } while (local_90 != 0);
    iRes = FUN_0055a9a0(**(undefined4 **)(param_1 + 0x70),local_94,local_84,
                        *(undefined4 *)(param_1 + 0x80),*(undefined4 *)(param_1 + 0x88),0xf);
    *(int *)(param_1 + 0x84) = iRes;
    FUN_00573670(param_1,*(undefined4 *)(param_1 + 0x80),iRes,(undefined4)puVar9,local_74,local_80,
                 local_78);
    FUN_0056b9d0(param_1,local_94,local_84,(undefined4)puVar9,local_74,local_80,local_78);
    if (local_84 != 0) {
      local_8c = local_84;
      do {
        local_90 = (uint)*(ushort *)(*local_94 + 0xc);
        uVar8 = *(ushort *)((int)puVar9 + (uint)*(ushort *)(*local_94 + 0x20) * 2);
        if (local_90 != 0) {
          iVar23 = 0;
          do {
            FUN_0056be80(*(undefined4 *)(**(int **)(param_1 + 0x70) + 0x10),
                         *(undefined4 *)(*(int *)(*(int *)(*local_94 + 0x10) + 4 + iVar23) + 4),
                         *(float *)((int)local_80 + (uint)uVar8 * 4));
            iVar23 = iVar23 + 0xc;
            local_90 = local_90 - 1;
          } while (local_90 != 0);
        }
        (*(RwpBodyFn1c *)(*(int *)*local_94 + 0x1c))((int *)*local_94);
        FUN_0055e050((int *)*local_94);
        local_94 = local_94 + 1;
        local_8c = local_8c + -1;
      } while (local_8c != 0);
    }
  }
  return param_1;
}

// --- gta-reversed-style hook registration — CLUSTER 23 (9 of 9 — COMPLETE). ---
RH_ScopedInstall(FUN_00561390, 0x00561390);
RH_ScopedInstall(FUN_0055fe50, 0x0055fe50);
RH_ScopedInstall(FUN_0055fea0, 0x0055fea0);
RH_ScopedInstall(FUN_0055ff70, 0x0055ff70);
RH_ScopedInstall(FUN_0055ff90, 0x0055ff90);
RH_ScopedInstall(FUN_00561040, 0x00561040);
RH_ScopedInstall(FUN_00561c50, 0x00561c50);
RH_ScopedInstall(FUN_00561e60, 0x00561e60);
RH_ScopedInstall(FUN_00561e80, 0x00561e80);

}  // namespace Collision
}  // namespace mashed_re
