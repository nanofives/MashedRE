// Mashed RE — the per-car vehicle physics record (the 0xd04-byte struct the
// original's vehicle cluster operates on).
//
// MAP (with every RVA citation + the base-resolution proof):
//   re/analysis/structs/vehicle.md   (WS-A / A1, 2026-06-16)
//
// Layout summary (see the .md for the full table + evidence):
//   Base symbol:  DAT_008815a0  (car 0).  Stride 0xd04.  16 slots in .bss.
//   Car N base  = DAT_008815a0 + N*0xd04.
//   The per-frame cluster receives this base in EAX/EDI:
//     FUN_00470670  control-input integrator   (drive/steer force from input)
//     FUN_0046ddb0  per-wheel physics core      (contact, drive/susp torque)
//     FUN_00467650  drivetrain/gear step        (A6 — not yet mapped here)
//     FUN_00468980  3rd integration step        (A6 — not yet mapped here)
//   Init:  FUN_0046b540  (suspension/spring/mass; reads handling table 0x613140)
//   Dispatcher: FUN_00470c70 (16-car loop, stride 0x341 ints)
//   Destroy: FUN_0046c5c0
//
// IMPORTANT base note: vehicle_dynamics.md mislabelled the base as DAT_00881000
// (it is 0x5a0 too low) and copied some EDI[N] int-indices into "+0x.." columns
// without the ×4. The offsets in THIS file are byte offsets from DAT_008815a0,
// re-derived from the decompiler this session. See vehicle.md §0.
//
// This is a RAW MEMORY OVERLAY: many bytes are still unmapped, and a few global
// scalars (e.g. accel at +0x190) physically fall inside the +0x16c wheel-0
// footprint — fields interleave, so we model the record as a sized raw buffer
// with typed offset accessors rather than a padded field list with a fake clean
// wheel[] array. sizeof == 0xd04 is asserted.
#pragma once

#include <cstdint>
#include <cstddef>

