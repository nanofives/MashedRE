// Mashed RE - steer-0 heading assist (exe only, default ON since dflip 2026-10-17; MASHED_NO_HEADING_ASSIST=1 opts out).
//
// 0x0046c7d0  FUN_0046c7d0  (852 b)  per car: when both steer bytes of the car's input block are 0,
//             rotate the car's live body matrix about (0,1,0) toward the heading of the three track
//             pieces ahead, scaled by speed, frame time and the track curvature.
// 0x004093b0  FUN_004093b0  (496 b)  the three-piece track heading (mean) and curvature (sum of
//             |heading deltas|) from piece `p` on.
// Caller: FUN_00424eb0's loop over cars 0..3 (present FUN_0040e370 != 0 and alive FUN_0046c7b0 == 1),
// which the game loop runs after the AI tick FUN_00418860 (0x00425a73) and before the physics frame
// FUN_00470c70 (0x00425a8d). Evidence that the original applies it and the port did not:
// verify/d3b_20261016 (PREREG_d3b.md amendments 3-5): on car 1's steer-0 frames the original's live
// matrix turns by -0.19 .. -0.26 deg between frames, written by 0x004c4cf1 (FUN_004c4a50) under
// FUN_004c4d20 called at 0x0046cb10 (accurate backtrace, o_ww2.msd.watch.csv).
// Transcribed from the listings (verify/d3b_20261016/disasm.py), PC_24 in the original, so each x87 op
// is computed here as a float op. Constants read from original/MASHED.exe.unpatched:
//   0x006146f0 = (1,0,0)   0x006146fc = (0,1,0)   0x005cc33c = -1   0x005cc320 = 1
//   0x005ccae0 = 57.295799255371094 (double)   0x005ccad8 = 90.0 (double)   0x005ccad0 = 90.0f
//   0x005ccac4 = 360   0x005cc72c = 30   0x005cc728 = 60   0x005ccacc = 45   0x005ccac8 = 0x3eaaaaab
//   0x005cea58 = 0x392ec33e (1/6000)   0x005cc574 = 2.0   0x005cc94c = 2^32   0x005cd694 = 0x3c888889 (1/60)
//   0x005cc35c = 4.0   0x005cd984 = 0x3cb60b61 (1/45)
//   0x00613134 = 1.0 in the image and 1.0 at runtime on the reference race (o_pk1.log, nine samples;
//   control 0x00613108 = 105 there), so the port uses 1.0.
#include "HeadingAssist.h"
#include "PathProgStep.h"
#include "../Ai/AiState.h"
#include "../Math/RwLutTables.h"
#include "../Vehicle/VehiclePhysicsRun.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);   // 0x0046c7b0
extern "C" std::uint32_t __cdecl IsCarSlotActive(int param_1);                   // 0x0040e370
extern "C" void* __cdecl RwMatrixRotate(void* matrix, const float* axis, float angle_deg, int mode);   // 0x004c4d20

