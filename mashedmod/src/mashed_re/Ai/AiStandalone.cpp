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
//   DONE (P4a 2026-07-04): FUN_00416a30/FUN_00417da0 control-step mode-4/9/8
//                      variants (ControlStepM49/ControlStepM8) — same
//                      simplification as ControlStep (targeting chain forced
//                      mode 0 via SplineLookahead; mode-10/mode-6 predicate
//                      overrides omitted, same stubbed predicates below).
//                      FUN_00417180 bank-switch timer/RNG (BankSwitch) —
//                      verbatim state machine; FUN_00472650 PRNG substituted
//                      with std::rand() (documented approximation, out of
//                      scope). FUN_00417640 post-step powerup-brake
//                      (PostStepPowerupBrake) — verbatim gates; the dead
//                      displacement/Vec3Magnitude computation elided (see
//                      function comment); rate operand pinned via the shared
//                      [U-C-RATE1]=0 substitute so the brake branch is
//                      currently unreachable (always coasts).
//   DONE (P4b 2026-07-04): FUN_004177b0 pre-tick rubber-banding
//                      (AiPreTickRubberBand) — race-metric/finish-order-slot
//                      update + mode-9/mode-4 speed rubber-banding + powerup
//                      speed doubling + slow-line difficulty-flag state
//                      machine, verbatim from asm (all previously-undecoded
//                      U-8993 constants memory_read this session; U-8992/
//                      U-8993/U-8994 left open, semantic/structural,
//                      non-blocking). FUN_00472650 PRNG substituted with a
//                      deterministic `return lo` stand-in (ForceIntegratorStubs.cpp
//                      precedent) — only feeds the difficulty-flag probability
//                      roll. FUN_0040e480 (CarSlotStateSet) alive-poke wired
//                      into Ai_Standalone_Tick, individually gated on
//                      car_alive per FUN_00418860's fresh decomp (corrects the
//                      older plate's unconditional pseudocode). ControlStep's
//                      m==5 (drum/oil) and m==2 (cross-product align, via
//                      FUN_00408af0) tails ported (both currently unreachable —
//                      `mode` stays hardcoded 0 pending the targeting-helper
//                      port, same dead-tail status as the existing m==7/9
//                      tails); m==2's vel[1] (vertical velocity) has no Host
//                      accessor and is approximated 0.0f [UNCERTAIN]. VehicleStep's
//                      override-replay tail (+0x40..0x4c, FUN_00418560's last
//                      block) ported — clean integer logic, no callees.
//   DONE (D3 2026-09-26, Ghidra pool0 read-only, re/analysis/D3_AI_PORT_2026-09-26.md):
//     FUN_00443300 Catmull-Rom: re-read against the decomp, ALREADY verbatim (the
//                  STUBBED entry that stood here was stale).
//     FUN_00443440 spline curvature walk (NEW, SplineCurvature) + FUN_004233e0 heading.
//     FUN_00443dc0 tail: the wall-march over the .AI tile grid 0x007f1a9c/0x007f9a9c
//                  (0x004446a4..0x00444a5e) + FUN_00416230 flag write; replaces the
//                  host-LOS approximation. Phase 2 index continuity made verbatim
//                  (reset value 1000 from FUN_00413fe0/FUN_00414030).
//     FUN_00416250 bands CORRECTED against the listing 0x004165a5..0x00416a28:
//                  magnitude = err * [+0x9e4] * k (x [curv]*0.05 when mode==0 and
//                  curv>20), mirror uses (360-err), brake rule on the x87-carried
//                  history value, speed-spike rule on [+0xb0c], 0x0089a368==1 accel
//                  x0.4 rescale. FUN_004a2c48 TRUNCATES (listing 0x004a2c48), it is not
//                  ROUND (the IDENTIFIED line above is wrong).
//     FUN_00415220 AI power-up fire decision (NEW, AiFireDecision): writes ctrl[7].
//   STUBBED (RVA TODO, port in WS-C follow-ups):
//     FUN_00414570/00415880/00414a70/00414c30/004150e0/00414f00/004148b0/00415020
//                  targeting + LOS helpers (modes 1..10) FUN_00416060 LOS  FUN_00415d00 wall
//     FUN_00417cf0 mode-8
//     FUN_00442a60 reference-distance array 0x008989b0 (read by FUN_00415220's gates);
//                  standalone holds it at 0, the value FUN_00442a60 itself stores first.
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
float h_zero_f(int, int) { return 0.0f; }

Host s_host = {
    h_zero_i, h_zero_i, h_zero_i, h_zero_i,
    h_zero_iv, h_zero_iv, h_zero_i,
    h_zero_xz, h_zero_xz, h_los_clear,
    h_zero_xz, h_zero_f, h_zero_iv,
};

inline float bits_to_f(std::uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }

// [D3 2026-09-27] step-locals mirror, see AiStandalone.h / the 0x004165a5 probe.
StepLocals s_step_locals[4] = {};
void RecordStepLocals(int v, float lx, float lz, float curv, float err, int mode,
                      float ox, float oz) {
    if (v < 0 || v >= 4) return;
    StepLocals& s = s_step_locals[v];
    s.look_x = lx; s.look_z = lz; s.curv = curv; s.err = err;
    s.own_x = ox;  s.own_z = oz;  s.mode = mode;  s.valid = true;
}

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
    while (err <= 0.0f) err += kWrap;       // `<0 != ==0` i.e. <=0 (decomp 0x00415e20 tail)
    while (err < 0.0f) err += kWrap;        // (verbatim double-wrap guard)
    while (kWrap < err) err -= kWrap;
    return err;
}

