// PORT-OT-HUD-2 (L-111 (a) step 1): exe-only port body of rva 0x004210b0 = the matched source re/match/s05/004210b0.c:
// tex = FUN_0040bb30("smoke"), FUN_004770c0(&DAT_0063e548, 0x817, 0x80, tex), FUN_00421080(), FUN_004b6480(&DAT_0063e4a8,
// 0x80). The matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls it on the MASHED_ORIG_TOP path
// (re/match/s02/0040bbb0.c:108), after fx.txd is loaded into DAT_0063b8f8, which 0x0040bb30 searches. The record it builds
// at 0x0063e548 is the one the matched 0x004210f0 (Match/PortTop_004210f0.cpp) tears down through 0x004768c0 in
// AppDestroy 0x00402a40.
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp, both targets).
//  - 0x004770c0 = Orig_004770c0, the exe-only matched copy Match/PortTop_004770c0.cpp (PORT-OT-HUD), on the original
//    0x40-byte record. Flags 0x817 = bits 0x800 | 0x10 | 4 | 2 | 1: arrays p[4] / p[5] / p[6] / p[8] through 0x00474db0
//    and the one optional setter 0x00476c10 (re/match/s12/004770c0.c bit 0x800).
//  - 0x00421080 = Orig_00421080 (Match/PortTop_00421080.cpp, exe-only copy).
//  - 0x004b6480 = Orig_004b6480 (Match/PortTop_004b6480.cpp, exe-only copy).
// The raw source passes a string literal to an undefined4 parameter and &DAT_0063e548 (undefined1*) to a uint*
// parameter (C conversions C++ refuses), so those two callee names bind to C++ overloads taking the pointer that forward
// to the exe symbol (the Match/PortTop_00413b80.cpp pattern). The source text's own prototypes stay declared and unused.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" int Search40bb30(const char* name);
extern "C" void Orig_004770c0(unsigned int* p0, unsigned int p1, unsigned int p2, unsigned int p3);
static inline unsigned int PortTop_Call0040bb30_004210b0(const char* name) {
    return static_cast<unsigned int>(Search40bb30(name));
}
static inline void PortTop_Call004770c0_004210b0(void* p0, unsigned int p1, unsigned int p2, unsigned int p3) {
    Orig_004770c0(static_cast<unsigned int*>(p0), p1, p2, p3);
}
#define FUN_004210b0 Orig_004210b0
#define FUN_0040bb30 PortTop_Call0040bb30_004210b0
#define FUN_004770c0 PortTop_Call004770c0_004210b0
#define FUN_00421080 Orig_00421080                          // matched, Match/PortTop_00421080.cpp
#define FUN_004b6480 PortTop_004210b0_Call004b6480
extern "C" {
void Orig_004b6480(unsigned char* p0, int p1);              // matched, Match/PortTop_004b6480.cpp
void PortTop_004210b0_Call004b6480(unsigned char* p0, unsigned int p1) { Orig_004b6480(p0, static_cast<int>(p1)); }
#include "../../../../re/match/s05/004210b0.c"
}
#endif  // MASHED_STANDALONE
