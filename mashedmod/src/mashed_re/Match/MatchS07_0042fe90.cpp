// MATCH-S07: port body of 0x0042fe90 = the byte-matched source re/match/s07/0042fe90.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/MenuSpriteDispatch.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_0042fe90 MenuSpriteDispatchC
#define FUN_0042ac00 MatchS07_0042fe90_Call0042ac00
#define FUN_0042ac50 Calc42ac50
#define FUN_0042b8b0 ScreenWidthGet
#define FUN_0042b8c0 ScreenHeightGet
#define FUN_0040bb50 MatchS07_0042fe90_Call0040bb50
#define FUN_00427e00 MatchS07_0042fe90_Call00427e00
#define FUN_004282a0 MatchS07_0042fe90_Call004282a0
#define FUN_00473870 MatchS07_0042fe90_Call00473870
#define FUN_004739f0 MatchS07_0042fe90_Call004739f0
// Callees of the matched source. FUN_0042ac00 / 0042ac50 / 0042b8b0 / 0042b8c0 are HELD copies in the .c; each binds
// to its port body: MenuGroupCount (Frontend/MenuGetters.cpp, __fastcall with an unused ECX, so a forwarder),
// Calc42ac50 (Util/PromoLoop_sessionB.cpp, asi-only: this wrapper has no exe target), ScreenWidthGet / ScreenHeightGet
// (Match/MatchS07_0042b8b0.cpp / 0042b8c0.cpp). The old Frontend/MenuSpriteDispatch.cpp body called the other five
// through their original addresses (__cdecl function-pointer casts), so each forwarder below makes that same call.
extern "C" int __fastcall MenuGroupCount(int, int*);
extern "C" int MatchS07_0042fe90_Call0042ac00(int *p) {
    return MenuGroupCount(0, p);
}
#pragma comment(linker, "/EXPORT:MenuSpriteDispatchC=_MenuSpriteDispatchC")
extern "C" {
#include "../../../../re/match/s07/0042fe90.c"
}
// Raw-address forwarders (after the include: they take the source's RwV2d / RwRGBA by value, as the original does).
extern "C" int MatchS07_0042fe90_Call0040bb50(const char *name) {
    return reinterpret_cast<int (__cdecl*)(const char *)>(0x0040bb50u)(name);
}
extern "C" void MatchS07_0042fe90_Call00427e00(int text, float x, float y, RwRGBA col, float scale, int flags) {
    reinterpret_cast<void (__cdecl*)(int, float, float, RwRGBA, float, int)>(0x00427e00u)(text, x, y, col, scale, flags);
}
extern "C" float MatchS07_0042fe90_Call004282a0(int text, float scale) {
    return reinterpret_cast<float (__cdecl*)(int, float)>(0x004282a0u)(text, scale);
}
extern "C" void MatchS07_0042fe90_Call00473870(int tex, RwV2d pos, RwV2d size, RwRGBA col, int flags) {
    reinterpret_cast<void (__cdecl*)(int, RwV2d, RwV2d, RwRGBA, int)>(0x00473870u)(tex, pos, size, col, flags);
}
extern "C" void MatchS07_0042fe90_Call004739f0(int tex, RwV2d pos, RwV2d size, RwRGBA col, float a6, int a7, int a8, float a9, int a10,
                                    int a11) {
    reinterpret_cast<void (__cdecl*)(int, RwV2d, RwV2d, RwRGBA, float, int, int, float, int, int)>(0x004739f0u)(
        tex, pos, size, col, a6, a7, a8, a9, a10, a11);
}
RH_ScopedInstall(MenuSpriteDispatchC, 0x0042fe90);
