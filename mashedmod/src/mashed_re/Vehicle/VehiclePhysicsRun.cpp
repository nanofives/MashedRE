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

unsigned char g_records[16 * kRec];            // the 0xd04 record array (mirror of DAT_008815a0)
bool          g_inited = false;

inline float& F(unsigned char* r, std::size_t o) { return *reinterpret_cast<float*>(r + o); }
inline int&   I(unsigned char* r, std::size_t o) { return *reinterpret_cast<int*>(r + o); }
inline unsigned char* rec(int slot) { return g_records + static_cast<std::size_t>(slot) * kRec; }
}  // namespace

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

    // --- bind the dispatcher-computed globals (FUN_00470c70) so suspension/force
    //     magnitudes are no longer zero (the WS-A-VERIFY-2 blocker). ---
    g_suspDtTerm      = dt * kSuspDtK;
    g_suspScale       = (g_suspDtTerm != 0.f) ? (kSuspNum / g_suspDtTerm) : 0.f;
    g_torqueRingPhase = (g_torqueRingPhase + 1) & 0xf;

    // --- adapter IN: world velocity, forward (= {cos,0,sin} per TrackRenderer), speed ---
    F(r, off::kVelocity + 0) = io.vel[0];
    F(r, off::kVelocity + 4) = io.vel[1];
    F(r, off::kVelocity + 8) = io.vel[2];
    F(r, off::kForward + 0)  = std::cos(io.yaw);
    F(r, off::kForward + 4)  = 0.f;
    F(r, off::kForward + 8)  = std::sin(io.yaw);
    F(r, off::kSpeed)        = io.speed;
    I(r, off::kActiveFlag)   = 1;

    // --- contact pass (B4): sets the per-wheel grounded state A5 reads.
    //     g_worldTris is unset in the standalone yet -> 0 ground contacts (documented). ---
    Collision::WheelContactSolver(reinterpret_cast<int*>(r), nullptr, 0);

    // --- control + integrate: A4 -> A5 -> A6a -> A6b (the verbatim chain) ---
    std::uint8_t input[8];
    std::memcpy(input, io.input, sizeof(input));
    VehicleControlIntegrate(reinterpret_cast<int*>(r), dt, input, nullptr);

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
