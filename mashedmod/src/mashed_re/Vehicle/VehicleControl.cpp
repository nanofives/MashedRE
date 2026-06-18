// Mashed RE — WS-A4: the per-vehicle control-input integrator FUN_00470670.
//
// Verbatim port of the control half of the per-frame vehicle update. Reads the
// per-car input descriptor (accel/brake bytes), computes the speed-normalized
// drive/reverse torque, writes it to the control-output slots (+0x1a8/+0x26c) and
// their 16-slot phase rings (+0x1ac/+0x270), then dispatches the three integration
// callees and applies the parked-state velocity damp.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool11, read_only, 2026-06-16). Every constant memory_read this session.
// Struct field map: re/analysis/structs/vehicle.md (base = DAT_008815a0, stride 0xd04).
//
// STATUS: standalone logic is verbatim. The chain callees are now all ported
// (PENDING diff-original C4 — see physics_completion_track doc):
//   FUN_0046ddb0 = VehicleWheelForceIntegrate (A5, ForceIntegrator.h)
//   FUN_00467650 = Vehicle_Integrate2          (A6a, Integrate2.cpp — spine; contact block deferred)
//   FUN_00468980 = Vehicle_AeroStabilize       (A6b, AeroStabilize.cpp — rotation-apply deferred)
//   FUN_004a2c48 = Vc_InputFilter              ([UNCERTAIN sig] input smoother, still a stub)
//   FUN_0040e350 = Fi_GameMode                 (ForceIntegrator.h dep, still a stub)
// The exact callee arg/register binding (incl. A6a param_1, FUN_004c3df0 arg order)
// and the dispatcher (FUN_00470c70) wiring are resolved at WS-A8 + WS-A-VERIFY
// (diff-original). This file is NOT yet in the exe source list (link closure = A8).
#include "ForceIntegrator.h"
#include <cstdint>
#include <cstring>

