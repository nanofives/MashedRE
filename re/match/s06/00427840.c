// FLAGS: /O2 /GS /Ob1
/* MATCH-S06 h5 0x00427840 (54 B). Best 100.0% (regs 100.0%). Raw converter 57.1%.
   Static callee with a register convention: dst in ECX, src in EAX, from param order (dst, src) ((src, dst)
   swapped the two registers, 90.5%). The strlen is RwGlobals (DAT_007d3ff8) slot +0xf4, cdecl, 1 argument.
   Real caller 0x00427be0 (its file holds a copy); this file has a STAND-IN caller (not matched, not shipped).
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
#ifdef __cplusplus
#define MATCH_STATIC
#else
#define MATCH_STATIC static
#endif
typedef int (*RwStrlenFn)(const unsigned char *s);
MGLOBAL(char *, DAT_007d3ff8, 0x007d3ff8);
MGLOBAL(int, DAT_0067d838, 0x0067d838);
MGLOBAL(int, DAT_0067d83c, 0x0067d83c);
MGLOBAL(float, DAT_008991c0, 0x008991c0);

unsigned short *FUN_00427780(int id);
int FUN_00552d10(void);
void FUN_00552d70(void);
float *FUN_00556e90(int a, void *b, void *c, void *d, void *e);
int FUN_005555b0(int a, short *text, float size, float *pos, int align, int f);

// MATCH: 0x00427840 FUN_00427840
MATCH_STATIC void FUN_00427840(short *dst, const unsigned char *src)
{
    int n = (*(RwStrlenFn *)(DAT_007d3ff8 + 0xf4))(src);
    int i;
    for (i = 0; i < n; i++)
        *dst++ = *src++;
    *dst = 0;
}

#ifndef __cplusplus
/* stand-in caller (not matched, not shipped): text in EAX, buffer in ECX, as 0x00427bf2..0x00427c04 do */
void FUN_00427840_standin(const unsigned char *str)
{
    short text[512];
    FUN_00427840(text, str);
    FUN_005555b0(DAT_0067d838, text, 1.0f, &DAT_008991c0, 0, DAT_0067d83c);
}
#endif
