// PORT-TOP-02: a byte-matched leaf the MASHED_ORIG_TOP knob path calls (see PortTop_00492370.cpp). EXE ONLY.
// (0x00492d20 and 0x00499730 share their existing wrappers Match/MatchS15_00492d20.cpp / MatchS16_00499730.cpp.)
//   rva 0x00499cc0 (re/match/s16/00499cc0.c): DestroyWindow(DAT_007e9584), returns DAT_007e95a8 (the pump MSG's wParam).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00499cc0 Orig_00499cc0
extern "C" {
#include "../../../../re/match/s16/00499cc0.c"
}
#endif // MASHED_STANDALONE
