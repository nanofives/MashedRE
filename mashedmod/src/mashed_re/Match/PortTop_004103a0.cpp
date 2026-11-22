// BIND-GATES-ALLKNOBS (2026-11-21): exe-only port body of rva 0x004103a0 = re/match/s02/004103a0.c
// (match.py BYTE-IDENTICAL, hooks.csv C4 c4=match). Reached on the exe only under MASHED_ORIG_TOP, from the bound caller named in
// re/tools/bind/state/gates/profile.json (re/tools/bind/porttop_bind.py --profile gates). Callees bind to exe bodies by
// name; a callee with no matched or admitted body, or outside the gate roots, is a named stop (Match/PortTop_GatesStops.cpp).
// RVA-LINT: ORIGTOP-ONLY
#ifdef MASHED_STANDALONE
#define MATCH_PORT 1
#define FUN_004103a0 PortTop_Bound_004103a0
#define FUN_0040e560 PortTop_Bound_0040e560   // exe body (Match/PortTop_0040e560.cpp)
#define FUN_00411ae0 PortTop_Bound_00411ae0   // exe body (Match/PortTop_00411ae0.cpp)
#define FUN_00411d60 PortTop_Bound_00411d60   // exe body (Match/PortTop_00411d60.cpp)
#define FUN_00418860 PortTop_GTStop_00418860   // named stop
#define FUN_0041da90 DeltaTimeOutGet   // exe body (Match/MatchS04_0041da90.cpp)
#define FUN_00426c90 PortTop_Bound_00426c90   // exe body (Match/PortTop_00426c90.cpp)
#define FUN_00429840 RaceStateLatchSet   // exe body (Match/MatchS06_00429840.cpp)
#define FUN_00429860 PortTop_Bound_00429860   // exe body (Match/PortTop_00429860.cpp)
#define FUN_0042f6a0 GetRaceSubMode   // exe body (Match/MatchS07_0042f6a0.cpp)
#define FUN_00430820 PortTop_Bound_00430820   // exe body (Match/PortTop_00430820.cpp)
#define FUN_0046baa0 VehiclePhysicsStateReset   // exe body (Race/SubStateCallees.cpp)
#define FUN_0046c730 EntityDamageStateGet   // exe body (Race/SubStateCallees.cpp)
#define FUN_0046c750 EntityVelocityCounterGet   // exe body (Race/SubStateCallees.cpp)
#define FUN_0046c7b0 PortTop_GTStop_0046c7b0   // named stop
#define FUN_0046d780 PortTop_Bound_0046d780   // exe body (Match/PortTop_0046d780.cpp)
#define FUN_0046d7f0 BoundedTableSignClamp46d7f0   // exe body (Vehicle/LaunchRevCharge.cpp)
extern "C" {
#include "../../../../re/match/s02/004103a0.c"
}
#endif // MASHED_STANDALONE
