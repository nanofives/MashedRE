// MATCH-S06: port body of 0x00426e10 = the byte-matched source re/match/s06/00426e10.c
// (XP DDK cl 13.00.9176 / c2 13.00.9178, /O2 /GS /Ob1, as C; re/tools/match/match.py = 100%).
// 0x00426e10 had no port body: this is its first body (new body, top-down rule L-72). No install, no port caller.
// Callees with a port body bind to it (symmap.tsv for the slice, rva_body_scan outside it).
// Compiled here as C++ with MATCH_PORT; globals bind to their original addresses (preamble MGLOBAL).

#define MATCH_PORT 1
#define FUN_004053d0 TrackDataSlotSet
#define FUN_00409680 LedArrayInit
#define FUN_00409710 MatchS06_00426e10_Call00409710
#define FUN_00412050 MatchS06_00426e10_Call00412050
#define FUN_0041e870 TrackNodeRecordScan
#define FUN_0041e8b0 TrackNodeDispatch14
#define FUN_0041e970 TrackNodeDispatch44
#define FUN_0041e980 TrackNodeRecordFind
#define FUN_0041e9d0 TrackNodeFnPtrGet14
#define FUN_0041ea90 TrackNodeFnPtrGet44
#define FUN_004235b0 AiPizLoad
#define FUN_00423630 AiDataBufInit
#define FUN_00425ab0 EntryHeaderClear
#define FUN_00426cd0 TrackSlotArrayReset
#define FUN_0042f510 Vehicle0HandleGet
#define FUN_00462950 MatchS06_00426e10_Call00462950
#define FUN_004671a0 MatchS06_00426e10_Call004671a0
#define FUN_004715a0 MatchS06_00426e10_Call004715a0
#define FUN_00478660 MatchS06_00426e10_Call00478660
#define FUN_00479330 MatchS06_00426e10_Call00479330
#define FUN_0047a020 MatchS06_00426e10_Call0047a020
#define FUN_0047a0f0 MatchS06_00426e10_Call0047a0f0
#define FUN_0047c0b0 ClearColl47c0b0
#define FUN_0047c0f0 MatchS06_00426e10_Call0047c0f0
#define FUN_00480340 MatchS06_00426e10_Call00480340
#define FUN_00491780 MatchS06_00426e10_Call00491780
#define FUN_004924c0 Set6147b4Triple
#define FUN_00495280 MatchS06_00426e10_Call00495280
#define FUN_004952f0 MatchS06_00426e10_Call004952f0
#define FUN_004987b0 MatchS06_00426e10_Call004987b0
#define FUN_004c1b10 MatchS06_00426e10_Call004c1b10
#pragma comment(linker, "/EXPORT:FUN_00426e10=_FUN_00426e10")
extern "C" {
#include "../../../../re/match/s06/00426e10.c"
}

// Callees with no port body: each forwarder makes the call at the original address (S04 rule).
// 0x004987b0: tail-jump forwarder (push/ret), so the stack the caller built (varargs / callback args) passes through as is.
extern "C" {
int MatchS06_00426e10_Call00409710(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00409710u)();
}
void MatchS06_00426e10_Call00412050(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00412050u)();
}
void MatchS06_00426e10_Call00462950(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x00462950u)(p0);
}
int MatchS06_00426e10_Call004671a0(int p0) {
    return reinterpret_cast<int (__cdecl*)(int)>(0x004671a0u)(p0);
}
void MatchS06_00426e10_Call004715a0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004715a0u)();
}
void MatchS06_00426e10_Call00478660(char *p0, int p1) {
    reinterpret_cast<void (__cdecl*)(char *, int)>(0x00478660u)(p0, p1);
}
int MatchS06_00426e10_Call00479330(char *p0, char *p1, char *p2) {
    return reinterpret_cast<int (__cdecl*)(char *, char *, char *)>(0x00479330u)(p0, p1, p2);
}
int MatchS06_00426e10_Call0047a020(char *p0, char *p1, char *p2) {
    return reinterpret_cast<int (__cdecl*)(char *, char *, char *)>(0x0047a020u)(p0, p1, p2);
}
int MatchS06_00426e10_Call0047a0f0(char *p0, char *p1) {
    return reinterpret_cast<int (__cdecl*)(char *, char *)>(0x0047a0f0u)(p0, p1);
}
void MatchS06_00426e10_Call0047c0f0(int p0) {
    reinterpret_cast<void (__cdecl*)(int)>(0x0047c0f0u)(p0);
}
void MatchS06_00426e10_Call00480340(void) {
    reinterpret_cast<void (__cdecl*)()>(0x00480340u)();
}
int MatchS06_00426e10_Call00491780(void) {
    return reinterpret_cast<int (__cdecl*)()>(0x00491780u)();
}
void MatchS06_00426e10_Call00495280(char *p0) {
    reinterpret_cast<void (__cdecl*)(char *)>(0x00495280u)(p0);
}
void MatchS06_00426e10_Call004952f0(void) {
    reinterpret_cast<void (__cdecl*)()>(0x004952f0u)();
}
__declspec(naked) void MatchS06_00426e10_Call004987b0(const char *fmt, ...) {
    __asm { push 0x004987b0 }
    __asm { ret }
}
int MatchS06_00426e10_Call004c1b10(int p0, float p1) {
    return reinterpret_cast<int (__cdecl*)(int, float)>(0x004c1b10u)(p0, p1);
}
}
