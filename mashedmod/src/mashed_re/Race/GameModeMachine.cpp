// Mashed RE — MATCH-PORTSTATE-GAMEFLOW-FU: the original game-mode machine 0x004929d0 in the exe (racing phases).
// Exe only. PREREG: verify/matchgameflowfu_20261102/PREREG_MATCH-PORTSTATE-GAMEFLOW-FU.md.
//
// Anchored to MASHED.exe SHA-256:
//   BDCAE093A30FBF226BDD852B9C36798A987AEE33B3AE82BF7404B0336EFD3C0E
//   (preserved in original\MASHED.exe.unpatched)
//
// The body is re/match/gffu/004929d0.c (match.py 95.7%, the residual is the position of the esi save only, see
// its header). Its 30 callees bind to their exe bodies by symbol, so this TU links only when every callee the
// racing cases reach has one. Case 2's callee 0x0040d440 (and under it 0x0040d270, the original's course loader:
// piz, COURSE.LUA, 0x0040cea0 / 0x0040d110 / 0x00426e10, about 24 callees) has no exe body; the exe's course load
// is TrackRenderer port code. 0x0040d440 binds to GameModeMachine_Case2Unreached below, which logs and returns.
// The tick never calls the machine in phase 2 (it calls it in phases 3 and 7 only, and neither case writes 2), so
// that function is unreachable by construction; the log line is there to prove it in the witness.
//
// Witness (mashed_re.log, prefix [portstate-gameflow-fu]): every phase-word change seen across a machine call
// (old -> new), the first non-zero return of 0x0042c220 / 0x00432080 / 0x0040e450, every change of the step word
// 0x00771980 written by 0x004927c0, and every 600 ticks a summary line with the counts.
#include "GameModeMachine.h"

#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

namespace {

bool s_on = false;
bool s_read = false;
unsigned s_ticks = 0, s_machine = 0, s_c220 = 0, s_c080 = 0, s_e450 = 0;

void Log(const char* fmt, ...) {
    if (std::FILE* fp = std::fopen("mashed_re.log", "a")) {
        std::fputs("[portstate-gameflow-fu] ", fp);
        va_list ap;
        va_start(ap, fmt);
        std::vfprintf(fp, fmt, ap);
        va_end(ap);
        std::fputc('\n', fp);
        std::fclose(fp);
    }
}

std::int32_t& I32(std::uintptr_t a) { return *reinterpret_cast<std::int32_t*>(a); }

} // namespace

// Counting forwarders for three callees (the bodies are the matched ones; the forwarders only count).
extern "C" int __cdecl FUN_0042c220(void);                 // Match/MatchS07_0042c220.cpp
extern "C" int __cdecl RaceEndCheckFinish(int param_1);    // Match/MatchS07_00432080.cpp
extern "C" unsigned int __cdecl Flag63b908Get(void);       // Match/MatchS02_0040e450.cpp
extern "C" int GameModeMachine_Fwd0042c220(void) {
    const int r = FUN_0042c220();
    if (r != 0 && s_c220++ == 0) Log("0x0042c220 returned %d (first), 0x0067eab0=%d", r, I32(0x0067eab0u));
    return r;
}
extern "C" int GameModeMachine_Fwd00432080(int p0) {
    const int r = RaceEndCheckFinish(p0);
    if (r != 0 && s_c080++ == 0) Log("0x00432080 returned %d (first), 0x007f1a0c=%d", r, I32(0x007f1a0cu));
    return r;
}
extern "C" int GameModeMachine_Fwd0040e450(void) {
    const int r = static_cast<int>(Flag63b908Get());
    if (r != 0 && s_e450++ == 0) Log("0x0040e450 returned %d (first)", r);
    return r;
}
extern "C" void GameModeMachine_Case2Unreached(void) {
    Log("0x0040d440 reached (case 2): no exe body for 0x0040d270, returned without loading");
}

