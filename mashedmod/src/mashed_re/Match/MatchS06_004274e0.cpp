// MATCH-S06-FU: port body of 0x004274e0 = the byte-matched source re/match/s06/004274e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK checked=12).
// The same source text is this RVA's only body in the .asi (moved out of Frontend/SkeletonAndScatter_t6.cpp, which was
// asi-only too). mashed_re.exe does not compile this TU.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL), which in the .asi
// are the original's own data.
//
// Behaviour change behind a default-OFF knob (verify/matchs06fu_20261102/PREREG_MATCH-S06-FU.md):
// the old body returned 0 for language index DAT_007f0f60 == 5 without opening a file. The original (= the matched
// source) opens "USA.dat" (0x005cd5ac) there. MASHED_LOC_USA_DAT=1 runs the matched body for index 5; unset, the
// installed entry keeps the old return 0 for index 5. Every other index runs the matched body in both settings.
#include "../Core/HookSystem.h"
#include "../Core/ShadowTrack.h"
#include "../Core/ShadowAB.h"
#include <cstdint>

#define MATCH_PORT 1
#define FUN_004274e0 LocalizationFileLoadBody
#define FUN_004cbd30 MatchS06_004274e0_Call004cbd30
#define FUN_004cc160 MatchS06_004274e0_Call004cc160
#define FUN_004cc230 MatchS06_004274e0_Call004cc230
// Callees with no port body in the .asi's link of this TU: raw-address forwarders (S04 rule), the call the original makes.
extern "C" unsigned int MatchS06_004274e0_Call004cbd30(void *stream, void *buf, unsigned int size) {
    return reinterpret_cast<unsigned int (__cdecl*)(void *, void *, unsigned int)>(0x004cbd30u)(stream, buf, size);
}
extern "C" int MatchS06_004274e0_Call004cc160(void *stream, void *a) {
    return reinterpret_cast<int (__cdecl*)(void *, void *)>(0x004cc160u)(stream, a);
}
extern "C" void * MatchS06_004274e0_Call004cc230(int type, int mode, const char *name) {
    return reinterpret_cast<void * (__cdecl*)(int, int, const char *)>(0x004cc230u)(type, mode, name);
}
extern "C" {
#include "../../../../re/match/s06/004274e0.c"
}

// MASHED_LOC_USA_DAT (default OFF), read once.
static bool LocUsaDatKnob() {
    static const bool s_on = GetEnvironmentVariableA("MASHED_LOC_USA_DAT", nullptr, 0) != 0;
    return s_on;
}

// Knob gate in front of the body (not a second body: it only returns early or forwards).
static std::uint32_t __cdecl LocalizationFileLoad_impl() {
    if (DAT_007f0f60 == 5 && !LocUsaDatKnob())
        return 0u;                                  // old body's index-5 result (knob OFF)
    return static_cast<std::uint32_t>(LocalizationFileLoadBody());
}

// --- shadow A/B (TT-11), moved with the body from Frontend/SkeletonAndScatter_t6.cpp ----------------------------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
extern "C" __declspec(dllexport) std::uint32_t __cdecl LocalizationFileLoad() {
    SHADOW_AB_COUNTER(ab, "LocalizationFileLoad", 0x004274e0u, ShadowAB::kPhaseRace);
    return ShadowAB::Run(ab, LocalizationFileLoad_impl);
}
RH_ScopedInstall(LocalizationFileLoad, 0x004274e0);
