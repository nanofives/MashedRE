// FLAGS: /O2 /GS /Ob1
/* MATCH-S14 0x0048bb70 (h7): 100% by bytes (match.py --bytes: 107 B whole symbol BYTE-IDENTICAL; instruction view 93.9% only because Ghidra's size stops before add esp,8 / ret). Raw 35.1%. What moved it: the 0x007220f0 slot clear sits in the SAME loop as the 0xa4-byte record clear (the compiler splits it into rep stosd plus a pointer loop, which leaves edx for the record pointer); separate loops gave eax and a 1-byte shorter cmp, shifting the loop alignment. Residual: none. */
/* ---- MATCH-S11 preamble (same text in every re/match/s11 source; not part of any function body) ----
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
typedef struct Rec007201d0 {
    float pos[3];       /* +0x00 */
    float a;            /* +0x0c */
    float b;            /* +0x10 */
    int count;          /* +0x14 */
    int countMax;       /* +0x18 */
    float accum;        /* +0x1c */
    float f20;          /* +0x20 */
    int pad24[0x20];
} Rec007201d0;
typedef struct Rec007206f0 {
    float v0[3];        /* +0x00 */
    int pad0c[6];
    float v24;          /* +0x24 */
    float v28;          /* +0x28 */
    int pad2c[2];
} Rec007206f0;
MGLOBAL(char, DAT_007200c0, 0x007200c0);
MGLOBALA(Rec007201d0, DAT_007201d0, 8, 0x007201d0);
MGLOBALA(Rec007206f0, DAT_007206f0, 0x80, 0x007206f0);
MGLOBALA(int, DAT_007220f0, 8, 0x007220f0);
void FUN_004b6480(void *p0, int p1);

// MATCH: 0x0048bb70 _FUN_0048bb70
void FUN_0048bb70(void)
{
    int i;
    for (i = 0; i < 8; i++) {
        DAT_007220f0[i] = 0;
        DAT_007201d0[i].pos[2] = 0;
        DAT_007201d0[i].pos[1] = 0;
        DAT_007201d0[i].pos[0] = 0;
        DAT_007201d0[i].count = 0;
        DAT_007201d0[i].countMax = 0;
        DAT_007201d0[i].f20 = 0;
    }
    for (i = 0; i < 0x80; i++) {
        DAT_007206f0[i].v28 = 0;
        DAT_007206f0[i].v24 = 0;
        DAT_007206f0[i].v0[2] = 0;
        DAT_007206f0[i].v0[1] = 0;
        DAT_007206f0[i].v0[0] = 0;
    }
    FUN_004b6480(&DAT_007200c0, 0x80);
}
