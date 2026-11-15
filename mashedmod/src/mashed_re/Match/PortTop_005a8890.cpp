// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x005a8890 = re/match/s25/005a8890.c
// (L-126 admitted (re/match/ADMITTED.tsv, EQUIV)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a8890 PortTop_Bound_005a8890
#define FUN_005aad40 PortTop_Bound_005aad40
#define FUN_005b0c70 PortTop_Bound_005b0c70
extern "C" {
#include "../../../../re/match/s25/005a8890.c"
}
#endif // MASHED_STANDALONE
