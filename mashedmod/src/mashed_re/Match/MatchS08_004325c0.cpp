// PORT-STATE-03 / L-100 (Mariano 2026-11-08, answer (a)): port body of 0x004325c0 = the byte-matched source
// re/match/s08/004325c0.c (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100.0%,
// 152/152 ins, 564 B whole symbol BYTE-IDENTICAL; re/tools/match/relocheck.py OK checked=24, unchecked __ftol).
// The same source text is this RVA's only body in both targets. The hand body that stood in Frontend/MenuAnimTickTwin.cpp
// is deleted together with its L-32 rounding opt-out knob: the matched body truncates through (int) as the original's
// _ftol (0x004a2c48) does.
//
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL): the records at
// DAT_00898ac0, dt DAT_007f1004, phase DAT_0067eca4, gate DAT_0067e914, item count DAT_0067ece0. The callee 0x0042ac50 is
// called by name (Calc42ac50, Match/MatchS06_0042ac50.cpp, the source's MATCH_PORT branch). The .asi installs this body at
// 0x004325c0 on the original's live globals. The exe calls it from Frontend/MenuNavSM.cpp Anim_Tick through
// Frontend/PortStateFrontend.h AnimTickScope, which puts the frontend's inputs (and, with MASHED_PORTSTATE_FRONTEND OFF,
// its records) at those addresses for the one call.
// Export name kept as MenuAnimTickTwin (re/frida/menu_anim_diff.py finds the .asi export by that name).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004325c0 MenuAnimTickTwin
#pragma comment(linker, "/EXPORT:MenuAnimTickTwin=_MenuAnimTickTwin")
extern "C" {
#include "../../../../re/match/s08/004325c0.c"
}
RH_ScopedInstall(MenuAnimTickTwin, 0x004325c0);