#define MATCH_PORT 1
#define FUN_004929d0 GameModeMachine_004929d0
#define FUN_004927c0 Step771980Advance        // Match/MatchGFFU_004927c0.cpp
#define FUN_0042c2e0 GetDat0067ecb8           // Match/MatchS07_0042c2e0.cpp
#define FUN_0042c2d0 GetDat0067ecb4           // Match/MatchS07_0042c2d0.cpp
#define FUN_0042b940 StatePhaseSubSet         // Match/MatchS07_0042b940.cpp
#define FUN_0042c2f0 SetDat0067ecb8           // Match/MatchS07_0042c2f0.cpp
#define FUN_0042b930 MenuAlphaGet             // Match/MatchS07_0042b930.cpp
#define FUN_0042b8d0 StatePhaseIsIdle         // Match/MatchS07_0042b8d0.cpp
#define FUN_0040d440 GameModeMachine_Case2Unreached
#define FUN_0042c1d0 RaceStateArrayZero       // Match/MatchS07_0042c1d0.cpp
#define FUN_0042c220 GameModeMachine_Fwd0042c220
#define FUN_0042c280 Init42c280               // Match/MatchS07_0042c280.cpp
#define FUN_0042c1c0 RaceInterruptFlagGet     // Match/MatchS07_0042c1c0.cpp
#define FUN_0042f6a0 GetRaceSubMode           // Match/MatchS07_0042f6a0.cpp
#define FUN_0040e350 GetRenderSubMode         // Match/MatchS02_0040e350.cpp
#define FUN_005c9d00 GetRaceEndTrigger        // Util/GameStateGetters.cpp (xor eax,eax; ret; re/match/gffu/005c9d00.c)
#define FUN_00432080 GameModeMachine_Fwd00432080
#define FUN_0040e450 GameModeMachine_Fwd0040e450
#define FUN_0042b910 RaceEndConstGet          // Match/MatchS07_0042b910.cpp
#define FUN_004331a0 RaceFinalizeOnce         // Match/MatchS08_004331a0.cpp
#define FUN_0040b6e0 Add40b6e0                // Match/MatchS02_0040b6e0.cpp
#define FUN_0040b540 PlayerScoreRankOrder     // Match/MatchS02_0040b540.cpp
#define FUN_0040e340 GetLiveCarCount          // Match/MatchS02_0040e340.cpp
#define FUN_0042f500 GetDat0067ea64           // Match/MatchS07_0042f500.cpp
#define FUN_0040b430 UtilFieldAdd8a94f0       // Match/MatchS02_0040b430.cpp
#define FUN_0040b700 Counters8a9530Inc        // Match/MatchS02_0040b700.cpp
#define FUN_0040b6d0 UtilArrayGet8a94e0       // Match/MatchS02_0040b6d0.cpp
#define FUN_00448700 VehicleSeedWritePair     // Match/MatchS08_00448700.cpp
#define FUN_0040e460 Flag63b908Set            // Match/MatchS02_0040e460.cpp
#define FUN_0042b8f0 StatePhaseIsFinal        // Match/MatchS07_0042b8f0.cpp
#define FUN_0042b8e0 StatePhaseIsTwo          // Match/MatchS07_0042b8e0.cpp
extern "C" {
#include "../../../../re/match/gffu/004929d0.c"
}

namespace mashed_re {
namespace Race {

void GameModeMachine_Tick() {
    if (!s_read) {
        s_read = true;
        s_on = std::getenv("MASHED_PORTSTATE_GAMEFLOW_FU") != nullptr;
        if (s_on) Log("knob ON");
    }
    if (!s_on) return;
    const std::int32_t phase = I32(0x00771968u);   // DAT_00771968, 0x004929d0's switch word
    const std::int32_t step = I32(0x00771980u);    // DAT_00771980, 0x004927c0's step word
    ++s_ticks;
    if (phase == 3 || phase == 7) {
        ++s_machine;
        GameModeMachine_004929d0();
        const std::int32_t after = I32(0x00771968u);
        if (after != phase) Log("phase %d -> %d (tick %u)", phase, after, s_ticks);
    } else {
        Step771980Advance();
    }
    const std::int32_t step2 = I32(0x00771980u);
    if (step2 != step) Log("step 0x00771980 %d -> %d (tick %u)", step, step2, s_ticks);
    if (s_ticks % 600u == 0u)
        Log("ticks=%u machine=%u c220=%u c080=%u e450=%u phase=%d step=%d eab0=%d",
            s_ticks, s_machine, s_c220, s_c080, s_e450, I32(0x00771968u), I32(0x00771980u), I32(0x0067eab0u));
}

} // namespace Race
} // namespace mashed_re
