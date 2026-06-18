// Mashed RE — WS-A8: standalone driver for the ported vehicle-physics chain.
// See VehiclePhysicsRun.h for status + the documented approximations.
//
// Dispatcher scale constants harvested from FUN_00470c70 (Ghidra pool13, 2026-06-17):
//   _DAT_005cea80 = 0x3b360bc0 = 0.0027809   (suspDtTerm = dt * this)
//   _DAT_005ccd08 = 0x453b8000 = 3000.0      (suspScale  = this / suspDtTerm)
#include "VehiclePhysicsRun.h"
#include "VehicleStruct.h"
#include "ForceIntegrator.h"                 // extern globals + VehicleWheelForceIntegrate
#include "../Collision/ContactSolvers.h"     // WheelContactSolver (B4)
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <vector>
// #define MASHED_PHYS_DIAG 1   /* enable the steer-chain diag block (G2 drive debug) */

namespace mashed_re {
namespace Vehicle {

// Chain entry points (defined in their .cpp; declared here to avoid a header churn).
int  VehicleInit(int slot, int trackType);                                       // A3 0x0046b540
void VehicleControlIntegrate(int* self, float dt, std::uint8_t* input, void* xf); // A4 0x00470670
extern int g_torqueRingPhase;   // DAT_007f101c (defined in ForceIntegratorStubs.cpp)

// Build a vehicle WORLD-transform RwMatrix (the layout RwV3dTransformPointsCPU /
// RwMatrixRotate use: right@m[0..2], flags@m[3], up@m[4..6], at@m[8..10],
// pos@m[12..14]) for a heading `yaw` so that transforming the body forward axis
// (0,0,1) yields the car's heading {cos(yaw),0,sin(yaw)} (the TrackRenderer/adapter
// forward convention). In the original this matrix comes from the live RW scene
// graph; the standalone has no RW device, so we synthesize it from the car's yaw.
//   at    = (cos, 0, sin)         [forward; transform of (0,0,1)]
//   up    = (0, 1, 0)
//   right = up x at = (sin, 0, -cos)   [transform of (1,0,0)]
static void BuildYawMatrix(float yaw, float* m /*[16]*/) {
    const float c = std::cos(yaw), s = std::sin(yaw);
    m[0]  =  s; m[1]  = 0.f; m[2]  = -c; m[3]  = 0.f;   // right
    m[4]  = 0.f; m[5] = 1.f; m[6]  = 0.f; m[7]  = 0.f;  // up
    m[8]  =  c; m[9]  = 0.f; m[10] =  s; m[11] = 0.f;   // at (forward)
    m[12] = 0.f; m[13] = 0.f; m[14] = 0.f; m[15] = 0.f; // pos (origin; only axes used)
}

namespace {
constexpr std::size_t kRec   = 0xd04;          // record stride (== sizeof, vehicle.md)
constexpr float       kSuspDtK = 0.0027809f;   // _DAT_005cea80
constexpr float       kSuspNum = 3000.0f;      // _DAT_005ccd08
// [U-A8-SUBSTEP] the dispatcher FUN_00470c70 runs the chain in <=0x32 (50) chunks
// (local_24 = min(remaining, 0x32)) over the frame's substep budget. We subdivide
// the frame into <=kMaxSubstep fixed ~1ms steps (the dispatcher's chunks are in ms).
constexpr int         kMaxSubstep = 50;        // 0x32 (ms chunk cap)

unsigned char g_records[16 * kRec];            // the 0xd04 record array (mirror of DAT_008815a0)
bool          g_inited = false;

// Terrain contact soup (built from the track collision triangles) the B4 wheel
// solver's broadphase walks via Collision::g_worldTris.
std::vector<Collision::CollTriangle> g_worldTriStore;

inline float& F(unsigned char* r, std::size_t o) { return *reinterpret_cast<float*>(r + o); }
inline int&   I(unsigned char* r, std::size_t o) { return *reinterpret_cast<int*>(r + o); }
inline unsigned char* rec(int slot) { return g_records + static_cast<std::size_t>(slot) * kRec; }
}  // namespace

// [terrain] Build the wheel solver's contact soup from the track collision tris
// (TrackRenderer col_verts_/col_tris_) so WheelContactSolver reports grounded
// wheels -> A5's suspension force is no longer inert. verts = x,y,z flat;
// tris = 3 vertex indices per triangle.
void VehiclePhysics_SetWorld(const float* verts, int vertCount,
                             const unsigned* tris, int triCount) {
    g_worldTriStore.clear();
    if (!verts || !tris || triCount <= 0) {
        Collision::g_worldTris = nullptr; Collision::g_worldTriCount = 0; return;
    }
    g_worldTriStore.reserve(static_cast<std::size_t>(triCount));
    for (int t = 0; t < triCount; ++t) {
        const unsigned i0 = tris[t * 3 + 0], i1 = tris[t * 3 + 1], i2 = tris[t * 3 + 2];
        if ((int)i0 >= vertCount || (int)i1 >= vertCount || (int)i2 >= vertCount) continue;
        Collision::CollTriangle ct{};
        for (int k = 0; k < 3; ++k) {
            ct.v0[k] = verts[i0 * 3 + k];
            ct.v1[k] = verts[i1 * 3 + k];
            ct.v2[k] = verts[i2 * 3 + k];
        }
        const float e1x = ct.v1[0]-ct.v0[0], e1y = ct.v1[1]-ct.v0[1], e1z = ct.v1[2]-ct.v0[2];
        const float e2x = ct.v2[0]-ct.v0[0], e2y = ct.v2[1]-ct.v0[1], e2z = ct.v2[2]-ct.v0[2];
        float nx = e1y*e2z - e1z*e2y, ny = e1z*e2x - e1x*e2z, nz = e1x*e2y - e1y*e2x;
        const float m = std::sqrt(nx*nx + ny*ny + nz*nz);
        if (m > 1e-12f) { nx/=m; ny/=m; nz/=m; }
        ct.normal[0]=nx; ct.normal[1]=ny; ct.normal[2]=nz; ct.material=0; ct.surfaceKey=0;
        g_worldTriStore.push_back(ct);
    }
    Collision::g_worldTris     = g_worldTriStore.empty() ? nullptr : g_worldTriStore.data();
    Collision::g_worldTriCount = static_cast<int>(g_worldTriStore.size());
}

bool VehiclePhysics_Enabled() {
    static const bool e = (std::getenv("MASHED_REAL_PHYSICS") != nullptr);
    return e;
}

void VehiclePhysics_Init(int carCount, int trackType) {
    std::memset(g_records, 0, sizeof(g_records));
    // The integrator's "other cars" base (DAT_008815a0) -> our standalone array.
    g_vehicleArrayBase = reinterpret_cast<int*>(g_records);
    if (carCount < 1)  carCount = 1;
    if (carCount > 16) carCount = 16;
    g_playerCount = carCount;
    for (int s = 0; s < carCount; ++s) VehicleInit(s, trackType);  // A3: suspension/mass/geom
    g_inited = true;
}

// Mark all 4 wheels grounded (state=1) + grounded count=4.0, or airborne (state=0,
// count=0). STANDALONE SUBSTITUTE for the un-portable RW contact orchestrator
// FUN_0046f6c0 (it sets the wheel states self+0x198/0x25c/0x320/0x3e4 from the RW
// BSP broadphase FUN_00538c80 + device transform FUN_004c3d90 of the wheel mounts;
// both are stubbed standalone, so without this the wheels never ground -> the A6a
// drive block (gated on the wheel state p[-3]!=0) and the suspension block (gated on
// grounded count +0x9e0 == 0x40800000 = 4.0) never fire -> the car cannot move).
// The grounded fact comes from the caller's own GroundHeight collision (PlayerCarIO).
static void SetGrounded(unsigned char* r, bool grounded) {
    const int s = grounded ? 1 : 0;
    I(r, off::kWheel0State) = s;
    I(r, off::kWheel1State) = s;
    I(r, off::kWheel2State) = s;
    I(r, off::kWheel3State) = s;
    // grounded count is a FLOAT count of grounded wheels at byte +0x9e0 (int idx
    // 0x278); 4.0 (0x40800000) is the all-grounded sentinel A6a/A5 gate on.
    F(r, off::kGroundedCnt) = grounded ? 4.0f : 0.0f;

    // [U-A8-CONTACTLOAD] (G2 2026-06-18) Standalone substitute for the per-frame RW
    // broadphase contact query (FUN_00538c80 + callback LAB_00468b80) that the original
    // runs: it fills each wheel's contact NORMAL at +0x200/+0x204/+0x208 (A5 Phase-6
    // pf[3..5], FUN_0046ddb0 @0x46e6xx) and contact LOAD/depth at +0x20c (pf[6]).
    // Evidence (a6_diag.log): block#4 fired n4=4 but wrote w0f=(0,0,0) because A5 set
    // the grip coefficients pf[1]/pf[2]/pf[7] (+0x1f8/+0x1fc/+0x210) from pf[6]*pf[4]
    // = LOAD*normal.y, and LOAD was 0 -> A6a block#4 lbc/l98 = 0 -> zero suspension/grip
    // force -> no lateral grip -> no yaw torque (+0x9c0 stayed exactly 0) -> the car
    // could not turn. The flat Arctic ground normal is exactly world-up (0,1,0); kRestLoad
    // is the rest suspension compression the broadphase would report — a documented
    // standalone tunable (cf. kYawScale) that scales the overall grip stiffness.
    constexpr float kRestLoad = 1.0f;   // [U-A8-CONTACTLOAD] calibrate via runtime drive
    const float load = grounded ? kRestLoad : 0.0f;
    for (int w = 0; w < 4; ++w) {
        const std::size_t nb = 0x200u + static_cast<std::size_t>(w) * 0xc4u;  // contact frame base
        F(r, nb + 0x0) = 0.0f;                       // contact normal.x
        F(r, nb + 0x4) = grounded ? 1.0f : 0.0f;     // contact normal.y (world up on flat)
        F(r, nb + 0x8) = 0.0f;                       // contact normal.z
        F(r, nb + 0xc) = load;                       // +0x20c contact load/depth (pf[6])
    }
}

void VehiclePhysics_StepPlayer(float dt, PlayerCarIO& io) {
    VehiclePhysics_StepCar(0, dt, io);
}

void VehiclePhysics_StepCar(int slot, float dt, PlayerCarIO& io) {
    if (!g_inited) VehiclePhysics_Init(4, 0);
    if (dt <= 0.f) return;
    if (slot < 0 || slot >= 16) return;
    unsigned char* r = rec(slot);

    // --- adapter IN: world velocity, forward (= {cos,0,sin} per TrackRenderer), speed ---
    F(r, off::kVelocity + 0) = io.vel[0];
    F(r, off::kVelocity + 4) = io.vel[1];
    F(r, off::kVelocity + 8) = io.vel[2];
    F(r, off::kForward + 0)  = std::cos(io.yaw);
    F(r, off::kForward + 4)  = 0.f;
    F(r, off::kForward + 8)  = std::sin(io.yaw);
    F(r, off::kSpeed)        = io.speed;
    I(r, off::kActiveFlag)   = 1;

    std::uint8_t input[8];
    std::memcpy(input, io.input, sizeof(input));
    // WS-A8-STEER: map the steer command [-1,+1] onto the descriptor's mutually-
    // exclusive STEER bytes [0]/[1] — the byte channel A4 (FUN_00470670) reads (it
    // writes the scaled result to the FRONT-wheel steer-angle slots +0x1a8/+0x26c,
    // which A5 FUN_0046ddb0 Phase 0 turns into a per-wheel forward-axis rotation via
    // FUN_004c4d20). +steer -> input[0] (sign A, +angle), -steer -> input[1] (sign B,
    // -angle); 0..255 magnitude exactly as AI writer FUN_00416250 / human cook
    // FUN_00496530 produce. Mutually exclusive (the original writes one or the other).
    {
        float st = io.steer;
        if (st >  1.0f) st =  1.0f;
        if (st < -1.0f) st = -1.0f;
        const int mag = static_cast<int>((st < 0.f ? -st : st) * 255.f + 0.5f);
        const std::uint8_t m = static_cast<std::uint8_t>(mag > 255 ? 255 : mag);
        input[0] = (st > 0.f) ? m : 0;
        input[1] = (st < 0.f) ? m : 0;
    }

    // The chain works in the ORIGINAL's millisecond time base: FUN_00470c70 passes
    // A4 a dt that is the integer ms chunk count (local_24 = min(remaining,0x32)),
    // and computes the per-frame suspension scale from the FRAME dt in ms:
    //   _DAT_0088e610 (suspDtTerm) = frameMs * _DAT_005cea80 (0.0027809)
    //   _DAT_0088e5f0 (suspScale)  = _DAT_005ccd08 (3000) / suspDtTerm
    // (the chain constants — kDt 3.33e-4, etc. — are calibrated for ms, NOT seconds).
    const float frameMs = dt * 1000.0f;
    g_suspDtTerm = frameMs * kSuspDtK;
    g_suspScale  = (g_suspDtTerm != 0.f) ? (kSuspNum / g_suspDtTerm) : 0.f;

    // The body-forward/wheel-axis world transform A5 needs (zeroed +0x928 wheel
    // matrix block produced (0,0,0) -> no drive direction -> no motion). Synthesize
    // it from the car's yaw each frame (origin position; only the rotation axes are
    // read by A5's forward/right-axis transforms).
    float xform[16];
    BuildYawMatrix(io.yaw, xform);

    // Subdivide the frame ms budget into <=50ms chunks (the FUN_00470c70 chunk loop,
    // local_24 = min(remaining,0x32)); A4's dt per chunk is that ms count.
    float remMs = frameMs;
    int guard = 0;
    while (remMs > 0.0f && guard++ < 64) {
        float chunkMs = (remMs < (float)kMaxSubstep) ? remMs : (float)kMaxSubstep;
        // Stand-in for FUN_0046f6c0: ground the wheels from the caller's collision so
        // the verbatim A6a drive/suspension blocks engage (see SetGrounded).
        SetGrounded(r, io.grounded != 0);
        g_torqueRingPhase = (g_torqueRingPhase + 1) & 0xf;
        VehicleControlIntegrate(reinterpret_cast<int*>(r), chunkMs, input, xform);
        // A6a's drive block clears the wheel state in some branches; re-assert the
        // grounded count so the next chunk's suspension block stays engaged.
        SetGrounded(r, io.grounded != 0);
        remMs -= chunkMs;
    }

    // [U-A8-SPEEDCAP] The verbatim chain self-limits LATERAL velocity (grip-clamp #6,
    // Integrate2.cpp 324-348) but not STRAIGHT-line speed — the drive force ramps the
    // internal velocity unbounded (observed 77->553 going straight), far past the small
    // standalone collision strip. Cap the chain VELOCITY MAGNITUDE here (env MASHED_SPEEDCAP,
    // default 60) — applied AFTER the chunk loop so the chain's own grip/yaw dynamics run
    // at full fidelity; only the carried-over velocity is bounded. Kept well above the
    // grip-clamp's low-speed full-stop (16) so it does NOT re-trigger the over-damp that
    // killed +0x9c0 before the contact-load fix. 0 disables.
    {
        static const float kSpeedCapBase = [] {
            const char* e = std::getenv("MASHED_SPEEDCAP");
            return e ? (float)std::atof(e) : 45.0f;   // calibrated 2026-06-18 (smooth lap)
        }();
        // [G4] Per-car cap spread: AI opponents get slightly different top speeds so the
        // field SEPARATES on track (the elimination rule fires only when the race camera's
        // required zoom saturates at 10.0 — i.e. cars spread far apart; identical cars cluster
        // and never resolve a round). A documented standalone substitute for the un-ported
        // per-AI skill/rubber-band variation (FUN_004177b0/FUN_00417180). Slot 0 (player) =
        // full base so a human races at the base pace; opponents 1..3 vary. Env MASHED_AI_SPREAD=0
        // disables (all cars = base).
        static const bool kSpread = [] {
            const char* e = std::getenv("MASHED_AI_SPREAD");
            return !(e && (e[0] == '0'));   // on unless explicitly "0"
        }();
        static const float kSlotCapMul[4] = { 1.00f, 0.86f, 1.00f, 0.92f };
        const float kSpeedCap = kSpeedCapBase *
            ((kSpread && slot >= 1 && slot <= 3) ? kSlotCapMul[slot] : 1.0f);
        if (kSpeedCap > 0.f) {
            float vx = F(r, off::kVelocity + 0), vy = F(r, off::kVelocity + 4), vz = F(r, off::kVelocity + 8);
            float sp = std::sqrt(vx*vx + vy*vy + vz*vz);
            if (sp > kSpeedCap && sp > 1e-4f) {
                float s = kSpeedCap / sp;
                F(r, off::kVelocity + 0) = vx * s;
                F(r, off::kVelocity + 4) = vy * s;
                F(r, off::kVelocity + 8) = vz * s;
                F(r, off::kSpeed) = kSpeedCap;
            }
        }
    }

#if defined(MASHED_PHYS_DIAG)  /* TEMP diag — compiled only with -DMASHED_PHYS_DIAG */
    {
        static int dn = 0;
        if (dn < 200) {
            if (std::FILE* lf = std::fopen("phys_diag.log", "a")) {   // cwd-relative
                std::fprintf(lf, "DIAG in0=%d in1=%d in4=%d in5=%d gnd=%d sp=%g "
                    "vel=(%.2f,%.2f,%.2f) steerAng=(%g,%g) angvel=(%g,%g,%g) "
                    "wheel0st=%d wheelfwd=(%g,%g,%g)\n",
                    input[0], input[1], io.input[4], io.input[5], io.grounded,
                    F(r,0x9e4),
                    F(r,off::kVelocity), F(r,off::kVelocity+4), F(r,off::kVelocity+8),
                    F(r,0x1a8), F(r,0x26c),                 /* A4 front-wheel steer-angle out */
                    F(r,0x9bc), F(r,0x9c0), F(r,0x9c4),     /* A6a body angular velocity */
                    I(r,off::kWheel0State),                  /* grounded gate */
                    F(r,0x180), F(r,0x184), F(r,0x188));    /* wheel0 right/steer axis sample */
                std::fclose(lf);
            }
            ++dn;
        }
    }
#endif  /* TEMP diag */
    // --- adapter OUT: read back the integrated body state ---
    io.vel[0] = F(r, off::kVelocity + 0);
    io.vel[1] = F(r, off::kVelocity + 4);
    io.vel[2] = F(r, off::kVelocity + 8);
    io.speed  = F(r, off::kSpeed);

    // WS-A8-STEER: PHYSICS-real heading. The chain integrates the body angular
    // velocity into +0x9bc (vec3, world frame) — A6a FUN_00467650 (Integrate2.cpp
    // 0x9bc/0x9c0/0x9c4) accrues the per-wheel lateral-grip cross-product torque
    // there. The steer input drives it: input[0]/[1] -> A4 front-wheel steer angle
    // (+0x1a8/+0x26c) -> A5 rotates the wheel forward axes -> A6a's grip yaws the
    // body. Our body forward (+0x9d4 = {cos,0,sin}) is about world-up (Y), so the Y
    // component of the angular velocity (+0x9c0) is the yaw rate. Advance io.yaw by
    // it over the frame — this REPLACES the kinematic steer*kSteer*dt stopgap.
    //
    // [U-A8-YAWSCALE] In the original the orientation is integrated from +0x9bc by
    // the RW-Physics rigid-body integrator (the FUN_0055xxxx RW-Physics/qhull island,
    // applied to the RW frame matrix — NOT a first-party function; the standalone has
    // no RW node so we fold +0x9c0 into io.yaw ourselves). The exact rate->orientation
    // scale that engine uses is therefore not recoverable from a first-party decomp;
    // kYawScale is the one tunable here. 1.0 integrates +0x9c0 directly as rad over
    // the frame ms; the follow-up runtime smoke (WS-PHYS-SMOKE) calibrates it to match
    // the original's turn radius. The STEER PATH is physics; only this gain is tuned.
    constexpr std::size_t kAngVelY  = 0x9c0;   // angular-velocity Y = yaw rate
    // [U-A8-YAWSCALE] integrate the chain yaw rate (+0x9c0) over the frame ms. Matched
    // to kWorldVel (TrackRenderer) by the SAME factor so slowing the world pace keeps
    // the turn radius (= world_speed / yaw_rate) constant. Env-tunable (MASHED_YAWSCALE)
    // for runtime calibration without a rebuild; default tracks kWorldVel's 0.12.
    static const float kYawScale = [] {
        const char* e = std::getenv("MASHED_YAWSCALE");
        float v = e ? (float)std::atof(e) : 0.34f;   // calibrated 2026-06-18 (smooth lap)
        return (v > 0.f) ? v : 0.34f;
    }();
    const float yawRate = F(r, kAngVelY);
    io.yaw += yawRate * frameMs * kYawScale;
    // keep yaw bounded so the synthesized forward stays well-conditioned
    constexpr float kTwoPi = 6.2831853f;
    while (io.yaw >  kTwoPi) io.yaw -= kTwoPi;
    while (io.yaw < -kTwoPi) io.yaw += kTwoPi;
}

}  // namespace mashed_re::Vehicle
}  // namespace mashed_re
