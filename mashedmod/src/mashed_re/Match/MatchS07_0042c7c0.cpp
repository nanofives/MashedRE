// MATCH-S07: port body of 0x0042c7c0 = the byte-matched source re/match/s07/0042c7c0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU.
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// New body (no earlier port body, no install). Callee FUN_00427780 binds to the port's body
// FontText_StringTableLookup (HUD/HudBatch.cpp, .asi-only) through an extern "C" forwarder (the port symbol returns
// const std::uint8_t*, the matched source declares short*).

#define MATCH_PORT 1
#define FUN_00427780 MatchS07_0042c7c0_Fwd00427780
extern "C" const unsigned char* __cdecl FontText_StringTableLookup(int index);
extern "C" short* MatchS07_0042c7c0_Fwd00427780(int p0) {
    return reinterpret_cast<short*>(const_cast<unsigned char*>(FontText_StringTableLookup(p0)));
}
#pragma comment(linker, "/EXPORT:FUN_0042c7c0=_FUN_0042c7c0")
extern "C" {
#include "../../../../re/match/s07/0042c7c0.c"
}
