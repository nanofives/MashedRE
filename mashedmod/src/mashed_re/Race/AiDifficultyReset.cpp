// Mashed RE — FUN_00414060, STANDALONE body. **PARTIAL PORT: step 6 only.**
// [GATEFIRE idx364, 2026-10-08 — USER DECISION (Mariano): "port step 6 only"]
//
// EXE-ONLY TU. 0x00414060 has no .asi body, so there is no dual copy and no
// RH_ScopedInstall here: the standalone calls this directly.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
// PREREG: verify/d3_gatefire_20261008/PREREG_GATEFIRE.md
// Evidence: verify/d3_gatefire_20261008/RESULT_IDX364.md (the two writers),
//           verify/d3_gatefire_20261008/RESULT_A360.md  (why only step 6)
//
// ---------------------------------------------------------------------------
// THIS IS A PARTIAL PORT AND MUST NOT BE READ AS MORE THAN THAT.
//
// FUN_00414060 (0x00414060..0x00414118, 185 bytes, __fastcall) has six steps.
// Plate: re/analysis/bucket_util_0040e4b0_0042f790/0x00414060.md.
//
//   steps 1-5  compute and clamp a float into _DAT_0089a360.
//              **NOT PORTED. MEASURED BLOCKED 2026-10-08.** Every input reads 0
//              standalone (RESULT_A360.md): FUN_0042f6a0 = *(u32*)0x0067e9fc = 0
//              on 53,992/53,992 rows, so neither the mode-6 nor the mode-10
//              override arm fires; FUN_00431d80 = *(u32*)0x0067ea7c = 0;
//              FUN_00430790, _DAT_0089a37c and (&DAT_0089a384)[idx] all 0. A
//              faithful port of steps 1-5 would therefore write 0x0089a360 = 0.0
//              and REGRESS it from the 2.5 the original carries and
//              D3d9Render/TrackRenderer.cpp currently seeds. The blocker is
//              upstream of every callee: 0x0067ea7c's write-site set is open as
//              U-1305 and the only exe-side writer sets it to 0
//              (Frontend/SetupScreenRenderers.cpp:343).
//
//   step 6     an UNCONDITIONAL tail that READS NO INPUTS — two constant stores.
//              **PORTED HERE**, bit-faithfully, because it needs nothing that is
//              missing. This is what fixes GF0-IDX364.
//
// So: 0x0089a360 keeps its seed and U-D3-DIFF360 stays open (now with its
// producer identified); 0x0089a364 gets its real sentinel. hooks.csv's row for
// 0x00414060 must stay C2 and record "partial (step 6)" — that is a re-classify
// transaction, not a hand edit.
//
// ---------------------------------------------------------------------------
// WHY THIS MATTERS. 0x0089a364 is a -1 SENTINEL in the original, held there for
// the whole race (RESULT_WITNESS.md:66: -1 on all 512 calls). The standalone's
// blank-mapped memory reads 0, which is IN-RANGE FOR A VEHICLE INDEX — so the
// absence is silently wrong rather than obviously wrong. At
// Ai/AiLeaderTimer.cpp:94 the value decides whether FUN_0040e470 is called at
// all: the original skips that call every time, a 0-reading port would take it.
// Until this lands, any FUN_004148b0 measurement is of a different function.
// (memory: zeroed-granule-vs-minus-one-sentinel)
//
// Default-OFF behind MASHED_A364_RESET. Nothing in mashed_re.exe reads
// 0x0089a364 today, so turning it ON is expected to be INERT at the behaviour
// level — that is the predicted outcome, not a disappointment.

#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace mashed_re {
namespace Race {
namespace {

// Step 6's two constant stores, cited from the plate's "Mechanical description"
// and "Constants" table (0x00414060 body):
//   DAT_0089a364            = 0xffffffff   (-1, the index sentinel)
//   _DAT_0089a870/874/878/87c = 0xbf800000 (-1.0f x4, stride 4)
constexpr std::uintptr_t kIdx364    = 0x0089a364u;
constexpr std::uintptr_t kQuartet   = 0x0089a870u;
constexpr std::int32_t   kSentinel  = -1;          // 0xffffffff
constexpr std::uint32_t  kNegOneBits = 0xbf800000u; // -1.0f, stored as bits so the
                                                    // value is exact by construction
                                                    // rather than by compiler literal

}  // namespace

// 0x00414060  (step 6 tail only — steps 1-5 are NOT here, see the header above)
void AiDifficultyResetTail() {
    *reinterpret_cast<volatile std::int32_t*>(kIdx364) = kSentinel;
    for (std::uintptr_t i = 0; i < 4; ++i) {
        const std::uint32_t bits = kNegOneBits;
        std::memcpy(reinterpret_cast<void*>(kQuartet + i * 4u), &bits, sizeof(bits));
    }
}

// Default-OFF entry point. Called once per race reset from TrackRenderer, which
// is the standalone's analogue of the original's caller FUN_004111c0.
// extern "C" with a flat name: the caller lives in mashed_re::D3d9Render and
// cannot re-open mashed_re::Race from there.
void AiDifficultyResetTickImpl() {
    static const bool s_on = (std::getenv("MASHED_A364_RESET") != nullptr);
    if (!s_on) return;
    AiDifficultyResetTail();
}

}  // namespace Race
}  // namespace mashed_re

extern "C" void __cdecl RaceAiDifficultyResetTail() {
    mashed_re::Race::AiDifficultyResetTickImpl();
}
