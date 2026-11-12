// RO-BIND-TIMER (replay oracle 2, L-125 (a)): exe-only port body of rva 0x00493390 = the byte-matched source
// re/match/s15/00493390.c (match.py BYTE-IDENTICAL 239 B, relocheck OK checked=22; hooks.csv C4 c4=match).
// It is the tick-boundary clock: QPC/3 MHz via 0x004950b0, writes DAT_007f1000 = 50 (0x004933d5), DAT_007f1004,
// DAT_007f1010, DAT_007f0ffc, DAT_00771984, DAT_007f0ff0 += 50, DAT_007f1030 = now, and the 3 s window at
// DAT_007f102c / DAT_007f1024 / DAT_007f1028. Its only caller is the matched 0x00493480 (Match/MatchS15_00493480.cpp),
// which binds FUN_00493390 here in the exe. The exe reaches it only on the MASHED_ORIG_TOP path with the sub-knob
// MASHED_OT_TIMER=1 (Match/PortTop_00492370.cpp). The .asi keeps the raw-address call (no .asi body for this RVA).
// Callee 0x004950b0 binds by name to its matched exe body QpcTimeScaledTo3Mhz (Match/MatchS16_004950b0.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00493390 PortTop_Bound_00493390
#define FUN_004950b0 QpcTimeScaledTo3Mhz   // matched, Match/MatchS16_004950b0.cpp
extern "C" {
#include "../../../../re/match/s15/00493390.c"
}
#endif // MASHED_STANDALONE
