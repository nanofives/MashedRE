// Mashed RE — B5c: RenderWare-Physics-3.7 per-tick integrator SUBSET (clean-room).
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool0, read_only, 2026-07-15). Verbatim transcription of the coupling-side
// per-tick helpers that FUN_0047eb30 (the B5d coupling driver) drives each frame, plus
// the two trivial solve-entry wrappers. Source map: re/analysis/B5a_SYSTEM2_PLATING_2026-07-14.md
// + re/analysis/B5c_RWP37_INTEGRATOR_SUBSET_2026-07-15.md.
//
// SCOPE (narrow subset, ratified 2026-07-15): the 5 B5a-named helpers + FUN_0055b800 +
// the trivial wrappers. The full 12-stage RWP solver pipeline reached by FUN_0047e9c0
// (broadphase FUN_0055a1f0, island-partition FUN_00560260, CCD/impact FUN_00561390, ...)
// is the whole ~165 KB RwpConstraints/RwpPartition island and is DEFERRED to a later lane
// (it is NOT "the integrator subset"). See the B5c note §"scope correction".
//
// IMPORTANT NO-GUESSING correction carried by this file: the B5a note (OU#4) labelled the
// arrays at 0x007e9de0 (stride 0x404) as "the 4 proxy-body integration state". The live
// decomp shows 0x007e9de0 is a scalar ring write-index (wraps at 0xff) and
// &DAT_007e9de4/007ea1e4/007ea5e4/007ea9e4/007ec9e4 are a 256-slot on-screen event/collision
// MARKER draw-stream (Ghidra: "per-frame on-screen event-marker emitter", FUN_0047def0/
// FUN_0047d640), gated on game-mode FUN_0040e350() != {1..5}. They are NOT physics state.
// The real proxy-body state is pointer-indirect: body = FUN_0057c210(DAT_006c9a78[i]);
// its world matrix slot = body[1-derived], refreshed by FUN_0055dff0 -> FUN_0055b800.
//
// x87 conventions: no /arch:SSE2; the matrix math is done by the RW-Graphics callees
// (FUN_004c52f0/004c51a0/00546b10), which are extern (bit-identity A/B under the .asi calls
// the real game math at RVA — RwpBuildExterns.cpp forwards them). No transcendentals here.
#include "../Core/HookSystem.h"

