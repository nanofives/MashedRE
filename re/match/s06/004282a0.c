// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x004282a0 (helper h6): 100% (regs 100%), relocheck OK. By hand from the raw converter.
   Holds a static copy of 0x004277a0 (its own // MATCH line, 100%): the callee takes src in EAX and dst in EBX,
   which 9176 only produces when it is a static function defined before the caller in the same file
   (pilot finding 1). 0.0708f is the literal at 0x005cd5fc, 640.0f the literal at 0x005cd618 (a global operand
   puts the constant load first). text is short[512] (0x400 bytes of frame). Residual: none. */
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
MGLOBAL(float, DAT_0067d830, 0x0067d830);
MGLOBAL(int, DAT_0067d838, 0x0067d838);
MGLOBAL(int, DAT_007d3ff8, 0x007d3ff8);
float FUN_005554d0(int font, short *text, float size);
short *FUN_00427780(int id);

#ifndef __cplusplus
// MATCH-HELD: 0x004277a0 (static copy for the DDK register convention; port body: re/match/s06/004277a0.c)
static void FUN_004277a0(short *src, short *dst)
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
void FUN_004277a0(short *src, short *dst);   /* port: one body, bound by the wrapper's #define */
#endif
// MATCH: 0x004282a0 FUN_004282a0
float FUN_004282a0(int id, float scale)
{
    short text[512];
    FUN_004277a0(FUN_00427780(id), text);
    return FUN_005554d0(DAT_0067d838, text, scale * 0.0708f) / DAT_0067d830 * 640.0f;
}
