// Mashed RE — exe call sites of the DAT_0089a360 producer chain (0x00414060, 0x00414120, the DAT_0089a37c term
// of 0x00417750). EXE-ONLY TU: none of the functions here is an original-RVA body.
//
// Binary anchor: MASHED.exe size=2,846,720 sha256=BDCAE093...EFD3C0E
//
// History. GATEFIRE idx364 (2026-10-08, user decision "port step 6 only") put a hand-written step-6 tail of
// FUN_00414060 here. MATCH-T13-SEED-WRITER (2026-11-02, decision L-85 = c, log/orch/DECISIONS.md) replaces it
// with the byte-matched body Match/MatchS03_00414060.cpp (Race89a360Compute, the one body of 0x00414060 in both
// targets) and adds the rest of the chain behind a default-OFF knob.
// PREREG: verify/matcht13seedwriter_20261102/PREREG_MATCH-T13-SEED-WRITER.md
//
// DEFAULT (knob unset). RaceAiDifficultyResetTail runs the matched body at race load and then restores
// DAT_0089a360 to the value it held before the call (the 2.5 that TrackRenderer.cpp seeds two statements
// earlier). Net stores = the step-6 five (0x0089a364 = -1, 0x0089a870..87c = 0xbf800000), the same as the old hand
// tail. Every read the body makes is a plain global read with no side effect.
//
// MASHED_DIFF360_LIVE=1. Three sites, each the exe's analogue of an original caller:
//   RaceDiff360Live_RaceInit   FUN_00414060 at 0x0040ffc7 (FUN_004111c0 race init, right after FUN_0040b180 at
//                              0x0040ffb8). Exe: TrackRenderer::StartMatch after SeedMatchScores (FUN_0040b180).
//                              DAT_0089a360 keeps the computed value, so the rule-10 seed (MASHED_RULE10_SEED_DIFF=1)
//                              and the exe's AI readers of 0x0089a360 see it.
//   RaceDiff360Live_TableInit  FUN_00414120 at 0x0043d3d0 (FUN_0043d2a0 when its first argument == 1,
//                              0x0043d3cb, for every direction) and 0x0043fead (FUN_0043dfd0). Exe: Frontend::Nav and
//                              Nav_Init when the screen is 1. [UNCERTAIN] the exe analogue of the 0x0043fead arm is not
//                              wired: that arm stores [0x005f763c] / ebx into the nav slot words 0x0067ed78 / 0x0067ed7c
//                              (0x0043fe83..0x0043fea1) and which exe action reaches it is not traced.
//   RaceDiff360Live_RoundEnd37c the DAT_0089a37c term of FUN_00417750 (0x00417756..0x0041777e). Original caller:
//                              FUN_0040ddb0 at 0x0040ddb0 (DAT_0063ba8c case 9, 0x0040ddf0). The exe has no case-9
//                              handler (TrackRenderer.cpp MatchEndCaseB comment); the site is the one exe store of
//                              DAT_0063ba8c = 9 (0x00410b02 analogue). Its input DAT_0089a380 has no exe producer:
//                              FUN_00414220 (the six += terms at 0x00414266..0x004142ed) is not ported, so in the exe the
//                              term adds DAT_0089a380 = 0.0 / n.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

extern "C" void Race89a360Compute(void);                         // 0x00414060, Match/MatchS03_00414060.cpp
extern "C" void __cdecl CopyTable005f2a70To0089a384(void);       // 0x00414120, Frontend/SmallLeaves_t1.cpp
extern "C" std::uint32_t __cdecl GetDat008994c0(void);           // 0x004241b0, Match/MatchS05_004241b0.cpp

