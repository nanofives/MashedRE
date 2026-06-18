// Mashed RE — WS-C-STANDALONE: opponent-AI reimplementation for mashed_re.exe.
// See AiStandalone.h. PENDING diff-original C4. NO-GUESSING; anchor SHA-256
// BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E.
//
// Source decompilations (Ghidra pool11, read-only, 2026-06-16):
//   FUN_00418860 AiTickLoop      / FUN_00418560 AiVehicleStep  (Ai/AiController.cpp)
//   FUN_00415e20 steering-error  / FUN_004161e0 target-seed    / FUN_00443dc0 lookahead
//
// ===========================================================================
// CALLEE PORT LEDGER (standalone)
//   DONE (faithful):   tick spine, per-vehicle bank-select dispatch, nearest-point
//                      search (FUN_00443dc0 loop 1), target seed (FUN_004161e0).
//   DONE (faithful):   steering-angle FUN_00415e20 — [U-C-STEER] RESOLVED 2026-06-16
//                      (pool11): the clamp consts 0x005cd0c8/0x005cd0d0 are DOUBLES
//                      +1.0/-1.0 (acos-domain clamp to [-1,+1]) and the scale
//                      0x005cc970 is the DOUBLE 0x404ca5dc20000000 ~= 57.29578
//                      (rad->deg) — the prior 0/0/2^-63 were low-dword misreads.
//                      Steering output is now trustworthy (modulo final diff).
//   IDENTIFIED:        FUN_004a2c48 = ROUND(ST0) (round the FPU value to int — the
//                      ctrl-byte quantizer); steer split _DAT_005cd09c = 180.0.
//                      FUN_00416250 full decomp mapped (targeting modes 1..10).
//   DONE (WS-C-AITREE 2026-06-17): FUN_00416250 primary control step — the steer
//                      bands + anti-oscillation timer state machine + accel/brake
//                      bands + mode-7/9 tails + game-mode gate, verbatim from asm
//                      (ControlStep). [U-C-BANDS] CLOSED (consts 005cd0e8/ec/04c,
//                      005cc9a0, 005cd09c/0b8/0d8/dc/e0/e4, 005cc9b0/72c/55c all
//                      memory_read pool11). Residual: [U-C-STEER-MAG] exact ESP-slot
//                      / FPU-rounding of the ROUND(ST0) magnitude; [U-C-RATE0/1] the
//                      two rate floats FUN_0046d6a0/6d0 (speed substituted; rate1=0).
//   STUBBED (RVA TODO, port in WS-C follow-ups):
//     FUN_00414570/00415880/00414a70/00414c30/004150e0/00414f00/004148b0/00415020
//                  targeting + LOS helpers (modes 1..10) FUN_00416060 LOS  FUN_00415d00 wall
//     FUN_00416a30 / FUN_00417da0      control-step variants (modes 4/9 / 8)
//     FUN_004177b0 pre-tick rubber-banding  FUN_00417180 bank-switch timer/RNG
//     FUN_00417640 powerup-brake  FUN_00415220 powerup activation  FUN_00417cf0 mode-8
//     FUN_00443300 spline interpolation + FUN_00443dc0 curvature-walk/wall-march tail
//     .AI parser (populates the 0x00801aa0 spline arrays + 0x007f1a9c tile grid)
//   With targeting helpers stubbed, `mode`=0 (race-line follow); the REAL bands now
//   drive steering/throttle toward the seed (was a crude turn-rate-2.4 placeholder).
// ===========================================================================
#include "AiState.h"
#include "AiStandalone.h"

#include <cstdint>
#include <cmath>
#include <cstring>

