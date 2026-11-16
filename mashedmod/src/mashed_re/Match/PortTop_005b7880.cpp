// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005b7880 = re/match/s27/005b7880.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005b7880 PortTop_Bound_005b7880
#define FUN_005ad2e0 PortTop_Bound_005ad2e0
#define FUN_005b0f40 PortTop_AStop_005b0f40   // named stop
#define FUN_005b1500 PortTop_Bound_005b1500
#define FUN_005b7e20 PortTop_Bound_005b7e20
#define thunk_FUN_005b1500 PortTop_Bound_005b1500   // PORT-TOP-AUDIO-5A8F30-STOPS: Ghidra thunk_ name of 0x005b15d0 (jmp 0x005b1500, called at 0x005b789a); the port calls the target body
extern "C" {
#include "../../../../re/match/s27/005b7880.c"
}
#endif // MASHED_STANDALONE
