// MATCH-S13: port body of 0x0047b9e0 = the byte-matched source re/match/s13/0047b9e0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Track/CourseLoadDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
// asi only, like the old body: the callee (FUN_00496c10) has no port implementation, so the call
// goes into the original at its fixed address, as the old body did.
// The .asi's Frida/install entry stays Track_RenderQuadThunk_47b9e0 (Track/CourseLoadDispatch.cpp
// shadow A/B wrapper), which calls Track_RenderQuadThunk_47b9e0_impl.

// Call-through into the original's FUN_00496c10 (not a body of that function).
extern "C" void __cdecl MatchS13_0047b9e0_Call00496c10(int a, int b) {
    typedef void (__cdecl* Fn)(int, int);
    reinterpret_cast<Fn>(0x00496c10u)(a, b);
}

#define MATCH_PORT 1
#define FUN_0047b9e0 Track_RenderQuadThunk_47b9e0_impl
#define FUN_00496c10 MatchS13_0047b9e0_Call00496c10
#pragma comment(linker, "/EXPORT:Track_RenderQuadThunk_47b9e0_impl=_Track_RenderQuadThunk_47b9e0_impl")
extern "C" {
#include "../../../../re/match/s13/0047b9e0.c"
}
