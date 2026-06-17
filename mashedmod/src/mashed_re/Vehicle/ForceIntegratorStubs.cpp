// Mashed RE — WS-A5 residual-dependency stubs + runtime globals.
//
// FUN_0046ddb0 (ForceIntegrator.cpp) is a faithful port of the force LOGIC; the
// RW-math / PRNG / game-mode callees and the runtime globals it reads are wired
// in WS-B4 (when the integrator replaces TrackRenderer::UpdateCar and binds to
// the ported Math/ RW primitives + the live race state). Until then these stubs
// make the module compile + link inert. Each cites the real RVA / DAT.
#include "ForceIntegrator.h"

// Forward-decls at GLOBAL scope (must NOT be nested inside mashed_re::Vehicle).
namespace mashed_re { namespace Math {
    void RwV3dTransformPointsCPU(float* dst, const float* src, int count, const float* m);
} }
extern "C" void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode);

namespace mashed_re {
namespace Vehicle {

// --- runtime globals (B4 sets) ---------------------------------------------
int   g_playerCount    = 0;          // DAT_007f0fd0
int   g_raceTimer      = 0;          // DAT_007f0ff8
float g_gravScale      = 0.0f;       // _DAT_00803340
float g_gravX = 0.0f, g_gravY = 0.0f, g_gravZ = 0.0f;  // _DAT_00803334/38/3c
float g_suspDtTerm     = 0.0f;       // _DAT_0088e610
float g_suspScale      = 0.0f;       // _DAT_0088e5f0
float g_rubberBand[16] = {0};        // DAT_008989b0
int   g_rubberRefCar   = 0;          // DAT_008989c8
int*  g_vehicleArrayBase = nullptr;  // DAT_008815a0
// g_suspScratch (DAT_00881560) is defined in the Collision module (shared with
// the wheel solver) — Collision::g_suspScratch.

// --- residual engine deps (WS-A-DEVXFORM: now bound to real C++ impls) ------
// (RwV3dTransformPointsCPU + RwMatrixRotate forward-declared at global scope above)
// FUN_004c3df0 — RwV3dTransformPoints. The original dispatches the RW DEVICE
// transform; bound to the CPU 3x4 matrix*vec3 (Math/RwV3dTransformPointsCPU.cpp) so
// A5/A6a compute correctly standalone (mtx = the 16-float RwMatrix from FUN_004c4d20).
void Rw_TransformPoints(float* dst, const float* src, int count, void* mtx) {
    mashed_re::Math::RwV3dTransformPointsCPU(dst, src, count, reinterpret_cast<const float*>(mtx));
}
// FUN_004c4d20 — RwMatrix from axis+angle. Bound to Math/RwMatrixRotate (0x004c4d20).
// mode 0 (REPLACE) is standalone-correct; modes 1/2 (concat) dispatch the RW device
// matrix-mult (need RW device init, WS-E). A6a uses mode 0; A6b uses mode 1 (.asi only).
void Rw_MatrixFromAxisAngle(void* outMtx, const float* axis, float angle, int mode) {
    RwMatrixRotate(outMtx, axis, angle, mode);
}
// FUN_00472650 (+ PRNG FUN_00534870) — random float in [lo,hi).
float Fi_RandRange(float lo, float /*hi*/) { return lo; }  // deterministic stand-in
// FUN_0040e350 — game-mode discriminator (race = 6).
int Fi_GameMode() { return 0; }
// FUN_0040e340 — game-mode tick (side-effecting; no-op stand-in).
void Fi_GameModeTick() {}

// --- control-integrator residual deps -------------------------------------
// The per-frame torque-ring phase counter DAT_007f101c (& 0xf each frame). A8
// binds it to the real per-frame counter; inert here.
int g_torqueRingPhase = 0;                                   // DAT_007f101c
// A6a (Integrate2.cpp) + A6b (AeroStabilize.cpp) are now REAL ports — no stubs here.
// FUN_004a2c48 — per-input smoother/round-of-ST0 [UNCERTAIN signature/input].
int  Vc_InputFilter() { return 0; }
int  Vc_RoundST0()    { return 0; }                          // FUN_004a2c48 (ST0 input implicit)
// A6a runtime-ptr comparands (mode-4 boost-pad cars) — inert until A8 binds them.
int  g_modeCarA = 0;                                         // DAT_0088e668 [UNCERTAIN]
int  g_modeCarB = 0;                                         // DAT_0088e66c

}  // namespace Vehicle
}  // namespace mashed_re
