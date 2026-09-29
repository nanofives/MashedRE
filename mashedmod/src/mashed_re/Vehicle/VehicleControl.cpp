// Mashed RE — WS-A4: the per-vehicle control-input integrator FUN_00470670.
//
// Verbatim port of the control half of the per-frame vehicle update. Reads the
// per-car input descriptor's STEER bytes ([0]/[1] = sign A / sign B), scales them
// into a front-wheel steer angle, writes it to the wheel-0/wheel-1 steer slots
// (+0x1a8/+0x26c) and their 16-slot phase rings (+0x1ac/+0x270), then dispatches
// the three integration callees and applies the parked-state velocity damp.
//
// CORRECTED 2026-08-24: this header previously said "accel/brake bytes" and
// "speed-normalized drive/reverse torque". Both are the mislabel called out at
// VehiclePhysicsRun.h:26. Accel/brake are input[4]/[5] and are read by A6a, not
// here. See the descriptor map on VehicleControlIntegrate below.
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
// A6b 0x00468980 — aerodynamic stabilization (ported: AeroStabilize.cpp).
// orient = the RwMatrix the original loads into ESI at 0x0047093b, which is the SAME
// pointer it gives A5 as A5's second argument (both read stack slot E+0x0c). Bound
// 2026-09-29 (U-9149); it was `nullptr`, i.e. dead. See the call site below.
void Vehicle_AeroStabilize(int* self, float* orient, float dt);
// FUN_004a2c48 — per-input smoother/accumulator [UNCERTAIN signature] (pending).
int  Vc_InputFilter();

