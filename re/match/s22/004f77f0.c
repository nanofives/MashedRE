// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f77f0 (h3, by hand): 100% (regs 100%) under /O2 /Ob1 /G5. Raw converter 56.4%.
   RGBA -> ARGB dword copy with alpha AND. What moved it: indexed src[i] (gives the +3 pointer bias, 60.5 -> 78.9%)
   and `void *dst` copied to a local byte pointer (dst loaded at entry, 78.9 -> 100%). */
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
typedef struct { unsigned char red, green, blue, alpha; } S22RGBA;
// MATCH: 0x004f77f0 FUN_004f77f0
int FUN_004f77f0(void *dst, const S22RGBA *src, int num, int stride)
{
  unsigned char *out = (unsigned char *)dst;
  unsigned char alpha = 0xff;
  int i;
  for (i = 0; i < num; i++) {
    *(unsigned int *)out = ((unsigned int)src[i].alpha << 24) | ((unsigned int)src[i].red << 16) |
                           ((unsigned int)src[i].green << 8) | (unsigned int)src[i].blue;
    alpha &= src[i].alpha;
    out += stride;
  }
  return alpha != 0xff;
}
