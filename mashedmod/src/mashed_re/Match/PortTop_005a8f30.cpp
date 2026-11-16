// PORT-TOP-AUDIO-5A8F30-STOPS (2026-11-16): exe-only port body of rva 0x005a8f30 = re/match/s25/005a8f30.c
// (match.py --bytes BYTE-IDENTICAL 589 B, relocheck OK; C4 by match through re/promotions/PORT-TOP-AUDIO-5A8F30-STOPS.tsv).
// Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOINIT from 0x005a8ea0 (table 0x00632c28 entry, call at
// 0x005a8ece). Callees bind to exe bodies by name; a callee with no matched or admitted body is a named stop
// (Match/PortTop_AudioStops.cpp): 0x005a91b0 is the code pointer it stores at +0x28 (0x005a9160).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_005a8f30 PortTop_Bound_005a8f30
#define FUN_005a91b0 PortTop_AStop_005a91b0   // named stop
#define FUN_005a9e10 PortTop_Bound_005a9e10
#define FUN_005aee20 PortTop_Bound_005aee20
extern "C" {
#include "../../../../re/match/s25/005a8f30.c"
}
#endif // MASHED_STANDALONE