namespace mashed_re {
namespace Vehicle {

// ---- byte-offset field views on the 0xd04 vehicle record --------------------
static inline float& Fb(void* b, int off) { return *reinterpret_cast<float*>(reinterpret_cast<char*>(b) + off); }
static inline int&   Ib(void* b, int off) { return *reinterpret_cast<int*>  (reinterpret_cast<char*>(b) + off); }
// exact-bit float constructor: the decimal literals mis-round vs the original's
// .rdata (e.g. 1.66677e-4f -> 0x392ec604 != the real 0x392ec33e). Cf() takes the
// memory_read 32-bit pattern so the standalone body matches the .asi PhysicsChainHooks
// Cf(0x..) values and (where the math is bit-distinct-by-construction is moot) the
// original. (WS-PHYS-SMOKE-STEER const back-port; bits memory_read pool6 2026-06-17.)
static inline float Cf(std::uint32_t bits) { float f; std::memcpy(&f, &bits, 4); return f; }

// ---- tuning constants (raw value @ address, all memory_read 2026-06-16) -----
namespace vc {
constexpr float kZero       = 0.0f;          // DAT_005d757c
constexpr float kOne        = 1.0f;          // _DAT_005cc320 (0x3f800000)
constexpr float kHalf       = 0.5f;          // _DAT_005cc32c (0x3f000000)
constexpr float kAirSpeed   = 64.0f;         // _DAT_005cd6d4 (0x42800000) airborne-flag speed thr
constexpr float kInputScale = 0.00390625f;   // _DAT_005ceaa8 (0x3b800000) = 1/256 input->force
constexpr float kBoostMul   = 0.75f;         // _DAT_005cc950 (0x3f400000) boost-gate multiplier
constexpr float kInput5Thr  = 128.0f;        // _DAT_005cc9d0 (0x43000000) input[5] grip-branch thr
constexpr float kFilterClamp= 6000.0f;       // _DAT_005ceaa4 (0x45bb8000) filtered-input clamp
const     float kGripMul    = Cf(0x392ec33e); // _DAT_005cea58 ~= 1/6000 grip scale (EXACT bits)
constexpr float kElseMul    = 1.5f;          // _DAT_005cc348 (0x3fc00000) high-input branch mult
constexpr float kParkedDamp = 0.9f;          // _DAT_005cc9c8 (0x3f666666) motion-state-2 vel damp
} // namespace vc

// ---- runtime global the ring phase indexes ---------------------------------
extern int g_torqueRingPhase;   // DAT_007f101c (& 0xf each frame); provided by A8 wiring

// ---- callees (see header note) ---------------------------------------------
// A6a 0x00467650 — velocity/angular integration step (ported: Integrate2.cpp, PENDING C4).
void Vehicle_Integrate2(int* self, int param_1, float dt, void* wheelBlock, std::uint8_t* input);
// A6b 0x00468980 — aerodynamic stabilization (ported: AeroStabilize.cpp, PENDING C4).
// orient = vehicle world-transform RwMatrix (nullptr until A8 binds it; .asi supplies ESI).
void Vehicle_AeroStabilize(int* self, float* orient, float dt);
// FUN_004a2c48 — per-input smoother/accumulator [UNCERTAIN signature] (pending).
int  Vc_InputFilter();

// ===========================================================================
// 0x00470670  VehicleControlIntegrate(self, dt, input[], xform)
//   self    = the 0xd04 vehicle record (original: in_EAX)
//   dt      = substep delta (original: param_2 / param_4 == &dt)
//   input   = per-car input descriptor: [0]=accel, [1]=brake/reverse, [5]=byte gate
//   xform   = vehicle world-transform context (original: param_4, to A5)
// Verbatim of FUN_00470670 (body 0x00470670..0x00470991).
// ===========================================================================
void VehicleControlIntegrate(int* self, float dt, std::uint8_t* input, void* xform)
{
    void* v = self;
    const int gameMode = Fi_GameMode();                              // FUN_0040e350
    const bool atRest = (Fb(v, 0x9e4) == vc::kZero);                 // speed magnitude == 0
    // wheel-matrix block for this contact ring: [0x9a8]*0x40 + 0x928 + self
    int* wheelBlock = reinterpret_cast<int*>(
        reinterpret_cast<char*>(v) + Ib(v, 0x9a8) * 0x40 + 0x928);

    Ib(v, 0xb20) = 0; Ib(v, 0xb1c) = 0; Ib(v, 0xb18) = 0; Ib(v, 0xb14) = 0;
    Ib(v, 0x3f4) = 0; Ib(v, 0x330) = 0; Ib(v, 0x26c) = 0; Ib(v, 0x1a8) = 0;

    if (atRest) {
        Fb(v, 0xb0c) = 0.0f;
    } else {
        // slide measure = (1 - |fwd . vel| / speed) * speed   (+0xb0c)
        float dot = Fb(v, 0x9dc) * Fb(v, 0x9b8)
                  + Fb(v, 0x9d4) * Fb(v, 0x9b0)
                  + Fb(v, 0x9d8) * Fb(v, 0x9b4);
        if (dot < vc::kZero) dot = -dot;
        Fb(v, 0xb0c) = (vc::kOne - dot / Fb(v, 0x9e4)) * Fb(v, 0x9e4);
    }

    // filter the two analog inputs into the per-car scratch (+0xb24 accel, +0xb28 brake)
    Ib(v, 0xb24) = (input[0] == 0) ? 0 : Vc_InputFilter();
    Ib(v, 0xb28) = (input[1] == 0) ? 0 : Vc_InputFilter();

    const unsigned phase = static_cast<unsigned>(g_torqueRingPhase) & 0xf;
    float force = vc::kZero;

    if (input[0] != 0) {                                             // accelerate
        if (vc::kAirSpeed < Fb(v, 0x9e4)) Ib(v, 0xb20) = 1;
        force = static_cast<float>(input[0]) * Fb(v, 0x190) * vc::kInputScale;
        if (gameMode == 7) force = vc::kZero;
        force *= vc::kHalf;
        if (Ib(v, 0xbf0) != 0) force *= vc::kBoostMul;               // boost gate
        if (static_cast<float>(input[5]) <= vc::kInput5Thr) {
            float f = static_cast<float>(Ib(v, 0xb24));
            if (vc::kFilterClamp < f) f = vc::kFilterClamp;
            force = (f + vc::kFilterClamp) * force * vc::kGripMul;
        } else {
            force *= vc::kElseMul;
        }
        Fb(v, 0x1a8) = force;
        Fb(v, 0x26c) = force;
    }
    Fb(v, 0x1ac + phase * 4) = force;                               // drive-torque ring
    Fb(v, 0x270 + phase * 4) = force;                               // angular-torque ring

    if (input[1] != 0) {                                            // brake / reverse
        if (vc::kAirSpeed < Fb(v, 0x9e4)) Ib(v, 0xb20) = 1;
        force = static_cast<float>(input[1]) * Fb(v, 0x190) * vc::kInputScale;
        if (gameMode == 7) force = vc::kZero;
        force *= vc::kHalf;
        if (Ib(v, 0xbf0) != 0) force *= vc::kBoostMul;
        if (static_cast<float>(input[5]) <= vc::kInput5Thr) {
            float f = static_cast<float>(Ib(v, 0xb28));
            if (vc::kFilterClamp < f) f = vc::kFilterClamp;
            force = (f + vc::kFilterClamp) * force * vc::kGripMul;
        } else {
            force *= vc::kElseMul;
        }
        force = -force;
        Fb(v, 0x1a8) = force;
        Fb(v, 0x26c) = force;
        Fb(v, 0x1ac + phase * 4) = force;
        Fb(v, 0x270 + phase * 4) = force;
    }

    // the per-frame integration chain (callee arg binding finalized at A8)
    // A5 transforms the body-forward/wheel axes by the vehicle WORLD matrix. The
    // original passes A4's param_4 (the xform) here, NOT iVar1/wheelBlock:
    //   FUN_00470670: FUN_0046ddb0(param_2 /*dt*/, iVar1 /*wheelBlock*/, param_4 /*xform*/)
    // (decomp 0x004708.., Ghidra pool11 2026-06-17). The standalone supplies a
    // yaw world-rotation matrix as `xform` (StepPlayer) so forward = M*(0,0,1) is
    // the car's real heading; passing the zeroed +0x928 wheelBlock zeroed it -> no
    // drive direction -> no motion (root cause, WS-A-VERIFY-3).
    VehicleWheelForceIntegrate(self, dt, xform);                  // A5 0x0046ddb0 (ported)
    Vehicle_Integrate2(self, 0, dt, wheelBlock, input);            // A6a 0x00467650 (param_1=0 [UNCERTAIN])
    Vehicle_AeroStabilize(self, nullptr, dt);                    // A6b 0x00468980 (orient bound at A8)

    if (Ib(v, 0x9f0) == 2) {                                       // parked/stopped state
        Fb(v, 0x9b0) *= vc::kParkedDamp;
        Fb(v, 0x9b4) *= vc::kParkedDamp;
        Fb(v, 0x9b8) *= vc::kParkedDamp;
    }
}

} // namespace Vehicle
} // namespace mashed_re
