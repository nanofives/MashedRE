// MATCH-S07: port body of 0x0042e3a0 = the byte-matched source re/match/s07/0042e3a0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of
// Frontend/MenuSpriteDispatch.cpp by MATCH-S07-FU, 2026-11-01).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
//
// What the swap changes: the old body drew the second amber tick line at y = 65.5; the original draws it at y = 67.0
// (the float 0x42860000 at 0x0042e52f; no 65.5 occurs in 0x0042e3a0..0x0042e586). Everything else is the same draw
// list. The exe's only call (exe_main.cpp, the B16 chrome path) sits under `if (false && ...)`, so no exe caller
// reaches this body; the .asi installs it as before.
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042e3a0 MenuChromeShellA
#define FUN_0042d5a0 MatchS07_0042e3a0_Call0042d5a0
#define FUN_00472c60 MatchS07_0042e3a0_Call00472c60
#define FUN_00472f40 MatchS07_0042e3a0_Call00472f40
#define FUN_004730b0 MatchS07_0042e3a0_Call004730b0
#pragma comment(linker, "/EXPORT:MenuChromeShellA=_MenuChromeShellA")
extern "C" {
#include "../../../../re/match/s07/0042e3a0.c"
}
// Callees of the matched source: the old Frontend/MenuSpriteDispatch.cpp body called all four through their original
// addresses (__cdecl function-pointer casts s_CreditsSpriteLine, s_ChromeBaseDraw, s_TextGradientV0V1,
// s_TextGradientV2V3), so each forwarder makes that same call. They follow the include because they take the
// source's RwV2d / RwRGBA by value, as the original does (same stack layout as the old five-dword calls).
extern "C" {
void MatchS07_0042e3a0_Call0042d5a0(int t) {
    reinterpret_cast<void (__cdecl*)(int)>(0x0042d5a0u)(t);
}
void MatchS07_0042e3a0_Call00472c60(RwV2d pos, RwV2d size, RwRGBA col) {
    reinterpret_cast<void (__cdecl*)(RwV2d, RwV2d, RwRGBA)>(0x00472c60u)(pos, size, col);
}
void MatchS07_0042e3a0_Call00472f40(RwV2d pos, RwV2d size, RwRGBA col) {
    reinterpret_cast<void (__cdecl*)(RwV2d, RwV2d, RwRGBA)>(0x00472f40u)(pos, size, col);
}
void MatchS07_0042e3a0_Call004730b0(RwV2d pos, RwV2d size, RwRGBA col) {
    reinterpret_cast<void (__cdecl*)(RwV2d, RwV2d, RwRGBA)>(0x004730b0u)(pos, size, col);
}
}
RH_ScopedInstall(MenuChromeShellA, 0x0042e3a0);
