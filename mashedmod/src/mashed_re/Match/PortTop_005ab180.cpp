// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005ab180 = re/match/s25/005ab180.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ab180 PortTop_Bound_005ab180
#define FUN_005ab620 PortTop_AStop_005ab620   // named stop
#define thunk_FUN_005ab620 PortTop_Bound_005ab180   // PORT-TOP-AUDIO-5A8F30-STOPS: re/match/s25/005ab180.c names its body after the jump target (Ghidra thunk name)
extern "C" {
#include "../../../../re/match/s25/005ab180.c"
}
#endif // MASHED_STANDALONE
