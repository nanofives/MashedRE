// MATCH-S07: port body of 0x00430b30 = the byte-matched source re/match/s07/00430b30.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// The original takes (b, a) in ECX/EAX (callers 0x004344f7 / 0x00435363); here FUN_00430b30 is a plain cdecl
// body (MATCH_STATIC empty under C++). No port caller and no install: a hook at 0x00430b30 would need an
// ECX/EAX register adapter first.

#define MATCH_PORT 1
#pragma comment(linker, "/EXPORT:FUN_00430b30=_FUN_00430b30")
extern "C" {
#include "../../../../re/match/s07/00430b30.c"
}
