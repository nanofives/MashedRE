// PORT-TOP-02: 0x00499890 OS-version log (re/match/s16/00499890.c, MATCH-S16 h6), compiled verbatim for the
// MASHED_ORIG_TOP knob path (called by 0x00499ba0). EXE ONLY. No <windows.h> here (the source has its own types).
//   rva 0x00496400 -> matched, Match/PortTop_00496400.cpp (BIND-CRT-STDIO-CALLERS).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00499890 Orig_00499890
#define FUN_00496400 Orig_00496400
#pragma comment(lib, "advapi32.lib")
extern "C" {
#include "../../../../re/match/s16/00499890.c"
}
#endif // MASHED_STANDALONE
