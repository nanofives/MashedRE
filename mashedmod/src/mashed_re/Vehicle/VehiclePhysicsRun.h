// Mashed RE — WS-A8: run the ported vehicle-physics chain in mashed_re.exe.
//
// Standalone driver for the verbatim physics chain (A3 VehicleInit -> A4
// VehicleControlIntegrate -> A5 ForceIntegrator -> A6a Integrate2 -> A6b
// AeroStabilize), behind the MASHED_REAL_PHYSICS env toggle (default OFF -> the
// kinematic scaffold in TrackRenderer::UpdateCar stays the shipping path).
//
// This is the "make it RUN" half (NOT bit-identity C4 — that is WS-A-VERIFY-3).
// It allocates the 0xd04 record array (standalone mirror of DAT_008815a0), runs
// a SIMPLIFIED core of the FUN_00470c70 dispatcher (the global-binding + per-car
// integrate + contact pass — NOT the elimination/respawn/AI-targeting branches),
// and adapts the standalone car state <-> the record.
//
// STATUS: PENDING runtime-smoke (MAIN tree) + C4. Documented approximations:
//   - steering-input path into the chain is unmapped (A4 reads accel/brake only;
//     the wheel-steer input byte is [UNCERTAIN]) -> caller applies steer kinematically.
//   - terrain-contact feed is empty (Collision::g_worldTris unset) -> grounded
//     count 0, suspension force inert; drive/brake force still applies.
//   - gravity globals (_DAT_00803334..40) are set upstream of the dispatcher in
//     the original; left 0 here ([U-A8-GRAVITY]) -> no fall/settle yet.
//   - substep model simplified to one integrate per frame at dt ([U-A8-SUBSTEP]).
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Vehicle {

// Player car <-> record adapter payload (world space).
struct PlayerCarIO {
    float pos[3];           // in/out (caller integrates pos from vel)
    float vel[3];           // in/out -> record +0x9b0 (kVelocity)
    float yaw;              // in/out (radians); forward = {cos,0,sin}
    float speed;            // out    -> record +0x9e4 (kSpeed)
    std::uint8_t input[8];  // [0]=accel 0..255, [1]=brake/reverse 0..255, [5]=gate
};

bool VehiclePhysics_Enabled();                       // MASHED_REAL_PHYSICS set once
void VehiclePhysics_Init(int carCount, int trackType);
void VehiclePhysics_StepPlayer(float dt, PlayerCarIO& io);

}  // namespace Vehicle
}  // namespace mashed_re
