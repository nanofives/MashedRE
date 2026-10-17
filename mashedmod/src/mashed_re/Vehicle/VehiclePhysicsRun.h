// Mashed RE — WS-A8: run the ported vehicle-physics chain in mashed_re.exe.
//
// Standalone driver for the verbatim physics chain (A3 VehicleInit -> A4
// VehicleControlIntegrate -> A5 ForceIntegrator -> A6a Integrate2 -> A6b
// AeroStabilize), behind the MASHED_REAL_PHYSICS env toggle (default ON since 2026-09-14; MASHED_REAL_PHYSICS=0
// reverts to the kinematic scaffold in TrackRenderer::UpdateCar for A/B only).
//
// This is the "make it RUN" half (NOT bit-identity C4 — that is WS-A-VERIFY-3).
// It allocates the 0xd04 record array (standalone mirror of DAT_008815a0), runs
// a SIMPLIFIED core of the FUN_00470c70 dispatcher (the global-binding + per-car
// integrate + contact pass — NOT the elimination/respawn/AI-targeting branches),
// and adapts the standalone car state <-> the record.
//
// STATUS: PENDING runtime-smoke (MAIN tree) + C4. WS-A8-GAPS (2026-06-17) status:
//   - [U-A8-SUBSTEP] CLOSED: StepPlayer subdivides the frame into <=50 fixed ~1ms
//     substeps (the FUN_00470c70 chunk granularity, local_24 = min(remaining,0x32)).
//   - terrain contacts CLOSED: VehiclePhysics_SetWorld() builds the ground soup
//     from the track collision tris -> WheelContactSolver reports grounded wheels
//     -> A5 suspension force is live (caller must SetWorld at track load). Since d2close
//     the wheel solver's terrain query is the COLLI*.BSP replica walk, not this soup.
//   - [U-A8-GRAVITY] RESOLVED as VESTIGIAL: ref-to(0x00803338) -> the ONLY writer
//     is FUN_00470c70's frame-end zeroing (0x004713fc, =0) and the only reader is
//     A5 (0x0046e316). So g_gravX/Y/Z are ALWAYS 0 in the original; our stub=0 is
//     faithful. Vertical dynamics come from suspension/contacts, not this term.
//   - [U-A8-STEER] RESOLVED (WS-A8-STEER, 2026-06-17, Ghidra pool11 read-only):
//     The steer path is NOT a separate function — it IS the A4 (FUN_00470670)
//     channel previously mislabelled "drive torque". CORRECTED MODEL (RVA-cited):
//       * STEER cmd = descriptor bytes [0]/[1] (sign A / sign B; 0..255 magnitude).
//         AI writer FUN_00416250 stores *param_3 (=[0]) / param_3[1] (=[1]) from
//         the FUN_00415e20 signed steering-angle error (band split at _DAT_005cd09c,
//         0x004164.. / 0x0041650..). Human cook FUN_00496530 writes [0]/[1] + the
//         derived analog floats at +0x14/+0x18. ([4]/[5]=accel/brake — A6a path.)
//       * A4 FUN_00470670 READS input[0]/[1] (asm MOV EDI,EAX @0x470679; CMP [EBP],
//         BL @0x470732 = *param_3, [EBP+1] @0x470754 = param_3[1]) and writes the
//         scaled result to record +0x1a8 (input[0]) and +0x26c (input[1]). Those
//         two offsets ARE the WHEEL-0 / WHEEL-1 STEER-ANGLE slots (wheelN steer =
//         +0x16c + N*0xC4 + 0x3c => w0=+0x1a8, w1=+0x26c, w2=+0x330, w3=+0x3f4).
//         A4 zeroes all four every frame (0x004706c1..d3). Steer scale = input *
//         (+0x190 = 34.0; init FUN_0046b540 @0x42080000) * (1/256, _DAT_005ceaa8) *
//         0.5 -> ~16.93 deg at full lock. (+0x330/+0x3f4 have NO writer in
//         0x466000-0x472000 besides init + A4-clear -> rear wheels never steer =
//         FRONT-WHEEL steering; init sets wheel0/1 committed-mode +0x168/+0x22c = 2.)
//       * A5 FUN_0046ddb0 Phase 0 reads each wheel's steer angle piVar12[0xf]
//         (=+0x1a8/+0x26c/+0x330/+0x3f4); nonzero -> FUN_004c4d20(m, up=DAT_006146fc,
//         angleDEG, 0) rotates that wheel's forward axis. A6a (FUN_00467650) per-
//         wheel lateral grip then yaws the body via angular velocity +0x9bc (vec3;
//         world-Y component +0x9c0 = yaw rate).
//     WIRED: StepPlayer sets input[0]/[1] from io.steer; the chain produces the body
//     angular velocity; StepPlayer integrates io.yaw from the read-back yaw rate
//     (+0x9c0). The kinematic steer*kSteer*dt yaw stopgap is REMOVED when
//     MASHED_REAL_PHYSICS is on (TrackRenderer no longer kinematically yaws).
//     PENDING C4 (needs the matched-input installed-hook race diff, WS-A-VERIFY).
#pragma once

