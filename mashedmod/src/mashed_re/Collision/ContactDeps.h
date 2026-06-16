// Mashed RE — WS-B2/B3 contact-solver dependency surface.
//
// The contact solvers are ported VERBATIM from the decompiler (pool3, read_only,
// MASHED.exe anchor BDCAE0…). The original passes the vehicle record in a
// register (ESI/EDI/EAX); in this greenfield port it is an explicit `int* v`
// (the 0xd04 record base, == DAT_008815a0 + car*0xd04 in the original). Float
// fields are aliased over the same dword via vF()/vFP() — the decompiler's
// `(float)param_1[i]` is a *bit reinterpret* of the int slot, NOT a numeric
// int→float conversion, because the underlying memory is a float. Using vF()
// reproduces that exactly.
//
// Leaf vector ops (normalize/magnitude/dot/cross/sqrt) are implemented here
// faithfully. The non-trivial RW-engine calls (matrix transform/build, set-rot,
// acos, broadphase walk, dynamic-object list) are RESIDUAL DEPENDENCIES wired in
// WS-B4 / WS-A — declared here, stubbed in ContactStubs.cpp so the module
// compiles+links inert until wired. Each stub cites the real RVA.
#pragma once

#include <cstdint>
#include <cmath>
#include <cstring>

namespace mashed_re {
namespace Collision {

// --- vehicle-record field views (int* base, same memory) -------------------
inline float&  vF (int* v, int i)       { return *reinterpret_cast<float*>(v + i); }
inline float*  vFP(int* v, int i)       { return  reinterpret_cast<float*>(v + i); }
inline float   asF(int bits)            { float f; std::memcpy(&f, &bits, 4); return f; }

// --- leaf vector math (faithful to FUN_004c3b30/004c39b0/004c3ac0) ---------
// NOTE (bit-identity residual): the original FastSqrt FUN_004c3b30 is an x87
// approximation; here we use sqrtf. Equal to several ULP, not bit-identical —
// recorded for the eventual installed-hook scenario diff (see [[project-wsa2-rwmath-bitident]]).
inline float FastSqrt(float x) { return std::sqrt(x); }

// FUN_004c3ac0 — 3-vector magnitude.
inline float Vec3Mag(const float* p) { return FastSqrt(p[0]*p[0] + p[1]*p[1] + p[2]*p[2]); }

// FUN_004c39b0 — normalize src into dst in place (guarded against zero-length,
// matching the FUN_0046c5f0 normalize pattern; numerator _DAT_005cc320 = 1.0f).
inline void Vec3Normalize(float* dst, const float* src) {
    float m2 = src[0]*src[0] + src[1]*src[1] + src[2]*src[2];
    if (m2 <= 0.0f) { dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; return; }
    float inv = 1.0f / FastSqrt(m2);
    dst[0] = src[0]*inv; dst[1] = src[1]*inv; dst[2] = src[2]*inv;
}

// =====================  RESIDUAL ENGINE DEPENDENCIES  ======================
// Declared here; stubbed in ContactStubs.cpp. Real impls land in WS-B4 / WS-A
// (the standalone already ports the RW math under Math/ — these get bound to it
// when the contact path is wired into the per-frame update).

// FUN_004c3df0 — RwV3dTransformPoints: transform `count` vec3 at `src` by the
// RW matrix block `mtx`, writing to `dst`.
void Rw_TransformPoints(float* dst, const float* src, int count, void* mtx);

// FUN_004c3d90 — RW indirect vtable dispatch (the rigid-body contact query the
// dispatcher/wheel solver use to read contact normal/velocity into scratch).
void Rw_VtableDispatch(void* dst, void* src, int count, void* mtxBlock);

// FUN_004c4d20 — build an RW rotation matrix from `axis` (vec3) and `angleBits`
// (the original passes the float bit-pattern of the degree angle).
void Rw_MatrixFromAxisAngle(void* outMtx, const float* axis, float angleDeg, int flag);

// FUN_004c4dc0 — derive a working matrix `outMtx` from the vehicle contact
// matrix block `srcMtx` (car-car angular path).
void Rw_MatrixDerive(void* outMtx, void* srcMtx);

// FUN_004c52f0 — set the rotation of the wheel-ring RW matrix block from `rot`.
void Rw_SetRotation(void* mtxBlock, void* rot, int mode);

// FUN_004a3384 — acos approximation (returns the angle for the axis-angle build).
float Math_Acos(double x);

// FUN_004a2c48 — monotone tick counter.
int Sys_TickCount();

// FUN_0040e350 — current game-mode discriminator (race modes = 6/7/10/0xb).
int Game_Mode();

// DAT_00803320 — active player-count discriminator (car-car impact scaling).
extern int g_playerCount;

// FUN_00538c80 — RW broadphase walk of the collision world `world` against the
// query box `queryBox`; invokes `cb` per candidate, appending the triangle batch
// (fills DAT_0088e60c). `user` = &DAT_00828320.
void Rw_BroadphaseWalk(void* world, void* queryBox, void* cb, void* user);

// FUN_00485370 / FUN_00485360 / FUN_00485420 — dynamic-object contact list.
float* Obj_ListBase();          // &DAT_006e87b8
int    Obj_ListCount();         // DAT_006fa0f8
void   Obj_ReadWorldPos(void* obj, float* outPos);   // FUN_00485420

// --- per-tick contact scratch globals (DAT_0088e5e0..0088e654) -------------
// In the original these are file-static .bss filled by the broadphase callback;
// in the standalone the wiring (B4) provides the batch base + the scratch. Here
// they live in ContactStubs.cpp so the solver ports resolve.
extern int   g_wheelSkipFlags[4];   // DAT_0088e5e0  (per-wheel "already classified")
extern float g_contactQueryScratch[16]; // DAT_0088e600 (RW contact-normal query scratch)
extern float g_wheelContactPos[12]; // DAT_0088e620 base (4 × vec3 transformed wheel pos;
                                    // classifier reads from +1 float = DAT_0088e624)
extern int   g_activeContactCount;  // DAT_0088e650  (filled this tick, max 4)
extern int   g_terrainEntryCount;   // DAT_0088e60c
// Base of the per-tick terrain triangle batch the broadphase callback appends
// to (the byte-address the solvers iterate; first entry's data at +8). In the
// original this is a register-passed global; the standalone wiring (B4) sets it.
extern int*  g_terrainBatch;        // batch base (byte-addressable via int*)

}  // namespace Collision
}  // namespace mashed_re
