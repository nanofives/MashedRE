// MATCH-S07: port body of 0x0042bf30 = the byte-matched source re/match/s07/0042bf30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Util/PromoLoop_round72.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

// TRACKER NAME: MenuModalPost (hooks.csv). The exported symbol KEEPS the old Post0042bf30 name on purpose, as does the
// registry key 'post_0042bf30', so the 2026-06-13 promotion evidence (log/diff_post_0042bf30.csv) and hooks_registry.py
// keep resolving. It is the modal POSTER (fills 0x0067eab4..0x0067ead0, raises DAT_0067eab0); the renderer is
// MenuModalRender in Frontend/MenuModal.cpp. See re/analysis/race_hud_capture_20260902.md Findings 29, 32 and 37.

#define MATCH_PORT 1
#define FUN_0042bf30 Post0042bf30
#pragma comment(linker, "/EXPORT:Post0042bf30=_Post0042bf30")
extern "C" {
#include "../../../../re/match/s07/0042bf30.c"
}
RH_ScopedInstall(Post0042bf30, 0x0042bf30);
