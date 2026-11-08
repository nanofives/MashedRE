// FLAGS: /O2 /Ob1 /G5
/* MATCH-S22 0x004f81b0 (h6): 100% (regs 100%) under /O2 /Ob1 /G5. Converts one 2-float texcoord from packed source
   format fmt (0..14: float, byte, short, scaled byte/short, 10:10 unsigned/signed) into dst; default zeroes it.
   What moved it from the raw 62.7%: dst/src copied into block locals at entry (lets the compiler reuse the dead
   param homes for the int->float temps, 72.9 -> 92.3), case 13 reads the source dword twice (100), fmul by the
   float scale globals (raw had fimul). Residual: none. */
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
MGLOBAL(float, DAT_005dae88, 0x005dae88);
MGLOBAL(float, DAT_005cf00c, 0x005cf00c);
MGLOBAL(float, DAT_005dae98, 0x005dae98);
MGLOBAL(float, DAT_005ceb90, 0x005ceb90);
// MATCH: 0x004f81b0 FUN_004f81b0
void FUN_004f81b0(unsigned int fmt, float *dsti, const void *srci)
{
  float *dst = dsti;
  const void *src = srci;
  switch (fmt) {
  case 0:
    dst[0] = ((const float *)src)[0];
    dst[1] = 0.0f;
    return;
  case 1:
  case 2:
  case 3:
    dst[0] = ((const float *)src)[0];
    dst[1] = ((const float *)src)[1];
    return;
  case 4:
    dst[0] = (float)((const unsigned char *)src)[2] * DAT_005ceb90;
    dst[1] = (float)((const unsigned char *)src)[1] * DAT_005ceb90;
    return;
  case 5:
    dst[0] = (float)((const unsigned char *)src)[0];
    dst[1] = (float)((const unsigned char *)src)[1];
    return;
  case 6:
  case 7:
    dst[0] = (float)((const short *)src)[0];
    dst[1] = (float)((const short *)src)[1];
    return;
  case 8:
    dst[0] = (float)((const unsigned char *)src)[0] * DAT_005ceb90;
    dst[1] = (float)((const unsigned char *)src)[1] * DAT_005ceb90;
    return;
  case 9:
  case 10:
    dst[0] = (float)((const short *)src)[0] * DAT_005dae98;
    dst[1] = (float)((const short *)src)[1] * DAT_005dae98;
    return;
  case 11:
  case 12:
    dst[0] = (float)((const short *)src)[0] * DAT_005cf00c;
    dst[1] = (float)((const short *)src)[1] * DAT_005cf00c;
    return;
  case 13:
    {
      unsigned int u = *(const unsigned int *)src & 0x3ff;
      unsigned int v = (*(const unsigned int *)src >> 10) & 0x3ff;
      dst[0] = (float)u;
      dst[1] = (float)v;
    }
    return;
  case 14:
    {
      int x = *(const int *)src;
      int u = x & 0x3ff;
      int v;
      if (u & 0x200) u |= 0xfffffc00;
      v = (x >> 10) & 0x3ff;
      if (v & 0x200) v |= 0xfffffc00;
      dst[0] = (float)u * DAT_005dae88;
      dst[1] = (float)v * DAT_005dae88;
    }
    return;
  default:
    dst[0] = 0.0f;
    dst[1] = 0.0f;
    return;
  }
}
