// PORT-OT-OTHER-2 (L-111 (a) step 1, subsystem particle): exe-only port body of rva 0x0048a830 = the matched source
// re/match/s14/0048a830.c (MATCH-S14, BYTE-IDENTICAL): DAT_0071fa34 = 0, then FUN_004b6480(&DAT_007151f0, 0x200).
// Its only caller in the exe is the matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp, MASHED_ORIG_TOP path), which calls
// it as FUN_0048a830(0) (re/match/s02/0040bbb0.c:95). The body takes no argument (cdecl, caller pops).
// Callee by name, no raw-address call: 0x004b6480 -> Orig_004b6480 (Match/PortTop_004b6480.cpp, exe-only copy since
// PORT-OT-HUD-2). The source declares it (byte*, uint), the exe copy is (unsigned char*, int): one forwarder.
// Shared state: no exe TU names 0x0071fa34 or 0x007151f0 (the other sources that do, re/match/s14/0048a850.c,
// 0048ade0.c and re/match/racepath/0048ab70.c, are compiled only in .asi-only TUs or not at all).
// The other body of this RVA is the .asi-only Match/MatchS14_0048a830.cpp (ZeroFill48a830), so this TU is the
// ORIGTOP-ONLY class (CROSS-TARGET).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" {
void Orig_004b6480(unsigned char* p0, int p1);              // matched, Match/PortTop_004b6480.cpp
void PortTop_0048a830_Call004b6480(unsigned char* p0, unsigned int p1) { Orig_004b6480(p0, static_cast<int>(p1)); }
}
#define FUN_0048a830 Orig_0048a830
#define FUN_004b6480 PortTop_0048a830_Call004b6480
extern "C" {
#include "../../../../re/match/s14/0048a830.c"
}
#endif  // MASHED_STANDALONE
