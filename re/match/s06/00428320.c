// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 0x00428320 (helper h6): 100% (regs 100%), relocheck OK. By hand.
   Holds a static copy of 0x00427840 (its own // MATCH line, 100%): the callee takes src in EAX and dst in ECX;
   as a static function defined first, parameter order (dst, src) gives that assignment, (src, dst) swaps it.
   0.0708f / 640.0f are the literals at 0x005cd5fc / 0x005cd618. Residual: none. */
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
typedef int (*StrLenFn)(char *);

// MATCH: 0x00427840 FUN_00427840
static void FUN_00427840(short *dst, char *src)
{
    int n = (*(StrLenFn *)(DAT_007d3ff8 + 0xf4))(src);
    for (; n > 0; n--)
        *dst++ = (unsigned char)*src++;
    *dst = 0;
}
// MATCH: 0x00428320 FUN_00428320
float FUN_00428320(char *str, float scale)
{
    short text[512];
    FUN_00427840(text, str);
    return FUN_005554d0(DAT_0067d838, text, scale * 0.0708f) / DAT_0067d830 * 640.0f;
}
