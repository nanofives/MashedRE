// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dfab0 (h1, by hand): best 36.4% exact, regs 60.6% (raw 28.1%). Luminance (r*30+g*59+b*11)/100
   (signed divide), times alpha, unsigned /255. Residual: multiply-by-constant decomposition. The original builds
   g*59 as 7g -> 29g -> 59g and uses imul for r*30; 9178 emits 3g -> 15g -> 60g-g and a lea chain for r*30.
   Tried: term order (r,g,b / g,b,r), a lum temporary, int/uint/ushort casts on the bytes, alpha first, permuter
   (86 compiles, no gain). Class (a). C++ ok. */
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
// MATCH: 0x004dfab0 FUN_004dfab0
uint FUN_004dfab0(uchar *param_1)

{
  int lum;

  lum = (param_1[0] * 30 + param_1[1] * 59 + param_1[2] * 11) / 100;
  return (uint)(lum * param_1[3]) / 255;
}