#include <cstdint>

namespace mashed_re {
namespace Vehicle {

// Player car <-> record adapter payload (world space).
struct PlayerCarIO {
    float pos[3];           // in/out (caller integrates pos from vel)
    float vel[3];           // in/out -> record +0x9b0 (kVelocity)
    float yaw;              // in/out (radians); forward = {cos,0,sin}. When physics
                            // steering is on, StepPlayer advances yaw from the
                            // chain-integrated yaw rate (+0x9c0), NOT kinematically.
    float speed;            // out    -> record +0x9e4 (kSpeed)
    // The vehicle input descriptor (&DAT_007f1038 + slot*0x4c). Byte map RESOLVED
    // (ai_ctrl_byte_map_RESOLVED + WS-A8-STEER): [0]/[1]=STEER cmd (sign A/B, 0..255
    // magnitude — A4 FUN_00470670 reads these, writes the front-wheel steer angle to
    // +0x1a8/+0x26c), [4]=accelerator, [5]=brake/reverse (A6a FUN_00467650 reads
    // these). StepPlayer derives input[0]/[1] from `steer` and input[4]/[5] from the
    // caller-set throttle bytes — both MUST be set for a full drive+steer.
    std::uint8_t input[8];  // [0]/[1]=steer A/B, [4]=accel, [5]=brake (0..255)
    // Steering command in [-1,+1] (left/right; caller's in.steer). StepPlayer maps it
    // to the mutually-exclusive descriptor steer bytes [0]/[1] the original AI/human
    // writers use (FUN_00416250/FUN_00496530): +steer -> input[0], -steer -> input[1].
    float steer;            // in     -> descriptor steer bytes [0]/[1]
    // Standalone wheel-grounding hint (substitute for the un-portable RW BSP
    // broadphase FUN_00538c80 + device transform FUN_004c3d90): the caller knows
    // the car is on the track via its own GroundHeight collision; when set, the
    // chain marks the 4 wheels grounded so the drive/suspension blocks engage.
    int grounded;           // 1 = car is in ground contact (caller's GroundHeight ok)
    // A8 VELOCITY-VECTOR MOTION MODEL (2026-08-25). REPLACES the old
    // `drive_speed` scalar (heading * scalar speed), which could not represent
    // slip by construction: it projected the chain velocity onto {cos,0,sin(yaw)}
    // and the caller re-expanded it along the same axis, so velocity and body
    // heading were forced equal. FUN_0046e9e0 integrates position from the
    // velocity VECTOR, independently of the orientation it integrates in the same
    // function, so the port now does the same. This is the frame's accumulated
    // WORLD-space position delta, summed over the substep loop exactly as the
    // original accumulates it per substep:
    //   inc = dtMs * _DAT_005cc948 * _DAT_005cea80 * (+0x9b0,+0x9b4,+0x9b8)
    // (FMULs at 0x0046e9e8 / 0x0046e9f6; velocity reads at 0x0046e9fe / 0x0046ea0a
    //  / 0x0046ea14; stores at 0x0046ea5f / 0x0046ea69 / 0x0046ea73).
    // Decode: re/analysis/data/A8_position_law_20260825.md.
    // The caller applies it as pos += drive_delta (already includes dt). 0 until
    // the first physics step.
    float drive_delta[3];   // out: world-space position delta for this frame
    // [D3 2026-09-26] 1 = input[0]/[1] already hold the descriptor steer bytes (the AI
    // tick's ctrl block, written by FUN_00416250) and `steer` is ignored; 0 = derive
    // them from `steer` (the player path). The original's A4 FUN_00470670 reads the
    // bytes directly (0x00470732 / 0x00470754), so for the AI this is the faithful feed.
    int raw_steer = 0;
    // [d2posmodel 2026-10-16] fix B part 3 (MASHED_POS_TRANS, PosModelKnobs.h). The caller's
    // pos[1] is its render/game y (TrackRenderer: ground + car_ground_off_); the physics y is
    // pos[1] - ground_off, which at a spawn is the ground under the car centre, the original's
    // FUN_004704c0 convention (decomp: local_8 = param_3 - FUN_0046d400(x, y, z, world), and
    // FUN_0046d400 casts (x, y+3, z) -> (x, y-3, z), _DAT_005cc31c = 3.0). With part 3
    // on, StepCar returns the solver-corrected position in pos (all three axes, caller's
    // convention) and the caller takes it instead of pos + drive_delta. Unused with part 3 off.
    float ground_off = 0.f;
};

bool VehiclePhysics_Enabled();                       // true unless MASHED_REAL_PHYSICS=0 (D2, 2026-09-14)
void VehiclePhysics_Init(int carCount, int trackType);
// Feed the track collision triangles (TrackRenderer col_verts_/col_tris_) to ProbeGround's
// triangle soup (wheel seat height). The wheel solver's broadphase walks the COLLI*.BSP replica
// instead (Collision/CollisionWorld.cpp, d2close). Call once at track load.
// mats = per-triangle material index (col_mat_, triCount entries) or nullptr;
// it reaches record +0x1ec through contact entry +0x30 (original 0x0046d00b).
void VehiclePhysics_SetWorld(const float* verts, int vertCount,
                             const unsigned* tris, int triCount,
                             const unsigned* mats = nullptr);
void VehiclePhysics_StepPlayer(float dt, PlayerCarIO& io);
// Step any car slot through the same physics chain (slot 0 = player; 1..3 = AI
// opponents). The caller supplies the descriptor input (io.input[4]/[5] throttle +
// io.steer) and persists vel/yaw/pos per car. StepPlayer == StepCar(0, ...).
void VehiclePhysics_StepCar(int slot, float dt, PlayerCarIO& io);
// [d2engage 2026-10-16] fix B part 4 (MASHED_POS_ORDER): run only slot's one-shot spawn-settle step
// (no-op once it has run). With part 3 on, io.pos returns the settled position, as from StepCar.
void VehiclePhysics_SpawnSettle(int slot, float dt, PlayerCarIO& io);
// [a4order 2026-10-17] MASHED_A4_ORDER=1 (default OFF, U-9746): FUN_00470c70's order, every car's gate + A4
// (0x00470f80..0x00471099) and refresh (0x004710a1..0x004710da) before any substep (0x00471137). Per frame, before
// the player's StepCar: A4PreBegin(), A4Pre(slot, dt, io) for each AI car with the io its StepCar will get, then
// A4PreEnd(). The car's StepCar later runs its substeps without the gate and A4. A car's first step, a frame
// budget over 0x32 and MASHED_A8_A4_FIRST=0 keep the per-car order.
bool VehiclePhysics_A4OrderEnabled();
void VehiclePhysics_A4PreBegin();
void VehiclePhysics_A4Pre(int slot, float dt, PlayerCarIO io);
void VehiclePhysics_A4PreEnd();
// A8 (2026-08-25): (re)seed a slot's integrated body-orientation basis from a yaw.
// The body basis is now persistent state inside the physics module (the original
// keeps it in the record's +0x928 double buffer; see BodyOrientationIntegrate.cpp
// for why the port holds it separately), so any caller that teleports or re-aims
// the car (spawn, grid placement, off-mesh recovery) must tell the physics module,
// otherwise the integrated basis keeps the pre-teleport heading.
void VehiclePhysics_ResetOrientation(int slot, float yaw);
// [D3 2026-09-26] float at byte offset `off` of slot's 0xd04 record (the standalone mirror
// of DAT_008815a0 + slot*0xd04): the AI reads +0x9e4 (FUN_0046d6d0) and +0xb0c
// (FUN_0046d6a0). 0 before VehiclePhysics_Init or for an out-of-range slot/offset.
float VehiclePhysics_RecordF32(int slot, int off);
// [U-9156 2026-09-30] int32 at byte offset `off` of the same record. Needed because
// +0x9ec (the active-contact count the fixup divides by, `fild [edi+0x9ec]` at
// 0x0046f3cd) is an INTEGER, so reading it as a float gives a denormal, not a count.
int VehiclePhysics_RecordI32(int slot, int off);
// [d3b] the live body basis (RwMatrix float[16]) of `slot`, or nullptr before it is seeded.
float* VehiclePhysics_BodyBasis(int slot);
// [pontoon 2026-10-16] the slot's record (nullptr before Init), and the Warzone platform's in-box carry: publish the
// carried RwMatrix m to the ring's current half, the integrated basis and the adapter positions; returns the caller's
// position (y + ground_off) and the basis heading.
unsigned char* VehiclePhysics_RecordPtr(int slot);
void VehiclePhysics_CarryTo(int slot, const float* m, float ground_off, float callerPos[3], float* yaw);

}  // namespace Vehicle
}  // namespace mashed_re
