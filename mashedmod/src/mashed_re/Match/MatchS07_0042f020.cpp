// MATCH-S07: port body of 0x0042f020 = the byte-matched source re/match/s07/0042f020.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original takes (row, id) in EAX/ECX (register args, see the .c header); here FUN_0042f020 is a plain cdecl
// body (MATCH_STATIC empty under C++; the stand-in caller is C-only). No port caller and no install: a hook at
// 0x0042f020 would need an EAX/ECX register adapter first.

#define MATCH_PORT 1
#pragma comment(linker, "/EXPORT:FUN_0042f020=_FUN_0042f020")
extern "C" {
#include "../../../../re/match/s07/0042f020.c"
}
