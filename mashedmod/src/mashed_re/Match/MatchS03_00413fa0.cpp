// MATCH-T13-SEED-WRITER: port body of 0x00413fa0 = the byte-matched source re/match/s03/h2_00413fa0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// This RVA's only body in both targets. It was an asi-only naked transcription (Calc413fa0 in
// Util/PromoLoop_sessionB.cpp, `mov eax,abs; call eax` into the original's two getters); the exe needs a body
// because the matched 0x00414060 (Match/MatchS03_00414060.cpp) calls it. Callees bind to their matched bodies:
// FUN_00430790 = GetDat0067f17c (Match/MatchS07_00430790.cpp), FUN_0042f6a0 = GetRaceSubMode
// (Match/MatchS07_0042f6a0.cpp). Exported under its old name so the asi install keeps the same symbol.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_00413fa0 Calc413fa0
#define FUN_00430790 GetDat0067f17c
#define FUN_0042f6a0 GetRaceSubMode
#pragma comment(linker, "/EXPORT:Calc413fa0=_Calc413fa0")
extern "C" {
int GetDat0067f17c(void);
int GetRaceSubMode(void);
// 0x00413fa0  (49 B)
#include "../../../../re/match/s03/h2_00413fa0.c"
}
#undef FUN_0042f6a0
#undef FUN_00430790
#undef FUN_00413fa0
RH_ScopedInstall(Calc413fa0, 0x00413fa0);  // promote-loop install, moved with the body
