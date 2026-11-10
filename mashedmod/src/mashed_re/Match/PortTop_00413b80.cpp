// PORT-OT-HUD (L-111 (a) step 1): exe-only port body of rva 0x00413b80 = the matched source re/match/s03/raw_00413b80.c
// (BYTE-IDENTICAL 43 B): tex = FUN_0040bb30("VehicleIcons"), FUN_004770c0(&DAT_0063bd50, 0x37, 0x14, tex), then
// FUN_00476cb0(&DAT_0063bd50, 5, 6). The matched 0x0040bbb0 (Match/PortTop_0040bbb0.cpp) calls it on the MASHED_ORIG_TOP
// path (re/match/s02/0040bbb0.c:107), after fx.txd is loaded into DAT_0063b8f8 (:73), which 0x0040bb30 searches.
// Callees bind by name, no raw-address call:
//  - 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp, both targets).
//  - 0x004770c0 = Orig_004770c0, the exe-only matched copy Match/PortTop_004770c0.cpp. The exe's hand body of this RVA
//    (Render/RfxEmitter.cpp Rfx::Create) takes a port Emitter struct, not the original 0x40-byte record at 0x0063bd50.
//  - 0x00476cb0 = Batch476cb0 (Match/MatchS12_00476cb0.cpp, the matched source, original record layout).
// The raw source passes &DAT_0063bd50 (undefined1*) to uint* / int parameters and a string literal to an undefined4
// parameter (C conversions C++ refuses), so each callee name binds to a C++ overload taking the pointer that forwards
// to the exe symbol. The source text's own prototypes stay declared and unused.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" int Search40bb30(const char* name);
extern "C" void Orig_004770c0(unsigned int* p0, unsigned int p1, unsigned int p2, unsigned int p3);
extern "C" void Batch476cb0(void* holder, unsigned int p2, unsigned int p3);
static inline unsigned int PortTop_Call0040bb30_00413b80(const char* name) {
    return static_cast<unsigned int>(Search40bb30(name));
}
static inline void PortTop_Call004770c0_00413b80(void* p0, unsigned int p1, unsigned int p2, unsigned int p3) {
    Orig_004770c0(static_cast<unsigned int*>(p0), p1, p2, p3);
}
static inline void PortTop_Call00476cb0_00413b80(void* p0, unsigned int p1, unsigned int p2) { Batch476cb0(p0, p1, p2); }
#define FUN_00413b80 Orig_00413b80
#define FUN_0040bb30 PortTop_Call0040bb30_00413b80
#define FUN_004770c0 PortTop_Call004770c0_00413b80
#define FUN_00476cb0 PortTop_Call00476cb0_00413b80
extern "C" {
#include "../../../../re/match/s03/raw_00413b80.c"
}
#endif  // MASHED_STANDALONE
