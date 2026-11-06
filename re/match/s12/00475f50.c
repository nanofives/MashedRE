// FLAGS: /O2 /GS /Ob1
/* MATCH-S12 0x00475f50: 100% (regs 100%), relocheck OK (_ftol unchecked by name). By hand (raw 17.6%): output pointer
   arrives in ESI, so the function is file-static (pilot finding 1) with a small stand-in caller (not the real TU;
   the real caller is 0x00475f90). 0.5f / 1.0f / 255.0f are compiler literals (0x5cc32c, 0x5cc320, 0x5cd04c). */
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
// MATCH: 0x00475f50 FUN_00475f50
static void FUN_00475f50(unsigned char *out, float param_1)
{
  out[3] = (unsigned char)((param_1 < 0.5f ? param_1 : 1.0f - param_1) * 255.0f);
  out[0] = 0;
  out[1] = 0;
  out[2] = 0;
}
/* stand-in caller (not the real TU): makes the compiler pass `out` in ESI */
void FUN_00475f50_standin(unsigned char *o, float a)
{
  FUN_00475f50(o, a);
  FUN_00475f50(o + 4, a);
}
