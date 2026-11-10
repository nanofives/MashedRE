// PORT-OT-HUD (L-111 (a) step 1): exe-only port body of rva 0x004768c0 = the matched source re/match/s12/004768c0.c
// (MATCH-S12, BYTE-IDENTICAL 211 B): for the record p, frame = atomic(p[1])->+0x04, FUN_004e7e30(atomic, 0), RwFrameDestroy
// (frame), RpAtomicDestroy(atomic), RwGlobals +0x10c (librw free) on each non-zero p[4..10], then FUN_004b6520(p, 0x40).
// Its only caller in the exe is the matched 0x00413bb0 (Match/PortTop_00413bb0.cpp), which passes the record 0x00413b80
// built (0x0063bd50), on the MASHED_ORIG_TOP path (AppDestroy 0x00402a40 -> 0x0040bd00). The other caller on that path,
// the matched 0x004210f0 (Match/PortTop_004210f0.cpp), keeps the stand-in PortTop_StandIn04C_004768c0: its record
// 0x0063e548 is built by 0x004210b0, still the stand-in PortTop_StandIn04G_004210b0, so p[1] is 0 there and the first
// read (*(p[1] + 4)) would be at address 4.
// Callees bind by name, no raw-address call: 0x004e7e30 Orig_004e7e30 (matched), 0x004c0c20 RwFrameDestroy and
// 0x004e6920 RpAtomicDestroy (LibRw/RwApiBind.cpp, the PTank-record case frees the record and drops its texture),
// 0x004b6520 Orig_004b6520 (matched).
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
extern "C" int MashedRw_FrameDestroy_004c0c20(int frame);
extern "C" int MashedRw_AtomicDestroy_004e6920(int atomic);
#define FUN_004768c0 Orig_004768c0
#define FUN_004b6520 Orig_004b6520                              // matched, Match/PortTop_004b6520.cpp
#define FUN_004c0c20 PortTop_004768c0_Call004c0c20
#define FUN_004e6920 PortTop_004768c0_Call004e6920
#define FUN_004e7e30 Orig_004e7e30                              // matched, Match/PortTop_004e7e30.cpp
extern "C" {
unsigned int PortTop_004768c0_Call004c0c20(int p0) { return static_cast<unsigned int>(MashedRw_FrameDestroy_004c0c20(p0)); }
unsigned int PortTop_004768c0_Call004e6920(int p0) { return static_cast<unsigned int>(MashedRw_AtomicDestroy_004e6920(p0)); }
#include "../../../../re/match/s12/004768c0.c"
}
#endif  // MASHED_STANDALONE
