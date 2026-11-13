// RO-EXE-REPLAY-KNOB (replay oracle 7): exe-only port body of rva 0x00497310 = the byte-matched source
// re/match/s16/00497310.c (match.py --bytes BYTE-IDENTICAL, hooks.csv C4). The per-player controller binding lookup
// (player, action) -> 0 / 0xff / axis value from the binding record 0x007e95c0 + 0x200 * player.
// The exe reaches it only through the replay dispatcher PortTop_Replay_00497310 (Match/PortTop_Replay.cpp), which
// returns the replay script's value for player 0 while MASHED_REPLAY is on. Its caller in the original, 0x00496530
// (cooked input, 13 call sites 0x004965af..0x00496690), has no exe body yet, so the dispatcher is called by the
// replay self-test only until that binding lands (PORT-OT-RACE / the input poll row).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00497310 PortTop_Bound_00497310
extern "C" {
#include "../../../../re/match/s16/00497310.c"
}
#endif // MASHED_STANDALONE
