// RACE-RWGLOBALS-FRAMELIST (2026-11-21): exe-only port body of rva 0x00458c70 = re/match/s09/00458c70.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD,
// through PortTop_RL_00458c70 (Match/PortTop_StandInsRL.cpp), from the matched 0x0040d270 and the bound 0x004264d0.
// Its one callee 0x004c15c0 is the RW core body RwCore_004c15c0 (RwCore/RwCore_004c15c0.cpp, L-124); it links the frame
// root into the core's dirty-frame list at [RwGlobals+0xbc], which RwApiBind.cpp's RaceLoadFrameOpen (0x004c07b0 on the
// record) sets up under the same knob. PREREG verify/ro_20261116/PREREG_RACE-RWGLOBALS-FRAMELIST.md.
// Hand-written, not a rl_bind_gen.py wrapper (the generator ALIASes 0x00458c70 to PortTop_RL_00458c70 and leaves this file).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00458c70 PortTop_Bound_00458c70
#define FUN_004c15c0 RwCore_004c15c0   // RW core body (RwCore/RwCore_004c15c0.cpp)
extern "C" {
#include "../../../../re/match/s09/00458c70.c"
}
#endif // MASHED_STANDALONE
