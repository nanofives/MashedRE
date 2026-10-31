// MATCH-S02 / MATCH-L68-SWAP: port body of 0x004114c0 = the byte-matched source re/match/s02/004114c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Boot/BootLowRvaCluster.cpp, whose body added one dereference against the original).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// No install: the old RH_ScopedInstall was MASS-DISABLED 2026-05-24 (needs-canonical-vtable-state) and stays off.
// No exe callers: match evidence identical + swapped, C4 only when a caller lands (L-68 ruling, 2026-10-31).

#define MATCH_PORT 1
#define FUN_004114c0 VtableTeardown_114c0
#pragma comment(linker, "/EXPORT:VtableTeardown_114c0=_VtableTeardown_114c0")
extern "C" {
#include "../../../../re/match/s02/004114c0.c"
}
