// MATCH-S15: port body of 0x00490f80 = the byte-matched source re/match/s15/00490f80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Render/GlobalByteQuad.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The exported entry ByteQuadSet80 keeps the old TU's shadow A/B (TT-11) front (not a second body: it only runs
// the matched body, through ShadowAB::RunTracked when MASHED_SHADOW_AB arms it). Render/GlobalByteQuadAB.cpp
// declares ByteQuadSet80(int,int,int,int): the int arguments narrow to the matched body's byte parameters, the same
// truncation the old body's static_cast<std::uint8_t> made.
#include "../Core/HookSystem.h"
#include "../Core/ShadowAB.h"
#include "../Core/ShadowTrack.h"

#define MATCH_PORT 1
#define FUN_00490f80 ByteQuadSet80Body
extern "C" {
#include "../../../../re/match/s15/00490f80.c"
}

static void __cdecl ByteQuadSet80_impl(int a, int b, int c, int d) {
    ByteQuadSet80Body(static_cast<undefined1>(a), static_cast<undefined1>(b), static_cast<undefined1>(c),
                      static_cast<undefined1>(d));
}
// --- shadow A/B (TT-11), moved with the body from Render/GlobalByteQuad.cpp ------------------------------------
// In-process original-vs-port comparison at the real call site; arm with
// MASHED_SHADOW_AB=1 (or --hooks in scenario_launch.py). Results: shadow_ab.log.
// LANE 3: page-level write tracking (Core/ShadowTrack.h).
extern "C" __declspec(dllexport) void __cdecl ByteQuadSet80(int a, int b, int c, int d) {
    SHADOW_AB_COUNTER(ab, "ByteQuadSet80", 0x00490f80u, ShadowAB::kPhaseRace);
    ShadowAB::RunTracked(ab, ByteQuadSet80_impl, SHADOW_STACK_WINDOW(), a, b, c, d);
}
RH_ScopedInstall(ByteQuadSet80, 0x00490f80);
