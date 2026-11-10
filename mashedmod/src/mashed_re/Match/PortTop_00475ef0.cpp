// PORT-OT-RENDER (L-111 (a) step 1): exe-only port body of rva 0x00475ef0 = the matched source re/match/s12/00475ef0.c
// (MATCH-S12, BYTE-IDENTICAL): for each of the 16 0xd8-byte records at 0x00691698, 0x004b6520(rec, 0xd8) and
// rec+0xc0 = DAT_00613254. Its only caller on the MASHED_ORIG_TOP path is the matched 0x00476390
// (Match/PortTop_00476390.cpp), called by the matched 0x0040bbb0.
// Callee binds by name, no raw-address call: 0x004b6520 -> Orig_004b6520 (Match/PortTop_004b6520.cpp), through a forwarder
// with the source's prototype.
// Shared state: no other exe TU reads or writes 0x00691698..0x00692417 or 0x00613254.
// The other body of this RVA is the .asi-only Match/MatchS12_00475ef0.cpp, so this TU is the ORIGTOP-ONLY class.
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00475ef0 Orig_00475ef0
#define FUN_004b6520 PortTop_00475ef0_Call004b6520
extern "C" {
void Orig_004b6520(unsigned int dst, unsigned int size);
#include "../../../../re/match/s12/00475ef0.c"
void PortTop_00475ef0_Call004b6520(void* p0, int p1) {
    Orig_004b6520(static_cast<unsigned int>(reinterpret_cast<unsigned long>(p0)), static_cast<unsigned int>(p1));
}
}
#endif  // MASHED_STANDALONE
