// MATCH-S06: port body of 0x004264d0 = the byte-matched source re/match/s06/004264d0.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// It also holds 0x00426460 (FUN_00426460, canonical here: no own file; MATCH_STATIC so other TUs can call it).
// Neither RVA had a port body: these are their first bodies (new bodies, top-down rule L-72). No install, no port caller.
// 0x0041e980 binds to Match/MatchS04_0041e980.cpp, 0x0042fe30 to Frontend/MenuRaceEnd.cpp; 0x0042a470 / 0x0042a5d0 /
// 0x0042a8d0 to the batch-2 new bodies (symmap.tsv).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_0041e980 TrackNodeRecordFind
#define FUN_0042fe30 RaceEndFlagIfEndMode
#define FUN_00458c70 MatchS06_004264d0_Call00458c70
#define FUN_00458fd0 MatchS06_004264d0_Call00458fd0
#define FUN_0047a130 MatchS06_004264d0_Call0047a130
#define FUN_00495280 MatchS06_004264d0_Call00495280
#define FUN_004952f0 MatchS06_004264d0_Call004952f0
#define FUN_004b3fc0 MatchS06_004264d0_Call004b3fc0
#define FUN_004b5190 MatchS06_004264d0_Call004b5190
#define FUN_004e6e00 MatchS06_004264d0_Call004e6e00
#pragma comment(linker, "/EXPORT:FUN_004264d0=_FUN_004264d0")
extern "C" {
#include "../../../../re/match/s06/004264d0.c"
}

// Callees with no port body: each forwarder makes the call at the original address (S04 rule).
extern "C" {
void MatchS06_004264d0_Call00458c70(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00458c70u)();
}
void MatchS06_004264d0_Call00458fd0(int p0, uint p1, float p2) {
    reinterpret_cast<void (__cdecl*)(int, uint, float)>(0x00458fd0u)(p0, p1, p2);
}
void MatchS06_004264d0_Call0047a130(char *p0) {
    reinterpret_cast<void (__cdecl*)(char *)>(0x0047a130u)(p0);
}
void MatchS06_004264d0_Call00495280(char *p0) {
    reinterpret_cast<void (__cdecl*)(char *)>(0x00495280u)(p0);
}
void MatchS06_004264d0_Call004952f0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004952f0u)();
}
int MatchS06_004264d0_Call004b3fc0(int p0, int *p1) {
    return reinterpret_cast<int (__cdecl*)(int, int *)>(0x004b3fc0u)(p0, p1);
}
uint MatchS06_004264d0_Call004b5190(int p0, int p1, int p2) {
    return reinterpret_cast<uint (__cdecl*)(int, int, int)>(0x004b5190u)(p0, p1, p2);
}
int MatchS06_004264d0_Call004e6e00(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004e6e00u)(p0);
}
}
