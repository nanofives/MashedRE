// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00426340 (280 B): 100.0% exact (regs 100, ops 100). Raw converter 46.8%.
   Ghidra's unaff_retaddr is the /GS cookie (cookie ^ [ebp+4]), not a register argument: no static callee.
   Written by hand from the disassembly. What moved the score: two `char x[128] = ""` buffers, with the call
   FUN_0041e980(track) as the initializer of a local declared BETWEEN them (`char *name = FUN_0041e980(track);`),
   so the second buffer's zero-fill runs after the call (88.6% with the call as a statement after both
   declarations: frame 0x114 instead of 0x110 and the fills in the wrong order). RwGlobals @0x007d3ff8 as a struct
   (+0xcc / +0xd4 string-copy pointers). The 4-byte local passed to FUN_0047b9b0 (`int len`, frame offset 4) is
   typed int: its type is not visible in this function [UNCERTAIN]. FUN_00440bc0 is passed as a callback.
   Residual: none. */
/* ---- MATCH-S06 preamble (same text in every re/match/s06 source; not part of any function body) ----
   C under the DDK (match.py), C++ under VS2022 when the port includes this file (MATCH_PORT). */
#ifndef MATCH_PREAMBLE
#define MATCH_PREAMBLE
typedef unsigned char undefined; typedef unsigned char undefined1; typedef unsigned short undefined2;
typedef unsigned int undefined3; typedef unsigned int undefined4; typedef unsigned __int64 undefined8;
typedef unsigned int uint; typedef unsigned short ushort; typedef unsigned char uchar; typedef unsigned char byte;
typedef unsigned long ulong; typedef __int64 longlong; typedef unsigned __int64 ulonglong; typedef signed char sbyte;
typedef unsigned short word; typedef unsigned int dword; typedef long double float10;
#ifndef __cplusplus
typedef unsigned char bool;
#endif
#ifdef __cplusplus
/* port: a global is a reference to its original address (the exe maps the original data range) */
#define MGLOBAL(T, n, a) static T &n = *(T *)(a)
#define MGLOBALA(T, n, N, a) static T (&n)[N] = *(T (*)[N])(a)
#else
#define MGLOBAL(T, n, a) extern T n
#define MGLOBALA(T, n, N, a) extern T n[N]
#endif
#endif
/* ---- end preamble ---- */
typedef struct RwGlobals {
    char pad0[0xcc];
    char *(*rwstrcpy)(char *, const char *);  /* +0xcc */
    void *pad1;                               /* +0xd0 */
    char *(*rwstrcat)(char *, const char *);  /* +0xd4 */
} RwGlobals;
MGLOBAL(RwGlobals *, DAT_007d3ff8, 0x007d3ff8);
MGLOBALA(char, DAT_005cd280, 4, 0x005cd280);
void FUN_004073b0(void);
void FUN_004074a0(void);
char *FUN_0041e980(int p0);
int FUN_0042a470(int p0, const char *p1, int p2);
char *FUN_0042a8d0(char *p0);
void FUN_00458c70(void);
void FUN_00440bc0(void);
int FUN_0047b9b0(const char *p0, int *p1, void (*p2)(void));
void FUN_00495280(char *p0);
void FUN_004952f0(void);
// MATCH: 0x00426340 FUN_00426340
void FUN_00426340(int track)
{
    char path[128] = "";
    char *name = FUN_0041e980(track);
    char piz[128] = "";
    int len;

    DAT_007d3ff8->rwstrcpy(path, "toastart/tracks/");
    DAT_007d3ff8->rwstrcat(path, name);
    DAT_007d3ff8->rwstrcat(path, DAT_005cd280);
    DAT_007d3ff8->rwstrcpy(piz, FUN_0042a8d0(path));
    FUN_00458c70();
    FUN_004074a0();
    FUN_00495280(piz);
    if (FUN_0042a470(0, "KTCScript.lua", 1)) {
        FUN_0047b9b0("KTCScript.lua", &len, FUN_00440bc0);
        FUN_004073b0();
    }
    FUN_004952f0();
}
