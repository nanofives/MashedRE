// MATCH-S06: port body of 0x00428320 = the byte-matched source re/match/s06/00428320.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/TextMeasure.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_00428320 TextWidthMeasureB
#define FUN_00427840 FontText_UTF16WidenCopyBody
// FUN_005554d0: no port body (HUD/HudBatch.cpp FontText_StringWidthAccumulator is under #if 0), so a raw-address
// forwarder in both targets (S04 rule): in the .asi it calls the original; the exe has no caller of this function.
extern "C" float MatchS06_00428320_Call005554d0(int font, short *text, float size) {
    return reinterpret_cast<float (__cdecl*)(int, short *, float)>(0x005554d0u)(font, text, size);
}
#define FUN_005554d0 MatchS06_00428320_Call005554d0
#pragma comment(linker, "/EXPORT:TextWidthMeasureB=_TextWidthMeasureB")
extern "C" {
#include "../../../../re/match/s06/00428320.c"
}
