// Mashed RE — FUN_00442a60 / FUN_0040e180, STANDALONE bodies.
// [U-9186 leg H3, 2026-10-08 — DEFERRED leg B, unblocked by USER DECISION (Mariano)]
//
// EXE-ONLY TU. Neither RVA has an .asi body, so there is no dual copy and no
// RH_ScopedInstall here: the standalone calls these directly.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
// PREREG: verify/d3_u9186_20261008/PREREG_H3.md
//
// WHAT THIS PRODUCES. 0x008989b0[v] = the planar XZ distance from a reference
// car, scaled. Readers: the AI fire gates FUN_004148b0 / FUN_00414c30 /
// FUN_00415020 / FUN_00415200 / FUN_00415220 and LeaderInRange, via FUN_00442cc0
// (Ai/AiStandalone.cpp:711 RefDist). Until this leg they all read 0.
//
// It is a BRIDGE, not a C4 port: it inherits the 2026-10-06 USER DECISION
// (knob-gated, non-bit-identical substrate acceptable, NO C-level follows).
// Default-OFF behind MASHED_REFDIST.
//
// ---------------------------------------------------------------------------
// TUNING CONSTANTS — read from original/MASHED.exe.unpatched, NOT guessed.
// Gate H3-CONSTS, verify/d3_u9186_20261008/RESULT_H3.md:
//   0x005cc730  .rdata fileoff 0x1cc730  bits 0x42a00000  =  80.0f
//   0x005ccd6c  .rdata fileoff 0x1ccd6c  bits 0x41a00000  =  20.0f
//   0x005cc568  .rdata fileoff 0x1cc568  bits 0x42c80000  = 100.0f
//   0x005cc9bc  .rdata fileoff 0x1cc9bc  bits 0x3f4ccccd  =   0.8f
// They decode the wrap branch: race_pct is 0..100 per lap, so "one car above 80
// and the other below 20" is the pair straddling the start/finish line, and
// subtracting 100.0 un-wraps the one that already crossed.
//
// DEVIATION, REGISTERED. The original's vector length is FUN_004c3ac0, the
// RenderWare fast-sqrt lookup (table at DAT_007d3ff8), which the standalone does
// not own. std::sqrt is used instead — the same stand-in Ai/AiStandalone.cpp:770
// already uses for this exact RVA. So the magnitudes here are NOT bit-identical
// to the original's and no C-level follows from them. Pair SELECTION is
// unaffected in principle (both functions are monotone in the squared length, so
// the argmax is the same) but that is an argument, not a measurement, and gate
// H3-PAIR reports the selected indices so it can be checked.


#include "RaceCamera.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>

// Readers made standalone-reachable by legs H1a / H1b.
extern "C" std::uint32_t __cdecl VehicleSlotGetter(std::uint32_t vehicleIdx);          // 0x0046c7b0
extern "C" int          __cdecl VehicleCarStateRead(std::uint32_t carIdx,              // 0x0046cbb0
                                                    std::uint32_t* outState,
                                                    std::uint32_t* outSecondary);
extern "C" std::uint32_t __cdecl PtrCompute881ec8(std::uint32_t* out,                  // 0x0046d4a0
                                                  std::uint32_t idx);
extern "C" std::uint32_t __cdecl IsCarSlotActive(int param_1);                         // 0x0040e370

