// MATCH-S15: port body of 0x004921d0 = the byte-matched source re/match/s15/004921d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// The same source text is this RVA's only body in the targets that compile this TU (moved out of Boot/SubsystemInit.cpp).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).
#include "../Core/HookSystem.h"

#define MATCH_PORT 1
#define FUN_004921d0 DisplayInit
// [PORT-TOP-04B] exe: the callees bind by name (MASHED_ORIG_TOP path, SubsystemInit 0x00492270). RW core calls bind to
// LibRw/RwApiBind.cpp; rva 0x00498bc0 / 0x00498bd0 bind to the exe resolution (PortTop_Width / PortTop_Height, as in
// PortTop_00499ba0.cpp: VideoGetRenderWidth reads DAT_00616028, zero in the exe). The .asi keeps the raw-address calls.
#ifdef MASHED_STANDALONE
#define FUN_004c2f00 MashedRw_EngineGetCurrentMode_004c2f00          // RW: RwEngineGetCurrentMode (TOP-03)
#define FUN_004c2ed0 MashedRw_EngineGetModeInfo_004c2ed0             // RW: RwEngineGetModeInfo (TOP-03)
#define FUN_00498bc0 PortTop_Width                                   // stand-in: exe render width
#define FUN_00498bd0 PortTop_Height                                  // stand-in: exe render height
#define FUN_00467110 Orig_00467110                                   // matched, Match/PortTop_00467110.cpp
#define FUN_0042b890 RenderWidthSet                                  // matched, Match/MatchS07_0042b890.cpp
#define FUN_0042b8a0 RenderHeightSet                                 // matched, Match/MatchS07_0042b8a0.cpp
#define FUN_0042d560 Orig_0042d560                                   // matched, Match/PortTop_0042d560.cpp
#define FUN_0042f660 DefaultViewportCameraInit                       // matched, Match/PortTop_0042f660.cpp
#define FUN_004997b0 Win32ResourceLoader                             // matched, Match/MatchS16_004997b0.cpp
#define FUN_004b3eb0 PortTop_Port_004b3eb0                           // port (decomp, no matched source), LibRw/RwApiBind.cpp
#define FUN_004c5c00 MashedRw_TexDictionaryFindNamedTexture_004c5c00 // RW: RwTexDictionaryFindNamedTexture (TOP-03)
#else
#define FUN_004c2f00 MatchS15_004921d0_Call004c2f00
#define FUN_004c2ed0 MatchS15_004921d0_Call004c2ed0
#define FUN_00498bc0 MatchS15_004921d0_Call00498bc0
#define FUN_00498bd0 MatchS15_004921d0_Call00498bd0
#define FUN_00467110 MatchS15_004921d0_Call00467110
#define FUN_0042b890 MatchS15_004921d0_Call0042b890
#define FUN_0042b8a0 MatchS15_004921d0_Call0042b8a0
#define FUN_0042d560 MatchS15_004921d0_Call0042d560
#define FUN_0042f660 MatchS15_004921d0_Call0042f660
#define FUN_004997b0 MatchS15_004921d0_Call004997b0
#define FUN_004b3eb0 MatchS15_004921d0_Call004b3eb0
#define FUN_004c5c00 MatchS15_004921d0_Call004c5c00
// Callees of the matched source: the old Boot/SubsystemInit.cpp body called each one through its original address, so these
// forwarders make that same raw-address call (in the .asi an installed callee hook is reached through its JMP).
extern "C" short MatchS15_004921d0_Call004c2f00(void) {
    return reinterpret_cast<short (__cdecl*)()>(0x004c2f00u)();
}
extern "C" int MatchS15_004921d0_Call004c2ed0(void* p0, int p1) {
    return reinterpret_cast<int (__cdecl*)(void* p0, int p1)>(0x004c2ed0u)(p0, p1);
}
extern "C" int MatchS15_004921d0_Call00498bc0(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00498bc0u)();
}
extern "C" int MatchS15_004921d0_Call00498bd0(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00498bd0u)();
}
extern "C" void MatchS15_004921d0_Call00467110(int p0, int p1) {
    reinterpret_cast<void (__cdecl*)(int p0, int p1)>(0x00467110u)(p0, p1);
}
extern "C" void MatchS15_004921d0_Call0042b890(int p0) {
    reinterpret_cast<void (__cdecl*)(int p0)>(0x0042b890u)(p0);
}
extern "C" void MatchS15_004921d0_Call0042b8a0(int p0) {
    reinterpret_cast<void (__cdecl*)(int p0)>(0x0042b8a0u)(p0);
}
extern "C" int MatchS15_004921d0_Call0042d560(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0042d560u)();
}
extern "C" void MatchS15_004921d0_Call0042f660(void) {
    reinterpret_cast<void (__cdecl*)()>(0x0042f660u)();
}
extern "C" int MatchS15_004921d0_Call004997b0(int p0, const char* p1, int* p2, int* p3) {
    return reinterpret_cast<int (__cdecl*)(int p0, const char* p1, int* p2, int* p3)>(0x004997b0u)(p0, p1, p2, p3);
}
extern "C" int MatchS15_004921d0_Call004b3eb0(int p0, int p1, int p2, int p3) {
    return reinterpret_cast<int (__cdecl*)(int p0, int p1, int p2, int p3)>(0x004b3eb0u)(p0, p1, p2, p3);
}
extern "C" int MatchS15_004921d0_Call004c5c00(int p0, const char* p1) {
    return reinterpret_cast<int (__cdecl*)(int p0, const char* p1)>(0x004c5c00u)(p0, p1);
}
#endif
#pragma comment(linker, "/EXPORT:DisplayInit=_DisplayInit")
extern "C" {
#include "../../../../re/match/s15/004921d0.c"
}
RH_ScopedInstall(DisplayInit, 0x004921d0);
