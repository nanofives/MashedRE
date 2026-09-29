// Mashed RE — WS-A5 residual-dependency stubs + runtime globals.
//
// FUN_0046ddb0 (ForceIntegrator.cpp) is a faithful port of the force LOGIC; the
// RW-math / PRNG / game-mode callees and the runtime globals it reads are wired
// in WS-B4 (when the integrator replaces TrackRenderer::UpdateCar and binds to
// the ported Math/ RW primitives + the live race state). Until then these stubs
// make the module compile + link inert. Each cites the real RVA / DAT.
#include "ForceIntegrator.h"
#include <cstdlib>    // getenv — MASHED_GAMEMODE_STUB A/B revert

// Forward-decls at GLOBAL scope (must NOT be nested inside mashed_re::Vehicle).
namespace mashed_re { namespace Math {
    void RwV3dTransformPointsCPU(float* dst, const float* src, int count, const float* m);
    void RwV3dTransformVectorsCPU(float* dst, const float* src, int count, const float* m);
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
float g_a8WheelLe4[4]  = {0,0,0,0};  // [A8-ORIENT] diag only
float g_a8WheelLd4[4]  = {0,0,0,0};  // [A8-ORIENT] diag only
A6aWheelDump g_a6aDump[4] = {};      // [U-9147] A6a block-#4 capture, diag only
A6aFrameDump g_a6aFrame   = {};      // [U-9147] A6a frame-level inputs, diag only
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
}
// FUN_004c4d20 — RwMatrix from axis+angle. Bound to Math/RwMatrixRotate (0x004c4d20).
// mode 0 (REPLACE) is standalone-correct; modes 1/2 (concat) dispatch the RW device
// matrix-mult (need RW device init, WS-E). A6a uses mode 0; A6b uses mode 1 (.asi only).
void Rw_MatrixFromAxisAngle(void* outMtx, const float* axis, float angle, int mode) {
    RwMatrixRotate(outMtx, axis, angle, mode);
}
// FUN_00472650 (+ PRNG FUN_00534870) — random float in [lo,hi).
float Fi_RandRange(float lo, float /*hi*/) { return lo; }  // deterministic stand-in
// FUN_0040e350 — game-mode discriminator. Body is one instruction:
//   `return DAT_0063ba8c;`   (0x0040e350..0x0040e355, decomp confirmed 2026-09-29)
//
// CHANGED 2026-09-29 from `return 0` to 6 (U-D3-DRIVE-FORCE). The standalone has no
// DAT_0063ba8c state machine (TrackRenderer.cpp:3568 already records that gap), so this
// is a constant stand-in, and 0 was the wrong constant. The original's IN-RACE value is
// 6, established by TWO independent measurements rather than assumed:
//
//  1. FUN_00470c70 (the physics dispatcher) replaces each car's ctrl block with the
//     neutral block DAT_007f19b8 unless FUN_0040e350() is 6, 0xb or 0xa
//     (0x00470ff2 / 0x00470ffe / 0x0047100a, three separate CALLs + CMPs). The original's
//     AI cars demonstrably drive from their real ctrl bytes — verify/d3_drive_20260928/e3.msd
//     carries +0xb20 == 1 and a +0xb1c consistent with accel byte 255 clamped to 160 — so
//     the in-race value is in {6, 10, 11}.
//  2. The +0xbf4 boost timer steps exactly -200 per frame in that same capture
//     (1100, 900, 700, 500, 300, 100, 0). A6a has exactly two decrement sites, one gated on
//     `FUN_0040e350() == 6` (0x00467cd0) and one inside the +0xbf8 == 1 arm (0x00467dc6),
//     each running once per QUALIFYING wheel. The capture has two state-2 active wheels
//     (+0x168/+0x22c == 2) and dt == 50, so 2 x 2 x 50 = 200 requires BOTH sites, i.e. the
//     mode-6 site runs. Intersected with (1): the value is 6.
//
// Blast radius, audited call site by call site in the exe target (Integrate2.cpp,
// VehicleControl.cpp, BodyOrientationIntegrate.cpp, ForceIntegrator.cpp are exe-only;
// PhysicsChainHooks.cpp is asi-only and reads the LIVE global, so the .asi is untouched):
//   Integrate2.cpp:117     mode -> compared `!= 7` / `== 7` only         -> no change
//   Integrate2.cpp:~243    `== 6`                                        -> NOW RUNS (intended)
//   VehicleControl.cpp:96  A4's iVar5, compared `== 7` only              -> no change
//   BodyOrientationIntegrate.cpp:289  `== 7`                             -> no change
//   ForceIntegrator.cpp:316 RubberBandGate `mode == 6 && ... &&
//                           kRubberThr < g_rubberBand[car]`              -> no change:
//     g_rubberBand[16] is all zero and has NO writer anywhere in the standalone (only
//     readers at ForceIntegrator.cpp:296/318), so the gate still returns 0.
// forceint_selftest.cpp:59 asserts RubberBandGate(0)==0 "because the game-mode stub != 6";
// it is in NEITHER exe_sources.rsp nor asi_sources.rsp, so it does not build — but its
// stated REASON is now stale even though its expected value still holds (g_rubberBand).
//
// Revert for A/B only (v3 rule: a flag may only turn the ported behaviour OFF):
// MASHED_GAMEMODE_STUB=0 restores the pre-2026-09-29 constant.
int Fi_GameMode() {
    static const int s_mode = [] {
        const char* e = std::getenv("MASHED_GAMEMODE_STUB");
        return (e && e[0] == '0') ? 0 : 6;
    }();
    return s_mode;
}
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
// A6a boost-state-1 comparands. CORRECTED 2026-09-29 (U-D3-DRIVE-FORCE) — the old
// comment "mode-4 boost-pad cars, inert until A8 binds them / [UNCERTAIN]" is wrong and
// is replaced: they are car INDICES, and they select which cars get 8e6 instead of 5e6
// of start-boost force in A6a (0x00467d62 / 0x00467d6a).
//
// FUN_00470c70 maintains them (0x00470e2e seed, 0x00470e66..0x00470f0a sort):
//     for (i = 0; i < 4; ++i) (&DAT_0088e660)[i] = i;
//     n = FUN_0040e340();                          // participant count
//     for (i = 0; i < n; ++i) local_10[i] = FUN_00408a50(i);   // per-car progress float
//     bubble-sort local_10 DESCENDING, permuting DAT_0088e660..66c alongside
// so after it, DAT_0088e668 / DAT_0088e66c are the two LEAST-progressed cars.
// FUN_00408a50 (0x00408a50) = *(float*)(0x008a96e8 + car*0x30c).
int  g_modeCarA = 2;                                         // DAT_0088e668
int  g_modeCarB = 3;                                         // DAT_0088e66c
// Participant count — FUN_0040e340 (0x0040e340, 5 bytes) = `MOV EAX,[0x008a94d0] / RETN`.
// Already ported at C4 as Util/UtilLeaves.cpp `GetLiveCarCount` (hooks.csv row 0040e340),
// which reads the global directly — correct in-process, useless in the standalone, because
// 0x008a94d0 has NO WRITER in mashed_re.exe (grep: Race/ScoringHooks.cpp:41/236/245/325 and
// UtilLeaves.cpp:54 only READ it, and ScoringHooks.cpp is asi-only). So this is set from the
// car count VehiclePhysics_Init is given — 4 on the D3 recipe, which is the measured value.
// (ForceIntegrator.h's `void Fi_GameModeTick()` is the SAME original function mis-typed as
// void; that decl is left alone because ForceIntegrator.cpp:294 calls it only for its side
// effect.)
int  g_participantCount = 4;                                 // DAT_008a94d0
int  Fi_ParticipantCount() { return g_participantCount; }
// Ported form of the FUN_00470c70 sort above. In the STANDALONE the per-car progress
// float at 0x008a96e8 + car*0x30c has NO REACHABLE WRITER — checked, not assumed: the
// original's writer FUN_00408a70 IS ported (Frontend/MenuMixed.cpp FrontendC2RoundI,
// RH_ScopedInstall at MenuMixed.cpp:634, hooks.csv C3) but MenuMixed.cpp is in
// asi_sources.rsp only, so it is an .asi export with no call site on the mashed_re.exe
// race path; the only exe-side reference to the field is the read-only accessor
// Frontend/Leaves.cpp PerCarRaceProgressGet (0x00408a50). So every comparand is 0.0,
// every `<` is false, no swap happens and the pair stays at the seeded grid order {2, 3}.
// That is
// exactly the MEASURED value at the lights — verify/d3_force_20260929/o_c2.msd and
// o_c3.msd take the 8e6 arm and verify/d3_drive_20260928/e3.msd (car 1) does not.
// [UNCERTAIN] U-D3-BOOST-ORDER: because the progress float is never written, the pair
// cannot EVOLVE with race order as the original's does, so a mid-race boost (state 2 /
// a re-arm) would use the grid pair instead of the current last two. It does not affect
// criterion (e), whose boost window is the 6 frames at the lights where the original's
// progress values are all equal too. Resolution: port FUN_00408a70 (the progress writer)
// and call this from the frame loop; next command
// `py -3.12 re/tools/decomp_pc.py 0x00408a70 --callers`.
void Fi_UpdateBoostOrder() {
    int   idx[4] = { 0, 1, 2, 3 };                       // 0x00470e2e
    float m[4]   = { 1000.0f, 1000.0f, 1000.0f, 1000.0f };
    const int n = (g_participantCount < 4) ? g_participantCount : 4;
    for (int i = 0; i < n; ++i)                          // FUN_00408a50
        m[i] = *reinterpret_cast<const float*>(0x008a96e8u + static_cast<unsigned>(i) * 0x30cu);
    for (bool sw = true; sw; ) {                         // descending bubble sort
        sw = false;
        for (int i = 0; i < 3; ++i) {
            if (m[i] < m[i + 1]) {
                float t = m[i]; m[i] = m[i + 1]; m[i + 1] = t;
                int   u = idx[i]; idx[i] = idx[i + 1]; idx[i + 1] = u;
                sw = true;
            }
        }
    }
    g_modeCarA = idx[2];                                 // DAT_0088e668
    g_modeCarB = idx[3];                                 // DAT_0088e66c
}

}  // namespace Vehicle
}  // namespace mashed_re
