// PORT-OT-RACE (2026-11-16): exe-only port body of rva 0x00478660 = re/match/s12/00478660.c
// (hooks.csv C4 c4=frida-replay (L-128)). Reached on the exe only under MASHED_ORIG_TOP + MASHED_OT_RACELOAD
// through the race-load closure of 0x0040d270 (verify/ro_20261116/rltools/rl_bind_gen.py). Callees bind to exe bodies
// by name; a callee with no matched or admitted body is a named stop (Match/PortTop_RaceLoadStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_00478660 PortTop_Bound_00478660
#define FUN_004783f0 PortTop_RLStop_004783f0   // named stop
#define FUN_004785e0 PortTop_RLStop_004785e0   // named stop
#define FUN_004987b0 DbgPrintf_004987b0   // exe body (Match/PortTop_004987b0.cpp)
#define FUN_004b4550 PortTop_RLStop_004b4550   // named stop
#define FUN_004b46b0 Vec3Equal   // exe body (Render/RenderLeaves_ae1.cpp)
#define FUN_004b5030 PortTop_Bound_004b5030
#define FUN_004c39b0 RwCore_004c39b0   // exe body (RwCore/RwCore_004c39b0.cpp)
#define FUN_004c3d90 PortTop_RLStop_004c3d90   // named stop
#define FUN_004c45f0 RwCore_004c45f0   // exe body (RwCore/RwCore_004c45f0.cpp)
#define FUN_004c4dc0 RwCore_004c4dc0   // exe body (RwCore/RwCore_004c4dc0.cpp)
extern "C" {
#include "../../../../re/match/s12/00478660.c"
}
#endif // MASHED_STANDALONE
