// MATCH-S13: port body of 0x004826d0 = the byte-matched source re/match/s13/004826d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Collision/CollisionBodyCreate.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


// Callees: the cone-cast hull builder is the port's C++ ConeCastHullBuild (CollisionBodyCreate.cpp), reached
// through the C forwarder below. FUN_00482140 / FUN_00563810 bind by C name to the raw-address forwarders the
// old body called (Collision/RwpBuildExterns.cpp).
namespace mashed_re { namespace Collision { void* ConeCastHullBuild(int shape, float scale, void* mtx); } }
extern "C" int __cdecl MatchS13_004826d0_ConeCastHullBuild(int shape, float scale, int mtx)
{
    return (int)mashed_re::Collision::ConeCastHullBuild(shape, scale, (void*)mtx);
}

#define MATCH_PORT 1
#define FUN_004826d0 PhysicsCollisionBodyCreate
#define FUN_00481e00 MatchS13_004826d0_ConeCastHullBuild
#pragma comment(linker, "/EXPORT:PhysicsCollisionBodyCreate=_PhysicsCollisionBodyCreate")
extern "C" {
#include "../../../../re/match/s13/004826d0.c"
}
