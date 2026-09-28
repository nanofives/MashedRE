// Mashed RE — WS-A5 residual-dependency stubs + runtime globals.
//
// FUN_0046ddb0 (ForceIntegrator.cpp) is a faithful port of the force LOGIC; the
// RW-math / PRNG / game-mode callees and the runtime globals it reads are wired
// in WS-B4 (when the integrator replaces TrackRenderer::UpdateCar and binds to
// the ported Math/ RW primitives + the live race state). Until then these stubs
// make the module compile + link inert. Each cites the real RVA / DAT.
#include "ForceIntegrator.h"
#define MASHED_U9138_DIAG 1   // TEMPORARY — U-9138 evidence build only; remove after.
#ifdef MASHED_U9138_DIAG
#include <cstdio>
#include <cstring>
#endif

// Forward-decls at GLOBAL scope (must NOT be nested inside mashed_re::Vehicle).
namespace mashed_re { namespace Math {
    void RwV3dTransformPointsCPU(float* dst, const float* src, int count, const float* m);
    void RwV3dTransformVectorsCPU(float* dst, const float* src, int count, const float* m);
} }
extern "C" void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode);

namespace mashed_re {
namespace Vehicle {

#ifdef MASHED_U9138_DIAG
// TEMPORARY U-9138 evidence instrumentation. Counts, over a whole race, how many
// Rw_TransformPoints calls the pre-fix (points) binding would have answered
// differently from the post-fix (vectors) binding, and how many of those calls were
// handed a matrix with a nonzero translation row at all. Dumped at process exit to
// u9138_diag.txt (CWD-relative, like mashed_re.log). Removed after the measurement.
static struct U9138Diag {
    long long calls = 0, nonzeroPos = 0, mismatches = 0;
    float firstBad[9] = {0};
    ~U9138Diag() {
        if (FILE* f = std::fopen("u9138_diag.txt", "w")) {
            std::fprintf(f, "calls=%lld nonzero_pos_row=%lld value_mismatches=%lld\n",
                         calls, nonzeroPos, mismatches);
            if (mismatches)
                std::fprintf(f, "first src=(%g,%g,%g) pos=(%g,%g,%g) pts=(%g,%g,%g)\n",
                             firstBad[0], firstBad[1], firstBad[2], firstBad[3],
                             firstBad[4], firstBad[5], firstBad[6], firstBad[7], firstBad[8]);
            std::fclose(f);
        }
    }
} g_u9138Diag;

static void U9138_DiagCompare(const float* vecOut, const float* src, int count, const float* m)
{
    g_u9138Diag.calls += count;
    if (!m) return;
    if (m[12] != 0.0f || m[13] != 0.0f || m[14] != 0.0f) g_u9138Diag.nonzeroPos += count;
    for (int i = 0; i < count; ++i) {
        float pts[3];
        mashed_re::Math::RwV3dTransformPointsCPU(pts, src + i * 3, 1, m);
        const float* v = vecOut + i * 3;
        if (std::memcmp(pts, v, sizeof pts) != 0) {
            if (!g_u9138Diag.mismatches) {
                g_u9138Diag.firstBad[0] = src[i * 3 + 0];
                g_u9138Diag.firstBad[1] = src[i * 3 + 1];
                g_u9138Diag.firstBad[2] = src[i * 3 + 2];
                g_u9138Diag.firstBad[3] = m[12]; g_u9138Diag.firstBad[4] = m[13];
                g_u9138Diag.firstBad[5] = m[14];
                g_u9138Diag.firstBad[6] = pts[0]; g_u9138Diag.firstBad[7] = pts[1];
                g_u9138Diag.firstBad[8] = pts[2];
            }
            ++g_u9138Diag.mismatches;
        }
    }
}
#endif  // MASHED_U9138_DIAG

// --- runtime globals (B4 sets) ---------------------------------------------
int   g_playerCount    = 0;          // DAT_007f0fd0
int   g_raceTimer      = 0;          // DAT_007f0ff8
float g_gravScale      = 0.0f;       // _DAT_00803340
float g_gravX = 0.0f, g_gravY = 0.0f, g_gravZ = 0.0f;  // _DAT_00803334/38/3c
float g_suspDtTerm     = 0.0f;       // _DAT_0088e610
float g_suspScale      = 0.0f;       // _DAT_0088e5f0
float g_a8WheelLe4[4]  = {0,0,0,0};  // [A8-ORIENT] diag only
float g_a8WheelLd4[4]  = {0,0,0,0};  // [A8-ORIENT] diag only
float g_rubberBand[16] = {0};        // DAT_008989b0
int   g_rubberRefCar   = 0;          // DAT_008989c8
int*  g_vehicleArrayBase = nullptr;  // DAT_008815a0
// g_suspScratch (DAT_00881560) is defined in the Collision module (shared with
// the wheel solver) — Collision::g_suspScratch.

// --- residual engine deps (WS-A-DEVXFORM: now bound to real C++ impls) ------
// (RwV3dTransformPointsCPU + RwMatrixRotate forward-declared at global scope above)
// FUN_004c3df0 — the RW DEVICE transform thunk. It dispatches device slot +0x14
// (`MOV EAX,[0x7d3ff8]` 0x004c3e02; `MOV ECX,[0x7d3ffc]` 0x004c3e08;
// `CALL [ECX+EAX+0x14]` 0x004c3e10). The standalone has no device table, so this
// binds to a CPU stand-in.
//
// WHICH stand-in: the VECTORS one (U-9138, fixed 2026-09-28; was the points form).
// Slot +0x14 IGNORES the matrix translation row m[12..14] and slot +0xc (FUN_004c3d90)
// adds it — MEASURED 2026-09-27, log/diff_rw_v3d_transform_points_cpu.csv is RED on
// exactly the 4 of 10 vectors with a nonzero pos row while
// log/diff_rw_v3d_transform_vectors_cpu.csv is GREEN 10/10 against the same 0x004c3df0
// (re/analysis/D3_CONTACT_2026-09-27.md §2.2; Collision/ContactStubs.cpp:70 was flipped
// the same day). Every live caller of THIS copy dispatches +0x14 in the original:
//   0x0046ddc9, 0x0046de1f, 0x0046de5b  (FUN_0046ddb0  -> ForceIntegrator.cpp:40,49,72)
//   0x00468072                          (FUN_00467650  -> Integrate2.cpp:289)
// all four `CALL 0x4c3df0`; none call 0x004c3d90. Disassembly + the D2 re-check that
// cleared the flip: re/analysis/U9138_FIX_2026-09-28.md.
void Rw_TransformPoints(float* dst, const float* src, int count, void* mtx) {
    mashed_re::Math::RwV3dTransformVectorsCPU(dst, src, count, reinterpret_cast<const float*>(mtx));
#ifdef MASHED_U9138_DIAG
    // TEMPORARY (U-9138 evidence build only, removed after the measurement): does the
    // points form — the pre-fix binding — ever differ here in a real race? Output above
    // is the fixed value, so this build is behaviourally identical to the shipping one.
    U9138_DiagCompare(dst, src, count, reinterpret_cast<const float*>(mtx));
#endif
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
