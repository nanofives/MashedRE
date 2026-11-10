// PORT-OT-RENDER (L-111 (a) step 1): exe-only port body of rva 0x00476390 = the matched source re/match/s12/00476390.c
// (MATCH-S12, BYTE-IDENTICAL): 0x00475ef0, then two 0x40-byte records at 0x00691618 built by 0x004770c0(rec, 0x817, 0x40,
// "flames3") + 0x00476cb0(rec, 5, 2), two at 0x00692418 by 0x004770c0(rec, 0x817, 0x40, "smoke"), and 0x00692498 by
// 0x004770c0(rec, 0x807, 0x40, 0). The matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls it on the MASHED_ORIG_TOP path.
// Its teardown 0x00475ea0 (Match/PortTop_00475ea0.cpp, from the matched 0x0040bd00) runs 0x004768c0 on the same five records.
// Callees bind by name, no raw-address call (forwarders carry the source's prototypes):
//  - 0x00475ef0 -> Orig_00475ef0 (Match/PortTop_00475ef0.cpp, exe-only).
//  - 0x0040bb30 -> Search40bb30 (Match/MatchS02_0040bb30.cpp).
//  - 0x004770c0 -> Orig_004770c0 (Match/PortTop_004770c0.cpp, the original 0x40-byte record). Flag 0x800 calls 0x00476c10
//    (Batch476c10, Render/RfxEmitter.cpp), which writes the PTank block +0xd8..+0xe4 and sets +0x40 bit 0x80000.
//  - 0x00476cb0 -> Batch476cb0 (Match/MatchS12_00476cb0.cpp).
// Shared state: no other exe TU reads or writes 0x00691618..0x00691697, 0x00692418..0x00692457 or 0x00692498..0x006924d7.
// No other body of this RVA in the source tree.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00476390 Orig_00476390
#define FUN_00475ef0 Orig_00475ef0                              // matched, Match/PortTop_00475ef0.cpp
#define FUN_0040bb30 PortTop_00476390_Call0040bb30
#define FUN_00476cb0 PortTop_00476390_Call00476cb0
#define FUN_004770c0 PortTop_00476390_Call004770c0
extern "C" {
int Search40bb30(const char* name);
void Batch476cb0(void* holder, unsigned int p2, unsigned int p3);
void Orig_004770c0(unsigned int* p0, unsigned int p1, unsigned int p2, unsigned int p3);
#include "../../../../re/match/s12/00476390.c"
undefined4 PortTop_00476390_Call0040bb30(char* p0) { return static_cast<undefined4>(Search40bb30(p0)); }
void PortTop_00476390_Call00476cb0(Rec40* p0, int p1, int p2) {
    Batch476cb0(p0, static_cast<unsigned int>(p1), static_cast<unsigned int>(p2));
}
void PortTop_00476390_Call004770c0(Rec40* p0, uint p1, uint p2, undefined4 p3) {
    Orig_004770c0(reinterpret_cast<unsigned int*>(p0), p1, p2, p3);
}
}
#endif  // MASHED_STANDALONE
