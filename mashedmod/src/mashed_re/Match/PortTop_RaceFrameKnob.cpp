// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-06 (L-96 top-down lane, 2026-11-21): sub-knob MASHED_OT_RACEFRAME for the race frame of the frame machine.
// The matched 0x00492d30 (Match/PortTop_00492d30.cpp) calls 0x004111c0 in its cases 3 / 6 (re/match/s15/00492d30.c:72,
// FUN_004111c0(0x32)). That call binds here:
//   MASHED_OT_RACEFRAME unset or not "1" (default): PortTop_StandIn05_004111c0, the PORT-TOP-05 counting stand-in
//     (Match/PortTop_StandIns05.cpp); the knob path's race stays the hand session (PortTop_HandFrameGameFlow).
//   MASHED_OT_RACEFRAME=1: PortTop_Bound_004111c0, the byte-matched race frame (Match/PortTop_004111c0.cpp, source
//     re/match/t01g/004111c0.c, whole symbol BYTE-IDENTICAL) with its closure bound by re/tools/bind/porttop_bind.py
//     --profile raceframe; 0x0040fc00 (Race::Tick) and 0x004102f0 run as file-local members of that TU. Its callees with
//     no matched body are named stops (Match/PortTop_RaceFrameStops.cpp). The hand session still runs after the frame
//     machine in this mode (Match/PortTop_00492370.cpp), so the closure is only meaningful once the top-down race load
//     (MASHED_OT_RACELOAD, 0x0040d270) completes: verify/porttop06_20261121/RESULT_PORT-TOP-06.md.
// Only reached on the MASHED_ORIG_TOP path. The dispatcher carries no RVA comment: it is not a body of 0x004111c0.
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdlib>

extern "C" {
void PortTop_StandIn05_004111c0(unsigned int);   // Match/PortTop_StandIns05.cpp
void PortTop_Bound_004111c0(unsigned int);       // Match/PortTop_004111c0.cpp
unsigned g_PortTop06_RaceFrameBoundCalls = 0;

int PortTop_RaceFrameKnobOn(void) {
    static int s = -1;
    if (s < 0) {
        const char* const v = std::getenv("MASHED_OT_RACEFRAME");
        s = (v && v[0] == '1') ? 1 : 0;
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] PORT-TOP-06 sub-knob MASHED_OT_RACEFRAME=%d: rva 0x004111c0 -> %s\n", s,
                         s ? "PortTop_Bound_004111c0 (matched race frame, raceframe closure)" : "PortTop_StandIn05_004111c0 (counting stand-in)");
            std::fclose(f);
        }
    }
    return s;
}

void PortTop_RaceFrame_004111c0(unsigned int p0) {
    if (!PortTop_RaceFrameKnobOn()) {
        PortTop_StandIn05_004111c0(p0);
        return;
    }
    if (g_PortTop06_RaceFrameBoundCalls++ == 0) {
        if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
            std::fprintf(f, "[ORIG_TOP] PORT-TOP-06: first call of the bound race frame 0x004111c0 (arg %u)\n", p0);
            std::fclose(f);
        }
    }
    PortTop_Bound_004111c0(p0);
}
}
#endif // MASHED_STANDALONE
