// TOOLCHAIN rw9044
// FLAGS: /O2 /Ob1 /G5
/* MATCH-S21 0x004dfb10 (h2): 20.0% (regs 60.0%, ops 66.7%). RwRGBA -> (green*59 + blue*11 + red*30)/100 (signed
   divide, magic 0x51eb851f) | alpha << 8. Operation order matches.
   Residual: constant-multiply strength reduction. The original builds *59 as ((8x-x)*4+x)*2+x, *11 as (5x)*2+x,
   and *30 as `imul 0x1e`, each byte loaded with xor/mov; 9178 /G5 builds *59 as (3x*5)*4-x and *30 as a lea chain.
   Same decomposition for 3 statement orders, int/unsigned locals, compound assignment; /G3../G7, /GB, /O1 change it
   to other forms, none the original's. Class (a): multiply decomposition is a code-generator choice. The raw
   converter form scored 24.1% but divides unsigned (the original divide is signed); this form is kept. */
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
typedef struct RwRGBA { unsigned char red, green, blue, alpha; } RwRGBA;
// MATCH: 0x004dfb10 FUN_004dfb10
unsigned int FUN_004dfb10(RwRGBA *c)
{
  int lum;
  lum = c->green * 59;
  lum += c->blue * 11;
  lum += c->red * 30;
  lum /= 100;
  return lum | (c->alpha << 8);
}
