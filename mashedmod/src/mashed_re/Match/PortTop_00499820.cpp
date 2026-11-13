// PORT-TOP-02: 0x00499820 window procedure (re/match/s16/00499820.c, MATCH-S16 h6), compiled verbatim for the
// MASHED_ORIG_TOP knob path (see PortTop_00492370.cpp). EXE ONLY. No <windows.h> here (the source has its own types).
//   rva 0x00496490 log open (WM_CREATE) -> matched, Match/PortTop_00496490.cpp (BIND-CRT-STDIO-CALLERS): mashed.log in
//     the origtop_rw write overlay; MASHED_ORIGTOP_WRITE_DIR=off gives the reference's disable_log value (returns 0).
//   rva 0x00496470 log close (WM_DESTROY) -> matched, Match/PortTop_00496470.cpp (BIND-CRT-STDIO-CALLERS).
//   rva 0x004960e0 activate notify (WM_ACTIVATE) -> stand-in: the hand WndProc's job on WM_ACTIVATE, Audio::SetOutputActive.
#ifdef MASHED_STANDALONE
#include <cstdio>
#include <cstdlib>
#define MATCH_PORT 1
#define FUN_00499820 Orig_00499820
#define FUN_00496490 Orig_00496490
#define FUN_00496470 Orig_00496470
#define FUN_004960e0 PortTop_StandIn_004960e0
extern "C" {
void PortTop_HandSetOutputActive(int active);       // exe_main.cpp
void PortTop_StandIn_004960e0(int active) {
    PortTop_HandSetOutputActive(active);
    // no_focus_pause equivalent (see PortTop_00492370.cpp): the reference never parks in WaitMessage.
    static const bool s_focusPause = std::getenv("MASHED_ORIG_TOP_FOCUSPAUSE") != nullptr;
    if (!s_focusPause) *reinterpret_cast<volatile int*>(0x0077391cu) = 1;
}
#include "../../../../re/match/s16/00499820.c"
}
#endif // MASHED_STANDALONE
