// Mashed RE — WS-B2/B3 residual-dependency stubs + per-tick scratch globals.
//
// The contact solvers (CarWorldContacts.cpp / CarCarContacts.cpp) are faithful
// ports of the original contact LOGIC. The RW-engine queries and the dynamic-
// object list they call are NOT yet ported; they are wired in WS-B4 / WS-A when
// the contact path replaces the ground-raycast scaffold and binds to the real
// RW math (already ported under Math/) + the COLLI*.BSP broadphase walk.
//
// Until then these stubs make the module COMPILE + LINK inert (nothing calls
// the solvers yet; Obj_ListCount()=0 and g_terrainEntryCount=0 keep them no-op).
// Each stub cites the real RVA so the wiring step knows what to bind.
//
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
//
// 2026-09-27 (D3-CONTACT): the two RW-math residuals below are no longer stubs.
// `Rw_TransformPoints` and `Rw_MatrixFromAxisAngle` are bound to the real ported
// primitives, exactly as Vehicle/ForceIntegratorStubs.cpp:39-48 already binds the
// identically-named Vehicle-namespace pair. The remaining entries stay stubbed and
// each still cites its RVA.
#include "ContactDeps.h"

// Forward-decls at GLOBAL scope (must NOT be nested inside mashed_re::Collision).
namespace mashed_re { namespace Math {
    void RwV3dTransformPointsCPU(float* dst, const float* src, int count, const float* m);
    void RwV3dTransformVectorsCPU(float* dst, const float* src, int count, const float* m);
} }
extern "C" void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode);

