// PORT-TOP-AUDIOTICK-STOPS (2026-11-13): exe-only port body of rva 0x00407600 = re/match/s01/00407600.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_AUDIOTICK
// through the audio-tick closure of 0x00466b50 / 0x0045d3f0 / 0x0045d430. Callees bind to exe bodies by name;
// a callee with no matched or admitted body is a named stop (Match/PortTop_AudioStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00407600 PortTop_Bound_00407600
#define FUN_0047d150 PortTop_Bound_0047d150
// MASHED_RE_PORT drops the source extern block (same as Match/MatchS01.cpp); the table is read at its original address.
#define MASHED_RE_PORT 1
#define DAT_00639d80 ((Rec639d80 *)0x00639d80u)
extern "C" {
char *FUN_0047d150(int h);   // declared here: the source declares it only outside MASHED_RE_PORT
#include "../../../../re/match/s01/00407600.c"
}
#endif // MASHED_STANDALONE
