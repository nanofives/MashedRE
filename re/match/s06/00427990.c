// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h5 0x00427990 (306 B). Best 100.0% (regs 100.0%). Raw converter 40.0%.
   This file HOLDS the static callee FUN_004277a0 (0x004277a0, EAX = src, EBX = dst), defined first.
   pos is float[4] {x, y, w, h} at the frame bottom, text short[256] above it.
   What moved it: this compiler reassociates float products freely (no /Op), so operand order needs the
   parenthesized forms: `DAT_0067d830 * (w * 0.0015625f)` and `DAT_0067d834 * (h * 0.0020833334f)` (plain
   `w * c * g` gives constant-first loads, 93.3%); pos[1] = (1.0f - y * c) - pos[3] (1 - (y*c + pos[3]) gives
   fadd, 98.7%). Constants (literals, checked by relocheck byte compare): 0.0015625f @0x005cd5a8, 0.0020833334f @0x005cc560, 1.0f @0x005cc320, 0.0708f @0x005cd5fc. Residual: none. */
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
MGLOBAL(int, DAT_0067d838, 0x0067d838);
MGLOBAL(int, DAT_0067d83c, 0x0067d83c);
MGLOBAL(float, DAT_008991c0, 0x008991c0);
MGLOBAL(float, DAT_0067d830, 0x0067d830);
MGLOBAL(float, DAT_0067d834, 0x0067d834);

unsigned short *FUN_00427780(int id);
int FUN_00552d10(void);
void FUN_00552d70(void);
float *FUN_00556e90(int a, void *b, void *c, void *d, void *e);
int FUN_005555b0(int a, short *text, float size, float *pos, int align, int f);

#ifndef __cplusplus
// MATCH-HELD: 0x004277a0 (static copy for the DDK register convention; port body: re/match/s06/004277a0.c)
static void FUN_004277a0(unsigned short *src, short *dst)
{
    short n = *src++;
    short i;
    for (i = 0; i < n; i++) {
        short c = src[i];
        if (c == 8) c = 0x81;
        else if (c == 9) c = 0x7f;
        else if (c == 10) c = 0x81;
        else if (c == 11) c = 0x8d;
        else if (c == 12) c = 0x80;
        else if (c == 13) c = 0x87;
        else if (c == 14) c = 0x8f;
        dst[i] = c;
    }
    dst[n] = 0;
}
#else
void FUN_004277a0(unsigned short *src, short *dst);   /* port: one body, bound by the wrapper's #define */
#endif

// MATCH: 0x00427990 FUN_00427990
void FUN_00427990(int id, float x, float y, float w, float h, int color, float size, int align)
{
    float pos[4];
    short text[256];
    int a = 0;
    switch (align) {
    case 0: a = 0; break;
    case 1: a = 2; break;
    case 2: a = 1; break;
    }
    FUN_004277a0(FUN_00427780(id), text);
    FUN_00552d10();
    FUN_00556e90(DAT_0067d83c, &color, &color, &color, &color);
    pos[2] = DAT_0067d830 * (w * 0.0015625f);
    pos[3] = DAT_0067d834 * (h * 0.0020833334f);
    pos[0] = x * 0.0015625f;
    pos[1] = (1.0f - y * 0.0020833334f) - pos[3];
    FUN_005555b0(DAT_0067d838, text, size * 0.0708f, pos, a, DAT_0067d83c);
    FUN_00552d70();
}