namespace mashed_re {
namespace Vehicle {

// Byte offsets from the record base (DAT_008815a0). Every entry cites the
// function it was read/written in; see vehicle.md for the exact line evidence.
namespace off {
    // --- identity / lifecycle ---
    constexpr std::size_t kCarIndex     = 0x000; // [0]    self-id / skip-self
    constexpr std::size_t kActiveFlag   = 0x004; // [1]    1=active (init/destroy)
    constexpr std::size_t kStateFlag    = 0x010; // [4]    0=normal; ==1 friction br
    // --- mass / inertia (init FUN_0046b540) ---
    constexpr std::size_t kMass         = 0x050; // [0x14] init 1000.0
    constexpr std::size_t kMassFactorB  = 0x054; // [0x15] init inertia sum
    constexpr std::size_t kInvInertia   = 0x058; // [0x16] init 1.0/+0x54
    // --- drivetrain ---
    constexpr std::size_t kGearTorque0  = 0x150; // [0x54] gear const product…
    constexpr std::size_t kGearTorque1  = 0x154; // [0x55]
    constexpr std::size_t kGearTorque2  = 0x158; // [0x56]
    constexpr std::size_t kAccelMag     = 0x190; // per-vehicle accel scalar
    constexpr std::size_t kDriveTorque  = 0x1a8; // this-frame drive torque out
    constexpr std::size_t kDriveRing    = 0x1ac; // float[16] ring (DAT_007f101c&0xf)
    constexpr std::size_t kAngTorque    = 0x26c; // this-frame angular torque out
    constexpr std::size_t kAngTorqueRing= 0x270; // float[16] ring
    constexpr std::size_t kScratch330   = 0x330; // cleared/tick [UNCERTAIN U-V01]
    constexpr std::size_t kScratch3f4   = 0x3f4; // cleared/tick [UNCERTAIN U-V02]
    // --- per-wheel state flags (FUN_0046f6c0 writes; FUN_00467650 drive-gate reads
    //     as p[-3]; FUN_0046ddb0/FUN_0046f6c0 sum them into the grounded count) ---
    //     wheelN state = byte +0x198 + N*0xC4 (int idx 0x66/0x97/0xc8/0xf9).
    constexpr std::size_t kWheel0State  = 0x198; // [0x66]
    constexpr std::size_t kWheel1State  = 0x25c; // [0x97]
    constexpr std::size_t kWheel2State  = 0x320; // [0xc8]
    constexpr std::size_t kWheel3State  = 0x3e4; // [0xf9]
    //     wheelN committed-mode = byte +0x168 + N*0xC4 (FUN_0046b540: 2,2,1,1);
    //     FUN_00467650 drive block fires only where committed-mode == 2 (wheels 0,1).
    // --- wheel-matrix blocks ---
    constexpr std::size_t kWheelMatrices= 0x928; // RwMatrix[] (0x40 each)
    constexpr std::size_t kContactBufIdx= 0x9a4; // double-buffer index A (init 0)
    constexpr std::size_t kWheelSetSel  = 0x9a8; // [0x26a] matrix set sel (init 1)
    constexpr std::size_t kContactPosArr= 0x958; // [i*0x10+0x256] world contact pos
    // --- body dynamics ---
    constexpr std::size_t kVelocity     = 0x9b0; // [0x26c] vec3 linear velocity
    constexpr std::size_t kForward      = 0x9d4; // [0x275] vec3 forward (world)
    constexpr std::size_t kGroundedCnt  = 0x9e0; // [0x278] +1.0/contact; 4.0=all
    constexpr std::size_t kSpeed        = 0x9e4; // [0x279] speed mag / denom
    constexpr std::size_t kMotionState  = 0x9f0; // ==2 ⇒ parked velocity damp
    constexpr std::size_t kHistRingIdx  = 0xad0; // [0x2b4] pos/ang-vel ring idx
    constexpr std::size_t kHistBuffer   = 0xad4; // [0x2b5] float[6] (2×vec3)
    constexpr std::size_t kRandImpulse  = 0xb00; // [0x2c0] vec3 (FUN_00472650)
    constexpr std::size_t kSlideMeasure = 0xb0c; // (1-|fwd·vel|/spd)*spd
    constexpr std::size_t kBoostForce   = 0xb14; // vec3 (written by FUN_00467650)
    constexpr std::size_t kAirDampCtr   = 0xb18; // [0x2c6] airborne damp counter
    constexpr std::size_t kAirborneFlag = 0xb20; // [0x2c8] set on speed/contact thr
    constexpr std::size_t kFiltAccel    = 0xb24; // FUN_004a2c48(input[0])
    constexpr std::size_t kFiltBrake    = 0xb28; // FUN_004a2c48(input[1])
    constexpr std::size_t kTickRing     = 0xbec; // [0x2fb] (n+1)&0xf
    constexpr std::size_t kRingB34      = 0xb34; // [0x2cd] f[] ring
    constexpr std::size_t kBoostGate    = 0xbf0; // ≠0 ⇒ drive ×_DAT_005cc950
    constexpr std::size_t kSlipHoverFlag= 0xd00; // [0x340] slip factor select
    // --- sub-objects (vehicle_damage.md, re-based +0x4) ---
    constexpr std::size_t kControlSource= 0xb10; // destroy writes DAT_007f1030
    constexpr std::size_t kTransformSub = 0x924; // ptr region (FUN_0046d4a0)
    constexpr std::size_t kSpinoutFlag  = 0xa50; // 0=alive 2=slide (FUN_0046cbb0)
}  // namespace off

// Per-wheel block: 4 wheels, first at off + 0x16c, stride 0xC4. Offsets here are
// relative to the wheel block base. Read/written in FUN_0046ddb0 (and seeded by
// FUN_0046b540). NOTE: global scalars interleave into wheel-0's byte span; this
// view names only the fields the wheel loop actually touches.
struct Wheel {
    static constexpr std::size_t kBase0   = 0x16c; // wheel[0]
    static constexpr std::size_t kStride  = 0x0c4; // = 0x31 ints
    static constexpr std::size_t kCount   = 4;
    // wheel-relative byte offsets:
    static constexpr std::size_t kMountX  = 0x00;  // [0] component (→scratch)
    static constexpr std::size_t kMountZ  = 0x08;  // [2] component
    static constexpr std::size_t kRightAxis = 0x14;// [5] vec3 lateral/right (world)
    static constexpr std::size_t kContact = 0x2c;  // [0xb] contact flag (≠0 ground)
    static constexpr std::size_t kSteerAng= 0x3c;  // [0xf] steer angle (0=straight)
    static constexpr std::size_t kSteerOut= 0xa0;  // [0x28] steer-torque out / load
    static constexpr std::size_t kSuspForce = 0xa8;// [0x2a] vec3 suspension force
    static constexpr std::size_t kSteeredFwd= 0xb4;// [0x2d] vec3 steered forward
    static constexpr std::size_t Base(int n) { return kBase0 + n * kStride; }
};

#pragma pack(push, 1)
// Raw overlay. Cast a record base (DAT_008815a0 + car*0xd04, or the standalone
// equivalent allocation) to MashedVehicle* and use the typed accessors. The
// fields that are still unmapped remain accessible via at<T>(byte_offset).
struct MashedVehicle {
    std::uint8_t raw[0xd04];

