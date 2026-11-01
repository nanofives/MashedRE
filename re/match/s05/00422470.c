// FLAGS: /O2 /GS /Ob1
/* MATCH-S05 0x00422470: pilot head start re/match/pilot/m_00422470.c brought into s05 form, 100% (both functions
   byte-identical). This file holds the static callee 0x004223f0 (register-passed args: i in EAX, seed in ECX;
   the caller keeps EDX live across the call). Needed: callee static + defined first, parameter order (i, seed),
   t and b computed into locals, /Ob1. Constants: 4.656612873077393e-10 = float at 0x005cd314 (1/2^31); the 2^32
   fix-up at 0x005cc94c is the compiler's unsigned->float conversion. floor = CRT 0x004a3090. */
/* ---- MATCH-S05 preamble (same text in every re/match/s05 source; not part of any function body) ----
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
double floor(double);
float FUN_00422440(float a, float b, float t);

// MATCH: 0x004223f0 _FUN_004223f0
static float FUN_004223f0(unsigned int i, unsigned int seed)
{
    unsigned int n;
    if (seed)
        i &= seed;
    n = (i << 13) ^ i;
    n = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return (float)n * 4.656612873077393e-10f;
}

// MATCH: 0x00422470 _FUN_00422470
float FUN_00422470(float x, unsigned int seed)
{
    unsigned int i = (int)floor(x);
    float t = x - i;
    float b = FUN_004223f0(i + 1, seed);
    return FUN_00422440(FUN_004223f0(i, seed), b, t);
}
