// Mashed RE — WS-A5: the per-wheel force integrator FUN_0046ddb0.
//
// Constants + helpers + dependency surface for the verbatim port (ForceIntegrator.cpp).
// Anchored to MASHED.exe BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
// (Ghidra pool3, read_only, 2026-06-16). Every value read from the binary this
// session. Map: re/analysis/WSA5_FORCE_INTEGRATOR_MAP.md
#pragma once

#include <cstdint>
#include <cmath>
#include <cstring>

namespace mashed_re {
namespace Vehicle {

// ---- vehicle-record field views (int* base = the 0xd04 record, unaff_EDI) ----
inline float&  vF (int* v, int i)       { return *reinterpret_cast<float*>(v + i); }
inline float*  vFP(int* v, int i)       { return  reinterpret_cast<float*>(v + i); }
inline int     asI(float f)             { int i; std::memcpy(&i, &f, 4); return i; }
inline float   asFb(int b)              { float f; std::memcpy(&f, &b, 4); return f; }

// ---- tuning constants (raw value @ address — all MCP-read this session) -------
namespace fi {
constexpr float kZero      = 0.0f;        // DAT_005d757c
constexpr float kOne       = 1.0f;        // _DAT_005cc320
constexpr float kHalf      = 0.5f;        // _DAT_005cc32c
constexpr float kSpinScale = 0.01f;       // _DAT_005cc328
constexpr float kQuarter   = 0.25f;       // _DAT_005cc564
constexpr float k0p6       = 0.6f;        // _DAT_005cc318
constexpr float k0p05      = 0.05f;       // _DAT_00613138
// [U-9147 2026-09-29] EIGHT constants in this header were 6-significant-digit DECIMALS of
// the `_DAT_` they cite, not the value the original holds. Read straight out of
// `original/MASHED.exe` (`.rdata`, PE mapping from `re/tools/disasm_va.py`): every one of
// the eight originals is an exact round number — 1/3000, 0.99, 1/3, 1e-4, 2^-31, 5e-6,
// 1/300, -1/30000 — and every declared decimal was low by 1e-6 to 1e-3 relative. The two
// that matter are `kDt` (-1.96e-4 relative; it scales the whole drive-drag term at
// `ForceIntegrator.cpp:90`) and `kSteerOut` (-1.00e-3; all four steer-scratch writes).
// They are now written with `asFb(bits)`, the EXACT-bits idiom this header already uses for
// `k3000` and `kAngScale`, so the literal cannot drift from the comment again. The `.asi`
// C4 copy bit-pins the same four it covers (`PhysicsChainHooks.cpp:127-139`).
const     float kDt        = asFb(0x39aec33e);  // _DAT_005cc948 = 1/3000   substep ms->s
const     float kDraftDot  = asFb(0x3f7d70a4);  // _DAT_005cc9b4 = 0.99     draft cos thresh
constexpr float kRubberThr = 7.0f;        // _DAT_005cc9b8 (rubber-band activate)
const     float kThird     = asFb(0x3eaaaaab);  // _DAT_005ccac8 = 1/3
const     float k3000      = asFb(0x453b8000);  // _DAT_005ccd08 = 3000.0 (EXACT bits)
constexpr float k20        = 20.0f;       // _DAT_005ccd6c
const     float kSpeedMin  = asFb(0x38d1b717);  // _DAT_005cd03c = 1e-4
constexpr float kDraft0p125= 0.125f;      // _DAT_005cd050
constexpr float kDraftDist = 6.0f;        // _DAT_005cd0a0
const     float kPrngScale = asFb(0x30000000);  // _DAT_005cd314 = 2^-31
constexpr float kNeg20     = -20.0f;      // _DAT_005cd61c
constexpr float kSteerProj = 0.002f;      // _DAT_005ce018 (0x3b03126f)
constexpr float kAirThr    = 65536.0f;    // _DAT_005cea64 (airborne vel threshold)
const     float kRandScale = asFb(0x36a7c5ac);  // _DAT_005cea68 = 5e-6
const     float kSteerOut  = asFb(0x3b5a740e);  // _DAT_005cea6c = 1/300
constexpr float kGripCntNeg= -0.15f;      // _DAT_005cea70
const     float kGripRampK = asFb(0xb80bcf65);  // _DAT_005cea74 = -1/30000
constexpr float kGripRampLo= -30000.0f;   // _DAT_005cea78
constexpr float kGripRampHi= 60000.0f;    // _DAT_005cea7c
constexpr float k2         = 2.0f;        // _DAT_005cc574
constexpr float k3         = 3.0f;        // _DAT_005cc31c
const     float kAngScale  = asFb(0x3727c5ac);  // _DAT_005cc990 ~= 9.982e-6 (EXACT bits)
constexpr float kRubberA   = 0.6f;        // _DAT_005cc318 (rubber grip coeff, == k0p6)
// surface jitter coefficients (selected by integer surface key)
constexpr float kJit0p25 = 0.25f;  // 0x3e800000 default
constexpr float kJit0p1  = 0.1f;   // 0x3dcccccd
constexpr float kJit0p01 = 0.01f;  // 0x3c23d70a
constexpr float kJit0p2  = 0.2f;   // 0x3e4ccccd
// surface keys (integer bit patterns compared via CMP EAX,imm in the original)
constexpr int   kSurfRandom = (int)0xffff32ff;
constexpr int   kSurf0p1    = (int)0xffa08080;
constexpr int   kSurf0p01   = (int)0xffaa8080;
constexpr int   kSurfSlipA  = (int)0xff961e5a;
constexpr int   kSurfSlipB  = (int)0xff1e80b4;
constexpr int   kSurf0p2    = (int)0xffc81e5a;
// local axes transformed by the vehicle matrix
// DAT_006146fc = (0,1,0) up;  DAT_00614708 = (0,0,1) forward
}  // namespace fi

// ---- leaf vec math (faithful; bit-identity residual vs FastSqrt FUN_004c3b30) -
inline float FiSqrt(float x) { return std::sqrt(x); }
inline float Vec3Mag3(const float* p) { return FiSqrt(p[0]*p[0]+p[1]*p[1]+p[2]*p[2]); }
inline void  Vec3Norm3(float* dst, const float* src) {
    float m2 = src[0]*src[0]+src[1]*src[1]+src[2]*src[2];
    if (m2 <= 0.0f) { dst[0]=src[0]; dst[1]=src[1]; dst[2]=src[2]; return; }
    float inv = 1.0f / FiSqrt(m2);
    dst[0]=src[0]*inv; dst[1]=src[1]*inv; dst[2]=src[2]*inv;
}

// ---- residual engine deps (stubbed in ForceIntegratorStubs.cpp; bind in B4) ---
void  Rw_TransformPoints(float* dst, const float* src, int count, void* mtx);   // FUN_004c3df0
void  Rw_MatrixFromAxisAngle(void* outMtx, const float* axis, float angle, int);// FUN_004c4d20
float Fi_RandRange(float lo, float hi);                                         // FUN_00472650 (+FUN_00534870 PRNG)
int   Fi_GameMode();                                                            // FUN_0040e350
void  Fi_GameModeTick();                                                        // FUN_0040e340

// ---- runtime globals the integrator reads (provided by B4 wiring) ------------
extern int   g_playerCount;     // DAT_007f0fd0
extern int   g_raceTimer;       // DAT_007f0ff8
extern float g_gravScale;       // _DAT_00803340
extern float g_gravX, g_gravY, g_gravZ;  // _DAT_00803334/38/3c
extern float g_suspDtTerm;      // _DAT_0088e610 (gravity*dt per frame)
extern float g_suspScale;       // _DAT_0088e5f0
// [U-9147 2026-09-29] _DAT_00613108, the steer-torque scale A3 seeds to 100.0 and the
// handling-override table then rewrites. MEASURED at 105.0 in the running original on
// the reference scenario (nine `scenario_launch.py --peek 00613108:f` samples over 30 s).
// Written by VehicleInit (A3 0x0046b540), read by BodyOrient_OmegaFromSteer. Was a
// hardcoded 100.0f at BodyOrientationIntegrate.cpp:218, which made the port's body yaw
// rate a flat 100/105 of the original's.
extern float g_handlingTorque;  // _DAT_00613108
// [U-9151 2026-09-29] DIAG ONLY. The matrix pointer VehicleWheelForceIntegrate (A5,
// 0x0046ddb0) was handed on this frame, recorded so the exe's A6b witness can check the
// original's invariant that A5 and A6b get the SAME pointer (A4 computes it once at
// 0x00470699 and passes it at 0x00470918 and 0x0047093b). Not read by any law.
extern float* g_a6bA5Matrix;
extern float g_a8WheelLe4[4];   // [A8-ORIENT] per-wheel le4 from Integrate2 block #4 (diag only)
extern float g_a8WheelLd4[4];   // [A8-ORIENT] per-wheel ld4 from Integrate2 block #4 (diag only)

// [U-9147 2026-09-29] A6a block-#4 per-wheel capture, DIAG ONLY, written by
// Integrate2.cpp block #4 (0x00467650) on every call, printed for slot 0 only by
// VehiclePhysicsRun.cpp under MASHED_A6ADUMP. It exists because the previous
// cross-side lateral-coefficient comparison (re/tools/statediff/a8_wheelfit.py) was
// unsound in BOTH modes -- it rebuilt the port's `lat` from a 2-D velocity heading
// instead of reading A6a's own vector -- and was withdrawn
// (re/analysis/D2_REOPEN_2026-09-29.md §6.0 third correction). These are the actual
// values A6a computed, so a replay of the law can be validated against them before
// the same replay is trusted on the original's .msd record fields.
struct A6aWheelDump {
    float off[3];      // p[-9..-7]     wheel offset            (b-0x24/-0x20/-0x1c)
    float ax[3];       // p[0x1f..0x21] wheel forward axis       (b+0x7c/+0x80/+0x84)
    float p15, p16, p1b;  // b+0x54, b+0x58, b+0x6c
    int   pm1;         // p[-1]         b-0x04 (the (l94 & 0x100) gate)
    float le[3];       // the wheel-point relative velocity, Integrate2.cpp:412/415
    float le4raw;      // |le| BEFORE the 1024 cap
    float le4;         // le4 AFTER the cap (what lbc consumes)
    float lat[3];      // lac/la8/la4, Integrate2.cpp:427
    float ld4;         // |lat|,       Integrate2.cpp:428
    float lbc;         // Integrate2.cpp:424
    float f5;          // the APPLIED lateral scale, :442/:447/:452
    float dF[3];       // the block-#4 delta actually added to p[0x1c..0x1e]
    int   spin;        // 1 = the Integrate2.cpp:404 spin branch was taken
    int   fired;       // 1 = block #4 ran (:418 gate passed) this call
};
extern A6aWheelDump g_a6aDump[4];
// frame-level A6a inputs, captured at A6a ENTRY (before its tail touches +0x9b0..)
struct A6aFrameDump {
    float vel[3], av[3], bf[3];   // +0x9b0.., +0x9bc.., +0x9d4..
    float sp, angSp, gc;          // +0x9e4, +0x9e8, +0x9e0
    float suspScale;              // g_suspScale as block #4 read it
    // trailing grip-clamp #6 (Integrate2.cpp:635-722): the DIRECT slip suppressor.
    // It removes `lateral_velocity * kVel` from +0x9b0.. and scales +0x9bc.. by kAv.
    double l60;                   // sum(ld4*le4) over the wheels block #4 fired on
    float  m18c;                  // +0x18c, the l_60 divisor at :635
    float  speed;                 // the speed :633 recomputed and :657 multiplies by
    float  grip;                  // AFTER the track/modifier scaling and the *speed
    float  kVel, kAv;             // the applied bleed and the angular scale (1-k)
    int    arm;                   // 1 = the `32768 < grip` hi arm, 0 = lo
    int    clampRan;              // the :652 gate passed
};
extern A6aFrameDump g_a6aFrame;
extern float g_rubberBand[16];  // DAT_008989b0 (per-player catch-up float)
extern int   g_rubberRefCar;    // DAT_008989c8
extern int*  g_vehicleArrayBase;// DAT_008815a0 (16-car array; other cars' contact summary + drafting)
#ifdef MASHED_STANDALONE
// [U-9174] start-line launch rev-charge wiring — Vehicle\LaunchRevCharge.cpp.
// The standalone equivalent of FUN_004103a0's two launch loops (0x00410441 charge,
// 0x0041049b release). Called from TrackRenderer's countdown branch, slot 0 only.
// Standalone binding for the original's cooked accel byte at 0x007f103c+ctrl*0x4c.
extern unsigned char g_launchAccelByte[16];
void LaunchRev_PreRaceTick(float dt, float accel01);
void LaunchRev_Release();
#endif
// DAT_00881560 (per-wheel suspension/steer scratch) lives in the Collision module
// (Collision::g_suspScratch) since the wheel solver writes it; ForceIntegrator.cpp
// references it qualified.

// The per-car contact summary the integrator consumes (B2/B3 output): the 4-car
// {active@+4, count@+8} fields at the vehicle base DAT_008815a0 (stride 0xd04).
// In the standalone these live in the vehicle records themselves; B4 ensures the
// contact path fills them before this runs.

// 0x0046ddb0 — the force integrator. `self` = vehicle record; dt = param_1;
// xform = the vehicle's world transform matrix (param_2).
void VehicleWheelForceIntegrate(int* self, float dt, void* xform);

float RubberBandGrip(int car, float current, float band);   // 0x00442ce0
int   RubberBandGate(int car);                              // 0x00442c80
int   CarContactCount(int car);                             // 0x0046dbe0

}  // namespace Vehicle
}  // namespace mashed_re