namespace mashed_re {
namespace Race {

namespace {

constexpr float kWrapHi    =  80.0f;   // _DAT_005cc730
constexpr float kWrapLo    =  20.0f;   // _DAT_005ccd6c
constexpr float kLapSpan   = 100.0f;   // _DAT_005cc568
constexpr float kDistScale =   0.8f;   // _DAT_005cc9bc

// FUN_00408ad0 (0x00408ad0, 17-byte leaf): race_pct of car i.
// `return (float10)*(float *)(&DAT_008a96ec + param_1 * 0x30c);`
// Supplied standalone by the D-11072 race_pct bridge (MASHED_RACEPCT_BRIDGE).
inline float RacePct(int i) {
    return *reinterpret_cast<const float*>(
        0x008a96ecu + static_cast<std::uintptr_t>(i) * 0x30cu);
}

// The three per-car predicates FUN_0040e180 and FUN_00442a60 both gate on.
inline bool SlotUsable(int i) {
    if (!IsCarSlotActive(i)) return false;                      // FUN_0040e370
    if (VehicleSlotGetter(static_cast<std::uint32_t>(i)) != 1u)  // FUN_0046c7b0
        return false;
    std::uint32_t st = 0u, sec = 0u;
    VehicleCarStateRead(static_cast<std::uint32_t>(i), &st, &sec);  // FUN_0046cbb0
    return st == 0u;
}

// World XZ of car i, from the body-matrix translation row that FUN_0046d4a0
// resolves (+0x30/+0x38 off the returned pointer).
inline bool CarXZ(int i, float* x, float* z) {
    std::uint32_t p = 0u;
    if (!PtrCompute881ec8(&p, static_cast<std::uint32_t>(i)) || !p) return false;
    *x = *reinterpret_cast<const float*>(static_cast<std::uintptr_t>(p) + 0x30u);
    *z = *reinterpret_cast<const float*>(static_cast<std::uintptr_t>(p) + 0x38u);
    return true;
}

}  // namespace

// ---------------------------------------------------------------------------
// NOTE (not a body anchor — this comment deliberately does NOT lead with the RVA
// token, because scripts/lint_rva_bodies.py binds a function to the first RVA it
// sees in the preceding comment, and an earlier draft of this block made the
// lint read FillCamCars as a second body for that RVA).
//
// The most-separated-pair function is NOT re-implemented here. The exe has one body
// (Race/RaceCamera.cpp:182 RaceCamera::MostSeparatedPair) and
// scripts/lint_rva_bodies.py enforces one body per RVA per target; a second was
// written here first and the lint correctly rejected it (DUP-IN-TARGET).
//
// Using the existing body is also the more faithful choice, not merely the
// permitted one: RaceCamCar's own field comments (RaceCamera.h) name exactly the
// sources FUN_0040e180 reads — pos = vehicle +0x30/+0x34/+0x38 via FUN_0046d4a0,
// active = IsCarSlotActive 0x0040e370, alive = FUN_0046c7b0 == 1, dead_flag =
// FUN_0046cbb0 out1 — and its loop, its `<=` tie rule and its -1 tail fixups are
// the transcription of 0x0040e2f3 .. 0x0040e330 (cited in RaceCamera.cpp). What H3 supplies is the ARRAY,
// filled from the readers legs H1a/H1b made live, so the function answers the
// original's question on the original's inputs.
//
// OUT-PARAM ORDER is source-verified at RaceCamera.cpp:206-207: first out = INNER
// index, second = OUTER. FUN_00442a60's `local_3c`/`local_38` take them in that
// order, so they are passed through unswapped.
//
// Residual, registered: that body is DEMOTED C4->C2 (2026-09-29, hooks.csv:877)
// because it uses std::sqrt where the .asi twin uses the RW fast-sqrt, a 7.8%
// magnitude disagreement. Only the ARGMAX matters here, and both functions are
// monotone in the squared length, so the selected pair should be unaffected —
// an argument, not a measurement. Gate H3-PAIR logs the selected indices.
// ---------------------------------------------------------------------------

// Fill the RaceCamCar array FUN_0040e180 consumes, from the live record.
void FillCamCars(RaceCamCar cars[4]) {
    for (int i = 0; i < 4; ++i) {
        cars[i] = RaceCamCar{};
        cars[i].active = IsCarSlotActive(i) != 0u;                          // 0x0040e370
        cars[i].alive  = VehicleSlotGetter(static_cast<std::uint32_t>(i)) == 1u;  // 0x0046c7b0
        std::uint32_t st = 0u, sec = 0u;
        VehicleCarStateRead(static_cast<std::uint32_t>(i), &st, &sec);      // 0x0046cbb0
        cars[i].dead_flag = (st != 0u);
        cars[i].dead_ms   = static_cast<float>(sec);
        std::uint32_t p = 0u;
        if (PtrCompute881ec8(&p, static_cast<std::uint32_t>(i)) && p) {     // 0x0046d4a0
            const char* r = reinterpret_cast<const char*>(static_cast<std::uintptr_t>(p));
            cars[i].pos[0] = *reinterpret_cast<const float*>(r + 0x30);
            cars[i].pos[1] = *reinterpret_cast<const float*>(r + 0x34);
            cars[i].pos[2] = *reinterpret_cast<const float*>(r + 0x38);
        }
        cars[i].race_pct = RacePct(i);                                      // 0x00408ad0
    }
}

// ---------------------------------------------------------------------------
// 0x00442a60  Spectator::ComputeDistances — writes 0x008989b0[0..3].
// ---------------------------------------------------------------------------
void ComputeDistances() {
    float* const refdist = reinterpret_cast<float*>(0x008989b0u);
    for (int i = 0; i < 4; ++i) refdist[i] = 0.0f;      // the entry zeroing

    RaceCamCar cars[4];
    FillCamCars(cars);
    int a = 4, b = 0;
    RaceCamera::MostSeparatedPair(cars, &a, &b);        // FUN_0040e180

    float pa = RacePct(a);                              // FUN_00408ad0
    float pb = RacePct(b);
    if ((pa <= kWrapHi) || (kWrapLo <= pb)) {
        if ((kWrapHi < pb) && (pa < kWrapLo)) pb -= kLapSpan;
    } else {
        pa -= kLapSpan;
    }

    std::uint32_t sa = 0u, sb = 0u, ta = 0u, tb = 0u;
    VehicleCarStateRead(static_cast<std::uint32_t>(a), &sa, &ta);   // FUN_0046cbb0
    VehicleCarStateRead(static_cast<std::uint32_t>(b), &sb, &tb);

    int ref = a, other = b;
    std::uint32_t sref = sa, sother = sb;
    if (pb < pa) {                 // the rearmost of the two becomes the reference
        ref = b; other = a;
        sref = sb; sother = sa;
    }

    // The original stores both indices before the guard chain, so the port does
    // too. Both land in blank-mapped space standalone (inert), but omitting them
    // would not be faithful.
    *reinterpret_cast<std::int32_t*>(0x008989a8u) = ref;
    *reinterpret_cast<std::int32_t*>(0x008989c8u) = other;

    if (!((sother == 0u) || (sref == 0u))) return;
    if (!IsCarSlotActive(ref)) return;                              // FUN_0040e370
    if (VehicleSlotGetter(static_cast<std::uint32_t>(ref)) != 1u) return;  // FUN_0046c7b0

    float rx = 0.0f, rz = 0.0f;
    if (!CarXZ(ref, &rx, &rz)) return;                              // FUN_0046d4a0

    for (int i = 0; i < 4; ++i) {
        if (!IsCarSlotActive(i)) continue;
        if (VehicleSlotGetter(static_cast<std::uint32_t>(i)) != 1u) continue;
        float cx = 0.0f, cz = 0.0f;
        if (!CarXZ(i, &cx, &cz)) continue;
        const float dx = rx - cx;
        const float dz = rz - cz;                                   // y term is 0.0
        refdist[i] = std::sqrt(dx * dx + dz * dz) * kDistScale;     // FUN_004c3ac0 * 0.8
    }
}

// Default-OFF entry point. Called once per race tick from TrackRenderer.
// extern "C" with a flat name: the caller lives in mashed_re::D3d9Render and
// cannot re-open mashed_re::Race from there.
void ComputeDistancesTickImpl() {
    static const bool s_on = (std::getenv("MASHED_REFDIST") != nullptr);
    if (!s_on) return;
    ComputeDistances();
}

}  // namespace Race
}  // namespace mashed_re

extern "C" void __cdecl RaceComputeDistancesTick() {
    mashed_re::Race::ComputeDistancesTickImpl();
}

