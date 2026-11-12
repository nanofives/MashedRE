// RVA-LINT: ORIGTOP-ONLY
// PORT-TOP-08 (L-96 top-down lane, L-126): exe-only port body of rva 0x004332a0 = re/match/s08/004332a0.c, 97.3%,
// admitted by equiv.py (EQUIV [LAYOUT,VALUE-FLOW], ADMITTED.tsv row of EQUIV-ADMIT-1). Admitted, not C4. Called by
// 0x0043c000 (Match/PortTop_0043c000.cpp). The source also holds its two callees as local bodies: 0x0042ac50 (static,
// file-local) and 0x00430760, renamed here to Orig08_00430760 so it does not collide with the exe body
// IsMultiplayerMode (Frontend/MenuScoreGetters.cpp); EQUIV-CHECK section 7 found both ledger PASS.
#ifdef MASHED_STANDALONE
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004332a0 Orig_004332a0
#define FUN_00430760 Orig08_00430760     // local body of the admitted source
#define FUN_0040e470 CarSlotStateGet     // matched exe body, Match/MatchS02_0040e470.cpp
#define FUN_00496900 SlotActiveThunk     // matched exe body, Match/MatchGFFU_00497450.cpp
#define FUN_00496910 Global772ffcGet     // matched body, Match/MatchS16_00496910.cpp (exe since PORT-TOP-08)
extern "C" {
#include "../../../../re/match/s08/004332a0.c"
}
#endif // MASHED_STANDALONE