namespace mashed_re {
namespace Race {
namespace {

constexpr std::uintptr_t k360 = 0x0089a360u;
constexpr std::uintptr_t k37c = 0x0089a37cu;
constexpr std::uintptr_t k380 = 0x0089a380u;

std::uint32_t U32At(std::uintptr_t a) { return *reinterpret_cast<const volatile std::uint32_t*>(a); }

bool Diff360LiveEnabled() {
    static const bool s = [] { const char* e = std::getenv("MASHED_DIFF360_LIVE"); return e && e[0] && e[0] != '0'; }();
    return s;
}

// One line per call into mashed_re.log, knob ON only (calls are per race / per menu entry / per round end).
void Diff360Log(const char* what) {
    std::FILE* lf = std::fopen("mashed_re.log", "a");
    if (!lf) return;
    const std::int32_t mode = *reinterpret_cast<const std::int32_t*>(0x0067e9fcu);   // FUN_0042f6a0
    const std::int32_t row  = *reinterpret_cast<const std::int32_t*>(0x0067f17cu);   // FUN_00430790
    const std::int32_t idx  = row * 3 + (mode == 4 ? 1 : (mode == 5 ? 2 : 0));       // FUN_00413fa0 (log only)
    const std::int32_t tab  = (idx >= 0 && idx < 39)
        ? *reinterpret_cast<const std::int32_t*>(0x0089a384u + static_cast<std::uintptr_t>(idx) * 4u) : -1;
    std::fprintf(lf, "[diff360] %s mode=%d row=%d idx=%d a384=%d a37c_raw=%08x a380_raw=%08x a378=%d n4c0=%u "
                     "ea7c=%d ea90=%d a360_raw=%08x a364=%d\n",
                 what, mode, row, idx, tab, U32At(k37c), U32At(k380),
                 *reinterpret_cast<const std::int32_t*>(0x0089a378u), U32At(0x008994c0u),
                 *reinterpret_cast<const std::int32_t*>(0x0067ea7cu), *reinterpret_cast<const std::int32_t*>(0x0067ea90u),
                 U32At(k360), *reinterpret_cast<const std::int32_t*>(0x0089a364u));
    std::fclose(lf);
}

}  // namespace
}  // namespace Race
}  // namespace mashed_re

// Race load (TrackRenderer.cpp Ai_BridgeLoad, the caller the GATEFIRE tail had). Knob-independent.
extern "C" void __cdecl RaceAiDifficultyResetTail() {
    const std::uint32_t keep = mashed_re::Race::U32At(mashed_re::Race::k360);
    Race89a360Compute();                                                     // 0x00414060
    *reinterpret_cast<volatile std::uint32_t*>(mashed_re::Race::k360) = keep; // default: the 2.5 seed stays
}

extern "C" bool __cdecl RaceDiff360Live_Enabled() { return mashed_re::Race::Diff360LiveEnabled(); }

// 0x0040ffc7 analogue (StartMatch). Knob ON only.
extern "C" void __cdecl RaceDiff360Live_RaceInit() {
    if (!mashed_re::Race::Diff360LiveEnabled()) return;
    Race89a360Compute();                                                     // 0x00414060
    mashed_re::Race::Diff360Log("raceinit");
}

// 0x0043d3d0 analogue (nav reaches screen 1). Knob ON only.
extern "C" void __cdecl RaceDiff360Live_TableInit(const char* site) {
    if (!mashed_re::Race::Diff360LiveEnabled()) return;
    CopyTable005f2a70To0089a384();                                           // 0x00414120
    mashed_re::Race::Diff360Log(site ? site : "tableinit");
}

// The DAT_0089a37c term of FUN_00417750 (0x00417756..0x0041777e). Knob ON only.
//   n = (float)FUN_004241b0()            0x00417756 call, 0x0041775f fild
//   if (n != DAT_005d757c (0.0))         0x00417763 fcom, 0x0041776b test ah,0x44, 0x0041776e jnp (skip when equal)
//     DAT_0089a37c = DAT_0089a380 / n + DAT_0089a37c     0x00417770 fld, 0x00417776 fdiv, 0x00417778 fadd, 0x0041777e fstp
// The rest of FUN_00417750 (the FUN_00414220 call at 0x00417751, DAT_0089a378++ at 0x0041778c, DAT_0089a380 = 0 at
// 0x00417793, DAT_0089a420[FUN_00413fa0()]++ at 0x004177a2) is not part of this step.
extern "C" void __cdecl RaceDiff360Live_RoundEnd37c() {
    if (!mashed_re::Race::Diff360LiveEnabled()) return;
    const float n = static_cast<float>(static_cast<std::int32_t>(GetDat008994c0()));
    if (n != 0.0f) {
        float& a37c = *reinterpret_cast<float*>(mashed_re::Race::k37c);
        a37c = *reinterpret_cast<const float*>(mashed_re::Race::k380) / n + a37c;
    }
    mashed_re::Race::Diff360Log("roundend37c");
}
