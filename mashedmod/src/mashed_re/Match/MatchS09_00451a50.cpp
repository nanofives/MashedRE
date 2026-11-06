// MATCH-S09: port body of 0x00451a50 = the byte-matched source re/match/s09/00451a50.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Track/WarzonePontoon.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// Exe only (WarzonePontoon.cpp is). Callees: 0x004c4dc0 = RwMatrixInvertEntry (Render/RwMatrixInvert.cpp),
// 0x004c52f0 = FUN_004c52f0 (Collision/RwpSolverMath2.cpp, by its own name), 0x004c4670 = `jmp 0x004c4680`, forwarded
// below to Math::RwMatrixOrthoNormalizeLut (Math/RwMatrixOrthoNormalize.cpp, the 0x004c4680 body), the callee the
// old Track/WarzonePontoon.cpp TrackBody used. Callers: Track/WarzonePontoon.cpp TrackBody (Pontoon_Tick, in place).
#include "../Math/RwMatrixOrthoNormalize.h"


#define MATCH_PORT 1
#define FUN_00451a50 WarzonePontoonTrackBody
#define FUN_004c4dc0 RwMatrixInvertEntry
#define FUN_004c4670 MatchS09_00451a50_Ortho004c4670
#pragma comment(linker, "/EXPORT:WarzonePontoonTrackBody=_WarzonePontoonTrackBody")
extern "C" {
#include "../../../../re/match/s09/00451a50.c"
}
// The 0x004c4670 thunk's forwarder (not a body of 0x004c4670: it calls the port's 0x004c4680 body).
extern "C" void MatchS09_00451a50_Ortho004c4670(M16_00451a50* p0, M16_00451a50* p1) {
    mashed_re::Math::RwMatrixOrthoNormalizeLut(reinterpret_cast<float*>(p0), reinterpret_cast<const float*>(p1));
}
