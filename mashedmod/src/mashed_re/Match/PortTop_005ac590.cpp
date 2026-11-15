// PORT-TOP-RWAUDIO-TABLE-632BA8 (2026-11-13): exe-only port body of rva 0x005ac590 = re/match/s30/005ac590.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT
// through the RW-audio init/shutdown table 0x00632ba8 that 0x005a5f60 walks for 0x005a5f00 (slots bound by the
// image code-pointer fix-up, Compat/PortTopCodePtrImage.inc). Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005ac590 PortTop_Bound_005ac590
#define FUN_005aa0c0 PortTop_Bound_005aa0c0
#define FUN_005ae7e0 PortTop_Bound_005ae7e0
// LAB_005ac5c0 is a code address the body stores, not called here: the port stores the original address (no exe body;
// a later call through it is reported by MASHED_CODE_TRAP, image design (b)).
#define LAB_005ac5c0 (*(undefined1 *)0x005ac5c0u)
extern "C" {
#include "../../../../re/match/s30/005ac590.c"
}
#endif // MASHED_STANDALONE
