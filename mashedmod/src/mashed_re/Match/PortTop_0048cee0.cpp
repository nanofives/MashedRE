// PARTICLE-3-SWAP: exe-only port body of rva 0x0048cee0 = the matched source re/match/s14/0048cee0.c (MATCH-S14 h7, 100%,
// 140 B incl. jump and index tables). Per emitter i (0..16) it picks the texture ("smoke"; "wfall" for 0xd / 0xe / 0x10;
// "flatshad" for 1) through 0x0040bb30, then builds record i of the 17 0x40-byte records at 0x00722470:
// 0x004770c0(&rec[i], 0x817, 0x100, tex) and 0x00476cb0(&rec[i], 5, 6), and zeroes DAT_007668b0[i].
// The original passes i in EDI (Ghidra unaff_EDI): the matched source models that as `static void FUN_0048cee0(int i)`
// beside a stand-in caller. PortTop_PS3_Cee0_0048cee0(i) is the exported entry; its one caller is the 0x0048e820 bridge
// (Match/PortTop_0048e820.cpp), which hands it the same index the original keeps in EDI.
// Callees bind by name: 0x0040bb30 = Search40bb30 (Match/MatchS02_0040bb30.cpp), 0x004770c0 = Orig_004770c0
// (Match/PortTop_004770c0.cpp), 0x00476cb0 = Batch476cb0 (Match/MatchS12_00476cb0.cpp), 0x004b6480 = Orig_004b6480
// (only the source's own stand-in caller references it; that caller is renamed and never called).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_0048cee0 Orig_0048cee0
#define Standin_0048e820 PortTop_PS3_s14StandinUnused_0048e820
#define FUN_0040bb30 Search40bb30
#define FUN_004770c0 Orig_004770c0
#define FUN_00476cb0 Batch476cb0
#define FUN_004b6480 Orig_004b6480
extern "C" {
#include "../../../../re/match/s14/0048cee0.c"

void PortTop_PS3_Cee0_0048cee0(int i) { Orig_0048cee0(i); }
}  // extern "C"
#endif  // MASHED_STANDALONE