namespace mashed_re {
namespace Collision {

// --- per-tick contact scratch (DAT_0088e5e0..) ------------------------------
int   g_wheelSkipFlags[4]       = {0,0,0,0};   // DAT_0088e5e0
float g_contactQueryScratch[16] = {0};         // DAT_0088e600
float g_wheelContactPos[12]     = {0};         // DAT_0088e624
int   g_activeContactCount      = 0;           // DAT_0088e650
int   g_terrainEntryCount       = 0;           // DAT_0088e60c
int   g_terrainPassCount        = 0;           // [D2 attempt 20] uncapped admissions
int*  g_terrainBatch            = nullptr;     // batch base (B4 sets)
float g_suspScratch[12]         = {0};         // DAT_00881560 (shared w/ ForceIntegrator)
int   g_playerCount             = 0;           // DAT_00803320 (B4/A sets)

// --- residual engine deps (real RVAs cited; two are now BOUND, rest stubbed) ---
// FUN_004c3df0 — RwV3dTransformPoints.  BOUND 2026-09-27.
// Argument contract MEASURED at caller 0x0046d53a (inside FUN_0046d510). The pushes
// in reverse order are arg1 = EDI = ESI+0x881f74 (destination — 0x0046d53f reads the
// result back with `MOV ECX,[EDI]`), arg2 = 0x614708, arg3 = 1, arg4 = ESI+0x881ec8.
// The .data constant at 0x614708 is the vec3 {0.0, 0.0, 1.0} (bytes 00000000
// 00000000 0000803f), i.e. the SOURCE point; arg4 is the RwMatrix inside the
// 0xd04-stride vehicle record (`IMUL ESI,ESI,0xd04` at 0x0046d51e). So the contract
// is (dstOut, srcIn, count, matrix) — the standard RW order, the same binding
// Vehicle/ForceIntegratorStubs.cpp:39 already uses.  [The gloss
// "fn(out_vec3, matrix, 1, in_vec3)" in Math/RwV3dTransformPoints.cpp:39 and
// re/frida/hooks_registry.py:3188 has arg2/arg4 swapped; corrected from the asm.]
//
// The original 0x004c3df0 is a __cdecl thunk that dispatches the RW *device*
// transform through *(DAT_007d3ffc + DAT_007d3ff8 + 0x14) (asm 0x004c3e02..
// 0x004c3e10, caller-cleanup `ADD ESP,0x10` at 0x004c3e14, returns arg1 via
// `MOV EAX,ESI` at 0x004c3e17). The standalone has no RW device table, so this binds
// to a CPU stand-in. The device-dispatch form itself is ported and C4-verified in
// Math/RwV3dTransformPoints.cpp (hooks.csv 004c3df0), used by the .asi target.
//
// WHICH stand-in: the VECTORS one. MEASURED 2026-09-27 that device slot +0x14
// IGNORES the matrix translation row while slot +0xc (FUN_004c3d90) adds it —
// log/diff_rw_v3d_transform_points_cpu.csv is RED on exactly the 4 of 10 vectors
// with a nonzero pos row, and log/diff_rw_device_dispatch_0c_points.csv puts the
// same points impl 9/10 bit-identical (1 ULP on the 10th) against +0xc. Full
// evidence and the U-1891 resolution: Math/RwV3dTransformPointsCPU.cpp header.
// `RwV3dTransformVectorsCPU` is bit-identical to the original on all 10 vectors
// (log/diff_rw_v3d_transform_vectors_cpu.csv, hook rw_v3d_transform_vectors_cpu).
void Rw_TransformPoints(float* dst, const float* src, int count, void* mtx) {
    mashed_re::Math::RwV3dTransformVectorsCPU(dst, src, count,
                                              reinterpret_cast<const float*>(mtx));
}
// FUN_004c3d90 — RW indirect vtable dispatch.  BOUND 2026-09-29 (U-9154).
//
// WHICH VTABLE, WHICH SLOT, with RVAs — read from the anchored binary, not inferred
// (`py -3.12 re/tools/disasm_fn.py 0x004c3d90 0x004c3df0`, RET at 0x004c3dba):
//     0x004c3d90  mov eax,[esp+0x10]      ; arg4 = matrix
//     0x004c3d94  mov ecx,[esp+0x0c]      ; arg3 = count
//     0x004c3d98  mov edx,[esp+0x08]      ; arg2 = src
//     0x004c3d9d  mov esi,[esp+0x08]      ; arg1 = dst   (after `push esi`)
//     0x004c3da2  mov eax,[0x007d3ff8]    ; RW device table base
//     0x004c3da8  mov ecx,[0x007d3ffc]    ; per-device offset
//     0x004c3db0  call [ecx+eax+0xc]      ; DEVICE SLOT +0xc, cdecl (dst,src,count,mtx)
//     0x004c3db4  add esp,0x10            ; caller cleanup, 4 dwords
//     0x004c3db7  mov eax,esi             ; returns arg1
// `0x004c3df0` is the same thunk through **+0x14**. The two slots are NOT
// interchangeable and the
// difference was MEASURED, not reasoned (the measurement is already in this file,
// two entries up): slot **+0x14 IGNORES the matrix translation row** — it is
// `RwV3dTransformVectors` — while slot **+0xc ADDS it** — it is
// `RwV3dTransformPoints`.
//   evidence: `log/diff_rw_v3d_transform_points_cpu.csv` is RED on exactly the 4 of
//   10 vectors with a nonzero pos row when the POINTS impl is run against +0x14, and
//   `log/diff_rw_device_dispatch_0c_points.csv` puts that same POINTS impl 9/10
//   bit-identical (1 ULP on the 10th, a summation-order difference) against +0xc.
// So the correct standalone stand-in for `Rw_VtableDispatch` is the POINTS one,
// `Math::RwV3dTransformPointsCPU` — the mirror image of `Rw_TransformPoints` above,
// which binds the VECTORS one to the +0x14 thunk.
//
// WHY IT MATTERS HERE. `CarWorldContacts.cpp:407` is
//     Rw_VtableDispatch(self + 0x27e, self + 0x18, 0x12, self + sel*0x10 + 0x24a)
// i.e. the 18 body contact points at rec+0x60 transformed by the rec+0x928 RwMatrix
// into rec+0x9f8, which `VehicleTerrainContactSolver` (0x00468d80) then tests against
// the terrain batch (it reads them at `vFP(param_1, 0x27f)`, stride 3, 18 slots, and
// their untransformed source at `local_98[-0x267]` = rec+0x60). With the no-op stub
// those 18 points were all zero, so the car-vs-world half of the chain could never
// report a contact and the standalone drove through every wall. U-9154 /
// re/analysis/D2_REOPEN_2026-09-29.md §14.5-§14.6.
//
// The transform needs a real rotation in rec+0x928; the port's ring previously held
// only the translation row. VehiclePhysicsRun.cpp mirrors g_bodyBasis into it (see
// `SyncContactRingMatrix` there) so the ring is what the original's ring is.
//
// rec+0x60..+0x137 holds 18 contact points. A3 (0x0046b540) writes only the FIRST FOUR
// (the wheel points) and the original does not write them in 0x0046b540 either — that
// function READS them (loops at 0x0046b915 `lea edi,[esi+0x94]` x8 and 0x0046b98e
// `lea edi,[esi+0xf4]` x6) to build the per-slot radii at +0x5bc/+0x7bc.
//
// CORRECTED 2026-10-05 — this paragraph used to end "Their producer is NOT yet
// identified. Consequence … the standalone's hull slots 4..17 all transform to the car
// origin, so the car-vs-world contact is a POINT at the body centre rather than a
// 14-point hull." BOTH halves were already false when written, and the second was the
// load-bearing one. The producer of points 4..17 is **0x0046b1c0** (C3 `impl`,
// `Vehicle/VehicleSlotAabbExpand.cpp`, `frida_diff log/diff_vehicle_slot_aabb_expand.csv`),
// and the exe build has its own record-base-relative copy at
// `Vehicle/VehicleInit.cpp:151` which `VehicleInit.cpp:196` CALLS — so the standalone
// does populate all 14. MEASURED on the port's own capture: the three edge invariants of
// the hull's top face are within 1e-3 of 0.437600 / 0.977100 / 1.070616 on 7772 of 7772
// non-degenerate rows of `verify/d3_carcar_20261005/P1.csv`, median deviations
// <= 1.9e-07, and on 10,278 of 10,278 frames across four original captures and two cars
// (`re/tools/hull_invariants.py`). Full chain:
// `re/analysis/CARCAR_CALLSITE_2026-10-05.md` section 4.
//
// [UNCERTAIN U-9155] what remains open is one level further out and is NOT this: the
// producer of the SIX-FLOAT BOX at `DAT_0063d9e0 + slot*0x2ac + 0x230` that 0x0046b1c0
// consumes. The port seeds one measured box for every slot. U-9155's row in
// `UNCERTAINTIES.md` is current on that question (structure corrected 2026-09-30, search
// bounded to 0x0041ec0f..0x0042089b); nothing above closes it.
void Rw_VtableDispatch(void* dst, void* src, int count, void* mtxBlock) {
    mashed_re::Math::RwV3dTransformPointsCPU(reinterpret_cast<float*>(dst),
                                             reinterpret_cast<const float*>(src),
                                             count,
                                             reinterpret_cast<const float*>(mtxBlock));
}
// FUN_004c4d20 — RwMatrix from axis+angle (degrees).  BOUND 2026-09-27 to the ported
// Math/RwMatrixRotate.cpp (hooks.csv 004c4d20, C4 verified). Disasm of the original
// 0x004c4d20..0x004c4dba: `FLD [esp+0x18]; FMUL [0x5cd7a8]` (pi/180) at
// 0x004c4d23..0x004c4d27; axis sum-of-squares 0x004c4d36..0x004c4d4e;
// `CALL 0x4c3b90` (FastInvSqrt) at 0x004c4d5d; `FSIN` at 0x004c4d95;
// `FCOS` + `FSUBR [0x5cc320]` (1.0f) at 0x004c4d9f..0x004c4da1;
// `CALL 0x4c4a50` (Rodrigues inner) at 0x004c4dac.
// Both call sites pass mode 0 (WheelContactSolver.cpp:243, CarWorldContacts.cpp:203),
// the self-contained REPLACE build; modes 1/2 dispatch the RW device matrix-mult and
// are not reachable from here.
void Rw_MatrixFromAxisAngle(void* outMtx, const float* axis, float deg, int flag) {
    RwMatrixRotate(outMtx, axis, deg, flag);
}
// FUN_004c4dc0 — derive working matrix from the vehicle contact-matrix block.
void Rw_MatrixDerive(void* /*outMtx*/, void* /*srcMtx*/) {}
// FUN_004c52f0 — set rotation on the wheel-ring RW matrix block.
void Rw_SetRotation(void* /*mtxBlock*/, void* /*rot*/, int /*mode*/) {}
// FUN_004a3384 — acos approximation.
float Math_Acos(double /*x*/) { return 0.0f; }
// FUN_004a2c48 — monotone tick counter.
int Sys_TickCount() { return 0; }
// FUN_0040e350 — game-mode discriminator (race modes 6/7/10/0xb).
int Game_Mode() { return 0; }
// FUN_00538c80 — RW broadphase walk filling the terrain batch (DAT_0088e60c).
void Rw_BroadphaseWalk(void* /*world*/, void* /*box*/, void* /*cb*/, void* /*user*/) {}
// FUN_00485370 / FUN_00485360 / FUN_00485420 — dynamic-object contact list.
float* Obj_ListBase() { static float dummy[36] = {0}; return dummy; }   // &DAT_006e87b8
int    Obj_ListCount() { return 0; }                                    // DAT_006fa0f8
void   Obj_ReadWorldPos(void* /*obj*/, float* outPos) { outPos[0]=outPos[1]=outPos[2]=0.0f; }

}  // namespace Collision
}  // namespace mashed_re
