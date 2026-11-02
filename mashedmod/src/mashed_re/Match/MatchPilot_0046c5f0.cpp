// MATCH-TOPDOWN-T13: port body of 0x0046c5f0 = the byte-matched source re/match/pilot/a5_0046c5f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%, relocheck OK).
// It replaces Collision::TriangleFaceNormal's body (Collision/CarWorldContacts.cpp), which the exe calls from
// VehicleWheelForceIntegrator 0x0046ddb0 (Vehicle/ForceIntegrator.cpp, A5FaceNormal, original site 0x0046e6f3).
// Exe only, as CarWorldContacts.cpp is. x87 TU (x87_tus.txt), like the original.
//
// Argument order. Under the DDK the static callee gets a register convention: 1st param EAX, 2nd EDX, 3rd ECX,
// out ESI (0x0046da80's call loads EDX = &DAT_0088e620[2], ECX = &DAT_0088e620[3] for its 2nd / 3rd args). The
// matched body forms A = a - (2nd), B = a - (3rd), n = B x A. TriangleFaceNormal(apex, p1, p2) forms
// d1 = apex - p1, d2 = apex - p2, n = d1 x d2, so it is FUN_0046c5f0(apex, p2, p1, out): the same products in
// the same operand order. 0x0046e6f3 loads ECX = &scratch[6] (3rd) and EDX = &scratch[3] (2nd), and
// ForceIntegrator.cpp passes p1 = &scratch[6], p2 = &scratch[3], so the original's register assignment holds.
//
// FUN_004c3b30 binds to Collision::FastSqrt (ContactDeps.h): the RW LUT sqrt, std::sqrt under MASHED_NO_COLL_LUT,
// the same callee the replaced body used.
#include "../Core/HookSystem.h"
#include "../Collision/ContactDeps.h"
#include "../Collision/ContactSolvers.h"

#define MATCH_PORT 1
extern "C" {
static float MatchSqrt_46c5f0(float x) { return mashed_re::Collision::FastSqrt(x); }
#define FUN_004c3b30 MatchSqrt_46c5f0
// 0x0046c5f0
#include "../../../../re/match/pilot/a5_0046c5f0.c"
#undef FUN_004c3b30
}

namespace mashed_re {
namespace Collision {
void TriangleFaceNormal(const float* apex, const float* p1, const float* p2, float* outN)
{
    FUN_0046c5f0(const_cast<float*>(apex), const_cast<float*>(p2), const_cast<float*>(p1), outN);
}
}  // namespace Collision
}  // namespace mashed_re