namespace Ai {

namespace {

// ---- host (no-op defaults so the module is inert until WS-C-WIRE binds it) ---
int  h_zero_i()                 { return 0; }
int  h_zero_iv(int)             { return 0; }
void h_zero_xz(int, float* a, float* b) { *a = 0.0f; *b = 0.0f; }

Host s_host = {
    h_zero_i, h_zero_i, h_zero_i, h_zero_i,
    h_zero_iv, h_zero_iv, h_zero_i,
    h_zero_xz, h_zero_xz,
};

inline float bits_to_f(std::uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }

// per-vehicle behaviour record field address (base + v*0x74; AiState.h field bases).
inline std::uintptr_t st_field(std::uintptr_t field, int v) {
    return field + static_cast<std::uintptr_t>(v) * 0x74u;
}

// ---- FUN_00415e20 constants (memory_read 2026-06-16; [U-C-STEER] flagged) ----
const float kSteerClampLo = -1.0f;                  // _DAT_005cc33c (0xbf800000)
const float kSteerClampHi =  1.0f;                  // _DAT_005cc320 (0x3f800000)
// [U-C-STEER] RESOLVED 2026-06-16 (pool11): 0x005cd0c8/0x005cd0d0 are DOUBLES, not
// floats. 0x005cd0c8 = 0x3ff0000000000000 = +1.0; 0x005cd0d0 = 0xbff0000000000000 =
// -1.0. The decomp's (float)_DAT_... cast = +1.0/-1.0: a standard acos-domain clamp of
// the normalized dot to [-1,+1] (the prior 0.0/0.0 were the doubles' low dwords).
const float  kSteerThrLo   = -1.0f;                 // (float)_DAT_005cd0d0 = -1.0 (acos clamp lo)
const float  kSteerThrHi   =  1.0f;                 // (float)_DAT_005cd0c8 = +1.0 (acos clamp hi)
// _DAT_005cc970 is a DOUBLE 0x404ca5dc20000000 ~= 57.29578 (rad->deg), NOT 2^-63 (that
// was the low dword 0x20000000 read as a float). acos()[0,pi] * this -> degrees [0,180].
const double kSteerScale   = 57.2957802;            // _DAT_005cc970 (0x404ca5dc20000000)
const float kWrap         =  360.0f;                // _DAT_005ccac4 (0x43b40000)

// ---------------------------------------------------------------------------
// FUN_00415e20 — signed steering-angle error (bearing-to-target vs heading).
// Verbatim structure; param_2/param_3 = target X/Z. Returns degrees in [0,kWrap].
// own pos via host (orig FUN_0046d4a0 +0x30/+0x38); heading via host velocity
// (orig FUN_0046d510). normalize3 collapses to the X component (the y/z terms are
// multiplied by DAT_005d757c==0 in the original).
// ---------------------------------------------------------------------------
float SteerAngleError(int v, float targetX, float targetZ)
{
    float ownX, ownZ;
    s_host.own_xz(v, &ownX, &ownZ);

    // bearing to target
    float bx = targetX - ownX;
    float bz = -(targetZ - ownZ);          // local_4 = -(param_3 - own.z)
    float bn = std::sqrt(bx * bx + bz * bz);
    float bnx = (bn > 0.0f) ? bx / bn : bx; // FUN_004c39b0 normalize (x kept; *0 terms drop)
    float d = bnx;
    if (d < kSteerThrLo) d = kSteerClampLo; // [U-C-STEER] degenerate-looking clamp, verbatim
    if (kSteerThrHi < d) d = kSteerClampHi;
    float ang = std::acos(d) * kSteerScale; // FUN_004a3384 acos
    if (bz < 0.0f) ang = -ang;              // local_4 - local_c*0 < 0 → negate
    while (ang < 0.0f) ang += kWrap;

    // heading (velocity)
    float vx, vz;
    s_host.own_vel_xz(v, &vx, &vz);
    float hz = -vz;
    float hn = std::sqrt(vx * vx + hz * hz);
    float hnx = (hn > 0.0f) ? vx / hn : vx;
    float dh = hnx;
    if (dh < kSteerThrLo) dh = kSteerClampLo;
    if (kSteerThrHi < dh) dh = kSteerClampHi;
    float head = std::acos(dh) * kSteerScale;
    if (hz < 0.0f) head = -head;
    while (head < 0.0f) head += kWrap;

    float err = ang - head;
    while (err < 0.0f) err += kWrap;
    if (err < 0.0f) err += kWrap;           // (verbatim double-wrap guard)
    while (kWrap < err) err -= kWrap;
    return err;
}

// ---------------------------------------------------------------------------
// FUN_00443dc0 (core, loop 1) — nearest spline point to (px,pz). Faithful.
// spline layout: points = float2 at (spline + i*8); count at (spline + 0x200).
// The curvature-walk refinement + wall-march tail are STUBBED (see ledger).
// Returns nearest index, or -1 if the bank is empty.
// ---------------------------------------------------------------------------
int SplineNearestIndex(std::uintptr_t spline, float px, float pz)
{
    int count = I32(spline + 0x200);
    if (count <= 0) return -1;
    int best = 0;
    float bestD = 1.0e9f;                    // orig seed 100000.0
    for (int i = 0; i < count; ++i) {
        float dx = F32(spline + i * 8)     - px;
        float dz = F32(spline + i * 8 + 4) - pz;
        float d  = dx * dx + dz * dz;
        if (d < bestD) { bestD = d; best = i; }
    }
    return best;
}

// mode-0 follow lookahead: BOUNDED raw-point advance (the FUN_00443dc0 curvature
// walk + FUN_00443300 interpolation refinement are TODO). Advance kLookahead raw
// points from nearest. [U-C-BANDS / lookahead-refine pending].
const int kLookahead = 1;   // [G4] closer target -> small bearing error -> the ControlStep
                            // "mildly off" full-steer band (err 30..180) rarely fires, so the
                            // car drives FORWARD along the line instead of orbiting a far point.

void TargetPoint(std::uintptr_t spline, int nearestIdx, float* tx, float* tz)
{
    int count = I32(spline + 0x200);
    int idx = (nearestIdx + kLookahead) % count;
    *tx = F32(spline + idx * 8);
    *tz = F32(spline + idx * 8 + 4);
}

// ===========================================================================
// FUN_00416250 — primary per-vehicle control step (behaviour tree + ctrl bands).
// [U-C-BANDS] CLOSED 2026-06-17 (pool11): the real steer/accel/brake band logic +
// the per-vehicle anti-oscillation timer state machine, verbatim from the asm
// (0x004165c0.. listing). Targeting helpers (modes 1..10) are STUBBED → return 0,
// so `mode` stays 0 (race-line follow) and the real bands steer toward the seed.
//
// Band constants (memory_read pool11 2026-06-17; addr cited):
const float kSteerDeadband = 1.0f;       // _DAT_005cc320  (err>1° before steering)
const float kSteerSplit    = 180.0f;     // _DAT_005cd09c  (err<180 one way, >180 other)
const float kSteerMagScale = 0.0030034f; // _DAT_005cd0e8  fresh: err*speed*this
const float kSteerExtra    = 0.05f;      // _DAT_005cc9a0  *this when rate1<=20
const float kSteerMagClamp = 255.0f;     // _DAT_005cd04c  steer-byte clamp (max)
const float kCounterScale  = 0.005f;     // _DAT_005cd0ec  counter: (200-el)*this*stored
const int   kSettleFrames  = 200;        // CMP 0xc8       anti-oscillation window
const float kBrakeSpeedDel = 15.0f;      // _DAT_005cc9b0
const float kBrakeMinSpeed = 10.0f;      // _DAT_005cc55c
const float kAccelErrLo    = 30.0f;      // _DAT_005cc72c
const float kAccelErrHi    = 330.0f;     // _DAT_005cd0e0
const float kSteer359      = 359.0f;     // _DAT_005cd0e4  (err>180 active < this)
const float k20f           = 20.0f;      // _DAT_005ccd6c
const float kRate1Brake    = 2000.0f;    // _DAT_005cd0b8
const float kMode9BrakeA   = 1750.0f;    // _DAT_005cd0dc
const float kMode9BrakeB   = 1250.0f;    // _DAT_005cd0d8
const float kZeroF         = 0.0f;       // DAT_005d757c
// per-vehicle state arrays (ORIGINAL image-pad addresses; writable in the exe):
//   stride 0x14: 0x008032d8 hist(other) / 0x008032dc hist(this) / 0x008032e0 prevSpeed
//   stride 0x74: 0x0089a4ec timerState / 0x0089a4f0 timerStart / 0x0089a4f4 storedSteer
//                0x0089a52c behaviour mode
//   0x007f0ff4 frame counter (host-ticked) ; 0x007f0ff8 race timer
inline std::uintptr_t a14(std::uintptr_t b, int v){ return b + (std::uintptr_t)v*0x14u; }
inline std::uintptr_t a74(std::uintptr_t b, int v){ return b + (std::uintptr_t)v*0x74u; }
// FUN_004a2c48 = ROUND(ST0)→int; orig stores AL (low byte). [U: FPU rounding mode].
inline std::uint8_t RoundST0(float x){
    long r = std::lround(x);
    if (r < 0) r = 0; if (r > 255) r = 255;
    return (std::uint8_t)r;
}

void ControlStep(std::uintptr_t spline, int v, std::uint8_t* ctrl)
{
    const int gameMode = s_host.game_sub_mode();   // FUN_0040e350 (local_34) — the SUB-mode
    // (race=6), NOT DAT_007f0fd0 (game_mode_fd0). The throttle gate below keeps accel for
    // race-class sub-modes {6,5,9,10,11}; calling game_mode_fd0() here (=0) wrongly gated
    // throttle OFF (ctrl[4]=0 -> AI cars never moved). [G3 fix 2026-06-18]
    float ownX, ownZ;  s_host.own_xz(v, &ownX, &ownZ);
    float vx, vz;      s_host.own_vel_xz(v, &vx, &vz);
    const float speed = std::sqrt(vx*vx + vz*vz);  // local_38 ~ FUN_0046d6a0 [U-C-RATE0]
    const float rate1 = 0.0f;                       // local_3c = FUN_0046d6d0 [U-C-RATE1]

    // --- targeting chain (FUN_00414570/15880/14a70/14c30/150e0/14f00/148b0/15020/
    //     15220 + LOS 00416060 + wall 00415d00). STUBBED → mode 0; target = the
    //     FUN_004161e0 seed (spline lookahead). Port the helpers in a follow-up. ---
    int mode = 0;
    float tx = ownX, tz = ownZ;
    // [G4] MONOTONIC waypoint follower (standalone nav aid; the original's curvature-walk
    // FUN_00443300 / FUN_00443dc0 tail is stubbed). Re-searching the NEAREST spline point
    // each frame let a circling car "stick" near spawn (nearest never advances) so it never
    // drove a full lap. Instead keep a per-car progress index that ONLY moves forward:
    // advance when the car reaches its current waypoint, target a few points ahead for smooth
    // steering, and re-seed to nearest if the car is far off its waypoint (spawn / knocked
    // off). This makes the AI drive the full racing line -> laps + elimination can resolve.
    static int s_prog[4] = {0, 0, 0, 0};
    const int count = I32(spline + 0x200);
    if (count > 1 && v >= 0 && v < 4) {
        // mean inter-point spacing (for the lost-reseed threshold only).
        float total = 0.f;
        for (int k = 0; k < count; ++k) {
            int b = (k + 1) % count;
            float ax = F32(spline + k * 8) - F32(spline + b * 8);
            float az = F32(spline + k * 8 + 4) - F32(spline + b * 8 + 4);
            total += std::sqrt(ax * ax + az * az);
        }
        float meanSp = total / static_cast<float>(count);
        if (meanSp < 1.0f) meanSp = 1.0f;
        int cur = ((s_prog[v] % count) + count) % count;
        int nxt = (cur + 1) % count;
        float cx = F32(spline + cur * 8), cz = F32(spline + cur * 8 + 4);
        float nx = F32(spline + nxt * 8), nz = F32(spline + nxt * 8 + 4);
        float dc = (cx - ownX) * (cx - ownX) + (cz - ownZ) * (cz - ownZ);
        float dn = (nx - ownX) * (nx - ownX) + (nz - ownZ) * (nz - ownZ);
        const float lost = meanSp * 6.0f;
        if (dc > lost * lost) {                  // genuinely lost (spawn/knocked off) -> reseed
            int nrst = SplineNearestIndex(spline, ownX, ownZ);
            if (nrst >= 0) s_prog[v] = nrst;
        } else if (dn <= dc) {                   // closer to the NEXT waypoint -> advance forward
            s_prog[v] = s_prog[v] + 1;           // monotonic; scale-free (no arrive radius)
        }
        int tgt = ((s_prog[v] + kLookahead) % count + count) % count;
        tx = F32(spline + tgt * 8);
        tz = F32(spline + tgt * 8 + 4);
    } else {
        int nearest = SplineNearestIndex(spline, ownX, ownZ);   // fallback
        if (nearest >= 0) TargetPoint(spline, nearest, &tx, &tz);
    }
    I32(a74(0x0089a52cu, v)) = mode;                // commit behaviour mode

    const float err = SteerAngleError(v, tx, tz);  // FUN_00415e20, [0,360)
    const int   frame = I32(0x007f0ff4u);

    // ---- STEER bands (asm 0x004165c0..) : anti-oscillation timer state machine ----
    if (err < kSteerSplit) {                        // steer toward (state 1)
        F32(a14(0x008032d8u, v)) = 360.0f;
        F32(a14(0x008032dcu, v)) = err;
        if (err > kSteerDeadband) {
            const int st = I32(a74(0x0089a4ecu, v));
            const int el = frame - I32(a74(0x0089a4f0u, v));
            if (st == 1 || el > kSettleFrames - 1) { // fresh steer
                float mag = err * speed * kSteerMagScale; // [ESP+0x1c]~speed [U-C-STEER-MAG]
                if (rate1 <= k20f) mag *= kSteerExtra;
                if (mag > kSteerMagClamp) mag = kSteerMagClamp;
                ctrl[0] = RoundST0(mag);
                I32(a74(0x0089a4f4u, v)) = (long)std::lround(mag);
                I32(a74(0x0089a4ecu, v)) = 1;
                I32(a74(0x0089a4f0u, v)) = frame;
            } else {                                 // counter-steer (settling)
                float cs = (float)(kSettleFrames - el) * kCounterScale
                           * (float)I32(a74(0x0089a4f4u, v));
                ctrl[1] = RoundST0(cs);
            }
        }
    }
    if (err > kSteerSplit) {                         // steer the other way (state 2)
        F32(a14(0x008032dcu, v)) = 0.0f;
        F32(a14(0x008032d8u, v)) = err;
        if (err < kSteer359) {
            const int st = I32(a74(0x0089a4ecu, v));
            const int el = frame - I32(a74(0x0089a4f0u, v));
            if (st == 2 || el > kSettleFrames - 1) { // fresh steer (mirror) [U-C-STEER-MAG]
                float mag = err * speed * kSteerMagScale;
                if (rate1 <= k20f) mag *= kSteerExtra;
                if (mag > kSteerMagClamp) mag = kSteerMagClamp;
                ctrl[1] = RoundST0(mag);
                I32(a74(0x0089a4f4u, v)) = (long)std::lround(mag);
                I32(a74(0x0089a4ecu, v)) = 2;
                I32(a74(0x0089a4f0u, v)) = frame;
            } else {
                float cs = (float)(kSettleFrames - el) * kCounterScale
                           * (float)I32(a74(0x0089a4f4u, v));
                ctrl[0] = RoundST0(cs);
            }
        }
    }

    // ---- ACCEL / BRAKE bands ----
    ctrl[4] = 0xff;                                  // accel full (default)
    const float prevSpeed = F32(a14(0x008032e0u, v));
    F32(a14(0x008032e0u, v)) = speed;
    if (kBrakeSpeedDel < speed - prevSpeed && kBrakeMinSpeed < speed) { // hard speed spike
        ctrl[4] = 0; ctrl[5] = 0xff;
    }
    if (k20f < err && kRate1Brake < rate1) { ctrl[4] = 0; ctrl[5] = 0xff; } // [U-C-RATE1]
    if (err < kSteerSplit && kAccelErrLo < err) {   // mildly off → accel+brake+steer
        ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[0] = 0xff;
    }
    if (kSteerSplit < err && err < kAccelErrHi) {
        ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[1] = 0xff;
    }

    // ---- behaviour-mode tails (7/5/9/2) : inert until targeting helpers ported ----
    const int m = I32(a74(0x0089a52cu, v));
    if (m == 7) { ctrl[4] = 0x40; }
    else if (m == 9) {
        if (kMode9BrakeA < rate1) { ctrl[4] = 0; ctrl[5] = 0xff; }   // [U-C-RATE1]
        if (kMode9BrakeB < rate1) { ctrl[4] = 0; }
    }
    // m==5 (drum/oil) + m==2 (cross-product align, needs FUN_00408af0) tails: TODO.

    // ---- final game-mode gate (race-class modes 6/5/9/10/11 keep throttle) ----
    if (gameMode != 6 && gameMode != 5 && gameMode != 9 &&
        gameMode != 10 && gameMode != 11) {
        ctrl[4] = 0; ctrl[5] = 0;
    }
}

// FUN_00418560 bank-select: pick the spline ptr for this vehicle's line type/index.
// Faithful integer logic (falls back to the race bank when a bank is too short).
std::uintptr_t SelectSpline(int v)
{
    if (2 < I32(st_field(kAiSplineIndex, v))) I32(st_field(kAiSplineIndex, v)) = 0;
    int type = I32(st_field(kAiLineType, v));
    int idx  = I32(st_field(kAiSplineIndex, v));
    std::uintptr_t race = kSplineRace;
    auto pick = [&](std::uintptr_t base, std::uintptr_t cntBase) -> std::uintptr_t {
        if (I32(cntBase + idx * kSplineStride) < 4) return race;
        return base + idx * kSplineStride;
    };
    switch (type) {
        case 0:
            if (I32(kSplineRaceCnt + idx * kSplineStride) < 4) { I32(st_field(kAiSplineIndex, v)) = 0; idx = 0; }
            return kSplineRace + idx * kSplineStride;
        case 1: return pick(kSplineInside, kSplineInsideCnt);
        case 2: return pick(kSplineSlow,   kSplineSlowCnt);
        case 3: return pick(kSplineCheat,  kSplineCheatCnt);
        default: return race;
    }
}

void VehicleStep(int v)
{
    int slot = I32(kSlotTableBase + static_cast<std::uintptr_t>(v) * kSlotTableStride);
    std::uint8_t* ctrl = reinterpret_cast<std::uint8_t*>(kCtrlBlockBase + static_cast<std::uintptr_t>(slot) * kCtrlBlockStride);
    ctrl[0] = ctrl[1] = ctrl[4] = ctrl[5] = ctrl[6] = ctrl[7] = 0;

    // FUN_00417180 bank-switch timer/RNG: STUB (TODO) — type/index left as-is.
    std::uintptr_t spline = SelectSpline(v);

    int fd0 = s_host.game_mode_fd0();
    // modes 4/9 (FUN_00416a30) and 8 (FUN_00417da0) variants: STUB → main step.
    (void)fd0;
    ControlStep(spline, v, ctrl);   // FUN_00416250 (bands real; targeting stubbed)
    // FUN_00417640 post-step powerup-brake: STUB (TODO).
    // override-replay (+0x40..0x4c): STUB (TODO) — clean integer logic, low risk.
}

} // namespace

void Ai_SetHost(const Host* host)
{
    if (host) s_host = *host;
    else {
        s_host = Host{ h_zero_i, h_zero_i, h_zero_i, h_zero_i,
                       h_zero_iv, h_zero_iv, h_zero_i, h_zero_xz, h_zero_xz };
    }
}

// FUN_00418860 — per-frame tick. Guard on race-line count; rubber-band STUB.
void Ai_Standalone_Tick()
{
    if (I32(kSplineRaceCnt) <= 3) return;        // DAT_00801ca0 > 3 (splines loaded)

    int r = s_host.round_type();
    int fd0 = s_host.game_mode_fd0();
    bool aiRound = (r == 3 || r == 4 || r == 5 || r == 10 ||
                    fd0 == 4 || fd0 == 9 || fd0 == 8 || fd0 == 10);
    (void)aiRound;  // CarSlotStateSet alive-poke (FUN_0040e480) = host/entity TODO.

    // FUN_004177b0 pre-tick rubber-banding: STUB (TODO).

    for (int v = 0; v < 4; ++v) {
        int t = s_host.veh_type(v);
        if ((t != 0 && t != 1) || s_host.ai_target_enable() == 1) {
            if (s_host.car_alive(v) == 1) VehicleStep(v);
        }
    }
}

} // namespace Ai
