// MATCH-S16: port body of 0x00494a80 = the byte-matched source re/match/s16/00494a80.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Frontend/IntroSplash.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).


// [PORT-OT-BIND-D3D] exe: compiled in mashed_re.exe too (one body in both targets). On the MASHED_ORIG_TOP path its
// callees bind by name: 0x004944c0 to its DirectShow binding on the exe's own graph / device
// (Match/PortTop_MovieBind.cpp), RwRasterCreate 0x004c77c0 to LibRw/RwApiBind.cpp. Its knob-path callers (0x00495350,
// the thunk 0x00494f00 from 0x0043dfd0) run as stand-ins today, so only MASHED_DBG_MOVIEBIND_SELFTEST reaches it.
// The .asi keeps the raw-address forwarders.
#define MATCH_PORT 1
#define FUN_00494a80 FUN_00494a80
#ifdef MASHED_STANDALONE
#define FUN_004c77c0 MashedRw_RasterCreate_004c77c0      // RW: RwRasterCreate -> rw::Raster::create
#define FUN_004944c0 PortTop_Bound_004944c0_Ptr           // DirectShow: Match/PortTop_MovieBind.cpp
extern "C" int PortTop_Bound_004944c0(int idx);
// 00494a80.c declares the callee `char *FUN_004944c0(int)`. The binding returns the original's int HRESULT,
// and 0x00494a80 discards it (re/match/s16/00494a80.c:36).
extern "C" char *PortTop_Bound_004944c0_Ptr(int idx) {
    return reinterpret_cast<char *>(static_cast<unsigned int>(PortTop_Bound_004944c0(idx)));
}
#else
#define FUN_004c77c0 MatchS16_00494a80_Call004c77c0
#define FUN_004944c0 MatchS16_00494a80_Call004944c0
#endif
#pragma comment(linker, "/EXPORT:FUN_00494a80=_FUN_00494a80")
extern "C" {
#include "../../../../re/match/s16/00494a80.c"
}
#ifndef MASHED_STANDALONE
// Callees with no port body: raw-address forwarders, the calls the original makes (the old Frontend/IntroSplash.cpp
// OrigVideoLoad called 0x00494a80 itself through its original address).
extern "C" {
int MatchS16_00494a80_Call004c77c0(undefined4 p0, undefined4 p1, undefined4 p2, undefined4 p3) {
    return reinterpret_cast<int (__cdecl*)(undefined4, undefined4, undefined4, undefined4)>(0x004c77c0u)(p0, p1, p2, p3);
}
char *MatchS16_00494a80_Call004944c0(int p0) {
    return reinterpret_cast<char *(__cdecl*)(int)>(0x004944c0u)(p0);
}
}
#endif  // !MASHED_STANDALONE