namespace mashed_re {
namespace Collision {

// ---- RW-Graphics matrix callees used by FUN_0055b800 (extern; RwpBuildExterns.cpp
//      forwards to the real RVA under the .asi so the A/B is bit-identical). ----
extern "C" unsigned* FUN_004c52f0(unsigned* dst, unsigned* src, int combineOp); // 0x004c52f0 RwMatrixCombine
extern "C" void      FUN_004c51a0(float* mtx, float* vec, int combineOp);        // 0x004c51a0 RwMatrixTranslate
extern "C" unsigned  FUN_00546b10(float* quatOut, float* rotMtx);                // 0x00546b10 matrix->quat
// ---- The full RWP world step (12-stage solver pipeline). DEFERRED — extern only. ----
extern "C" void      FUN_0047e9c0(int solverCtx, unsigned arg2);                 // 0x0047e9c0 RWP world step

// Body-table base (physics body-pointer lookup table) = the game global DAT_007dc8d8.
// The hooked lookup runs LIVE under the .asi, so it must read the game's real global
// (its value is a base pointer set by FUN_0057c220 at load), NOT a standalone placeholder
// — bind to the absolute address, matching the reinterpret_cast-RVA idiom used across the
// tree (e.g. Race/ScoringHooks.cpp). Unmapped on the standalone exe, but uncalled there.
#define MASHED_DAT_007dc8d8 (*reinterpret_cast<int*>(0x007dc8d8u))

// ---------------------------------------------------------------------------
// 0x0057c210  RwpMgr — body accessor. Byte-offset read into the body-pointer
// table DAT_007dc8d8. Verbatim: `return *(undefined4*)(DAT_007dc8d8 + param_1)`.
// ---------------------------------------------------------------------------
extern "C" int __cdecl RwpBodyTableLookup(int key)
{
    return *(int*)(MASHED_DAT_007dc8d8 + key);                  // 0x0057c212
}

// ---------------------------------------------------------------------------
// 0x0055deb0  RwpMgr — world solver-handle getter. `return *(undefined4*)(param_1+4)`.
// The 2nd arg (dt = DAT_0061331c) is passed at the call site but ignored by this
// getter's body (cited 0x0055deb0). Kept in the signature to match the ABI.
// ---------------------------------------------------------------------------
extern "C" int __cdecl RwpWorldSolverHandle(int world, float /*dt*/)
{
    return *(int*)(world + 4);                                  // 0x0055deb0
}

// ---------------------------------------------------------------------------
// 0x0055ac00  RwpBodyObj — shape-active bit set/clear. Leaf, no callees, no abs writes.
//   param_1 = bitset owner (bitset at +0x5c), param_2 = body (shape idx at +0x20),
//   param_3 = set flag.
// ---------------------------------------------------------------------------
extern "C" void __cdecl RwpShapeActiveBitSet(int bitsetOwner, int body, int set)
{
    unsigned short sidx = *(unsigned short*)(body + 0x20);
    unsigned* p = (unsigned*)(*(int*)(bitsetOwner + 0x5c) + (unsigned)(sidx >> 5) * 4);
    if (set != 0) {
        *p = *p | (1u << ((unsigned char)sidx & 0x1f));         // 0x0055ac1e
        return;
    }
    *p = *p & ~(1u << ((unsigned char)sidx & 0x1f));            // 0x0055ac3e
}

// ---------------------------------------------------------------------------
// 0x0055b800  RwpBodyObj — refresh one body's world matrix from a source matrix.
//   param_1 = body-frame descriptor: [0]=owner ptr (its +0x10 -> **base tables),
//             [1]=body index, [2..4]=translation vec.
//   param_2 = source RwMatrix (the body's frame matrix to copy in).
// Writes the body's 0x40-byte world matrix slot (idx*0x40 + **(owner+0x10)) and the
// 0x10-byte quaternion slot (idx*0x10 + (*(owner+0x10))[4]). All pointer-indirect.
// ---------------------------------------------------------------------------
extern "C" int* __cdecl RwpBodyMatrixRefresh(int* param_1, void* param_2)
{
    int mtxBase  = **(int**)(*param_1 + 0x10);                   // **(owner+0x10)
    int mtxSlot  = param_1[1] * 0x40 + mtxBase;                  // idx*0x40 + base
    FUN_004c52f0((unsigned*)mtxSlot, (unsigned*)param_2, 0);     // 0x0055b811 combine op0 = copy
    FUN_004c51a0((float*)mtxSlot, (float*)(param_1 + 2), 1);     // translate by param_1[2..4]
    int quatBase = (*(int**)(*param_1 + 0x10))[4];              // (owner+0x10)[4]
    FUN_00546b10((float*)(param_1[1] * 0x10 + quatBase), (float*)mtxSlot); // matrix->quat
    return param_1;
}

// ---------------------------------------------------------------------------
// 0x0055dff0  RwpBodyObj — body world-matrix refresh GATE. Pure gate; no abs writes.
//   param_1 = body, param_2 = frame index (the coupling driver passes idx 0).
// Ghidra recovered only param_1 because param_2 is set up before the shared active-bit
// branch (the pre-branch-args pitfall, [[feedback_ghidra_prebranch_args]]). Resolved from
// the disassembly 0x0055dff0..0x0055e042:
//   if (*(body+0x24) == 0) return;                          owner
//   sidx = *(u16)(body+0x20);
//   if ((*( *(owner+0x60) + (sidx>>5)*4 ) & (1<<(sidx&0x1f))) == 0) return;   active bit
//   M = *(body+0x10);  e = M + (idx & 0xffff)*0xc;
//   FUN_0055b800(*(e+4), *(e+8));                           tail call (0x0055e03d JMP)
// ---------------------------------------------------------------------------
extern "C" void __cdecl RwpBodyRefreshGate(int body, int idx)
{
    int owner = *(int*)(body + 0x24);                           // 0x0055dff4
    if (owner == 0) return;
    unsigned short sidx = *(unsigned short*)(body + 0x20);
    unsigned bit = 1u << ((unsigned char)sidx & 0x1f);
    if ((*(unsigned*)(*(int*)(owner + 0x60) + (unsigned)(sidx >> 5) * 4) & bit) == 0)
        return;                                                // 0x0055e01a JZ
    int M = *(int*)(body + 0x10);                              // 0x0055e020
    int e = M + (idx & 0xffff) * 0xc;                          // 0x0055e023..e02b
    RwpBodyMatrixRefresh(*(int**)(e + 4), *(void**)(e + 8));   // 0x0055e03d JMP 0x0055b800
}

// ---------------------------------------------------------------------------
// 0x0055c000  RwpGjk — GJK support / extreme-point mapping. vtable-indirect on the
// shape's support virtual (slot +0x14). On the live per-frame path this is reached
// ONLY via the load-time build FUN_00481e00 (B5a:140-144); per-frame contact gen is OFF.
//   param_1 = shape, param_2 = mtx (may be 0), param_3 = dir (vec3), param_4 = out.
// Keeps the extern-"C" FUN_0055c000 name — CollisionBodyCreate.cpp calls it by name.
// Returns the world-space support point (float[3], in place) or NULL on miss.
// ---------------------------------------------------------------------------
extern "C" void* __cdecl FUN_0055c000(int shape, float* mtx, float* dir, void* out)
{
    float local[3];
    if (mtx != 0) {                                            // rotate dir into shape-local by 3x3
        local[0] = mtx[2] * dir[2] + mtx[0] * dir[0] + mtx[1] * dir[1];   // 0x0055c01b
        local[1] = mtx[6] * dir[2] + mtx[4] * dir[0] + mtx[5] * dir[1];
        local[2] = mtx[10] * dir[2] + mtx[8] * dir[0] + mtx[9] * dir[1];  // 0x0055c046
        dir = local;
    }
    // shape support virtual: (**(shape+0x5c + 0x14))(shape, dir, out)
    float* pt = (float*)(*(void*(**)(int, float*, void*))(*(int*)(shape + 0x5c) + 0x14))
                    (shape, dir, out);
    if (pt != 0 && mtx != 0) {                                 // transform support back to world (4x3 affine)
        float p0 = pt[0], p1 = pt[1], p2 = pt[2];
        pt[0] = mtx[0xc] + mtx[0] * p0 + mtx[4] * p1 + mtx[8] * p2;
        pt[1] = mtx[0xd] + mtx[1] * p0 + mtx[5] * p1 + mtx[9] * p2;
        pt[2] = mtx[0xe] + mtx[2] * p0 + mtx[6] * p1 + mtx[10] * p2;
    }
    return pt;
}

// ---------------------------------------------------------------------------
// 0x0055e200  RwpMgr — solver-context set. `*param_1 = param_2` (cited 0x0055e205).
// ---------------------------------------------------------------------------
extern "C" void __cdecl RwpSolverContextSet(unsigned* ctx, unsigned value)
{
    *ctx = value;                                             // 0x0055e205
}

// ---------------------------------------------------------------------------
// 0x0047ea40  RwpMgr — physics-scene step wrapper. `FUN_0047e9c0(param_1,param_2);
// return param_1;`. FUN_0047e9c0 is the DEFERRED 12-stage RWP solver pipeline (extern).
// ---------------------------------------------------------------------------
extern "C" int __cdecl RwpSceneStepWrapper(int solverCtx, unsigned arg2)
{
    FUN_0047e9c0(solverCtx, arg2);                            // 0x0047ea45
    return solverCtx;
}

// --- gta-reversed-style hook registration (inert on the exe via HookSystemNoOp;
//     installs the inline-JMP under the .asi for the diff-original A/B acceptance) ---
RH_ScopedInstall(RwpBodyTableLookup,    0x0057c210);
RH_ScopedInstall(RwpWorldSolverHandle,  0x0055deb0);
RH_ScopedInstall(RwpShapeActiveBitSet,  0x0055ac00);
RH_ScopedInstall(RwpBodyMatrixRefresh,  0x0055b800);
RH_ScopedInstall(RwpBodyRefreshGate,    0x0055dff0);
RH_ScopedInstall(FUN_0055c000,          0x0055c000);
RH_ScopedInstall(RwpSolverContextSet,   0x0055e200);
RH_ScopedInstall(RwpSceneStepWrapper,   0x0047ea40);

}  // namespace Collision
}  // namespace mashed_re
