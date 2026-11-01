// MATCH-S06: port body of 0x00426340 = the byte-matched source re/match/s06/00426340.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// 0x00426340 had no port body: this is its first body (new body, top-down rule L-72). No install, no port caller.
// 0x0041e980 binds to Match/MatchS04_0041e980.cpp; 0x0042a470 / 0x0042a8d0 to the batch-2 new bodies (symmap.tsv).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_004073b0 MatchS06_00426340_Call004073b0
#define FUN_004074a0 MatchS06_00426340_Call004074a0
#define FUN_0041e980 TrackNodeRecordFind
#define FUN_00440bc0 MatchS06_00426340_Call00440bc0
#define FUN_00458c70 MatchS06_00426340_Call00458c70
#define FUN_0047b9b0 MatchS06_00426340_Call0047b9b0
#define FUN_00495280 MatchS06_00426340_Call00495280
#define FUN_004952f0 MatchS06_00426340_Call004952f0
#pragma comment(linker, "/EXPORT:FUN_00426340=_FUN_00426340")
extern "C" {
#include "../../../../re/match/s06/00426340.c"
}

// Callees with no port body: each forwarder makes the call at the original address (S04 rule).
// 0x00440bc0: tail-jump forwarder (push/ret), so the stack the caller built (varargs / callback args) passes through as is.
extern "C" {
void MatchS06_00426340_Call004073b0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004073b0u)();
}
void MatchS06_00426340_Call004074a0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004074a0u)();
}
__declspec(naked) void MatchS06_00426340_Call00440bc0(void) {
    __asm { push 0x00440bc0 }
    __asm { ret }
}
void MatchS06_00426340_Call00458c70(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00458c70u)();
}
int MatchS06_00426340_Call0047b9b0(const char *p0, int *p1, void (*p2)(void)) {
    return reinterpret_cast<int (__cdecl*)(const char *, int *, void (*)(void))>(0x0047b9b0u)(p0, p1, p2);
}
void MatchS06_00426340_Call00495280(char *p0) {
    reinterpret_cast<void (__cdecl*)(char *)>(0x00495280u)(p0);
}
void MatchS06_00426340_Call004952f0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004952f0u)();
}
}
