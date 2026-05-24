// Mashed RE - Menu time-delta decomposition reimplementation.
// Analysis note: re/analysis/promote_c2_frontend_menus/0x0042d300.md
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)

#include "../Core/HookSystem.h"
#include <cstdint>

// ---------------------------------------------------------------------------
// TimeDiffDecompose  --  0x0042d300
//
// Original: FUN_0042d300 (128 bytes, 0x0042d300..0x0042d380)
// No callees; pure arithmetic leaf.
//
// Signature:
//   void (int time_a, int time_b,
//         uint32_t* sign_out,   // out: 0=positive diff, 1=negative, 2=equal
//         int*      min_out,    // out: minutes component
//         int*      sec_out,    // out: seconds component
//         float*    csec_out)   // out: centiseconds remainder
//
// Time unit: centiseconds (1/100 s).
// Constants cited from 0x0042d300 body:
//   6000 — centiseconds per minute
//   100  — centiseconds per second
//   60 (0x3c) — seconds per minute (used in csec remainder formula)
//
// Sign code convention (from decomp at 0x0042d300):
//   diff == 0            → *sign_out = 2  (equal)
//   diff <  0 (negated)  → *sign_out = 1  (time_a < time_b; time_a was faster)
//   diff >  0            → *sign_out = 0  (time_a > time_b; time_b was faster)
//
// ref: re/analysis/promote_c2_frontend_menus/0x0042d300.md
// ---------------------------------------------------------------------------

// Centiseconds per minute (6000) — cited at 0x0042d300 body.
static constexpr int kCsecPerMinute = 6000;
// Centiseconds per second (100) — cited at 0x0042d300 body.
static constexpr int kCsecPerSecond = 100;
// Seconds per minute (60 = 0x3c) — cited at 0x0042d300 body (remainder formula).
static constexpr int kSecPerMinute  = 60;

// 0x0042d300
extern "C" __declspec(dllexport) void __cdecl TimeDiffDecompose(
    int time_a, int time_b,
    std::uint32_t* sign_out,
    int*           min_out,
    int*           sec_out,
    float*         csec_out)
{
    int diff = time_a - time_b;

    // Sign classification.
    *sign_out = 0u;        // assume positive (time_a > time_b)
    if (diff == 0) {
        *sign_out = 2u;    // equal
    } else if (diff < 0) {
        diff = -diff;      // take absolute value
        *sign_out = 1u;    // negative (time_a was faster)
    }

    // Decompose into minutes + seconds + centiseconds.
    int minutes = diff / kCsecPerMinute;
    int seconds = (diff % kCsecPerMinute) / kCsecPerSecond;

    *min_out  = minutes;
    *sec_out  = seconds;
    // Centiseconds remainder: diff - (minutes * 60 + seconds) * 100
    // Formula cited from 0x0042d300 body.
    *csec_out = static_cast<float>(diff - (minutes * kSecPerMinute + seconds) * kCsecPerSecond);
}

RH_ScopedInstall(TimeDiffDecompose, 0x0042d300);  // re-enabled 2026-05-24 c3-frontend-a
