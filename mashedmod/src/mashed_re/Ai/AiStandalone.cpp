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
#include <cstdlib>   // std::getenv (G4 pure-pursuit toggle)
#include <cstdio>    // [G4] env-gated AI nav trace

namespace Ai {

namespace {

// ---- host (no-op defaults so the module is inert until WS-C-WIRE binds it) ---
int  h_zero_i()                 { return 0; }
int  h_zero_iv(int)             { return 0; }
void h_zero_xz(int, float* a, float* b) { *a = 0.0f; *b = 0.0f; }
int  h_los_clear(float, float, float, float) { return 1; }   // no-op = always clear

Host s_host = {
    h_zero_i, h_zero_i, h_zero_i, h_zero_i,
    h_zero_iv, h_zero_iv, h_zero_i,
    h_zero_xz, h_zero_xz, h_los_clear,
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

// ===========================================================================
// FUN_00443300 — Catmull-Rom cubic spline interpolation (XZ) over 4 wrapped
// control points P0..P3 = pts[idx-1..idx+2]. Verbatim (consts memory_read pool0
// 2026-06-30; see re/analysis/ai_spline_lookahead.md): 0x005cc31c=3.0,
// 0x005cc358=5.0, 0x005cc35c=4.0, 0x005cc32c=0.5; param clamped to [0,1]
// (0x005cc320=1.0 / DAT_005d757c=0).
// ===========================================================================
void CatmullRom(std::uintptr_t spline, int idx, float t, float* outX, float* outZ)
{
    const int count = I32(spline + 0x200);
    if (count <= 0) { *outX = 0.f; *outZ = 0.f; return; }
    int i0 = idx - 1; if (i0 < 0)      i0 += count;   // P0
    int i1 = i0 + 1;  if (i1 >= count) i1 -= count;   // P1 (= idx)
    int i2 = i1 + 1;  if (i2 >= count) i2 -= count;   // P2
    int i3 = i2 + 1;  if (i3 >= count) i3 -= count;   // P3
    float t0 = t; if (t0 > 1.0f) t0 = 1.0f; else if (t0 < 0.0f) t0 = 0.0f;
    const float t2 = t0 * t0;
    auto comp = [&](std::uintptr_t o) -> float {
        const float P0 = F32(spline + static_cast<std::uintptr_t>(i0) * 8 + o);
        const float P1 = F32(spline + static_cast<std::uintptr_t>(i1) * 8 + o);
        const float P2 = F32(spline + static_cast<std::uintptr_t>(i2) * 8 + o);
        const float P3 = F32(spline + static_cast<std::uintptr_t>(i3) * 8 + o);
        return (2.0f * P1 + (P2 - P0) * t0
                + (((P1 * 3.0f - P0) - P2 * 3.0f) + P3) * t2 * t0
                + ((P2 * 4.0f + (2.0f * P0 - P1 * 5.0f)) - P3) * t2) * 0.5f;
    };
    *outX = comp(0);
    *outZ = comp(4);
}

// Normalize a 2-float (XZ) vector in place (standalone equiv of FUN_004c3c60,
// which uses a fast-rsqrt LUT; std::sqrt is bit-equivalent for nav use).
inline void Normalize2(float* v) {
    const float m2 = v[0]*v[0] + v[1]*v[1];
    if (m2 > 0.0f) { const float inv = 1.0f / std::sqrt(m2); v[0] *= inv; v[1] *= inv; }
}

// ===========================================================================
// FUN_00443dc0 (param_5=1, param_6=0) — spline lookahead target finder. STAGE 1:
// Phases 1-7 (nearest -> per-vehicle index continuity -> Catmull-Rom closest-param
// ternary search -> 16-point forward walk scored by path/own alignment -> max pick).
// Replaces the crude 1-raw-point [G4] s_prog crutch so the target stays ON the
// (Catmull-smoothed) racing line and the verbatim ControlStep bands stop full-locking
// at corners. The Phase-8 LOS wall-march (tile grid 0x007f1a9c/0x007f9a9c) is a
// follow-up. Writes target XZ to out[0],out[1]. NO-GUESSING: consts cited above.
// ===========================================================================
void SplineLookahead(std::uintptr_t spline, float ownX, float ownZ, int v, float* out)
{
    const int count = I32(spline + 0x200);
    if (count <= 0) { out[0] = ownX; out[1] = ownZ; return; }

    // ---- Phase 1: nearest control point ----
    int nearest = 0; float bestD = 100000.0f;     // orig seed 100000.0
    for (int i = 0; i < count; ++i) {
        const float dx = F32(spline + static_cast<std::uintptr_t>(i)*8)     - ownX;
        const float dz = F32(spline + static_cast<std::uintptr_t>(i)*8 + 4) - ownZ;
        const float d  = dx*dx + dz*dz;
        if (d < bestD) { nearest = i; bestD = d; }
    }

    // ---- Phase 2: per-vehicle index continuity (DAT_008032d4 + v*0x14) ----
    // The original's "reject a jump >1" rule (anti-shortcut) assumes the stored index is
    // roughly where the car was. The standalone never seeds it at spline placement, so a
    // mid-track-spawned (or respawned, or knocked-off) car whose stored index is stale gets
    // pinned forever. Self-heal: if the car is now FAR from the stored point (>6 local
    // segment lengths, mirroring the old s_prog lost-reseed), accept the true nearest;
    // otherwise apply the original jump-reject.
    {
        const std::uintptr_t idxAddr = 0x008032d4u + static_cast<std::uintptr_t>(v) * 0x14u;
        int prev = I32(idxAddr);
        bool reseed = (prev < 0 || prev >= count);
        if (!reseed) {
            const int pn = (prev + 1) % count;
            const float sx = F32(spline + (std::uintptr_t)pn*8)     - F32(spline + (std::uintptr_t)prev*8);
            const float sz = F32(spline + (std::uintptr_t)pn*8 + 4) - F32(spline + (std::uintptr_t)prev*8 + 4);
            const float segLen2 = sx*sx + sz*sz;
            const float dx = F32(spline + (std::uintptr_t)prev*8)     - ownX;
            const float dz = F32(spline + (std::uintptr_t)prev*8 + 4) - ownZ;
            if (dx*dx + dz*dz > segLen2 * 36.0f) reseed = true;   // car teleported (spawn/respawn/knock-off)
        }
        if (!reseed) {
            const int diff = nearest - prev;
            if (diff != 1 - count && (diff < -1 || 1 < diff)) nearest = prev;
        }
        I32(idxAddr) = nearest;
    }

    // ---- Phase 3: sub-segment select (this+0.01 vs prev+0.99) ----
    float cx, cz;
    CatmullRom(spline, nearest, 0.01f, &cx, &cz);   // 0x3c23d70a
    float d0 = (cx-ownX)*(cx-ownX) + (cz-ownZ)*(cz-ownZ);
    int prevIdx = nearest - 1; if (prevIdx < 0) prevIdx += count;
    float qx, qz;
    CatmullRom(spline, prevIdx, 0.99f, &qx, &qz);   // 0x3f7d70a4
    if ((qx-ownX)*(qx-ownX) + (qz-ownZ)*(qz-ownZ) < d0) { cx=qx; cz=qz; nearest=prevIdx; }
    (void)cx; (void)cz;

    // ---- Phase 4: ternary search for the closest param in [0,1] (16 iters) ----
    const float kThird = bits_to_f(0x3eaaaa9fu);   // _DAT_005ce034 (~1/3)
    float hi = 1.0f, lo = 0.0f;
    float hx, hz, lx, lz;
    CatmullRom(spline, nearest, 1.0f, &hx, &hz);
    float dHi = (hx-ownX)*(hx-ownX) + (hz-ownZ)*(hz-ownZ);
    CatmullRom(spline, nearest, 0.0f, &lx, &lz);
    float dLo = (lx-ownX)*(lx-ownX) + (lz-ownZ)*(lz-ownZ);
    for (int it = 0; it < 16; ++it) {
        if (dHi <= dLo) {
            lo = (lo + lo + hi) * kThird;
            CatmullRom(spline, nearest, lo, &lx, &lz);
            dLo = (lx-ownX)*(lx-ownX) + (lz-ownZ)*(lz-ownZ);
        } else {
            hi = (hi + hi + lo) * kThird;
            CatmullRom(spline, nearest, hi, &hx, &hz);
            dHi = (hx-ownX)*(hx-ownX) + (hz-ownZ)*(hz-ownZ);
        }
    }
    float param, startX, startZ;
    if (dHi <= dLo) { param = hi; startX = hx; startZ = hz; }
    else            { param = lo; startX = lx; startZ = lz; }

    // ---- Phase 6: forward-walk 16 points; score by path/own alignment ----
    const float kStep = bits_to_f(0x3d4ccccdu);    // _DAT_005cc9a0 (0.05)
    int   seg   = nearest;
    float prevX = startX, prevZ = startZ;
    float pts[32];      // 16 (x,z) lookahead points
    float metric[16];
    for (int i = 0; i < 16; ++i) {
        param += kStep;
        if (param > 1.0f) { param -= 1.0f; if (++seg == count) seg = 0; }
        float wx, wz;
        CatmullRom(spline, seg, param, &wx, &wz);
        float seg2[2] = { wx - prevX, wz - prevZ };  Normalize2(seg2);
        float bak[2]  = { prevX - ownX, prevZ - ownZ }; Normalize2(bak);
        float m = seg2[0]*bak[0] + seg2[1]*bak[1];
        if (m < 0.0f) m = -param;     // DAT_005d757c=0
        if (m > 1.0f) m = 1.0f;       // _DAT_005cc320=1.0
        metric[i] = m;
        pts[i*2] = wx; pts[i*2 + 1] = wz;
        prevX = wx; prevZ = wz;
    }

    // ---- Phase 7: target = the max-aligned lookahead point (strict-<, first wins) --
    int best = 0; float bestM = 0.0f;   // init DAT_005d757c=0
    for (int i = 0; i < 16; ++i) if (bestM < metric[i]) { bestM = metric[i]; best = i; }

    // ---- Phase 8: LOS wall-march. The original marches own->target across the AI
    // tile grid (0x007f1a9c/0x007f9a9c) and, while a wall blocks the line, steps the
    // target back 2 lookahead points until reachable (or the closest point). Here the
    // host's los_clear backs the same logic with the track collision. This keeps the
    // target on a drivable straight line, so the verbatim ControlStep bands don't
    // full-lock / brake-latch at corners (the on-mesh stall root cause, 2026-06-30).
    for (int guard = 0; guard < 8; ++guard) {
        if (s_host.los_clear(ownX, ownZ, pts[best*2], pts[best*2 + 1])) break;
        if (best <= 1) { best = 0; break; }   // fall back to the closest forward point
        best -= 2;                              // step the target back (orig: iVar8 -= 2)
    }
    out[0] = pts[best*2];
    out[1] = pts[best*2 + 1];
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
    // FAITHFUL spline lookahead (FUN_00443dc0, param_5=1 param_6=0) — replaces the
    // [G4] crude 1-raw-point s_prog crutch. Nearest -> per-vehicle index continuity ->
    // Catmull-Rom closest-param -> 16-point forward walk -> max-aligned pick, so the
    // target tracks the (smoothed) racing line and the verbatim bands below stop
    // full-locking the steer at corners (the stall root cause measured 2026-06-30).
    // re/analysis/ai_spline_lookahead.md. Phase-8 LOS wall-march is a follow-up.
    float look[2] = { ownX, ownZ };
    SplineLookahead(spline, ownX, ownZ, v, look);
    float tx = look[0], tz = look[1];
    I32(a74(0x0089a52cu, v)) = mode;                // commit behaviour mode

    const float err = SteerAngleError(v, tx, tz);  // FUN_00415e20, [0,360)
    const int   frame = I32(0x007f0ff4u);

    // [G4] STANDALONE PURE-PURSUIT steering substitute. The ported ControlStep bands below
    // full-lock the steer (ctrl=255) for any bearing error in 30..180deg ("mildly off" hard
    // corrective) — correct in the original because its curvature-walk target (FUN_00443300 /
    // FUN_00443dc0 tail) keeps the error <30deg, but that refinement is STUBBED here, so with
    // the crude lookahead the error sits in the full-steer band at every corner and the car
    // ORBITS / drives into the edge instead of negotiating it (all cars stalled ~gate 7). This
    // proportional controller reuses the sign-correct SteerAngleError (err<180 -> one way,
    // >180 -> the other) but scales the steer by how far off the car is (full lock only past
    // ~kFullDeg), so the car drives FORWARD through bends -> full laps. A ratified standalone
    // shim (cf. the std::sqrt LUT fallback / contact stub). Env MASHED_AI_PUREPURSUIT=0 reverts
    // to the verbatim bands.
    static const bool s_pp = [] {
        const char* e = std::getenv("MASHED_AI_PUREPURSUIT");
        return (e && e[0] != '0');   // OPT-IN (default off -> verbatim bands). Experimental:
        // the nav trace showed steer has no effect once the car flings off-track + loses
        // grounding (no +0x9c0 authority) + the edge-stop traps it — so neither the bands nor
        // this pure-pursuit completes a lap yet. Kept as debugging infra; see PLAYTHROUGH_G4 doc.
    }();
    if (s_pp) {
        // SIGNED continuous error in [-180,180] (0 = aligned). The previous split at err=180
        // (err<180 -> one way, >180 -> other) was BANG-BANG: the car's error sat near 180 and
        // flipped steer direction every crossing -> a U-turn limit cycle near spawn (velocity
        // reversed north<->south each frame). A signed proportional steer has no discontinuity
        // to oscillate around: it eases off as the car aligns. Magnitude capped low (anti-spin,
        // G2's +0x9c0 is strong). toCtrl0 = (se>0): err<180 (se>0) -> ctrl[0], matching the bands.
        float se = err; if (se > 180.0f) se -= kWrap;              // [-180,180]
        // PD controller: proportional + DERIVATIVE damping. Proportional-only oscillated because
        // G2's strong +0x9c0 yaw rate overshoots alignment (momentum) and swings back. The D term
        // (rate of change of the error) opposes that overshoot -> the car SETTLES onto the target
        // heading instead of donut-ing. Derivative is on the error signal (per-car prevSe) so no
        // physics plumbing is needed; the wrap-corrected delta avoids the +-180 seam.
        static float s_prevSe[4] = {0, 0, 0, 0};
        float d = se - s_prevSe[v];
        while (d > 180.0f) d -= kWrap; while (d < -180.0f) d += kWrap;
        s_prevSe[v] = se;
        // DEADBAND: steer 0 when roughly aligned so the car drives STRAIGHT (a continuous small
        // steer makes it circle forever — that's why every car, incl. the AI-driven player,
        // donut'd near spawn; G2's "smooth drive" was itself a CIRCLE, not a lap). Outside the
        // band, steer proportionally toward the target; once within it, go straight and let the
        // car cover ground -> it reaches the waypoint -> advance -> new target.
        const float kDeadDeg = 12.0f;
        const float kP = 1.0f / 90.0f;                             // full P at 90deg off
        const float kD = 0.020f;                                   // damping (per-frame err rate)
        float s;
        if (se > -kDeadDeg && se < kDeadDeg) { s = 0.0f; }         // aligned -> drive straight
        else { s = kP * se + kD * d; if (s > 1.0f) s = 1.0f; if (s < -1.0f) s = -1.0f; }
        // [G4] kMaxSteer is PHYSICS-GROUNDED, not arbitrary: turn radius = worldSpeed/yawRate, and
        // at the old 0.45 cap the AI turned at radius ~10 on a radius-80 TRACK — 3-4x too tight, so
        // the car was geometrically LOCKED in a circle and could never reach a target ~20u ahead
        // (it donut'd near spawn). The player's G2 circle: steer 0.5 -> radius ~13. To follow the
        // track curvature the steer must arc at ~track radius -> steer ~0.08-0.12. Cap at 0.15 (a
        // little margin for real corners) so the car drives nearly STRAIGHT with gentle correction.
        const float kMaxSteer = 0.15f;
        s *= kMaxSteer;
        bool toCtrl0 = (s >= 0.0f);
        int mag = static_cast<int>((s < 0.f ? -s : s) * 255.0f + 0.5f); if (mag > 255) mag = 255;
        // [G4] STEER SIGN: the nav trace showed the car driving AWAY from the target — for a
        // target to the car's east/north it applied ctrl[1] (which, per the verified physics in
        // G2, increases yaw -> rotates forward toward -x/west = the WRONG way). The AI steer sign
        // was long flagged [UNCERTAIN] ("steer-sign pending verify"); it is INVERTED. Flip it so
        // err<180 -> ctrl[1] and err>180 -> ctrl[0]. Env MASHED_AI_STEERFLIP=0 reverts for A/B.
        static const bool s_flip = [] {
            const char* e = std::getenv("MASHED_AI_STEERFLIP");
            return !(e && e[0] == '0');           // flipped by default
        }();
        bool c0 = s_flip ? !toCtrl0 : toCtrl0;
        ctrl[0] = c0 ? static_cast<std::uint8_t>(mag) : 0;
        ctrl[1] = c0 ? 0 : static_cast<std::uint8_t>(mag);
        ctrl[4] = 0xff;                                            // accelerate (full; gentle steer
        ctrl[5] = 0;                                               // handles corners, no mid-corner
                                                                   // brake that bled momentum)
        const int gm = s_host.game_sub_mode();
        if (gm != 6 && gm != 5 && gm != 9 && gm != 10 && gm != 11) { ctrl[4] = 0; ctrl[5] = 0; }
        // [G4] env MASHED_AI_NAV -> a6_diag.log: trace one car's nav to see the failure mode.
        {
            static const bool s_nav = (std::getenv("MASHED_AI_NAV") != nullptr);
            static int s_nn = 0;
            if (s_nav && v == 1 && (s_nn % 30) == 0 && s_nn < 1200) {
                if (std::FILE* lf = std::fopen("ai_nav.log", "a")) {
                    std::fprintf(lf, "NAV v1 own=(%.1f,%.1f) tgt=(%.1f,%.1f) err=%.0f "
                                 "spd=%.1f vel=(%.1f,%.1f) ctrl[%d,%d,%d,%d]\n",
                                 ownX, ownZ, tx, tz, err,
                                 speed, vx, vz, ctrl[0], ctrl[1], ctrl[4], ctrl[5]);
                    std::fclose(lf);
                }
            }
            if (s_nav && v == 1) ++s_nn;
        }
        return;                                                    // skip the orbit-prone bands
    }

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
                       h_zero_iv, h_zero_iv, h_zero_i, h_zero_xz, h_zero_xz, h_los_clear };
    }
}

// Faithful racing-line lookahead target for vehicle v (see AiStandalone.h). Selects the
// vehicle's spline bank then runs the ported FUN_00443dc0 lookahead. The "where to go"
// for the standalone faithful-nav + robust-motion opponent drive.
bool Ai_ComputeTarget(int v, float ownX, float ownZ, float* outTx, float* outTz)
{
    if (I32(kSplineRaceCnt) <= 3) return false;   // .AI banks not loaded
    std::uintptr_t spline = SelectSpline(v);       // FUN_00418560 bank pick (race line)
    if (I32(spline + 0x200) <= 0) return false;    // empty bank
    float out[2] = { ownX, ownZ };
    SplineLookahead(spline, ownX, ownZ, v, out);   // FUN_00443dc0 (Phases 1-8)
    *outTx = out[0];
    *outTz = out[1];
    return true;
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