// [D3 2026-09-26] FUN_00415e20 re-read (pool0): the HEADING operand is not the
// velocity. The second half calls FUN_0046d510(&v3, v), which transforms the constant
// DAT_00614708 = (0,0,1) by the car's matrix (rec+0x928) and returns rec+0x9d4..0x9dc,
// the body FORWARD vector; then z is negated, y zeroed and the vector normalized. Every
// angle wrap is `<= 0 -> += 360` (the `x < 0 != (x == 0)` idiom), so exact 0 maps to 360.
// SteerAngleError above is kept for ControlStepM49/M8; ControlStep uses this one.
float SteerAngleErrorFwd(int v, float targetX, float targetZ)
{
    auto angOf = [](float x, float z) -> float {          // (x, 0, z) already z-negated
        const float n = std::sqrt(x * x + z * z);          // FUN_004c39b0 normalize
        const float nx = (n > 0.0f) ? x / n : x;
        const float nz = (n > 0.0f) ? z / n : z;
        float d = nx;                                      // z*0 + y*0 + x (DAT_005d757c = 0)
        if (d < kSteerThrLo) d = kSteerClampLo;
        if (kSteerThrHi < d) d = kSteerClampHi;
        float a = static_cast<float>(std::acos(d) * kSteerScale);   // FUN_004a3384 * 0x005cc970
        if (nz < 0.0f) a = -a;
        while (a <= 0.0f) a += kWrap;
        return a;
    };
    float ownX, ownZ;  s_host.own_xz(v, &ownX, &ownZ);
    const float ang = angOf(targetX - ownX, -(targetZ - ownZ));
    float fx, fz;      s_host.own_fwd_xz(v, &fx, &fz);     // FUN_0046d510 -> rec+0x9d4/+0x9dc
    const float head = angOf(fx, -fz);
    float err = ang - head;
    while (err <= 0.0f) err += kWrap;
    while (err < 0.0f)  err += kWrap;
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

// FUN_004233e0's rad->deg scale: DOUBLE at 0x005ccae0 = bytes 000000c0 dca54c40
// = 57.295799255371094 (FMUL qword at 0x004233fd).
inline double kRadToDeg4233() {
    const std::uint64_t b = 0x404ca5dcc0000000ull; double d; std::memcpy(&d, &b, 8); return d;
}

// FUN_00443dc0 wall-march tile probe [D3 2026-09-26], one sample of the march at
// 0x00444754..0x00444807 (centre line) / 0x00444910..0x004449c3 (offset line):
//   rz = __ftol(z*4 +-0.5) - 0x10, rx = __ftol(x*4 +-0.5) - 0x10  (+0.5 when > 0, else
//        -0.5; FUN_004a2c48 truncates, so this is round-half-away)
//   s  = word [0x007f1a9c + (((rz+0x200)>>3)*0x80 + ((rx+0x200)>>3))*2]
//   blocked iff 0 < s < 0x200 and byte [0x007f9a9c + ((rx&7) + s*8)*8 + (rz&7)] is 0 or 3.
// The grid is the .AI payload Ai_BridgeLoad copies to 0x007f1a9c (AiData.h
// kAiTileGridOff / kAiSubCellOff). The range guard is standalone-only: it keeps a
// sample far off the 128x128 grid from reading outside the image-pad.
inline int TruncHalfAway(float x) { return static_cast<int>(0.0f < x ? x + 0.5f : x - 0.5f); }
bool TileBlocked(float x, float z)
{
    const int rz = TruncHalfAway(z * 4.0f) - 0x10;                 // ESI (first __ftol)
    const int rx = TruncHalfAway(x * 4.0f) - 0x10;                 // EAX (second __ftol)
    const int cell = ((rz + 0x200) >> 3) * 0x80 + ((rx + 0x200) >> 3);
    if (cell < -0x8000 || cell > 0x10000) return false;            // standalone guard
    const std::int16_t s = *reinterpret_cast<const std::int16_t*>(
        0x007f1a9cu + static_cast<std::intptr_t>(cell) * 2);
    if (s <= 0 || s >= 0x200) return false;
    const std::uint8_t c = U8(0x007f9a9cu +
        static_cast<std::uintptr_t>(((rx & 7) + s * 8) * 8 + (rz & 7)));
    return c == 0 || c == 3;
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
    // [D3 2026-09-26] VERBATIM (decomp 0x00443e86..0x00443eb4): keep the stored index
    // when the nearest point jumped by more than one (except the wrap step 1-count) AND
    // the stored index is inside the bank. The stored index is reset to 1000 (>= any
    // count, so the next call takes the true nearest) by FUN_00413fe0 at race reset and
    // by FUN_00414030(v) per vehicle; the standalone calls those ports (AiResetAll /
    // Ai_ResetVehicleIndex) at race start and whenever it relocates a car. This replaces
    // the "far from stored point -> reseed" heuristic that stood here, which the
    // original does not have.
    {
        const std::uintptr_t idxAddr = 0x008032d4u + static_cast<std::uintptr_t>(v) * 0x14u;
        const int prev = I32(idxAddr);
        const int diff = nearest - prev;
        if (diff != 1 - count && (diff < -1 || 1 < diff) && prev < count) nearest = prev;
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

    out[0] = pts[best*2];
    out[1] = pts[best*2 + 1];

    // ---- Phase 8: wall-march over the .AI tile grid [D3 2026-09-26, VERBATIM from the
    // listing 0x004446a4..0x00444a5e, pool0]. Replaces the host los_clear approximation.
    //   loop:
    //     march the CENTRE line own -> target (0x004446a4..0x00444820): t = 0, 0.25, ...
    //       while t < len; point = own + (d/len)*t; a point whose x equals own.x OR whose
    //       z equals own.z is skipped (0x0044472e..0x0044474e); blocked => stop.
    //     if the centre line is clear, march the OFFSET line (0x00444831..0x004449d6):
    //       start = (own.x - 0.25*dz/len, own.z - 0.25*dx/len) -- x offset uses dz and z
    //       offset uses dx, exactly as the listing (0x00444875..0x0044489d); same step,
    //       same skip rule against the offset start.
    //     either line blocked (0x004449de): idx 0/1 -> idx 0 and DONE; else idx -= 2
    //       (target = pts[idx], not done). Both lines clear, or len <= 0: DONE (0x00444a14).
    //     FUN_00416230(v, idx == 0) (0x00444a2c) every pass.
    //   on DONE: if ANY pass was blocked (local_164, [ESP+0x4c]) step back once more
    //     (0/1 -> 0, else -2) and take that point (0x00444a40..0x00444a5b).
    int idx = best;
    int anyBlocked = 0;   // local_164
    int marchPasses = 0;  // [D3 2026-09-27] = the original's FUN_00416230 call count
    for (;;) {
        int done = 0;     // local_194
        const float dx  = out[0] - ownX;
        const float dz  = out[1] - ownZ;
        const float len = std::sqrt(dx * dx + dz * dz);           // FUN_004c3bf0 (2-D)
        bool blocked = false;
        if (0.0f < len) {                                          // 0x004446d0..0x004446ef
            const float inv = 1.0f / len;
            for (float t = 0.0f; t < len && !blocked; t += 0.25f) { // 0x005cc564 = 0.25
                const float px = dx * inv * t + ownX;
                const float pz = inv * dz * t + ownZ;
                if (px != ownX && pz != ownZ && TileBlocked(px, pz)) { blocked = true; anyBlocked = 1; }
            }
        }
        if (!blocked) {
            // offset line (0x00444831): recomputes d and len from the (unchanged) target
            if (0.0f < len) {
                const float ux = (1.0f / len) * dx;                // local_148
                const float uz = dz * (1.0f / len);                // local_16c
                const float sx = ownX - uz * 0.25f;                // local_160
                const float sz = ownZ - ux * 0.25f;                // local_15c
                for (float t = 0.0f; t < len && !blocked; t += 0.25f) {
                    const float px = ux * t + sx;
                    const float pz = uz * t + sz;
                    if (px != sx && pz != sz && TileBlocked(px, pz)) { blocked = true; anyBlocked = 1; }
                }
            }
            if (!blocked) done = 1;                                 // 0x00444a14
        }
        if (blocked) {                                              // 0x004449de
            if (idx == 0 || idx == 1) { idx = 0; done = 1; }
            else idx -= 2;
            out[0] = pts[idx*2];
            out[1] = pts[idx*2 + 1];
        }
        I32(0x0089a500u + static_cast<std::uintptr_t>(v) * 0x74u) = (idx == 0) ? 1 : 0; // FUN_00416230
        ++marchPasses;
        if (done) break;
    }
    if (anyBlocked) {                                               // 0x00444a40
        const int j = (idx == 0 || idx == 1) ? 0 : idx - 2;
        out[0] = pts[j*2];
        out[1] = pts[j*2 + 1];
        idx = j;
    }
    // [D3 2026-09-27] diagnostic only (MASHED_AI_STEPDUMP), no state the port reads.
    if (v >= 0 && v < 4) {
        StepLocals& sl = s_step_locals[v];
        sl.march_n = marchPasses; sl.march_idx0 = (idx == 0) ? 1 : 0;
        sl.look_best = best; sl.look_idx = idx; sl.look_blk = anyBlocked;
    }
}

// ===========================================================================
// FUN_004233e0 — heading of (a, b) in degrees [D3 2026-09-26, listing 0x004233e0..
// 0x0042347b]: b == 0 -> 90 (a >= 0) / 270 (a < 0); else 180 + atan(a/b)*57.29578
// (0x005ccae0 is a DOUBLE 0x404ca5dcc0000000), +180 when b > 0, -360 when >= 360;
// a == b == 0 -> 0.
// ===========================================================================
float Heading004233e0(float a, float b)
{
    float r;
    if (b == 0.0f) {
        r = (a >= 0.0f) ? 90.0f : 270.0f;                   // 0x005ccad0 / 0x005cd324
    } else {
        r = static_cast<float>(180.0 + std::atan(static_cast<double>(a) / b) * kRadToDeg4233());
        if (0.0f < b) r += 180.0f;
        if (360.0f <= r) r -= 360.0f;
    }
    if (a == 0.0f && b == 0.0f) r = 0.0f;
    return r;
}

// ===========================================================================
// FUN_00443440(spline, pos, dist, &out, 0) — spline curvature walk [D3 2026-09-26,
// decomp pool0]. Called by FUN_00416250 at 0x004162b0 with dist = 10.0 (0x41200000);
// its float10 return is discarded (FSTP ST0 at 0x004162b5), only *out is used.
//   nearest point + 0.01/0.99 sub-segment pick + 16-step ternary (same as FUN_00443dc0)
//   h0 = heading of the tangent (C(p+0.01) - C(p)) at the closest param
//   walk +0.1 param steps accumulating SQUARED chord lengths until >= dist*0.5, then
//   (h1, unused here) and on until >= dist; h2 = heading of the tangent there
//   *out = h0 - h2 wrapped to [0,360) (`< 0 -> += 360`, 0x00443c.. for-loop)
// ===========================================================================
float SplineCurvature(std::uintptr_t spline, float ownX, float ownZ, float dist)
{
    const int count = I32(spline + 0x200);
    if (count <= 0) return 0.0f;
    int nearest = 0; float bestD = 100000.0f;
    for (int i = 0; i < count; ++i) {
        const float dx = F32(spline + static_cast<std::uintptr_t>(i)*8)     - ownX;
        const float dz = F32(spline + static_cast<std::uintptr_t>(i)*8 + 4) - ownZ;
        const float d  = dx*dx + dz*dz;
        if (d < bestD) { nearest = i; bestD = d; }
    }
    float cx, cz;  CatmullRom(spline, nearest, 0.01f, &cx, &cz);
    const float ex = cx - ownX, ez = cz - ownZ;
    int prevIdx = nearest - 1; if (prevIdx < 0) prevIdx = count - 1;
    float qx, qz;  CatmullRom(spline, prevIdx, 0.99f, &qx, &qz);
    if ((qx-ownX)*(qx-ownX) + (qz-ownZ)*(qz-ownZ) < ex*ex + ez*ez) nearest = prevIdx;

    const float kThird = bits_to_f(0x3eaaaa9fu);             // _DAT_005ce034
    float hi = 1.0f, lo = 0.0f, hx, hz, lx, lz;
    CatmullRom(spline, nearest, 1.0f, &hx, &hz);
    float dHi = (hx-ownX)*(hx-ownX) + (hz-ownZ)*(hz-ownZ);
    CatmullRom(spline, nearest, 0.0f, &lx, &lz);
    float dLo = (lx-ownX)*(lx-ownX) + (lz-ownZ)*(lz-ownZ);
    for (int it = 0; it < 16; ++it) {
        if (dHi <= dLo) { lo = (lo + lo + hi) * kThird; CatmullRom(spline, nearest, lo, &lx, &lz);
                          dLo = (lx-ownX)*(lx-ownX) + (lz-ownZ)*(lz-ownZ); }
        else            { hi = (hi + hi + lo) * kThird; CatmullRom(spline, nearest, hi, &hx, &hz);
                          dHi = (hx-ownX)*(hx-ownX) + (hz-ownZ)*(hz-ownZ); }
    }
    float param = hi, px = hx, pz = hz;                        // bVar7 = dLo < dHi -> lo
    if (dLo < dHi) { param = lo; px = lx; pz = lz; }

    const float kTan = bits_to_f(0x3c23d70au);                 // _DAT_005cc328 (0.01)
    const float kWalk = bits_to_f(0x3dcccccdu);                // _DAT_005cc56c (0.1)
    auto tangentHeading = [&](int seg, float p, float fromX, float fromZ) -> float {
        float q = p + kTan;
        if (1.0f < q) { q -= 1.0f; if (++seg == count) seg = 0; }
        float tx, tz;  CatmullRom(spline, seg, q, &tx, &tz);
        return Heading004233e0(tx - fromX, tz - fromZ);
    };
    const float h0 = tangentHeading(nearest, param, px, pz);  // separate index, fVar6 untouched

    int   seg = nearest;
    float acc = 0.0f, lastX = px, lastZ = pz;
    auto step = [&]() {
        param += kWalk;
        if (1.0f < param) { param -= 1.0f; if (++seg == count) seg = 0; }
        float wx, wz;  CatmullRom(spline, seg, param, &wx, &wz);
        const float a = wx - lastX, b = wz - lastZ;
        lastX = wx; lastZ = wz;
        acc = a * a + b * b + acc;                             // SQUARED chord, as decompiled
    };
    const float half = dist * 0.5f;                            // _DAT_005cc32c
    if (0.0f < half) { do { step(); } while (acc < half); }
    // h1 = tangent here: its only consumer is the discarded float10 return, but the
    // original ADVANCES param/seg by +0.01 before walking on (0x00443b..), so do the same.
    param += kTan; if (1.0f < param) { param -= 1.0f; if (++seg == count) seg = 0; }
    { float tx, tz; CatmullRom(spline, seg, param, &tx, &tz); (void)tx; (void)tz; }
    while (acc < dist) step();
    const float h2 = tangentHeading(seg, param, lastX, lastZ);
    float out = h0 - h2;
    while (out < 0.0f) out += 360.0f;                          // _DAT_005ccac4
    return out;
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
// ---- FUN_00417640 (PostStepPowerupBrake) additional constant (memory_read pool14 2026-07-04) ----
const float kPowerupBrakeRateGate = 100.0f; // _DAT_005cc568
// ---- FUN_00417180 (BankSwitch) additional constant (memory_read pool14 2026-07-04) ----
const float kBankRandGate  = 0.5f;       // _DAT_005cc32c
// ---- ControlStep m==5/m==2 tail constants (memory_read Mashed_pool13, session
// d643c6ce5d6446ac8121c86ef902b00b, 2026-07-04; cited at FUN_00416250 0x004168a4+,
// cross-checked against Ai/AiControlStep.cpp's verbatim .asi-hook reference port) ----
const float kMode5AccelGate = 4.0f;      // _DAT_005cc35c
const float kMode5BrakeGate = 6.0f;      // _DAT_005cd0a0
const float kMode2AbsDotHi  = 0.9f;      // _DAT_005cc9c8
const float kMode2AbsDotLo  = 0.8f;      // _DAT_005cc9bc
// ---- FUN_004177b0 (AI pre-tick: race-metric + rubber-banding) constants
// (memory_read Mashed_pool13, session d643c6ce5d6446ac8121c86ef902b00b, 2026-07-04;
// cross-checked against re/analysis/race_rules_d1/0x004177b0.md + Ai/AiPreTick.cpp's
// verbatim .asi-hook reference port. This is the byte-level decode U-8993 asked for
// [U-8992]/[U-8993] stay open as-is — semantic, non-blocking.) ----
const float kRaceMetricScale = 0.01f;    // _DAT_005cc328  (lap-fraction scale)
const float kFinishThreshold = 3.0f;     // _DAT_005cc31c  (finish-order append gate)
const float kSlotSentinel    = -1.0f;    // _DAT_005cc33c  (empty finish-slot sentinel)
const float kMode9Thresh1    = 1.5f;     // _DAT_005cc348
const float kMode9Thresh2    = 0.75f;    // _DAT_005cc950  (shared w/ mode-4 formula)
const float kMode9Thresh3    = 0.025f;   // _DAT_005cc9a4
const float kMode9Thresh4    = 2.0f;     // _DAT_005cc574  (shared w/ slow-line band-0)
const float kMode4Mul1       = 0.1f;     // _DAT_005cc56c
const float kMode4Mul2a      = 0.16666667f; // _DAT_005cc8f4 (~1/6)
const float kMode4Mul2b      = 0.33333334f; // _DAT_005ccac8 (~1/3)
const float kMode4SubHi      = 0.15f;    // _DAT_005cc8f0
const float kMode4SubLo      = 0.25f;    // _DAT_005cc564
const float kTickScale       = 0.00033333333f; // _DAT_005cc948 (~1/3000)
const float kSlowBand1       = 12.0f;    // _DAT_005cc354
const float kSlowBand2       = 22.0f;    // _DAT_005cd0f8
const float kSlowBand3       = 42.0f;    // _DAT_005cd0f4
const float kSlowBand4Lo     = 60.0f;    // _DAT_005cc728
const float kSlowBand4Hi     = 62.0f;    // _DAT_005cd0f0
// per-vehicle state arrays (ORIGINAL image-pad addresses; writable in the exe):
//   stride 0x14: 0x008032d8 hist(other) / 0x008032dc hist(this) / 0x008032e0 prevSpeed
//   stride 0x74: 0x0089a4ec timerState / 0x0089a4f0 timerStart / 0x0089a4f4 storedSteer
//                0x0089a52c behaviour mode
//   0x007f0ff4 frame counter (host-ticked) ; 0x007f0ff8 race timer
inline std::uintptr_t a14(std::uintptr_t b, int v){ return b + (std::uintptr_t)v*0x14u; }
inline std::uintptr_t a74(std::uintptr_t b, int v){ return b + (std::uintptr_t)v*0x74u; }
// FUN_004a2c48 [D3 2026-09-26 CORRECTED]: the listing (0x004a2c48..) is FST + FISTP
// qword (current rounding) + FILD + FSUBP and an ADC correction on the residual's sign,
// i.e. the MSVC truncating __ftol: C float->int, toward zero. Not ROUND. Measured
// consequence in the original: accel 0x40 * 0.4 = 25.6 is stored as 25
// (verify/d3_ai_20260926/o_spread3.msd.aistep.csv). Callers store AL (low byte); every
// ctrl-byte operand here is already clamped to [0,255] by the listing.
inline std::uint8_t RoundST0(float x){
    long r = static_cast<long>(x);
    if (r < 0) r = 0; if (r > 255) r = 255;
    return (std::uint8_t)r;
}
inline long Ftol(float x) { return static_cast<long>(x); }

// U-D3-AIRAND RESOLVED 2026-09-27: the ring does not have to be read out of a live
// original — FUN_00534990 builds it from a hard-coded seed with no entropy input.
// [D3 2026-09-27] FUN_00534870 + its opener FUN_00534990, ported VERBATIM. The LCG
// stand-in is gone. Measured cause for replacing it (D3_AI_RESIDUE_2026-09-27.md §3):
// FUN_004177b0's one-shot band-0 roll is a 20% event, the original won it in every
// capture taken this session and the stand-in lost it deterministically on every run,
// which is what left DAT_0089a368 at 0 through the whole criterion-(b) window.
//
// State block, at `S = DAT_007dc578 + DAT_007d3ff8` in the original (0x00534876 on):
//   S+0x0 ring base, S+0x4 cursor p, S+0x8 cursor q, S+0xc ring end.
// FUN_00534990 allocates 0x7c bytes = 31 dwords (0x005349a0), seeds
//   ring[0] = 0x9a319039                                        (0x005349e2)
//   ring[i] = ring[i-1] * 0x41c64e6d + 0x3039, i = 1..30        (0x005349fb..0x00534a07)
// sets p = base + 0xc, q = base, end = base + 0x7c (0x005349bd..0x005349da, repeated at
// 0x00534a0e), then discards 0x136 = 310 draws (0x00534a22..0x00534a2f). There is NO
// entropy input: the original's stream is fixed from engine init, so this is reproducible
// standalone. What is NOT reproducible is the CALL INDEX — the original's other callers
// (FUN_00472690, FUN_004b44f0, FUN_004b4510) draw from the same ring — so this makes the
// distribution faithful, not the individual draw.
std::uint32_t s_rw_ring[31];
std::uint32_t* s_rw_p   = nullptr;
std::uint32_t* s_rw_q   = nullptr;
std::uint32_t* s_rw_end = nullptr;

std::uint32_t RwRandomNext()                                    // FUN_00534870
{
    *s_rw_p += *s_rw_q;                                         // 0x00534876..0x00534884
    std::uint32_t u = *s_rw_p;                                  // 0x00534889
    ++s_rw_p;                                                   // 0x0053488e
    u >>= 1;                                                    // 0x00534893
    if (s_rw_end <= s_rw_p) {                                   // 0x0053489b
        s_rw_p = s_rw_ring;                                     // 0x005348a3
        ++s_rw_q;                                               // 0x005348ac (NOT wrapped here)
        return u;
    }
    ++s_rw_q;                                                   // 0x005348b8
    if (s_rw_end <= s_rw_q) s_rw_q = s_rw_ring;                 // 0x005348c1
    return u;
}

void RwRandomOpen()                                             // FUN_00534990
{
    s_rw_ring[0] = 0x9a319039u;
    for (int i = 1; i < 31; ++i) s_rw_ring[i] = s_rw_ring[i - 1] * 0x41c64e6du + 0x3039u;
    s_rw_p = s_rw_ring + 3; s_rw_q = s_rw_ring; s_rw_end = s_rw_ring + 31;
    for (int i = 0; i < 0x136; ++i) RwRandomNext();
}

// FUN_00472650(lo, hi) = (hi - lo) * (u & 0x7fffffff) * _DAT_005cd314 + lo, with
// _DAT_005cd314 = 0x30000000 = 2^-31 and u = FUN_00534870(). Opened lazily once per
// process, as the original opens it once at engine init and never reseeds.
float AiRand(float lo, float hi)
{
    if (!s_rw_p) RwRandomOpen();
    const std::uint32_t u = RwRandomNext();
    return (hi - lo) * static_cast<float>(u & 0x7fffffffu) * 4.656612873e-10f + lo;
}

// ===========================================================================
// FUN_00415220 — AI power-up fire decision [D3 2026-09-26, decomp + listing
// 0x00415220..0x0041582a, pool0]. cdecl (spline, &target, &cand, v, curvInt). The
// stack slot of arg 4 is overwritten by FUN_0046d6d0 with the car speed (+0x9e4) at
// 0x00415244; arg 5 is __ftol of FUN_00443440's curvature (0x00416523). Every path
// returns 0 (XOR EAX,EAX before each RET), so the caller's mode-8 commit is dead in the
// original too. Writes ctrl[7] = 1 (MOV BYTE [EDI+7],1): the byte the power-up
// dispatcher reads as FIRE (0x0045bd72). Per-vehicle state, stride 0x74:
//   0x0089a51c held-time accumulator (+= DAT_007f1008), 0x0089a520 cooldown,
//   0x0089a524 held > 60000 flag, 0x0089a528 latch.
// Standalone inputs with no standalone writer, read from the image-pad exactly where
// the original reads them (so 0 here), each recorded in D3_AI_PORT_2026-09-26.md:
//   0x008989b0[v] (FUN_00442cc0; written by the unported FUN_00442a60),
//   0x0068ba00+v*0x58 (FUN_0045a0f0), 0x006885e0+v*0x2c (FUN_00455b40),
//   0x008a96dc+v*0x30c (FUN_00408af0).
// ===========================================================================
float RefDist(int c) { return (c < 4) ? F32(0x008989b0u + static_cast<std::uintptr_t>(c) * 4u) : 0.0f; } // FUN_00442cc0
int   RefDistZero0() { return RefDist(0) == 0.0f ? 1 : 0; }                                           // FUN_00415200
int   LeaderInRange(int v, float lo, float hi)                                                          // FUN_00415190
{
    if (RefDist(v) != 0.0f) return 0;
    int found = -1;
    for (int c = 0; c < 4; ++c) if (s_host.veh_type(c) == 1) found = c;   // FUN_0040e470(c) == 1
    if (found == -1) return 0;
    const float d = RefDist(found);
    return (lo <= d && d < hi) ? 1 : 0;
}
int PuField45a0f0(int v) { return (v >= 0 && v < 4) ? I32(0x0068ba00u + static_cast<std::uintptr_t>(v) * 0x58u) : -1; }
int PuField455b40(int v) { return I32(0x006885e0u + static_cast<std::uintptr_t>(v) * 0x2cu); }

void AiFireDecision(int v, std::uint8_t* ctrl, int curvInt)
{
    const float speed = s_host.veh_f32(v, 0x9e4);                      // FUN_0046d6d0 @0x00415244
    const std::uintptr_t acc = a74(0x0089a51cu, v), cd = a74(0x0089a520u, v);
    const std::uintptr_t lng = a74(0x0089a524u, v), lat = a74(0x0089a528u, v);
    const int dt = I32(0x007f1008u);
    const int t = I32(acc) + dt;  I32(acc) = t;                         // 0x00415254..0x00415264
    if (t > 60000) I32(lng) = 1;                                        // 0x0041525f / 0x00415271
    if (t < 3000)  I32(cd) = 3000;                                      // 0x00415277 / 0x0041527e
    if (I32(cd) != 0) { const int c = I32(cd) - dt; I32(cd) = (c < 0) ? 0 : c; }   // 0x00415288..0x0041529c
    const int type = s_host.held_powerup(v);                            // *[0x0088fc88 + v*0xb4]
    if (type == 0) return;                                              // 0x004152b6
    auto fire = [&]() { ctrl[7] = 1; };
    const float fc = static_cast<float>(curvInt);                       // FILD [ESP+0x44]
    switch (type) {                                                     // table 0x0041582c, types 7..19
    case 7:                                                             // MORTAR 0x004152d1
        if (I32(lng) == 0) {
            if (I32(cd) != 0) return;
            if (!RefDistZero0()) return;
            if (RefDist(v) <= 2.75f) return;                            // 0x005cd0c0
            if (25.0f <= fc) return;                                    // 0x005cc9e0
            const float r = AiRand(0.0f, 1.0f);
            const long row = Ftol(F32(0x0089a360u));                    // 0x00415335..0x0041533e
            const float thr = 1.0f - (10.0f - static_cast<float>(row)) * bits_to_f(0x3d888889u); // 0x005cd0bc
            if (thr <= r) { I32(cd) = 3000; return; }                   // 0x00415366 -> 0x00415733
        }
        fire(); I32(cd) = 750; return;                                  // 0x0041536c / 0x00415371 (0x2ee)
    case 9:                                                             // GUN 0x00415383
        if (I32(lat) != 0) { fire(); if (PuField45a0f0(v) != 0) I32(lat) = 0; return; }
        if (I32(lng) != 0) { I32(lat) = 1; return; }                    // 0x004157b4
        if (I32(cd) != 0 || !RefDistZero0() || PuField45a0f0(v) != 0) return;
        if (AiRand(0.0f, 1.0f) < 0.75f) I32(lat) = 1; else I32(cd) = 3000;   // 0x005cc950
        return;
    case 11:                                                            // MISSILE 0x00415421
        if (I32(lng) != 0) fire();
        if (I32(cd) != 0 || !RefDistZero0() || PuField45a0f0(v) != 0 || PuField455b40(v) == 0) return;
        if (AiRand(0.0f, 1.0f) < 0.75f) fire(); else I32(cd) = 6000;    // 0x00415627 (0x1770)
        return;
    case 17: {                                                          // SHOTGUN 0x00415499
        if (I32(lng) != 0) { fire(); I32(cd) = 500; return; }           // 0x00415593 (0x1f4)
        if (I32(cd) != 0) return;
        if (!LeaderInRange(v, 0.5f, 3.0f)) return;
        float p0x, p0z, pvx, pvz;
        s_host.own_xz(0, &p0x, &p0z);  s_host.own_xz(v, &pvx, &pvz);   // FUN_0046d4a0(0) / (v)
        const float dx = p0x - pvx, dz = p0z - pvz;
        const float len = std::sqrt(dx * dx + dz * dz);                 // FUN_004c3ac0 (y = 0)
        const float nx = (len > 0.0f) ? dx / len : dx, nz = (len > 0.0f) ? dz / len : dz; // FUN_004c39b0
        const std::uintptr_t pf = 0x008a96dcu + static_cast<std::uintptr_t>(v) * 0x30cu;   // FUN_00408af0
        const float val = -((F32(pf + 8) * nz + nx * F32(pf) + F32(pf + 4) * 0.0f) * len);
        if (!(-0.75f < val && val < 0.75f)) return;                     // 0x005cd0b0 / 0x005cc950
        if (0.9f <= AiRand(0.0f, 1.0f)) { I32(cd) = 3000; return; }     // 0x005cc9c8
        fire(); I32(cd) = 500; return;
    }
    case 10:                                                            // DRUM 0x004155aa
        if (I32(lng) == 0) {
            if (I32(cd) != 0) return;
            if (speed <= 2000.0f) return;                               // 0x005cd0b8
            if (!LeaderInRange(v, 4.0f, 10.0f)) return;
            if (15.0f <= fc) return;                                    // 0x005cc9b0
            if (!(AiRand(0.0f, 1.0f) < 0.9f)) { I32(cd) = 6000; return; }
        }
        fire(); I32(cd) = 1500; return;                                 // 0x004156a2 (0x5dc)
    case 12:                                                            // P_MINE 0x0041563a
        if (I32(lng) == 0) {
            if (I32(cd) != 0) return;
            if (speed <= 2000.0f) return;
            if (!LeaderInRange(v, 4.0f, 10.0f)) return;
            if (0.75f <= AiRand(0.0f, 1.0f)) { I32(cd) = 9000; return; } // 0x0041580d (0x2328)
        }
        fire(); I32(cd) = 1500; return;
    case 16:                                                            // R_FLAME 0x004156b9
        if (I32(lat) != 0) { fire(); return; }                          // 0x00415793
        if (I32(lng) != 0) { I32(lat) = 1; return; }
        if (I32(cd) != 0 || speed <= 2000.0f || !LeaderInRange(v, 3.0f, 10.0f)) return;
        if (AiRand(0.0f, 1.0f) < 0.5f) I32(lat) = 1; else I32(cd) = 3000;    // 0x005cc32c
        return;
    case 18:                                                            // FLASH 0x00415746
        if (I32(lng) != 0) fire();
        if (I32(cd) != 0) return;
        if (fc <= 80.0f) return;                                        // 0x005cc730
        if (0.5f <= AiRand(0.0f, 1.0f)) { I32(cd) = 9000; return; }
        fire(); return;
    case 19:                                                            // OIL 0x004157a0
        if (I32(lat) != 0) { fire(); return; }
        if (I32(lng) != 0) { I32(lat) = 1; return; }
        if (I32(cd) != 0) return;
        if (fc <= 80.0f) return;
        if (AiRand(0.0f, 1.0f) < 0.5f) I32(lat) = 1; else I32(cd) = 9000;
        return;
    default: return;                                                    // 8, 13..15 -> 0x00415822
    }
}

// ===========================================================================
// FUN_00416250 — primary per-vehicle control step [D3 2026-09-26: re-ported against
// the listing 0x00416250..0x00416a2e, pool0; D3_AI_PORT_2026-09-26.md section 2].
// Stack frame after the prologue (0x00416250..0x004162bb): [+0x10] mode (local_48),
// [+0x14] err, [+0x18] curvature (FUN_00443440 out, folded to [0,180] at 0x004162be),
// [+0x1c] FUN_0046d6d0 = rec+0x9e4 (speed), [+0x20] FUN_0046d6a0 = rec+0xb0c,
// [+0x24] FUN_0040e350 sub-state. The x87 stack across the bands was traced by hand:
// FUN_00415e20 leaves err in ST0 (FST, not FSTP, at 0x0041659e), a 0 is pushed above it
// (0x004165a5) and replaced by a history value X; the accel rule at 0x00416818 compares
// X (not err) with 20, the rules at 0x0041683e/0x00416863 compare err.
// Targeting helpers (modes 1..10) remain STUBBED -> mode stays 0.
// ===========================================================================
void ControlStep(std::uintptr_t spline, int v, std::uint8_t* ctrl)
{
    const int gameMode = s_host.game_sub_mode();                     // FUN_0040e350 -> [+0x24]
    const float rate0 = s_host.veh_f32(v, 0xb0c);                     // FUN_0046d6a0 -> [+0x20]
    const float speed = s_host.veh_f32(v, 0x9e4);                     // FUN_0046d6d0 -> [+0x1c]
    float ownX, ownZ;  s_host.own_xz(v, &ownX, &ownZ);                 // FUN_0046d4a0 +0x30/+0x38

    float curv = SplineCurvature(spline, ownX, ownZ, 10.0f);          // FUN_00443440 @0x004162b0
    if (kSteerSplit < curv) curv = kWrap - curv;                       // 0x004162be..0x004162d5

    float look[2] = { ownX, ownZ };
    SplineLookahead(spline, ownX, ownZ, v, look);                     // FUN_004161e0 -> FUN_00443dc0(...,1,0)
    const float tx = look[0], tz = look[1];

    int mode = 0;                     // targeting chain FUN_00414570.. STUBBED (all return 0)
    if (gameMode == 6 && s_host.ai_target_enable() == 0) {            // 0x0041649b / 0x004164a8
        // mode == 0: FUN_004148b0 / FUN_00415020 are stubbed (return 0).
        if (s_host.held_powerup(v) == 0) {                            // 0x0041651f -> 0x00416565
            I32(a74(0x0089a51cu, v)) = 0;
            I32(a74(0x0089a520u, v)) = 0;
            I32(a74(0x0089a524u, v)) = 0;
        } else {
            AiFireDecision(v, ctrl, static_cast<int>(Ftol(curv)));    // 0x00416523..0x00416539
            // returns 0 on every path, so the mode-8 commit at 0x0041655b is unreachable
        }
    }
    I32(a74(0x0089a52cu, v)) = mode;                                  // 0x00416590

    const float err = SteerAngleErrorFwd(v, tx, tz);                  // FUN_00415e20 @0x00416596
    const int   frame = I32(0x007f0ff4u);
    // [D3 2026-09-27] mirror of the original-side probe at 0x004165a5 (esp == locals
    // base there): the same six floats + mode, so the two step dumps stay column-for-
    // column comparable. Diagnostic only; no state the ported logic reads.
    RecordStepLocals(v, tx, tz, curv, err, mode, ownX, ownZ);
    float X = 0.0f;                                                    // x87 value pushed at 0x004165a5

    // ---- steer, err < 180 (0x004165c0..0x004166b4) ----
    if (err < kSteerSplit) {
        const float h = F32(a14(0x008032dcu, v));
        F32(a14(0x008032d8u, v)) = 360.0f;                            // 0x004165cc
        if (h < err) X = h;                                           // 0x004165d6..0x004165e3
        F32(a14(0x008032dcu, v)) = err;                               // 0x004165f7
        if (kSteerDeadband < err) {                                   // 0x004165f1..0x00416602
            const int el = frame - I32(a74(0x0089a4f0u, v));
            if (I32(a74(0x0089a4ecu, v)) == 1 || el >= kSettleFrames) {   // 0x00416608 / CMP 0xc8
                float m = err * speed * kSteerMagScale;                // 0x00416648..0x00416656
                if (mode == 0 && k20f < curv) m = m * (curv * kSteerExtra); // 0x0041665c..0x00416679
                if (!(m <= kSteerMagClamp)) m = kSteerMagClamp;        // 0x0041667b..0x0041668a
                ctrl[0] = RoundST0(m);                                 // 0x00416697
                I32(a74(0x0089a4f4u, v)) = Ftol(m);                    // 0x004166a4
                I32(a74(0x0089a4ecu, v)) = 1;
                I32(a74(0x0089a4f0u, v)) = frame;
            } else {
                const float cs = static_cast<float>(kSettleFrames - el) * kCounterScale
                                 * static_cast<float>(I32(a74(0x0089a4f4u, v)));  // FILD/FMUL/FIMUL
                ctrl[1] = RoundST0(cs);                                // 0x00416643
            }
        }
    }
    // ---- steer, err > 180 (0x004166cf..0x004167cf) ----
    if (kSteerSplit < err) {
        const float h = F32(a14(0x008032d8u, v));
        F32(a14(0x008032dcu, v)) = 0.0f;                              // 0x004166db
        if (err < h) X = kWrap - h;                                   // 0x004166e5..0x004166f8
        F32(a14(0x008032d8u, v)) = err;                               // 0x0041670c
        if (err < kSteer359) {                                        // 0x00416706 / 0x00416717
            const int el = frame - I32(a74(0x0089a4f0u, v));
            if (I32(a74(0x0089a4ecu, v)) == 2 || el >= kSettleFrames) {
                float m = (kWrap - err) * speed * kSteerMagScale;      // 0x0041675c..0x00416770
                if (mode == 0 && k20f < curv) m = m * (curv * kSteerExtra);
                if (!(m <= kSteerMagClamp)) m = kSteerMagClamp;
                ctrl[1] = RoundST0(m);                                 // 0x004167b1
                I32(a74(0x0089a4f4u, v)) = Ftol(m);
                I32(a74(0x0089a4ecu, v)) = 2;
                I32(a74(0x0089a4f0u, v)) = frame;
            } else {
                const float cs = static_cast<float>(kSettleFrames - el) * kCounterScale
                                 * static_cast<float>(I32(a74(0x0089a4f4u, v)));
                ctrl[0] = RoundST0(cs);                                // 0x00416758
            }
        }
    }

    // ---- accel / brake (0x004167d5..0x0041688b) ----
    ctrl[4] = 0xff;
    const float prev0 = F32(a14(0x008032e0u, v));
    F32(a14(0x008032e0u, v)) = rate0;                                 // 0x004167eb
    if (kBrakeSpeedDel < rate0 - prev0 && kBrakeMinSpeed < rate0) { ctrl[4] = 0; ctrl[5] = 0xff; }
    if (k20f < X && kRate1Brake < speed) { ctrl[4] = 0; ctrl[5] = 0xff; }   // 0x00416818..0x0041683a
    if (err < kSteerSplit && kAccelErrLo < err) { ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[0] = 0xff; }
    if (kSteerSplit < err && err < kAccelErrHi) { ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[1] = 0xff; }

    // ---- behaviour-mode tails (0x0041688d..0x004169de); mode stays 0 today ----
    const int m = I32(a74(0x0089a52cu, v));
    if (m == 7) { ctrl[4] = 0x40; }
    else if (m == 5) {
        const float m5 = F32(a74(0x0089a4e8u, v));
        if (!(m5 <= kMode5AccelGate)) ctrl[4] = 0;
        if (!(m5 <= kMode5BrakeGate)) ctrl[5] = 0x40;
        const int q = I32(0x007f0ff8u) / 0x3c;
        if (q & 0x20) ctrl[0] = 0xff; else ctrl[1] = 0xff;
    }
    else if (m == 9) {
        if (kMode9BrakeA < speed) { ctrl[4] = 0; ctrl[5] = 0xff; }
        if (kMode9BrakeB < speed) { ctrl[4] = 0; }
    }
    else if (m == 2) {
        const std::uintptr_t pf = 0x008a96dcu + static_cast<std::uintptr_t>(v) * 0x30cu; // FUN_00408af0
        const float pf0 = F32(pf), pf1 = F32(pf + 4), pf2 = F32(pf + 8);
        float fx, fz; s_host.own_fwd_xz(v, &fx, &fz);                 // FUN_0046d510 (forward, y not held)
        const float dot   = fz * pf2 + fx * pf0 + pf1 * 0.0f;
        const float cross = fx * pf2 - fz * pf0;
        const float absdot = (dot < kZeroF) ? -dot : dot;
        if (cross < kZeroF) {
            if (absdot < kMode2AbsDotHi) ctrl[1] = 0;
            if (absdot < kMode2AbsDotLo) ctrl[0] = 0xff;
        } else {
            if (absdot < kMode2AbsDotHi) ctrl[0] = 0;
            if (absdot < kMode2AbsDotLo) ctrl[1] = 0xff;
        }
    }

    // ---- 0x004169e0: difficulty flag == 1 -> accel * 0.4 (0x005ccac0), truncated ----
    if (I32(0x0089a368u) == 1) ctrl[4] = RoundST0(static_cast<float>(ctrl[4]) * 0.4f);

    // ---- final sub-state gate (0x00416a03..0x00416a24) ----
    if (gameMode != 6 && gameMode != 5 && gameMode != 9 &&
        gameMode != 10 && gameMode != 11) {
        ctrl[4] = 0; ctrl[5] = 0;
    }
}

// ===========================================================================
// FUN_00416a30 — control-step mode-4/9 variant (hooks.csv: AiControlStepM49,
// C3 2026-07-02; verbatim decomp+disasm re/analysis/wsr6_port_prep/
// 0x00416a30_decomp.md). Verified instruction-identical to FUN_00416250 for
// the STEER/ACCEL/BRAKE bands and the mode-7/9 tails; the only differences
// are in the targeting chain (no mode-10 block, mode-6 uses FUN_00415220
// instead of FUN_004148b0, NO commit to 0x0089a52c) — all downstream of the
// SAME predicate/LOS helpers (FUN_00414570/15880/14a70/14c30/14f00/148b0/
// 15220/16060/15d00) already STUBBED above (ControlStep's targeting chain
// forces mode=0 via SplineLookahead in their place). Ported here with the
// identical simplification, so the ONE portable, non-stubbed structural
// delta vs ControlStep is: this variant never writes the behaviour-mode
// record. Kept as an independent copy (not templated with ControlStep) so
// editing one cannot silently change the other's already-tuned behavior.
// ===========================================================================
void ControlStepM49(std::uintptr_t spline, int v, std::uint8_t* ctrl)
{
    const int gameMode = s_host.game_sub_mode();
    float ownX, ownZ;  s_host.own_xz(v, &ownX, &ownZ);
    float vx, vz;      s_host.own_vel_xz(v, &vx, &vz);
    const float speed = std::sqrt(vx*vx + vz*vz);
    const float rate1 = 0.0f;                        // [U-C-RATE1] (shared w/ ControlStep)

    float look[2] = { ownX, ownZ };
    SplineLookahead(spline, ownX, ownZ, v, look);
    const float tx = look[0], tz = look[1];
    // (no commit to 0x0089a52c here — the real delta vs ControlStep)

    const float err = SteerAngleError(v, tx, tz);
    const int   frame = I32(0x007f0ff4u);

    // ---- STEER bands (instruction-identical to ControlStep) ----
    if (err < kSteerSplit) {
        F32(a14(0x008032d8u, v)) = 360.0f;
        F32(a14(0x008032dcu, v)) = err;
        if (err > kSteerDeadband) {
            const int st = I32(a74(0x0089a4ecu, v));
            const int el = frame - I32(a74(0x0089a4f0u, v));
            if (st == 1 || el > kSettleFrames - 1) {
                float mag = err * speed * kSteerMagScale;
                if (rate1 <= k20f) mag *= kSteerExtra;
                if (mag > kSteerMagClamp) mag = kSteerMagClamp;
                ctrl[0] = RoundST0(mag);
                I32(a74(0x0089a4f4u, v)) = Ftol(mag);
                I32(a74(0x0089a4ecu, v)) = 1;
                I32(a74(0x0089a4f0u, v)) = frame;
            } else {
                float cs = (float)(kSettleFrames - el) * kCounterScale
                           * (float)I32(a74(0x0089a4f4u, v));
                ctrl[1] = RoundST0(cs);
            }
        }
    }
    if (err > kSteerSplit) {
        F32(a14(0x008032dcu, v)) = 0.0f;
        F32(a14(0x008032d8u, v)) = err;
        if (err < kSteer359) {
            const int st = I32(a74(0x0089a4ecu, v));
            const int el = frame - I32(a74(0x0089a4f0u, v));
            if (st == 2 || el > kSettleFrames - 1) {
                float mag = err * speed * kSteerMagScale;
                if (rate1 <= k20f) mag *= kSteerExtra;
                if (mag > kSteerMagClamp) mag = kSteerMagClamp;
                ctrl[1] = RoundST0(mag);
                I32(a74(0x0089a4f4u, v)) = Ftol(mag);
                I32(a74(0x0089a4ecu, v)) = 2;
                I32(a74(0x0089a4f0u, v)) = frame;
            } else {
                float cs = (float)(kSettleFrames - el) * kCounterScale
                           * (float)I32(a74(0x0089a4f4u, v));
                ctrl[0] = RoundST0(cs);
            }
        }
    }

    // ---- ACCEL / BRAKE bands (instruction-identical to ControlStep) ----
    ctrl[4] = 0xff;
    const float prevSpeed = F32(a14(0x008032e0u, v));
    F32(a14(0x008032e0u, v)) = speed;
    if (kBrakeSpeedDel < speed - prevSpeed && kBrakeMinSpeed < speed) {
        ctrl[4] = 0; ctrl[5] = 0xff;
    }
    if (k20f < err && kRate1Brake < rate1) { ctrl[4] = 0; ctrl[5] = 0xff; }
    if (err < kSteerSplit && kAccelErrLo < err) {
        ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[0] = 0xff;
    }
    if (kSteerSplit < err && err < kAccelErrHi) {
        ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[1] = 0xff;
    }

    // ---- mode-6 block (asm 0x00416c51..): the DAT_0088fc88==0 zero-fields
    // path is portable (no stub needed); the else-path needs FUN_00415220
    // ("powerup activation", STUBBED above) -> TODO.
    if (gameMode == 6 && s_host.ai_target_enable() == 0) {
        if (I32(0x0088fc88u + static_cast<std::uintptr_t>(v) * 0xb4u) == 0) {
            I32(a74(0x0089a51cu, v)) = 0;
            I32(a74(0x0089a520u, v)) = 0;
            I32(a74(0x0089a524u, v)) = 0;
        }
        // else: FUN_00415220 mode-8 activation — STUB (TODO), needs targeting chain.
    }

    // ---- behaviour-mode tails: mode is always 0 in this instantiation
    // (targeting stubbed; this variant never commits anyway) — m==7/9 tails
    // are unreachable here, matching ControlStep's own dead-tail note.
    const int m = I32(a74(0x0089a52cu, v));
    if (m == 7) { ctrl[4] = 0x40; }
    else if (m == 9) {
        if (kMode9BrakeA < rate1) { ctrl[4] = 0; ctrl[5] = 0xff; }
        if (kMode9BrakeB < rate1) { ctrl[4] = 0; }
    }

    // ---- final game-mode gate ----
    if (gameMode != 6 && gameMode != 5 && gameMode != 9 &&
        gameMode != 10 && gameMode != 11) {
        ctrl[4] = 0; ctrl[5] = 0;
    }
}

// ===========================================================================
// FUN_00417da0 — control-step mode-8 variant (hooks.csv: AiControlStepM8, C3
// 2026-07-02; verbatim decomp+disasm re/analysis/wsr6_port_prep/
// 0x00417da0_decomp.md). Verified instruction-identical to FUN_00416250 for
// the STEER/ACCEL/BRAKE bands and the mode-7/9 tails (hooks.csv: "CALL
// 0x00417cf0 replaces CALL 0x004148b0... debug-override block ABSENT...
// mode-10 block + mode commit PRESENT"). FUN_00417cf0 (mode-8 predicate) and
// the mode-10 block (FUN_00414f00) are downstream of the same STUBBED
// targeting-chain predicates as ControlStep, so they are omitted here with
// the same simplification (mode forced 0 via SplineLookahead). With that
// simplification applied uniformly, the ONLY portable structural delta left
// vs ControlStepM49 is the mode commit (present here, matching ControlStep;
// absent in ControlStepM49) — this function is otherwise identical to
// ControlStepM49's body.
// ===========================================================================
void ControlStepM8(std::uintptr_t spline, int v, std::uint8_t* ctrl)
{
    const int gameMode = s_host.game_sub_mode();
    float ownX, ownZ;  s_host.own_xz(v, &ownX, &ownZ);
    float vx, vz;      s_host.own_vel_xz(v, &vx, &vz);
    const float speed = std::sqrt(vx*vx + vz*vz);
    const float rate1 = 0.0f;                        // [U-C-RATE1] (shared w/ ControlStep)

    float look[2] = { ownX, ownZ };
    SplineLookahead(spline, ownX, ownZ, v, look);
    const float tx = look[0], tz = look[1];
    I32(a74(0x0089a52cu, v)) = 0;                    // commit behaviour mode (present for this variant)

    const float err = SteerAngleError(v, tx, tz);
    const int   frame = I32(0x007f0ff4u);

    // ---- STEER bands (instruction-identical to ControlStep) ----
    if (err < kSteerSplit) {
        F32(a14(0x008032d8u, v)) = 360.0f;
        F32(a14(0x008032dcu, v)) = err;
        if (err > kSteerDeadband) {
            const int st = I32(a74(0x0089a4ecu, v));
            const int el = frame - I32(a74(0x0089a4f0u, v));
            if (st == 1 || el > kSettleFrames - 1) {
                float mag = err * speed * kSteerMagScale;
                if (rate1 <= k20f) mag *= kSteerExtra;
                if (mag > kSteerMagClamp) mag = kSteerMagClamp;
                ctrl[0] = RoundST0(mag);
                I32(a74(0x0089a4f4u, v)) = Ftol(mag);
                I32(a74(0x0089a4ecu, v)) = 1;
                I32(a74(0x0089a4f0u, v)) = frame;
            } else {
                float cs = (float)(kSettleFrames - el) * kCounterScale
                           * (float)I32(a74(0x0089a4f4u, v));
                ctrl[1] = RoundST0(cs);
            }
        }
    }
    if (err > kSteerSplit) {
        F32(a14(0x008032dcu, v)) = 0.0f;
        F32(a14(0x008032d8u, v)) = err;
        if (err < kSteer359) {
            const int st = I32(a74(0x0089a4ecu, v));
            const int el = frame - I32(a74(0x0089a4f0u, v));
            if (st == 2 || el > kSettleFrames - 1) {
                float mag = err * speed * kSteerMagScale;
                if (rate1 <= k20f) mag *= kSteerExtra;
                if (mag > kSteerMagClamp) mag = kSteerMagClamp;
                ctrl[1] = RoundST0(mag);
                I32(a74(0x0089a4f4u, v)) = Ftol(mag);
                I32(a74(0x0089a4ecu, v)) = 2;
                I32(a74(0x0089a4f0u, v)) = frame;
            } else {
                float cs = (float)(kSettleFrames - el) * kCounterScale
                           * (float)I32(a74(0x0089a4f4u, v));
                ctrl[0] = RoundST0(cs);
            }
        }
    }

    // ---- ACCEL / BRAKE bands (instruction-identical to ControlStep) ----
    ctrl[4] = 0xff;
    const float prevSpeed = F32(a14(0x008032e0u, v));
    F32(a14(0x008032e0u, v)) = speed;
    if (kBrakeSpeedDel < speed - prevSpeed && kBrakeMinSpeed < speed) {
        ctrl[4] = 0; ctrl[5] = 0xff;
    }
    if (k20f < err && kRate1Brake < rate1) { ctrl[4] = 0; ctrl[5] = 0xff; }
    if (err < kSteerSplit && kAccelErrLo < err) {
        ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[0] = 0xff;
    }
    if (kSteerSplit < err && err < kAccelErrHi) {
        ctrl[4] = 0xff; ctrl[5] = 0xff; ctrl[1] = 0xff;
    }

    // ---- mode-6 block: the DAT_0088fc88==0 zero-fields path is portable;
    // the mode==0 inner block (FUN_00417cf0 predicate + FUN_00415020 spawn
    // check) and the else-path (FUN_00415220) need STUBBED targeting
    // predicates -> TODO.
    if (gameMode == 6 && s_host.ai_target_enable() == 0) {
        if (I32(0x0088fc88u + static_cast<std::uintptr_t>(v) * 0xb4u) == 0) {
            I32(a74(0x0089a51cu, v)) = 0;
            I32(a74(0x0089a520u, v)) = 0;
            I32(a74(0x0089a524u, v)) = 0;
        }
        // else: FUN_00415220 mode-8 activation — STUB (TODO), needs targeting chain.
    }

    // ---- behaviour-mode tails: mode is always 0 in this instantiation
    // (targeting stubbed) — m==7/9 tails are unreachable here, matching
    // ControlStep's own dead-tail note.
    const int m = I32(a74(0x0089a52cu, v));
    if (m == 7) { ctrl[4] = 0x40; }
    else if (m == 9) {
        if (kMode9BrakeA < rate1) { ctrl[4] = 0; ctrl[5] = 0xff; }
        if (kMode9BrakeB < rate1) { ctrl[4] = 0; }
    }

    // ---- final game-mode gate ----
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

// FUN_00414030(-1) — AiSplineBankTimerReset (hooks.csv C3): "sets
// DAT_008032d4[v*5]=1000; param=-1 resets all 5 slots". Every BankSwitch call
// site below passes -1, so only that path is ported (matches the C3 evidence:
// "param=-1 fill path UNTESTED by the harness" — U-7564, flagged
// data-semantic non-blocking). 0x008032d4 is the SAME per-vehicle spline-
// index-continuity field SplineLookahead's Phase 2 reads/writes; forcing it
// to 1000 (>= any real spline count) makes the next lookahead reseed to the
// true-nearest point, matching a "reset" semantic.
inline void SplineBankTimerReset() {
    for (int vv = 0; vv < 4; ++vv) {
        I32(0x008032d4u + static_cast<std::uintptr_t>(vv) * 0x14u) = 1000;
    }
}

// FUN_00472650(0, hiBits) — original PRNG (float10-ST0), NOT one of the 4
// ported RVAs. Substituted with std::rand()/RAND_MAX (documented
// approximation, same precedent as Normalize2's std::sqrt substitute for the
// original's fast-rsqrt LUT above). Only feeds BankSwitch's once-per-~9000-
// frame cosmetic spline-index variety roll — does not affect steer/accel/
// brake output.
// [D3 2026-09-26] now AiRand(0,1): the FUN_00472650 law with the LCG stand-in for u.
inline float RandUnit() { return AiRand(0.0f, 1.0f); }

// ===========================================================================
// FUN_00417180 — AI spline-bank switcher (hooks.csv: AiBankSwitch, C3
// 2026-07-02; verbatim decomp+disasm re/analysis/wsr6_port_prep/
// 0x00417180_decomp.md). Per-vehicle state machine: services a pending
// line-type switch request (race/inside/slow/cheat cycling with per-type
// count-check), runs the switch-in timer (up to 9000 host-tick units), then
// (once idle) applies the DAT_0089a368 slow-line-flag reset and a low-
// frequency (~every 9000 frames) random spline-index variation roll. Raw-asm
// corrections already folded in via the Ai/AiPreTick.cpp reference port: the
// type-0/type-3 branches call the timer-reset on BOTH count-check outcomes;
// type-2/type-1 test the line-type read ONCE into a local while type-0/
// type-3 RE-READ memory.
// ===========================================================================
void BankSwitch(int v)
{
    const std::uintptr_t s = static_cast<std::uintptr_t>(v) * 0x74u;

    if (I32(0x0089a500u + s) != 0) {                        // switch request pending
        const int t = I32(kAiLineType + s);                 // read ONCE (t==2/t==1 test this copy)
        I32(0x0089a504u + s) = 1;                            // start timer
        if (t == 2) {                                        // slow lines
            int idx = I32(kAiSplineIndex + s) + 1;
            I32(kAiSplineIndex + s) = idx;
            if (idx > 2) I32(kAiSplineIndex + s) = 0;
            SplineBankTimerReset();
        }
        if (t == 1) {                                        // inside lines
            int idx = I32(kAiSplineIndex + s) + 1;
            I32(kAiSplineIndex + s) = idx;
            if (idx > 2 || I32(kSplineInsideCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
                I32(kAiLineType + s) = 2;
                I32(kAiSplineIndex + s) = 0;
            }
        }
        if (I32(kAiLineType + s) == 0) {                     // race lines (RE-READS memory)
            int idx = I32(kAiSplineIndex + s) + 1;
            I32(kAiSplineIndex + s) = idx;
            if (idx > 2 || I32(kSplineRaceCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
                I32(kAiLineType + s) = 1;
                I32(kAiSplineIndex + s) = 0;
            }
            SplineBankTimerReset();                          // called on BOTH outcomes
        }
        if (I32(kAiLineType + s) == 3) {                     // cheat lines (RE-READS memory)
            int idx = I32(kAiSplineIndex + s) + 1;
            I32(kAiSplineIndex + s) = idx;
            if (idx > 2 || I32(kSplineCheatCnt + static_cast<std::uintptr_t>(idx) * kSplineStride) < 4) {
                I32(kAiLineType + s) = 0;
                I32(kAiSplineIndex + s) = 0;
            }
            SplineBankTimerReset();                          // called on BOTH outcomes
        }
        I32(0x0089a500u + s) = 0;                            // clear request
    }

    // switch-in timer
    const int timer = I32(0x0089a504u + s);
    if (timer != 0 && timer < 9000) {
        I32(0x0089a504u + s) = timer + I32(0x007f1008u);
        return;
    }
    I32(0x0089a504u + s) = 0;

    // line-type reset by DAT_0089a368 (shared slow-line-targeting flag; set by
    // FUN_004177b0 AiPreTick, out of scope — currently always 0 in the
    // standalone since AiPreTick isn't wired, so this always forces type=0).
    if (I32(0x0089a368u) == 0) I32(kAiLineType + s) = 0;
    if (I32(0x0089a368u) == 2) I32(kAiLineType + s) = 0;
    if (I32(0x0089a368u) == 1) {
        if (I32(kAiLineType + s) != 2) SplineBankTimerReset();
        I32(kAiLineType + s) = 2;
    }

    // low-frequency random spline-index variation
    const int period = I32(0x007f0ff8u) / 3000;
    if ((period & 0xf) == 0xf) {
        if (I32(0x0063bd90u) != period) {
            if (RandUnit() < kBankRandGate) {                // _DAT_005cc32c = 0.5
                I32(kAiSplineIndex + s) = I32(kAiSplineIndex + s) + 1;
                SplineBankTimerReset();
            }
        }
        I32(0x0063bd90u) = period;
    } else {
        I32(0x0063bd90u) = 0;
    }
}

// ===========================================================================
// FUN_00417640 — post-step powerup-brake override (hooks.csv:
// AiPostStepPowerupBrake, C3 2026-07-02; re/analysis/ai_update/
// 0x00417640.md). Gates: track_index()==0x21, PowerupRangeGet()<=30.0 (=
// _DAT_005cc72c == kAccelErrLo above), behaviour-mode != 10,
// Table88ff50Get(v)!=0. The active branch's displacement-to-target +
// Vec3Magnitude computation (FUN_00452160/FUN_0046d4a0/FUN_004c3ac0) is
// PROVABLY DEAD in the original — its C3-verified reference port
// (AiController.cpp::AiPostStepPowerupBrake) computes it but the result is
// never read afterward (discarded return, displacement floats never
// consumed by the branch below) — so it is OMITTED here, the same
// dead-artifact-elision precedent AiControlStep.cpp's own header documents
// ("FST [ESP+0x18] of the mode-2 dot product... not mirrored"). The
// rate-like float from FUN_0046d6d0 reuses ControlStep's [U-C-RATE1]=0.0f
// substitute (the SAME callee, not a new uncertainty): since the brake gate
// needs kPowerupBrakeRateGate (100.0, _DAT_005cc568) < rate and rate is
// pinned 0, the gate is provably always false — this function always coasts
// (ctrl[4]=0, ctrl[5]=0) once its predicate gates pass, pending
// FUN_0046d6d0's port. FUN_0046d570 (local_10, C2, not one of the 4 ported
// RVAs) is therefore a dead operand too and is not stubbed.
// ===========================================================================
void PostStepPowerupBrake(int v, std::uint8_t* ctrl)
{
    if (s_host.track_index() != 0x21) return;                       // FUN_00426c00 gate
    if (F32(0x00684de0u) > kAccelErrLo) return;                      // PowerupRangeGet > 30.0 -> skip
    if (I32(a74(0x0089a52cu, v)) == 10) return;                      // mode-10 skip
    if (I32(0x0088ff50u + static_cast<std::uintptr_t>(v) * 4u) == 0) return; // Table88ff50Get == 0 -> skip

    const float rate = 0.0f;   // FUN_0046d6d0 [U-C-RATE1] (shared substitute w/ ControlStep)
    if (kPowerupBrakeRateGate < rate) {   // local_10<90.0 term is dead (rate pinned 0)
        ctrl[4] = 0; ctrl[5] = 0xff;
        return;
    }
    ctrl[4] = 0; ctrl[5] = 0;
}

// ===========================================================================
// FUN_0040e480 — CarSlotStateSet (hooks.csv C2, drift-promoted 2026-05-14;
// re/analysis/c0_promotion_frontend_a/0x0040e480.md). Body: `*(undefined4 *)
// (PTR_PTR_005f2770 + param_1*4 + 0x34) = param_2;` -- ONE dereference of the
// global pointer at 0x005f2770 (all 40 references via `reference_to` this
// session are READ, never WRITE, so the pointer's value is a load-time .data
// constant, not something original CODE initializes -- safe to dereference
// directly in the image-pad, same as any other kSpline*/kAiState* address).
// U-3431 (struct identity at PTR_PTR_005f2770) stays open, non-blocking.
// ---------------------------------------------------------------------------
//
// [D3 2026-09-14] CORRECTION -- the note above is right about the ORIGINAL and wrong
// about the STANDALONE, and the difference is an access violation.
//
// Measured: the first wiring of Ai_Standalone_Tick into mashed_re.exe AV'd
// (0xC0000005) on the first in-race frame. The stage tracer (MASHED_AI_TICKTRACE)
// stopped at "pre-slotstate", i.e. inside this function. Cause: 0x005f2770 IS a
// load-time .data constant -- reading original/MASHED.exe.unpatched at file offset
// 0x1f2770 (section .data, raw_size 0x4d000, initialized on disk) gives
// *(uint32*)0x005f2770 == 0x005f2728 -- but the STANDALONE does not load the
// original's .data. Its image-pad owns the RVA range ZERO-FILLED (the same caveat
// AiState.h records for the DAT_005ccXXX tuning constants). So `base` is 0 here and
// the write lands at 0x34.
//
// This is NOT the same situation as the kSpline*/kAiState* addresses the original
// comment compares it to: those are direct addresses this port itself writes into
// the pad, whereas this one needs a VALUE that only the original's loaded image
// supplies. A pointer read out of the pad is never safe to dereference.
//
// Guarded, not faked: when the pad has no pointer there is no struct to write, so
// the poke is skipped. The original's effect (setting car slot v's state field at
// +0x34+v*4 of the 0x005f2728 table) is NOT reproduced standalone -- tracked as an
// open stub, see re/analysis/D3_AI_TICK_WIRING_2026-09-14.md.
inline void CarSlotStateSet(int v, std::int32_t state)
{
    const std::uintptr_t base = static_cast<std::uintptr_t>(U32(0x005f2770u));
    if (base == 0) return;   // standalone image-pad: no table (see note above)
    I32(base + static_cast<std::uintptr_t>(v) * 4u + 0x34u) = state;
}

// FUN_0046dd80/0046dd90 getter/setter pair used by AiPreTickRubberBand: reads
// the single global DAT_0061313c and round-trips it (optionally scaled) into
// car v's vehicle-struct field at 0x008815a0 + v*0xd04 + 0x154 (== 0x008816f4
// for v=0; Vehicle/VehicleStruct.h's `Vc::off::kGearTorque1`, WS-A1). Kept as
// local direct-address helpers (not a Host callback) since both addresses are
// fixed .data locations, consistent with this file's existing convention.
inline float GearConstGet()               { return F32(0x0061313cu); }
inline void  GearConstSet(int v, float f) { F32(0x008815a0u + static_cast<std::uintptr_t>(v) * 0xd04u + 0x154u) = f; }

// x87 ST0 round-to-int approximation (project convention for STANDALONE ports:
// RaceCamera.cpp's BankersRound / this file's own RoundST0 both use std::lround
// rather than reproducing FUN_004a2c48's exact CW-round + residual-correction
// bit pattern -- see Math/FPURound.cpp's header for why that's a HOOK-build-only
// bit-identity requirement, not a standalone one).
inline int RoundToInt(float x) { return static_cast<int>(std::lround(x)); }

// FUN_00472650(lo,hi) ranged-random float -- NOT one of the 4 ported RVAs for
// this session. Same deterministic stand-in precedent as ForceIntegratorStubs.cpp's
// Fi_RandRange (`return lo;`): only feeds the slow-line difficulty-flag probability
// roll below, not steer/accel/brake output.

// ===========================================================================
// FUN_004177b0 — AiPreTick: per-frame race-metric update (lap-count + lap-
// fraction per car, feeding the finish-order candidate slots for rules
// {4,7,8,9}) + mode-9/mode-4 AI speed rubber-banding + powerup-alive speed
// doubling + the DAT_0089a368 slow-line difficulty-flag state machine. Sole
// caller FUN_00418860, once per frame before the per-vehicle AI step loop
// (re/analysis/race_rules_d1/0x004177b0.md, C1; verbatim reference port
// Ai/AiPreTick.cpp for the .asi hook build, session 2026-07-02/07-04).
// [U-8992]/[U-8993]/[U-8994] stay open as-is (semantic/structural, Blocks:
// none) -- the constants those uncertainties left undecoded were memory_read
// this session (see the const block above) and are used directly below; that
// is a byte-level decode, not the semantic resolution U-8992 asks for.
// ===========================================================================
// [D3 2026-09-26] FUN_004177b0's two probability tables, int[row*5 + band], harvested
// from the ORIGINAL .data (memory_read pool0 0x005f30a0 / 0x005f3180, 0xe0 bytes each).
// The standalone image-pad holds zeros there, which with the old `return lo` random
// stand-in set DAT_0089a368 = 1 on every band change. Row = __ftol(DAT_0089a360);
// measured DAT_0089a360 = 2.5 in the original race (verify/d3_ai_20260926/o_inputs).
const int kDiffProb1[56] = { 60,80,90,90,100, 40,60,75,80,100, 20,40,60,75,100, 10,30,50,70,90,
    0,15,30,60,80, 0,15,20,50,70, 0,10,15,30,60, 0,10,15,20,50, 0,5,0,10,40, 0,5,0,0,30,
    0,0,0,0,20, 0 };
const int kDiffProb2[56] = { 10,10,10,10,0, 10,10,10,20,0, 10,10,15,30,0, 10,10,15,40,10,
    10,15,20,50,20, 10,15,20,60,30, 15,20,25,70,40, 15,20,30,70,50, 15,25,35,75,60,
    20,25,40,75,70, 20,30,50,75,80, 0 };

void AiPreTickRubberBand()
{
    if (s_host.ai_target_enable() != 0) I32(0x0089a368u) = 2;   // FUN_00443080 gate

    // ---- phase 2: race-angle array + finish-order candidate slots, v=0..3 ----
    int fd0 = 0;
    for (int v = 0; v < 4; ++v) {
        const float spd = GearConstGet();                        // FUN_0046dd80
        GearConstSet(v, spd);                                    // FUN_0046dd90 (round-trip)
        const int   a20 = I32(0x008a9648u + static_cast<std::uintptr_t>(v) * 0x30cu); // FUN_00407a20 lap counter
        const float a20f = static_cast<float>(a20);
        const float ra = F32(0x008a96ecu + static_cast<std::uintptr_t>(v) * 0x30cu);  // FUN_00408ad0
        fd0 = I32(kGameModeFd0);
        const float val = ra * kRaceMetricScale + a20f;
        F32(0x0089a880u + static_cast<std::uintptr_t>(v) * 4u) = val;                 // FUN_00417730 storage
        if (fd0 == 4 || fd0 == 9 || fd0 == 7 || fd0 == 8) {
            if (!(val < kFinishThreshold)) {
                const float fv = static_cast<float>(v);
                int recorded = 0;
                if (F32(0x0089a870u) == fv) recorded = 1;
                if (F32(0x0089a874u) == fv) recorded = 1;
                if (F32(0x0089a878u) == fv) recorded = 1;
                if (F32(0x0089a87cu) == fv) {
                    // already recorded in slot 4 -- nothing to do
                } else if (recorded == 0) {
                    if      (F32(0x0089a870u) == kSlotSentinel) F32(0x0089a870u) = fv;
                    else if (F32(0x0089a874u) == kSlotSentinel) F32(0x0089a874u) = fv;
                    else if (F32(0x0089a878u) == kSlotSentinel) F32(0x0089a878u) = fv;
                    else if (F32(0x0089a87cu) == kSlotSentinel) F32(0x0089a87cu) = fv;
                }
            }
        }
    }

    // ---- phase 3: mode-9 speed scaling, vehicle 1 ----
    if (fd0 == 9) {
        float mult = 1.0f;
        const float ang = F32(0x0089a880u + 4u);              // FUN_00417730(1)
        if (ang < kMode9Thresh1) mult = 0.95f;
        if (ang < kMode9Thresh2) mult = 0.9f;
        if (ang < kMode9Thresh3) mult = 0.25f;
        if (!(ang <= kMode9Thresh4)) mult = 1.15f;
        const float spd = GearConstGet();
        GearConstSet(1, spd * mult);
        fd0 = I32(kGameModeFd0);
    }

    // ---- phase 3b: mode-4 speed scaling, vehicles 1..3 ----
    if (fd0 == 4) {
        for (int v = 1; v < 4; ++v) {
            float mult = 1.0f;
            const float ang = F32(0x0089a880u + static_cast<std::uintptr_t>(v) * 4u);
            if (!(ang <= kMode9Thresh2)) {                     // 0.75 (shared address)
                mult = (kBrakeMinSpeed - F32(0x0089a360u)) * kMode4Mul1 * kMode4Mul2a + kSteerDeadband;
            }
            if (!(ang <= kMode9Thresh4)) {                     // 2.0 (shared address)
                mult = (kBrakeMinSpeed - F32(0x0089a360u)) * kMode4Mul1 * kMode4Mul2b + mult;
            }
            if (ang < kBankRandGate) {                          // 0.5 (shared address)
                mult = mult - kMode4SubHi;
            }
            if (ang < kSteerExtra) {                            // 0.05 (shared address)
                mult = mult - kMode4SubLo;
            }
            const float spd = GearConstGet();
            GearConstSet(v, spd * mult);
        }
    }

    // ---- phase 4: powerup-alive speed doubling ----
    bool any = false;
    for (int v = 0; v < 4; ++v) {
        if (I32(0x00688304u + static_cast<std::uintptr_t>(v) * 0x18u) != 0) any = true; // FUN_00454a30
    }
    if (any) {
        for (int v = 0; v < 4; ++v) {
            if (s_host.veh_type(v) == 2) {
                const float spd = GearConstGet();
                GearConstSet(v, spd + spd);
            }
        }
    }

    // ---- phase 5: slow-line difficulty-flag state machine ----
    const int m = I32(kGameModeFd0);
    if (m == 4 || m == 9) { I32(0x0089a368u) = 2; return; }
    if (m == 8)           { I32(0x0089a368u) = 0; return; }
    if (s_host.game_sub_mode() != 6) return;

    if (I32(0x0089a368u) == 2) {
        const int t = I32(0x0089a36cu) + I32(kOverrideStep);
        I32(0x0089a36cu) = t;
        if (t > 180000) { I32(0x0089a368u) = 1; I32(0x0089a36cu) = 0; }
    }

    const int row = static_cast<int>(Ftol(F32(0x0089a360u)));  // FUN_004a2c48 truncates (D3 2026-09-26)
    int ecx = 0, edx = 0;
    const float tickscale = static_cast<float>(I32(kFrame0ff8)) * kTickScale;
    const float diff = tickscale - F32(0x0089a370u);

    if (!(diff <= kSteerDeadband)) {                            // 1.0
        if (tickscale < kMode9Thresh4) { edx = 0; ecx = 1; }    // 2.0
    }
    if (!(diff <= kBrakeMinSpeed)) {                            // 10.0
        if (tickscale < kSlowBand1) { edx = 1; ecx = 1; }       // 12.0
        if (!(tickscale <= k20f)) {                             // 20.0
            if (tickscale < kSlowBand2) { edx = 2; ecx = 1; }   // 22.0
        }
        if (!(diff <= k20f)) {
            if (tickscale < kSlowBand3) { edx = 3; ecx = 1; }   // 42.0
            if (!(diff <= k20f)) {
                if (!(tickscale <= kSlowBand4Lo) && tickscale < kSlowBand4Hi) { // 60/62
                    edx = 4; ecx = 1;
                }
            }
        }
    }

    if (ecx != 0) {
        F32(0x0089a370u) = tickscale;
        I32(0x0089a374u) = edx;
        for (std::uintptr_t a = 0x0089a4c4u; a < 0x0089a694u; a += 0x74u) {
            I32(a + 4u) = 0;
            I32(a)      = 0;
        }
    }
    if (I32(0x0089a368u) != 0) return;
    if (ecx == 0) return;

    const float prob1 = static_cast<float>(kDiffProb1[(row * 5 + edx) % 56]);  // 0x005f30a0 + off
    const float rnd1 = AiRand(0.0f, 100.0f);                   // FUN_00472650(0,100.0f)
    if (!(rnd1 > prob1)) {
        I32(0x0089a368u) = 1;
        for (std::uintptr_t a = 0x0089a4c0u; a < 0x0089a690u; a += 0x74u) I32(a) = 0;
        return;
    }
    const float prob2 = static_cast<float>(kDiffProb2[(row * 5 + edx) % 56]);  // 0x005f3180 + off
    const float rnd2 = AiRand(0.0f, 100.0f);
    if (!(rnd2 > prob2)) {
        I32(0x0089a368u) = 2;
    }
}

// FUN_00413fe0 PROPER — the per-vehicle AI-state reset ONLY, with no clock zeroing.
// [GATEFIRE 2026-10-08] Split out of Ai_ResetRace so FUN_00418560's mode-5 branch can
// call exactly what the original calls there. The distinction is load-bearing:
// 0x0041859e calls FUN_00413fe0 and 0x00418598 zeroes ONLY 0x007f0ff8 — it does NOT
// touch 0x007f0ff4. Calling the bundled Ai_ResetRace from the mode-5 path would also
// zero 0x007f0ff4 every countdown frame and break the FUN_00416250 steer timer, which
// measures `el = DAT_007f0ff4 - start` against 200.
// Defined here (inside this TU's anonymous namespace) rather than forward-declared:
// Ai_ResetRace below is at namespace-Ai scope, so a declaration here would have
// created a SECOND function and an ambiguous call.
void Ai_ResetVehicleStates()
{
    I32(0x0089a36cu) = 0;
    for (int v = 0; v < 4; ++v) {
        const std::uintptr_t b = 0x0089a4f0u + static_cast<std::uintptr_t>(v) * 0x74u;
        I32(b - 4) = 0; I32(b) = 0; I32(b + 4) = 0;
        I32(b - 0x2c) = 0; I32(b - 0x28) = 0; I32(b - 0x24) = 0; I32(b - 0x20) = 0;
        I32(b + 0x18) = 0; I32(b + 0x2c) = 0; I32(b + 0x30) = 0; I32(b + 0x34) = 0; I32(b + 0x38) = 0;
        I32(0x008032d4u + static_cast<std::uintptr_t>(v) * 0x14u) = 1000;
    }
}

void VehicleStep(int v)
{
    int slot = I32(kSlotTableBase + static_cast<std::uintptr_t>(v) * kSlotTableStride);
    std::uint8_t* ctrl = reinterpret_cast<std::uint8_t*>(kCtrlBlockBase + static_cast<std::uintptr_t>(slot) * kCtrlBlockStride);
    ctrl[0] = ctrl[1] = ctrl[4] = ctrl[5] = ctrl[6] = ctrl[7] = 0;

    // ---- FUN_00418560 Branch A: mode 5 (0x0041858e..0x004185e4) ----------------
    // [GATEFIRE 2026-10-08] This branch was MISSING from the port: VehicleStep went
    // straight from the ctrl zeroing to BankSwitch. Its absence is why 0x007f0ff8
    // never re-zeroes standalone, which is the measured cause of the bias374
    // divergence (verify/d3_gatefire_20261008/RESULT_RAMP.md): the ORIGINAL re-zeroes
    // it about every 8 s so its tickscale never reaches the 11 s band-1 threshold,
    // while the port ramps monotonically with ZERO reversals in 13,498 frames and
    // parks in band 4. The band ladder itself is NOT at fault — both sides fire band 1
    // at the same ~11.0 s threshold.
    //
    // Plate: re/analysis/ai_update/0x00418560.md lines 28-34. Reference body with the
    // same shape: Ai/AiController.cpp:177-193 (.asi).
    //
    // DEVIATION, REGISTERED: FUN_0040e4a0 (ElapsedTimeGet) reads 0x005f29b8, which is
    // image .data the standalone never loads, so `elapsed` reads 0 here and the
    // countdown compare is always true while mode 5 holds. That makes the accel hold
    // permanent for the duration of mode 5 rather than releasing part-way. It does NOT
    // affect the 0x007f0ff8 zeroing this leg exists for, and mode 5 is transient.
    // Default-OFF behind MASHED_MODE5_RESET.
    {
        static const bool s_mode5 = (std::getenv("MASHED_MODE5_RESET") != nullptr);
        if (s_mode5) {
            const int subMode = s_host.game_sub_mode();          // FUN_0040e350 @0x0041858e
            if (subMode == 5) {                                  // 0x00418593
                I32(kFrame0ff8) = 0;                             // 0x00418598
                Ai_ResetVehicleStates();                         // FUN_00413fe0 @0x0041859e
                const int elapsed = I32(0x005f29b8u);            // FUN_0040e4a0 @0x004185a3
                const int cd = I32(a74(kAiMode5Countdown, v));   // 0x004185ab
                if (cd < 0) {                                    // 0x004185cb..0x004185e4
                    if (elapsed < (0x4a - cd) * 100) ctrl[4] = 0xff;
                } else {                                         // 0x004185b5..0x004185ca
                    if (elapsed < (cd + 0x40) * 100) ctrl[4] = 0xff;
                }
                return;                                          // mode 5 skips the whole step
            }
        }
    }

    BankSwitch(v);                            // FUN_00417180
    std::uintptr_t spline = SelectSpline(v);

    const int fd0 = s_host.game_mode_fd0();
    if (fd0 == 4 || fd0 == 9) {
        ControlStepM49(spline, v, ctrl);      // FUN_00416a30
    } else if (fd0 == 8) {
        ControlStepM8(spline, v, ctrl);       // FUN_00417da0
    } else {
        ControlStep(spline, v, ctrl);         // FUN_00416250 (bands real; targeting stubbed)
    }
    PostStepPowerupBrake(v, ctrl);            // FUN_00417640

    // override-replay (+0x40..0x4c) -- FUN_00418560 tail (0x00418790..0x00418844,
    // decomp Mashed_pool13 session d643c6ce5d6446ac8121c86ef902b00b 2026-07-04).
    // While the per-vehicle override timer (kAiOverrideTimer, +0x30) is running it
    // counts down by the frame-delta (kOverrideStep) and REPLACES ctrl[0,1,4,5]
    // with the stored bytes (kAiReplayB0/B1/B4/B5, +0x40..0x4c); either way (timer
    // running or not) the (possibly just-overridden) ctrl bytes are saved back into
    // that same storage for next frame -- clean integer logic, no callees.
    if (I32(a74(kAiOverrideTimer, v)) != 0) {
        int t = I32(a74(kAiOverrideTimer, v)) - I32(kOverrideStep);
        if (t < 0) t = 0;
        I32(a74(kAiOverrideTimer, v)) = t;
        ctrl[0] = static_cast<std::uint8_t>(I32(a74(kAiReplayB0, v)));
        ctrl[1] = static_cast<std::uint8_t>(I32(a74(kAiReplayB1, v)));
        ctrl[4] = static_cast<std::uint8_t>(I32(a74(kAiReplayB4, v)));
        ctrl[5] = static_cast<std::uint8_t>(I32(a74(kAiReplayB5, v)));
    }
    I32(a74(kAiReplayB0, v)) = ctrl[0];
    I32(a74(kAiReplayB1, v)) = ctrl[1];
    I32(a74(kAiReplayB4, v)) = ctrl[4];
    I32(a74(kAiReplayB5, v)) = ctrl[5];
}

} // namespace

void Ai_SetHost(const Host* host)
{
    if (host) s_host = *host;
    else {
        s_host = Host{ h_zero_i, h_zero_i, h_zero_i, h_zero_i,
                       h_zero_iv, h_zero_iv, h_zero_i, h_zero_xz, h_zero_xz, h_los_clear,
                       h_zero_xz, h_zero_f, h_zero_iv };
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
// [D3 2026-09-14] Stage tracer. Wiring this tick into the standalone AVs
// (0xC0000005) on the first in-race frame; MASHED_AI_TICKTRACE=<path> writes one
// line per stage so the fault can be bisected without a debugger. Off unless set.
static void TickTrace(const char* stage, int v)
{
    static const char* s_p = std::getenv("MASHED_AI_TICKTRACE");
    if (!s_p || !s_p[0]) return;
    static std::FILE* lf = nullptr;
    if (!lf) { lf = std::fopen(s_p, "w"); if (!lf) return; }
    std::fprintf(lf, "%s v=%d\n", stage, v);
    std::fflush(lf);
}

void Ai_Standalone_Tick()
{
    TickTrace("enter", -1);
    if (I32(kSplineRaceCnt) <= 3) return;        // DAT_00801ca0 > 3 (splines loaded)
    TickTrace("splines-ok", I32(kSplineRaceCnt));

    int r = s_host.round_type();
    int fd0 = s_host.game_mode_fd0();
    bool aiRound = (r == 3 || r == 4 || r == 5 || r == 10 ||
                    fd0 == 4 || fd0 == 9 || fd0 == 8 || fd0 == 10);
    // CarSlotStateSet alive-poke (FUN_0040e480) -- FUN_00418860's fresh decomp
    // (Mashed_pool13, session d643c6ce5d6446ac8121c86ef902b00b, 2026-07-04) shows
    // each of the 3 calls individually gated on FUN_0046c7b0(v)==1 (car_alive), NOT
    // unconditional as the older ai_path_following-20260512 plate's condensed
    // pseudocode implied -- corrected here against the live listing.
    TickTrace("pre-slotstate", aiRound ? 1 : 0);
    if (aiRound) {
        if (s_host.car_alive(1) == 1) CarSlotStateSet(1, 2);
        if (s_host.car_alive(2) == 1) CarSlotStateSet(2, 2);
        if (s_host.car_alive(3) == 1) CarSlotStateSet(3, 2);
    }

    TickTrace("pre-rubberband", -1);
    AiPreTickRubberBand();   // FUN_004177b0
    TickTrace("post-rubberband", -1);

    for (int v = 0; v < 4; ++v) {
        int t = s_host.veh_type(v);
        if ((t != 0 && t != 1) || s_host.ai_target_enable() == 1) {
            if (s_host.car_alive(v) == 1) {
                TickTrace("pre-vehiclestep", v);
                VehicleStep(v);
                TickTrace("post-vehiclestep", v);
            }
        }
    }
    TickTrace("leave", -1);
}


// [D3 2026-09-26] AI clock. The original's per-frame race update stores its tick budget
// in DAT_007f1008 (MOV [0x007f1008],ESI at 0x0040fc63; the same ESI is pushed to
// FUN_00425a40 at 0x0040fc6b, which forwards it to the physics dispatcher) and adds it
// to DAT_007f0ff4 and DAT_007f0ff8 (0x0040fe4f..0x0040fe5e, skipped in race sub-state 7
// at 0x0040fe46). Measured in the original race: DAT_007f1008 == 50 on every AI step
// and DAT_007f0ff4 advances by 50 per frame (verify/d3_ai_20260926/o_inputs). Before
// this, nothing in mashed_re.exe wrote these three, so the FUN_00416250 steer timer
// (el = DAT_007f0ff4 - start, fresh steer when el >= 200) never issued a fresh steer.
void Ai_AdvanceClock(int units)
{
    I32(kOverrideStep) = units;          // DAT_007f1008
    I32(0x007f0ff4u) += units;           // 0x0040fe5e
    I32(kFrame0ff8)  += units;
}

// FUN_00413fe0 — per-vehicle AI-state reset (decomp pool0 2026-09-26): DAT_0089a36c = 0;
// for v = 0..3 (stride 0x74 from 0x0089a4f0): +0x4ec/+0x4f0/+0x4f4, +0x4c4/+0x4c8/+0x4cc/
// +0x4d0, +0x508, +0x51c/+0x520/+0x524/+0x528 = 0; DAT_008032d4[v*5] = 1000. Plus the race
// clock zeroing at 0x0040ff17..0x0040ff23 (DAT_007f101c/0ff4/0ff8 = 0).
// [GATEFIRE 2026-10-08] exported wrapper for the anonymous-namespace CarSlotStateSet
// (FUN_0040e480). Placement rationale and the bridge label live at the call site,
// D3d9Render/TrackRenderer.cpp; this is only the accessor.
void Ai_SetCarSlotState(int v, int state)
{
    if (v < 0 || v > 3) return;
    CarSlotStateSet(v, state);
}

void Ai_ResetRace()
{
    Ai_ResetVehicleStates();                       // FUN_00413fe0 (defined above)
    // the race-clock zeroing at 0x0040ff17..0x0040ff23 — a DIFFERENT site from
    // FUN_00413fe0, folded in here because Ai_ResetRace models race setup.
    I32(0x007f0ff4u) = 0; I32(kFrame0ff8) = 0;
    // [D3 2026-09-27] DIAGNOSTIC knob, default OFF, no effect on the shipping path.
    // FUN_004177b0's band-0 roll (0x00417c43..0x00417c7a: prob 20 for difficulty row 2,
    // table 0x005f30a0[10]) is a ONE-shot 20% chance per race — the band needs
    // `tickscale - DAT_0089a370 > 1.0` AND `tickscale < 2.0`, and firing the band writes
    // DAT_0089a370 = tickscale, so it can never come round again. MEASURED 2026-09-27: the
    // original's o4 capture won that roll (DAT_0089a368 0 -> 1 at 61 calls into the racing
    // window, on all three cars), the standalone's LCG stand-in for FUN_00534870 loses it
    // and is deterministic per boot, so it loses it EVERY run. Setting this to 1 seeds the
    // won-roll regime so the two captures can be compared in the same regime; it is a
    // measurement aid, not a port of anything.
    {
        static const char* e = std::getenv("MASHED_AI_DIFFFLAG");
        if (e && e[0]) I32(0x0089a368u) = std::atoi(e);
    }
}

// FUN_00414030(v) — DAT_008032d4[v*5] = 1000 (decomp pool0 2026-09-26), so the next
// FUN_00443dc0 call takes the true nearest spline point. The standalone calls it when
// it relocates a car (its own respawn / off-mesh recovery).
void Ai_ResetVehicleIndex(int v)
{
    if (v < 0 || v > 3) return;
    I32(0x008032d4u + static_cast<std::uintptr_t>(v) * 0x14u) = 1000;
}

// [D3 2026-09-27] diagnostic accessor for MASHED_AI_STEPDUMP.
const StepLocals& Ai_LastStepLocals(int v)
{
    static const StepLocals kEmpty = {};
    return (v >= 0 && v < 4) ? s_step_locals[v] : kEmpty;
}

} // namespace Ai
