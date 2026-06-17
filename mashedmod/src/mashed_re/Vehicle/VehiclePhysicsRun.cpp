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
#include <vector>

namespace mashed_re {
namespace Vehicle {

// Chain entry points (defined in their .cpp; declared here to avoid a header churn).
int  VehicleInit(int slot, int trackType);                                       // A3 0x0046b540
void VehicleControlIntegrate(int* self, float dt, std::uint8_t* input, void* xf); // A4 0x00470670
extern int g_torqueRingPhase;   // DAT_007f101c (defined in ForceIntegratorStubs.cpp)

namespace {
constexpr std::size_t kRec   = 0xd04;          // record stride (== sizeof, vehicle.md)
constexpr float       kSuspDtK = 0.0027809f;   // _DAT_005cea80
constexpr float       kSuspNum = 3000.0f;      // _DAT_005ccd08
// [U-A8-SUBSTEP] the dispatcher FUN_00470c70 runs the chain in <=0x32 (50) chunks
// (local_24 = min(remaining, 0x32)) over the frame's substep budget. We subdivide
// the frame into <=kMaxSubstep fixed ~1ms steps (the dispatcher's chunks are in ms).
constexpr int         kMaxSubstep = 50;        // 0x32
constexpr float       kSubstepSec = 0.001f;    // ~1ms per substep (chunk granularity)

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

void VehiclePhysics_StepPlayer(float dt, PlayerCarIO& io) {
    if (!g_inited) VehiclePhysics_Init(4, 0);
    if (dt <= 0.f) return;
    unsigned char* r = rec(0);

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

    // [U-A8-SUBSTEP] subdivide the frame into <=kMaxSubstep fixed ~1ms substeps
    // (the dispatcher's chunk granularity) instead of one integrate/frame -> stable
    // integration. Per substep: bind the dispatcher-computed globals at the substep
    // dt (so g_suspScale is no longer 0 -- the WS-A-VERIFY-2 blocker), run the B4
    // contact pass (grounded state from the real terrain soup, [terrain] above), then
    // the verbatim chain A4 -> A5 -> A6a -> A6b.
    int n = static_cast<int>(dt / kSubstepSec + 0.5f);
    if (n < 1) n = 1; if (n > kMaxSubstep) n = kMaxSubstep;
    const float dtSub = dt / static_cast<float>(n);
    for (int s = 0; s < n; ++s) {
        g_suspDtTerm      = dtSub * kSuspDtK;
        g_suspScale       = (g_suspDtTerm != 0.f) ? (kSuspNum / g_suspDtTerm) : 0.f;
        g_torqueRingPhase = (g_torqueRingPhase + 1) & 0xf;
        Collision::WheelContactSolver(reinterpret_cast<int*>(r), nullptr, s);
        VehicleControlIntegrate(reinterpret_cast<int*>(r), dtSub, input, nullptr);
    }

    // --- adapter OUT: read back the integrated body state ---
    io.vel[0] = F(r, off::kVelocity + 0);
    io.vel[1] = F(r, off::kVelocity + 4);
    io.vel[2] = F(r, off::kVelocity + 8);
    io.speed  = F(r, off::kSpeed);
    const float fx = F(r, off::kForward + 0);
    const float fz = F(r, off::kForward + 8);
    if (fx != 0.f || fz != 0.f) io.yaw = std::atan2(fz, fx);   // forward {cos,0,sin}
}

}  // namespace mashed_re::Vehicle
}  // namespace mashed_re
