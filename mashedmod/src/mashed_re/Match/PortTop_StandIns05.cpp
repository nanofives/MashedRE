// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-05: named stand-ins for the frame subtree of the run function (rva 00492290) on the MASHED_ORIG_TOP path (exe
// only). Each logs its first call (and a call count where it can repeat); the reason is in the comment and in
// verify/porttop05_20261112/RESULT_PORT-TOP-05.md. Not bodies of these RVAs.
#ifdef MASHED_STANDALONE
#include <cstdio>

namespace {
void StandInLog(const char* rva, const char* what, int* once) {
    if (*once) return;
    *once = 1;
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva %s: stand-in (PORT-TOP-05) %s\n", rva, what);
        std::fclose(f);
    }
}
} // namespace

extern "C" {
void PortTop_HandFrameMenu(void);
void PortTop_HandFrameInputGlobals(void);

// rva 0043dfd0 (frontend machine): the hand menu navigation and the input-globals write, in the hand order.
unsigned g_PortTop05_FrontendCalls = 0;
void PortTop_StandIn05_0043dfd0(void) {
    static int once;
    StandInLog("0x0043dfd0", "hand menu navigation + input globals (HandTop_FrameMenu, HandTop_FrameInputGlobals)", &once);
    ++g_PortTop05_FrontendCalls;
    PortTop_HandFrameMenu();
    PortTop_HandFrameInputGlobals();
}
// rva 004111c0 (race tick; rva 00492d30 cases 3 / 6, once per 50 of DAT_007f1000): the knob path's race is the hand
// session (PortTop_HandFrameGameFlow after the frame machine). Counted, nothing run.
unsigned g_PortTop05_RaceTickCalls = 0;
void PortTop_StandIn05_004111c0(unsigned int) {
    static int once;
    StandInLog("0x004111c0", "race tick: the hand session runs the race (counted, not run)", &once);
    ++g_PortTop05_RaceTickCalls;
}
// rva 0040d270 (course load; rva 0040d440 = case 2 of the game-mode machine): the hand path loaded the course at the menu launch.
void PortTop_StandIn05_0040d270(int table, int row) {
    // logged on every call: case 2 runs once per launch
    if (std::FILE* f = std::fopen("mashed_re.log", "a")) {
        std::fprintf(f, "[ORIG_TOP] rva 0x0040d270: stand-in (PORT-TOP-05) course load, args table=%08X row=%d; the hand "
                        "path loaded the course at the menu launch\n", static_cast<unsigned>(table), row);
        std::fclose(f);
    }
}
// rva 0043df00 (tail of rva 004030d0): matched, not compiled (level 2).
void PortTop_StandIn05_0043df00(void) { static int once; StandInLog("0x0043df00", "matched, not compiled (level 2)", &once); }
// rva 0040ab40 / 0040ac80 / 0040de10 / 0040e360 / 0043d2a0: callees of the modal step, reached only after a modal box
// reached state 3.
void PortTop_StandIn05_0040ab40(void) { static int once; StandInLog("0x0040ab40", "matched, not compiled (level 2)", &once); }
void PortTop_StandIn05_0040ac80(void) { static int once; StandInLog("0x0040ac80", "matched, not compiled (level 2)", &once); }
void PortTop_StandIn05_0040de10(void) { static int once; StandInLog("0x0040de10", "matched, not compiled (level 2)", &once); }
void PortTop_StandIn05_0040e360(int) { static int once; StandInLog("0x0040e360", "port body in the .asi-only Util/PromoLoop_round10.cpp", &once); }
void PortTop_StandIn05_0043d2a0(int, int) { static int once; StandInLog("0x0043d2a0", "no matched source, no exe body", &once); }
// rva 004ce8e0 (image write under rva 00498810): matched, not compiled (level 2).
void PortTop_StandIn05_004ce8e0(int, char*) { static int once; StandInLog("0x004ce8e0", "matched, not compiled (level 2)", &once); }
}
#endif // MASHED_STANDALONE
