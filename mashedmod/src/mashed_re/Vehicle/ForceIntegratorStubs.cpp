// Mashed RE — WS-A5 residual-dependency stubs + runtime globals.
//
// FUN_0046ddb0 (ForceIntegrator.cpp) is a faithful port of the force LOGIC; the
// RW-math / PRNG / game-mode callees and the runtime globals it reads are wired
// in WS-B4 (when the integrator replaces TrackRenderer::UpdateCar and binds to
// the ported Math/ RW primitives + the live race state). Until then these stubs
// make the module compile + link inert. Each cites the real RVA / DAT.
#include "ForceIntegrator.h"

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
float g_suspScratch[12] = {0};       // DAT_00881560

// --- residual engine deps (stubbed; real RVAs cited) -----------------------
// FUN_004c3df0 — RwV3dTransformPoints. Real impl: Math/RwV3dTransformPoints.cpp.
void Rw_TransformPoints(float* dst, const float* src, int count, void* /*mtx*/) {
    for (int i = 0; i < count * 3; ++i) dst[i] = src[i];   // identity until wired
}
// FUN_004c4d20 — RwMatrix from axis+angle. Real impl: Math/RwMatrixRotate.cpp.
void Rw_MatrixFromAxisAngle(void* /*outMtx*/, const float* /*axis*/, float /*angle*/, int) {}
// FUN_00472650 (+ PRNG FUN_00534870) — random float in [lo,hi).
float Fi_RandRange(float lo, float /*hi*/) { return lo; }  // deterministic stand-in
// FUN_0040e350 — game-mode discriminator (race = 6).
int Fi_GameMode() { return 0; }
// FUN_0040e340 — game-mode tick (side-effecting; no-op stand-in).
void Fi_GameModeTick() {}

}  // namespace Vehicle
}  // namespace mashed_re