namespace mashed_re {
namespace Vehicle { int Fi_GameMode(); }   // FUN_0040e350
namespace Race {

namespace {

constexpr std::uintptr_t kNodeRec      = 0x00663658u;   // FUN_00426cc0(p) = kNodeRec + p*0x4c (Race/RoundGrid.cpp)
constexpr std::uintptr_t kNodeCount    = 0x0066d6d8u;
constexpr std::uintptr_t kRangeTable   = 0x0066ce58u;   // FUN_00426090 (GlobalDat0066ce58Get)
constexpr std::uintptr_t kNeutralBlock = 0x007f19b8u;   // &DAT_007f19b8, FUN_0046c7d0 0x0046c80x
const float kAxisX[3] = { 1.0f, 0.0f, 0.0f };           // 0x006146f0
const float kAxisY[3] = { 0.0f, 1.0f, 0.0f };           // 0x006146fc
constexpr float kK613134 = 1.0f;                        // _DAT_00613134, measured (header)

inline float Fbits(std::uint32_t b) { float f; std::memcpy(&f, &b, 4); return f; }
inline std::int32_t I32a(std::uintptr_t a) { std::int32_t v; std::memcpy(&v, reinterpret_cast<const void*>(a), 4); return v; }

// The heading law shared by 0x0046c8ab..0x0046c925 and 0x0040942c..0x004094a0: LUT-normalize (x, 0, z),
// dot with (1,0,0) in the listing's order, clamp to [-1, 1], acos (CRT FUN_004a3384), then
// -(acos*57.2958 - 90.0) + 90.0f. [UNCERTAIN] the CRT acos runs at PC_24 in the original; std::acos is
// correctly rounded in double, so the last float ulp may differ.
float HeadingDeg(float x, float z) {
    const float in3[3] = { x, 0.0f, z };
    float n[3];
    Math::LutNormalize(n, in3);                                              // FUN_004c39b0
    float d = kAxisX[1] * n[1] + kAxisX[0] * n[0];                           // 0x0046c8b2..0x0046c8c9
    d = d + kAxisX[2] * n[2];                                                // 0x0046c8cb..0x0046c8d5
    if (d < -1.0f) d = -1.0f;                                                // 0x0046c8d7..0x0046c8e6
    else if (d > 1.0f) d = 1.0f;                                             // 0x0046c8ee..0x0046c8fd
    const double a = std::acos(static_cast<double>(d));                      // 0x0046c909 FUN_004a3384
    float h = static_cast<float>(a * 57.295799255371094);                    // 0x0046c90e fmul qword 0x5ccae0
    h = static_cast<float>(static_cast<double>(h) - 90.0);                   // 0x0046c917 fsub qword 0x5ccad8
    h = -h;                                                                  // 0x0046c91d fchs
    h = h + 90.0f;                                                           // 0x0046c91f fadd 0x5ccad0
    return h;
}

// FUN_00409330(p, 0) on the range table FUN_00426090 returns. The exe has no writer for 0x0066ce58
// (a .bss buffer the original fills at track load; on Training it reads {0, 29, -1}, o_rt1). When the
// table is empty here the successor is (p + 1) mod N, which is what that single-range table gives.
int Successor(int p) {
    static int s_mode = -1;   // 1 = verbatim table walk, 0 = (p+1) mod N
    if (s_mode < 0) {
        s_mode = 0;
        for (int i = 0; i < 64; ++i) {
            const std::int32_t w = I32a(kRangeTable + static_cast<std::uintptr_t>(i) * 4u);
            if (w == -1 || w == -10) { s_mode = (i > 0) ? 1 : 0; break; }
        }
        if (std::FILE* lf = std::fopen("mashed_re.log", "a")) {
            std::fprintf(lf, "[d3b] HeadingAssist: range table 0x0066ce58 %s\n",
                         s_mode ? "populated, verbatim FUN_00409330 walk" : "empty, successor = (p+1) mod node count");
            std::fclose(lf);
        }
    }
    if (s_mode == 0) {
        const int n = I32a(kNodeCount);
        return (n > 0) ? (p + 1) % n : 0;
    }
    const std::int32_t* t = reinterpret_cast<const std::int32_t*>(kRangeTable);   // group 0: no skip
    std::int32_t w = t[0];
    const std::int32_t* q = t;
    for (;;) {                                                               // FUN_00409330 body
        if (w == -1 || w == -10) return t[0];
        if (q[1] == 0x100) { if (p == w) return q[2]; }
        else if (w <= p && p < q[1]) return p + 1;
        w = q[1];
        ++q;
    }
}

// FUN_004093b0(p, &mean, &curv). Returns false on the early exit at 0x0040941c (a zero direction),
// where the original leaves both outputs untouched.
bool TrackHeading(int p, float* mean, float* curv) {
    float sum = 0.0f, curvSum = 0.0f, prev = 0.0f;                           // 0x004093bf..0x004093cf
    const int n = I32a(kNodeCount);
    for (int i = 0; i < 3; ++i) {
        if (n <= 0 || p < 0 || p >= n) return false;                         // standalone guard (table bounds)
        const float* d = reinterpret_cast<const float*>(kNodeRec + static_cast<std::uintptr_t>(p) * 0x4cu);   // FUN_00426cc0
        if (d[0] == 0.0f && d[2] == 0.0f) return false;                      // 0x004093f9..0x0040941c
        float h = HeadingDeg(d[0], d[2]);                                    // 0x0040942c..0x004094a0
        const float c1 = kAxisX[0] * d[2] - kAxisX[2] * d[0];                 // 0x004094be..0x004094d1
        if (c1 < 0.0f) h = 360.0f - h;                                       // 0x004094ec..0x004094f9
        while (h > 90.0f) h = h - 90.0f;                                     // 0x004094ff..0x00409521
        if (h > 45.0f) h = h - 90.0f;                                        // 0x00409523..0x00409530
        sum = sum + h;                                                       // 0x00409538..0x0040953e
        if (i != 0) {                                                        // 0x00409536 / 0x00409542
            float dd = prev - h;                                             // 0x00409544..0x00409548
            if (dd < 0.0f) dd = -dd;
            curvSum = dd + curvSum;                                          // 0x00409559
        }
        prev = h;                                                            // 0x00409563
        p = Successor(p);                                                    // 0x00409568 FUN_00409330(p, 0)
    }
    *mean = sum * Fbits(0x3eaaaaabu);                                        // 0x0040957c..0x0040958c
    *curv = curvSum;                                                         // 0x0040958e..0x00409592
    return true;
}

std::FILE* TraceFile() {
    static bool s_init = false; static std::FILE* s_f = nullptr;
    if (!s_init) {
        s_init = true;
        if (const char* p = std::getenv("MASHED_HEADING_ASSIST_TRACE")) {
            s_f = std::fopen(p, "w");
            if (s_f) std::fprintf(s_f, "call,v,c0,c1,H,piece,mean,curv,err,lim,angle\n");
        }
    }
    return s_f;
}

// FUN_0046c7d0(v, dt). Returns the applied angle (0 when nothing rotated). `own` (player form, u9749): the car's
// input block when the exe does not keep it in the ctrl block table (slot 0's steer bytes come from the player's
// input, not from FUN_00496530's cooked block), nullptr = the slot table's block as for the AI slots.
float AssistCar(int v, int dtUnits, const std::uint8_t* own = nullptr) {
    using namespace mashed_re::Vehicle;
    const std::uint8_t* blk = own ? own : reinterpret_cast<const std::uint8_t*>(
        ::Ai::kCtrlBlockBase + static_cast<std::uintptr_t>(
            ::Ai::I32(::Ai::kSlotTableBase + static_cast<std::uintptr_t>(v) * ::Ai::kSlotTableStride)) * ::Ai::kCtrlBlockStride);
    if (VehiclePhysics_RecordI32(v, 0x9f0) != 0) blk = reinterpret_cast<const std::uint8_t*>(kNeutralBlock);   // DAT_00881f90
    if (VehiclePhysics_RecordI32(v, 0x10) != 0)  blk = reinterpret_cast<const std::uint8_t*>(kNeutralBlock);   // DAT_008815b0
    const int gm = Fi_GameMode();
    if (gm != 6 && gm != 0xb && gm != 10) blk = reinterpret_cast<const std::uint8_t*>(kNeutralBlock);
    if (blk[0] != 0 || blk[1] != 0) return 0.0f;                             // 0x0046c843..0x0046c851

    float* basis = VehiclePhysics_BodyBasis(v);
    if (!basis) return 0.0f;                                                 // standalone: not seeded yet
    const float fx = VehiclePhysics_RecordF32(v, 0x9d4);                     // [edi+0x881f74]
    const float fz = VehiclePhysics_RecordF32(v, 0x9dc);                     // [edi+0x881f7c]
    if (fx == 0.0f && fz == 0.0f) return 0.0f;                               // 0x0046c86b..0x0046c893
    float H = HeadingDeg(fx, fz);                                            // 0x0046c899..0x0046c925
    const float c1 = kAxisX[0] * fz - kAxisX[2] * fx;                        // 0x0046c947..0x0046c95d
    if (c1 < 0.0f) H = 360.0f - H;                                           // 0x0046c97b..0x0046c992

    const float prog = PathProg_NodeProgress(v);                             // 0x0046c997 FUN_00408a50
    const int piece = static_cast<int>(prog);                                // 0x0046c99c FUN_004a2c48 (truncate)
    float mean = 0.0f, curv = 0.0f;                                          // [UNCERTAIN] the original's are uninitialised stack
    TrackHeading(piece, &mean, &curv);                                       // 0x0046c9ac

    float e = H - mean;                                                      // 0x0046c9b1..0x0046c9b5
    while (e < 0.0f) e = e + 90.0f;                                          // 0x0046c9bc..0x0046c9e1
    while (e > 90.0f) e = e - 90.0f;                                         // 0x0046c9e3..0x0046ca01
    if (e < 30.0f) e = -e;                                                   // 0x0046ca03..0x0046ca10
    else if (e < 60.0f) e = 45.0f - e;                                       // 0x0046ca14..0x0046ca21
    else e = 90.0f - e;                                                      // 0x0046ca29
    float lim = VehiclePhysics_RecordF32(v, 0x9e4) * Fbits(0x392ec33eu);     // 0x0046ca2f..0x0046ca3b
    if (lim > 2.0f) lim = 2.0f;                                              // 0x0046ca3f..0x0046ca4c
    if (e > lim) e = lim;                                                    // 0x0046ca54..0x0046ca61
    const float nlim = -lim;                                                 // 0x0046ca65..0x0046ca6b
    if (e < nlim) e = nlim;                                                  // 0x0046ca6f..0x0046ca7c
    float dtf = static_cast<float>(dtUnits);                                 // 0x0046ca84 fild
    if (dtUnits < 0) dtf = dtf + 4294967296.0f;                              // 0x0046ca8c (0x5cc94c)
    float A = e * dtf;                                                       // 0x0046ca92
    A = A * Fbits(0x3c888889u);                                              // 0x0046ca94 (0x5cd694)
    A = A * kK613134;                                                        // 0x0046ca9a
    float c = curv * 4.0f;                                                   // 0x0046caa0..0x0046caa4
    if (c < 0.0f) c = -c;                                                    // 0x0046caaa..0x0046cab7
    if (c > 45.0f) c = 45.0f;                                                // 0x0046cab9..0x0046cac8
    float angle = A * (45.0f - c);                                           // 0x0046cace..0x0046cad6
    angle = angle * Fbits(0x3cb60b61u);                                      // 0x0046cada (0x5cd984)
    if (std::FILE* tf = TraceFile()) {
        static long s_n = 0;
        std::fprintf(tf, "%ld,%d,%u,%u,%.6f,%d,%.6f,%.6f,%.6f,%.6f,%.6f\n", s_n++, v, blk[0], blk[1],
                     H, piece, mean, curv, e, lim, angle);
    }
    if (angle != 0.0f)                                                       // 0x0046cae6..0x0046caf1
        RwMatrixRotate(basis, kAxisY, angle, 1);                             // 0x0046cb10 FUN_004c4d20(.., 0x6146fc, a, 1)
    return angle;
}

}  // namespace

unsigned HeadingAssist_Mask() {
    static const unsigned s_mask = [] {
        const char* no = std::getenv("MASHED_NO_HEADING_ASSIST");
        if (no && no[0] && no[0] != '0') return 0u;                          // [dflip] opt-out
        const char* e = std::getenv("MASHED_HEADING_ASSIST");
        if (e && e[0]) return static_cast<unsigned>(std::strtoul(e, nullptr, 0));        // explicit mask wins
        // [dflip] 0x1 (AI slots) default ON. [u9749] 0x2 (car 0) default ON since 2026-10-17 (user decision);
        // MASHED_NO_HEADING_ASSIST_PLAYER=1 opts out (MASHED_HEADING_ASSIST_PLAYER=1 is still accepted, a no-op now).
        const char* np = std::getenv("MASHED_NO_HEADING_ASSIST_PLAYER");
        return (np && np[0] && np[0] != '0') ? 0x1u : 0x3u;
    }();
    return s_mask;
}

// [u9749 2026-10-17] Player form (mask 0x2, default ON since 2026-10-17, MASHED_NO_HEADING_ASSIST_PLAYER=1 opts out; U-9743 / U-9749): FUN_00424eb0's loop covers car 0 too (cars 0..3, the same
// FUN_0040e370 / FUN_0046c7b0 gates), between the AI tick (0x00425a73) and the physics frame (0x00425a8d). The steer bytes
// are the player's this frame (`steer0` / `steer1`, the bytes StepCar maps io.steer to). Measured cause (u9749, B_SAR0_K1
// against o_pw1): on the player's 9 coast frames after its first car<->car contact the original turns 0.00264 per frame
// (forward x) more than the port while the yaw rate +0x148 is bit-identical, i.e. a constant-rate rotation the port did
// not apply.
void HeadingAssist_Player(int dtUnits, unsigned char steer0, unsigned char steer1) {
    if ((HeadingAssist_Mask() & 0x2u) == 0u) return;
    if (IsCarSlotActive(0) == 0) return;                                     // FUN_0040e370
    if (VehicleSlotGetter(0u) != 1u) return;                                 // FUN_0046c7b0
    const std::uint8_t blk[2] = { steer0, steer1 };
    AssistCar(0, dtUnits, blk);
}

void HeadingAssist_Frame(int dtUnits) {
    if ((HeadingAssist_Mask() & 0x1u) == 0u) return;
    for (int v = 1; v < 4; ++v) {                                            // FUN_00424eb0 loop, AI slots only (mask 0x1)
        if (IsCarSlotActive(v) == 0) continue;                               // FUN_0040e370
        if (VehicleSlotGetter(static_cast<std::uint32_t>(v)) != 1u) continue;   // FUN_0046c7b0
        AssistCar(v, dtUnits);
    }
}

}  // namespace Race
}  // namespace mashed_re
