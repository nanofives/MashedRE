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
//   STUBBED (RVA TODO, port in WS-C follow-ups):
//     FUN_00416250 primary control step (behaviour tree modes 1..10 + ctrl bands)
//     FUN_00416a30 / FUN_00417da0      control-step variants (modes 4/9 / 8)
//     FUN_004177b0 pre-tick rubber-banding (difficulty catch-up)
//     FUN_00417180 bank-switch timer/RNG detail   FUN_00417640 powerup-brake
//     FUN_00415220 powerup activation             FUN_00417cf0 mode-8 targeting
//     FUN_00414570/00415880/00414a70/00414c30/004150e0/00414f00/004148b0/00415020
//                  targeting + LOS helpers        FUN_00416060 LOS  FUN_00415d00 wall-ahead
//     FUN_00443300 spline interpolation + FUN_00443dc0 curvature-walk/wall-march tail
//     .AI parser (populates the 0x00801aa0 spline arrays + 0x007f1a9c tile grid)
//   The ctrl-byte STEER BANDS (which |error| → which 0/0x40/0xff) live in
//   FUN_00416250 (not yet ported) → marked [U-C-BANDS].
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
const int kLookahead = 4;

void TargetPoint(std::uintptr_t spline, int nearestIdx, float* tx, float* tz)
{
    int count = I32(spline + 0x200);
    int idx = (nearestIdx + kLookahead) % count;
    *tx = F32(spline + idx * 8);
    *tz = F32(spline + idx * 8 + 4);
}

// ---------------------------------------------------------------------------
// Mode-0 control step (the default spline-follow path of FUN_00416250). The full
// behaviour tree (targeting modes 1..10) + the exact ctrl steer/accel/brake band
// thresholds are STUBBED ([U-C-BANDS]); this writes a minimal follow output.
// ---------------------------------------------------------------------------
void ControlStepMode0(std::uintptr_t spline, int v, std::uint8_t* ctrl)
{
    float ownX, ownZ;
    s_host.own_xz(v, &ownX, &ownZ);
    int nearest = SplineNearestIndex(spline, ownX, ownZ);
    if (nearest < 0) return;                 // bank empty → leave ctrl zeroed

    float tx, tz;
    TargetPoint(spline, nearest, &tx, &tz);
    float err = SteerAngleError(v, tx, tz);  // [0,360); 0/360 = aligned, 180 = behind

    // ctrl[4]=accel full (mode-0 default). [U-C-BANDS]: real bands from FUN_00416250.
    ctrl[4] = 0xff;
    ctrl[5] = 0;
    // steer toward smaller wrap direction: err<180 → one side, else the other.
    if (err > 1.0f && err < (kWrap * 0.5f)) {
        ctrl[0] = 0xff; ctrl[1] = 0;
    } else if (err >= (kWrap * 0.5f) && err < (kWrap - 1.0f)) {
        ctrl[0] = 0; ctrl[1] = 0xff;
    } else {
        ctrl[0] = 0; ctrl[1] = 0;
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
    // modes 4/9 (FUN_00416a30) and 8 (FUN_00417da0) variants: STUB → mode-0 path.
    (void)fd0;
    ControlStepMode0(spline, v, ctrl);
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
