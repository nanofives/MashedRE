// PORT-OT-GAMEPLAY (L-111 (a) step 1): exe-only port body of rva 0x004725f0 = the matched source re/match/s11/004725f0.c
// (MATCH-S11 h7, match.py 100%): for i < 10, DAT_00691500[i].c = .d = 0, then 0x0046be10(&rec.a, i, i, 0) and
// 0x0046be10(&rec.b, i, i + 10, 0). Called on the MASHED_ORIG_TOP path by the matched 0x0040cfd0
// (Match/PortTop_0040cfd0.cpp, from AppDestroy 0x00402a40). Callee binds by name: 0x0046be10 -> Orig_0046be10, the
// exe-only matched copy Match/PortTop_0046be10.cpp (the existing Write46be10 is .asi-only). The other body of 0x004725f0
// is the .asi-only Match/MatchS11_004725f0.cpp.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004725f0 Orig_004725f0
#define FUN_0046be10 Orig_0046be10                              // matched, Match/PortTop_0046be10.cpp
extern "C" {
#include "../../../../re/match/s11/004725f0.c"
}
#endif  // MASHED_STANDALONE
