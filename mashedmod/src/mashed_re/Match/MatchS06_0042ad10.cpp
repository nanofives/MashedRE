// PORT-STATE-03 (2026-11-08): port body of 0x0042ad10 = the byte-matched source re/match/s06/0042ad10.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100.0%, 32/32 ins, 118 B whole symbol
// BYTE-IDENTICAL; re/tools/match/relocheck.py OK checked=9). The same source text is this RVA's only body.
//
// Exe only. FUN_0042ad10 is `static` in the source (the DDK passes idx in EAX and a in EDX), so there is no cdecl entry an
// .asi install at 0x0042ad10 could use, and the .asi has no body for this RVA. Compiled here as C++ with MATCH_PORT;
// globals bind to their original addresses (preamble MGLOBAL): it writes the menu records at DAT_00898ac0, so the exe calls
// it only with MASHED_PORTSTATE_FRONTEND ON, where those are the frontend's records (Frontend/MenuNavSM.cpp
// PromptStripAppend; knob OFF keeps the hand PromptRowInit on the port store). The source's `caller_0042ad10` is the
// match harness's stand-in caller (not matched, not called by the port).
#define MATCH_PORT 1
extern "C" {
#include "../../../../re/match/s06/0042ad10.c"

// Not a body: the cdecl name the exe calls, forwarding to the static FUN_0042ad10 above.
void __cdecl PromptRowInit0042ad10(int* idx, int a, int x, int y, int e) { FUN_0042ad10(idx, a, x, y, e); }
}
