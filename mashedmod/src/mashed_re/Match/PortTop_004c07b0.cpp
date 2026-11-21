// RACE-RWGLOBALS-FRAMELIST (2026-11-21): exe-only port body of rva 0x004c07b0 = re/match/s19/004c07b0.c (the RW core's
// frame-module open, match.py BYTE-IDENTICAL 124 B, hooks.csv C4 c4=match) for the MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// path. Called once by PortTop_RL_FrameOpenSeh (Match/PortTop_StandInsRL.cpp) from LibRw/RwApiBind.cpp RaceLoadFrameOpen,
// on the librw RwGlobals record with offset 0x164: it sets the core dirty-frame list head at +0xbc / +0xc0 that the RW
// core body 0x004c15c0 (under the bound 0x00458c70) links into.
// Its callee 0x004cc9e0 binds to the exe's existing thunk body Orig_004cc9e0 (Match/PortTop_004cc9e0.cpp), which jumps
// to the exe's freelist-create binding PortTop_RwFreeListCreate_004cc820 (Match/PortTop_004cc820.cpp), not to
// RwCore_004cc9e0: RwCore_004cc820 is a 74.5% source listed provisional only for MASHED_RWC_CORE
// (verify/rwccorerun_20261111/PROVISIONAL_RWC.tsv), and it links the new freelist into the RW core list at 0x007d45cc,
// which is 0 on this path (first knob boot: write to 4 with ecx 0x007D45CC, RESULT_RACE-RWGLOBALS-FRAMELIST.md). The
// binding's two declared deviations apply (no block preallocated, header +0x1c / +0x20 not linked into 0x007d45cc).
// RwCore/RwCore_004c07b0.cpp stays the MASHED_RWC_CORE copy and carries the ORIGTOP-ONLY marker (dual copy allowlisted, as
// Match/PortTop_004cc9e0.cpp beside RwCore/RwCore_004cc9e0.cpp). Reached only on the ORIG_TOP path.
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004c07b0 PortTop_Bound_004c07b0
#define FUN_004cc9e0 Orig_004cc9e0   // exe body (Match/PortTop_004cc9e0.cpp) -> PortTop_RwFreeListCreate_004cc820
extern "C" {
#include "../../../../re/match/s19/004c07b0.c"
}
#endif // MASHED_STANDALONE
