// PORT-TOP-06 (2026-11-21): exe-only port body of rva 0x00425a40 = re/match/s05/00425a40.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACEFRAME
// through the race-frame closure of 0x004111c0 (re/tools/bind/porttop_bind.py --profile raceframe). Callees bind to
// exe bodies by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceFrameStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00425a40 PortTop_Bound_00425a40
#define FUN_00418860 PortTop_RFStop_00418860   // named stop
#define FUN_00422ba0 PortTop_Bound_00422ba0
#define FUN_00423b00 PortTop_Bound_00423b00
#define FUN_00424eb0 PortTop_RFStop_00424eb0   // named stop
#define FUN_004252c0 PortTop_Bound_004252c0
#define FUN_00470c70 PortTop_RFStop_00470c70   // named stop
extern "C" {
#include "../../../../re/match/s05/00425a40.c"
}
#endif // MASHED_STANDALONE