    template <class T> T&       at(std::size_t o)       { return *reinterpret_cast<T*>(raw + o); }
    template <class T> const T& at(std::size_t o) const { return *reinterpret_cast<const T*>(raw + o); }
    template <class T> T*       ptr(std::size_t o)       { return reinterpret_cast<T*>(raw + o); }
    template <class T> const T* ptr(std::size_t o) const { return reinterpret_cast<const T*>(raw + o); }

    // hot accessors (vec3 returns float* to a contiguous 3-float run)
    std::int32_t& car_index()   { return at<std::int32_t>(off::kCarIndex); }
    std::int32_t& active_flag()  { return at<std::int32_t>(off::kActiveFlag); }
    float&  mass()               { return at<float>(off::kMass); }
    float&  accel_mag()          { return at<float>(off::kAccelMag); }
    float&  drive_torque()       { return at<float>(off::kDriveTorque); }
    float&  ang_torque()         { return at<float>(off::kAngTorque); }
    float*  velocity()           { return ptr<float>(off::kVelocity); }   // [3]
    float*  forward()            { return ptr<float>(off::kForward); }    // [3]
    float&  grounded_count()     { return at<float>(off::kGroundedCnt); }
    float&  speed()              { return at<float>(off::kSpeed); }
    std::int32_t& motion_state() { return at<std::int32_t>(off::kMotionState); }
    float&  slide_measure()      { return at<float>(off::kSlideMeasure); }
    std::int32_t& airborne_flag(){ return at<std::int32_t>(off::kAirborneFlag); }
    std::int32_t& boost_gate()   { return at<std::int32_t>(off::kBoostGate); }

    // wheel field accessors
    std::int32_t& wheel_contact(int n) { return at<std::int32_t>(Wheel::Base(n) + Wheel::kContact); }
    float&        wheel_steer(int n)   { return at<float>(Wheel::Base(n) + Wheel::kSteerAng); }
    float*        wheel_right(int n)    { return ptr<float>(Wheel::Base(n) + Wheel::kRightAxis); }
    float*        wheel_susp_force(int n){ return ptr<float>(Wheel::Base(n) + Wheel::kSuspForce); }
};
#pragma pack(pop)

static_assert(sizeof(MashedVehicle) == 0xd04, "vehicle record must be 0xd04 bytes");
static_assert(offsetof(MashedVehicle, raw) == 0, "raw overlay must start at 0");

// Per-vehicle-type handling data (init FUN_0046b540; consume in A3). Static keyed
// table at 0x00613140: 5-int entries [paramA, spring=40000, k1=1000, k2=1500,
// nextKey], -1 terminated, keyed by FUN_0040ce80(carIdx) =
// *(*(PTR_PTR_005f2770 + carIdx*4) + 4). Defaults: 100 / 40000 / 1.0 / 1.5.
struct HandlingEntry {
    std::int32_t param_a;   // 0x613144-relative
    std::int32_t spring;    // 40000
    std::int32_t k1;        // 1000  (×_DAT_005cc558 → struct)
    std::int32_t k2;        // 1500  (×_DAT_005cc558 → struct)
    std::int32_t next_key;  // -1 = end
};

}  // namespace Vehicle
}  // namespace mashed_re
