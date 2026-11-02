// MATCH-S06: port body of 0x004270f0 = the byte-matched source re/match/s06/004270f0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// 0x004270f0 had no port body: this is its first body (new body, top-down rule L-72). No install, no port caller.
// Callees with a port body bind to it (symmap.tsv for the slice, rva_body_scan / active installs outside it).
// 0x00450b10 binds to HudIm2DQuad (Frontend/DrawQuadPrimitives.cpp): the matched call passes (int, V2, V2, RGBA4, float*)
// by value, the same 7 cdecl stack slots as HudIm2DQuad(int, float, float, float, float, uint32, uint32*).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_0041e8d0 MatchS06_004270f0_Call0041e8d0
#define FUN_0041e8e0 MatchS06_004270f0_Call0041e8e0
#define FUN_0041e9b0 TrackNodeFieldCmp10
#define FUN_0041e9f0 MatchS06_004270f0_Call0041e9f0
#define FUN_0041ea00 MatchS06_004270f0_Call0041ea00
#define FUN_00426d90 HandleArrayRelease
#define FUN_004292c0 UtilWritePtr
#define FUN_00450b10 HudIm2DQuad
#define FUN_004671a0 MatchS06_004270f0_Call004671a0
#define FUN_00467210 FollowTargetField10
#define FUN_00472ad0 MatchS06_004270f0_Call00472ad0
#define FUN_00478220 MatchS06_004270f0_Call00478220
#define FUN_00478cd0 MatchS06_004270f0_Call00478cd0
#define FUN_004914b0 MatchS06_004270f0_Call004914b0
#define FUN_004924e0 Ret6147b4
#define FUN_004b5af0 MatchS06_004270f0_Call004b5af0
#define FUN_004e5660 MatchS06_004270f0_Call004e5660
#define FUN_0055dec0 UtilDeref55dec0
#pragma comment(linker, "/EXPORT:FUN_004270f0=_FUN_004270f0")
extern "C" {
#include "../../../../re/match/s06/004270f0.c"
}

// Callees with no port body: each forwarder makes the call at the original address (S04 rule).
extern "C" {
void MatchS06_004270f0_Call0041e8d0(void *p0) {
    reinterpret_cast<void (__cdecl*)(void *)>(0x0041e8d0u)(p0);
}
void MatchS06_004270f0_Call0041e8e0(void *p0) {
    reinterpret_cast<void (__cdecl*)(void *)>(0x0041e8e0u)(p0);
}
int MatchS06_004270f0_Call0041e9f0(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0041e9f0u)();
}
int MatchS06_004270f0_Call0041ea00(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x0041ea00u)();
}
int MatchS06_004270f0_Call004671a0(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004671a0u)(p0);
}
void MatchS06_004270f0_Call00472ad0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00472ad0u)(p0);
}
void MatchS06_004270f0_Call00478220(void *p0) {
    reinterpret_cast<void (__cdecl*)(void *)>(0x00478220u)(p0);
}
void MatchS06_004270f0_Call00478cd0(void *p0, int p1) {
    reinterpret_cast<void (__cdecl*)(void *, int)>(0x00478cd0u)(p0, p1);
}
void MatchS06_004270f0_Call004914b0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004914b0u)();
}
void MatchS06_004270f0_Call004b5af0(void *p0, int p1, float p2) {
    reinterpret_cast<void (__cdecl*)(void *, int, float)>(0x004b5af0u)(p0, p1, p2);
}
int MatchS06_004270f0_Call004e5660(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004e5660u)(p0);
}
}