// ===========================================================================
// 0x00470670  VehicleControlIntegrate(self, dt, input[], xform)
//   self    = the 0xd04 vehicle record (original: in_EAX)
//   dt      = substep delta (original: param_2 / param_4 == &dt)
//   input   = per-car input descriptor. CORRECTED 2026-08-24 — this line previously
//             read "[0]=accel, [1]=brake/reverse", which is the mislabel
//             VehiclePhysicsRun.h:26 calls out. The real map:
//               [0]/[1] = STEER command, sign A / sign B, 0..255 magnitude.
//                         VehiclePhysicsRun.cpp:404-405 sets them from io.steer
//                         (+steer -> [0], -steer -> [1]); the original writers are
//                         the AI FUN_00416250 and the human cook FUN_00496530.
//               [4]/[5] = accelerator / brake-reverse. A4 does NOT read these; A6a
//                         FUN_00467650 does (PhysicsChainHooks.cpp:1880, 1976).
//             So the two branches below are steer-right and steer-left, NOT
//             accelerate and brake. The ARITHMETIC IS UNAFFECTED (it is verbatim
//             either way) — only the names were wrong. [5] is still read here as the
//             grip-branch threshold at line ~112, which is a separate use.
//   xform   = vehicle world-transform context (original: param_4, to A5)
// Verbatim of FUN_00470670 (body 0x00470670..0x00470991).
// ===========================================================================
void VehicleControlIntegrate(int* self, float dt, std::uint8_t* input, void* xform, int car)
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

    // STEER HOLD-DURATION ACCUMULATORS +0xb24 / +0xb28 — real law, 2026-08-25.
    // These were `Vc_InputFilter()`, our stub for FUN_004a2c48, which returns 0 —
    // so both counters read 0 on every frame and the `(f + 6000)` term below was
    // stuck at its floor. FUN_004a2c48 is the MSVC CRT helper `_ftol2`, so each
    // site is just an int cast of the float expression built before the CALL:
    //   00470737  FILD dword [EDI+0xb24]   ; ST0 = (float)(int)counter
    //   0047073d  FADD float [ESP+0x1c]    ; + param_2 (dt)
    //   00470741  CALL 0x004a2c48          ; (int)(counter + dt)
    //   00470746  MOV  [EDI+0xb24],EAX
    // and the mirrored +0xb28 arm at 0x00470759/0x0047075f/0x00470763. Reset to 0
    // when the corresponding input byte is 0 (JZ at 0x00470735 / 0x00470757).
    // So these are "how long this steer input has been held", in budget units —
    // NOT filtered analog values, which is what the old stub name implied.
    // Decode: re/analysis/data/A8_ftol_gearbox_timer_20260825.md (Q4).
    // Runtime check against the original: +0xb24 is nonzero on 1430 of 1441 driving
    // frames (0..254, 128 distinct); +0xb28 is 0 throughout that capture because it
    // holds RIGHT lock only, i.e. input[1] == 0 — which matches this reset rule.
    Ib(v, 0xb24) = (input[0] == 0) ? 0 : (int)((float)Ib(v, 0xb24) + dt);
    Ib(v, 0xb28) = (input[1] == 0) ? 0 : (int)((float)Ib(v, 0xb28) + dt);

    const unsigned phase = static_cast<unsigned>(g_torqueRingPhase) & 0xf;
    float force = vc::kZero;

    if (input[0] != 0) {                                             // steer sign A (was mislabelled "accelerate")
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

    if (input[1] != 0) {                        // steer sign B (was mislabelled "brake / reverse")
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

    // the per-frame integration chain.
    //
    // A5's MATRIX ARGUMENT — CORRECTED 2026-09-29 (U-9149 decode). This block used
    // to read "the original passes A4's param_4 (the xform) here, NOT iVar1/
    // wheelBlock". That is wrong about which argument A5 CONSUMES. The original
    // pushes all three (asm 0x00470914..0x00470923, anchored MASHED.exe.unpatched):
    //   00470914  MOV ECX,[ESP+0x24]   ; = E+0x10 = A4 param_4
    //   00470918  MOV EDX,[ESP+0x20]   ; = E+0x0c = wheelBlock (set at 0x004706a2)
    //   0047091c  MOV EBX,[ESP+0x1c]   ; = E+0x08 = dt
    //   00470920  PUSH ECX / PUSH EDX / PUSH EBX / CALL 0x46ddb0
    // so cdecl A5(dt, wheelBlock, param_4). Inside A5 (balanced ESP walk from
    // 0x0046ddb0 to its RET at 0x0046e9d0) the caller-frame references are:
    //   E5+0x04 (dt)        read 6x: 0x0046df2a 0x0046e2dc 0x0046e3d1/e0/eb 0x0046e92a
    //   E5+0x08 (wheelBlock) read 2x: 0x0046ddb0 0x0046de01 — and BOTH feed the
    //                        matrix argument of RwV3dTransformVectors
    //                        (0x0046ddc9, 0x0046de1f)
    //   E5+0x0c (param_4)   read ZERO times.
    // So the original's transform matrix is the record's +0x928 RwMatrix, and A4's
    // param_4 is dead inside A5.
    //
    // We nonetheless keep passing `xform` here, deliberately: the port does NOT keep
    // the body orientation in +0x928 (that block is our contact ring — see
    // BodyOrientationIntegrate.cpp's BodyOrient_Init comment and VehiclePhysicsRun.cpp:128).
    // `xform` IS g_bodyBasis[slot], the port's stand-in for the original's +0x928
    // matrix. Passing the zeroed +0x928 instead zeroed forward -> no motion
    // (WS-A-VERIFY-3). Reconciling the two storage locations is [UNCERTAIN U-9152].
    VehicleWheelForceIntegrate(self, dt, xform);                  // A5 0x0046ddb0 (ported)
    // A6a 0x00467650. param_1 is the CAR INDEX, decoded 2026-09-29 (was passed 0 with an
    // [UNCERTAIN] note): the dispatcher FUN_00470c70 calls A4 as
    // `FUN_00470670(iVar6, fVar4, puVar18, param_2)` at 0x00471071 where iVar6 is the
    // 0..0xf slot counter of its per-vehicle loop, and A4 forwards its own param_1
    // unchanged as A6a's first argument (0x0047094c `FUN_00467650(param_1, param_2,
    // iVar1, param_3)`). A6a's only use of it is the boost-state-1 comparison against
    // DAT_0088e668 / DAT_0088e66c (0x00467d62 / 0x00467d6a), which are car indices.
    Vehicle_Integrate2(self, car, dt, wheelBlock, input);
    // A6b 0x00468980. U-9149 FIXED 2026-09-29: this was `nullptr`, which made BOTH
    // `if (orient)` legs of AeroStabilize.cpp dead in the shipping exe — airborne
    // auto-level (pitch+roll) and the velocity-align rotation never ran.
    // A6b takes its matrix in ESI, and the original loads it from the SAME STACK
    // SLOT it gave A5 as A5's second argument:
    //   004706a2  MOV [ESP+0x20],ECX   ; ESP=E-0x14 -> E+0x0c  := wheelBlock,
    //                                  ;   ECX = LEA [EAX+EDI+0x928] @0x00470699,
    //                                  ;   EAX = [EDI+0x9a8] << 6 (0x0047068e/96)
    //   00470918  MOV EDX,[ESP+0x20]   ; ESP=E-0x14 -> E+0x0c   (A5 arg 2)
    //   0047093b  MOV ESI,[ESP+0x3c]   ; ESP=E-0x30 -> E+0x0c   (A6b's ESI)
    // E+0x0c is A4's param_3 SLOT, reused as a local at 0x004706a2 — which is why
    // reading it as "param_3/input" or as "param_4/xform" both mis-identify it.
    // A6b then hands ESI straight to RwMatrixRotate (0x00468a31, 0x00468aaf,
    // 0x00468b29 -> 0x004c4d20) and reads at.y = [ESI+0x24] and right.y = [ESI+4],
    // so ESI is an RwMatrix, and 0x40 is exactly sizeof(RwMatrix) — matching A3's
    // corrected stride. Frame proof: 5 prologue pushes (0x00470670..0x00470678),
    // no SUB ESP, and ADD ESP,0x24 at 0x0047094e == A5's 3 + A6a's 4 + A6b's 2
    // pushed args (9 dwords), then 5 pops -> balanced. (U-9149's "there is a SUB ESP
    // or further pushes unaccounted for" is resolved: there is neither.)
    // Binding: in the original A5's matrix and A6b's ESI are LITERALLY the same
    // pointer, so whatever object plays A5's matrix role must also play A6b's. In
    // this build that object would be `xform` (g_bodyBasis[slot]) — see the A5 note.
    //
    // STILL nullptr HERE, AND THAT IS NOW A MEASURED BLOCKER, NOT AN OVERSIGHT
    // [UNCERTAIN U-9151]. Passing `xform` was tried 2026-09-29 and CRASHES the
    // standalone: exit code 0xC0000005 at ~3 s of the a8 solo recipe, 3 runs of 3
    // (verify/d2_reopen_20260929/solo_a6bfix{1,2,3}/PROVENANCE.txt). Cause, not
    // guessed — read from the code path: A6b's entire effect is two
    // `RwMatrixRotate(orient, axis, ang, 1)` calls (AeroStabilize.cpp), and mode 1
    // (rwCOMBINEPRECONCAT) in Math/RwMatrixRotateInner.cpp:169-176 dereferences the
    // RW DEVICE table at the absolute MASHED addresses 0x007d4028 and 0x007d3ff8 and
    // then calls the device matrix-mult through `devBase + 8`. Those addresses are
    // mapped in the injected .asi and NOT in mashed_re.exe. That file's own header
    // says so ("Modes 1/2 are thus device-state dependent ... in the standalone they
    // require RW device init"); only mode 0 is self-contained. The unblocker is a
    // standalone CPU port of RwMatrixMultiply 0x004c4600, which does not exist —
    // today it is only a function pointer into the original image (HUD/FontCtx.cpp:42,
    // C1). This is the same RVA-tunnel class as the kDegToRad fix in
    // Math/RwMatrixRotate.cpp and the U-9138 device-slot rebind.
    //
    // Scope note so nobody re-opens this expecting a D2 metric move: A6b CANNOT
    // affect the D2 slip statistics either way. It returns immediately unless
    // +0x9e0 == 0 (AeroStabilize.cpp, from 0x00468980/0x00468994), and
    // re/tools/statediff/a8_slip_axis.py:35 scores only frames with +0x9e0 >= 3.5.
    // The two conditions are disjoint. The .asi copy's dispatch IS fixed and verified
    // (Call_A6b, 144 samples incl. 64 airborne bit-identical) — see
    // re/analysis/D2_REOPEN_2026-09-29.md §4.
    Vehicle_AeroStabilize(self, nullptr, dt);                    // A6b 0x00468980

    if (Ib(v, 0x9f0) == 2) {                                       // parked/stopped state
        Fb(v, 0x9b0) *= vc::kParkedDamp;
        Fb(v, 0x9b4) *= vc::kParkedDamp;
        Fb(v, 0x9b8) *= vc::kParkedDamp;
    }
}

} // namespace Vehicle
} // namespace mashed_re
